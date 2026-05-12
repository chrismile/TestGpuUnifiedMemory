/*
 * BSD 2-Clause License
 *
 * Copyright (c) 2026, Christoph Neuhauser
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <iostream>
#include <string>
#include <stdexcept>
#include <functional>
#include <chrono>

#include <Math/Math.hpp>

#include "Common.hpp"
#include "TestHip.hpp"

#include <hip/hip_runtime_api.h>

static bool isHipRuntimeApiInitialized = false;

static void errorCheckHip(hipError_t hipError, const char* name) {
    if (hipError != hipSuccess) {
        throw std::runtime_error(
                std::string() + "HIP error (" + std::to_string(int(hipError)) + ") in " + name + ": "
                + hipGetErrorString(hipError));
    }
}
void _checkHipResult(hipError_t hipResult, const char* text, const char* locationText) {
    if (hipResult != hipSuccess) {
        const char* errorString = hipGetErrorString(hipResult);
        throw std::runtime_error(std::string() + locationText + ": " + text + errorString);
    }
}

bool getIsHipRuntimeApiInitialized() {
    return isHipRuntimeApiInitialized;
}

void setHipDevice(hipDevice_t hipDevice) {
    hipError_t hipError = hipSetDevice(hipDevice);
    errorCheckHip(hipError, "hipSetDevice");
    hipError = hipFree(nullptr);
    errorCheckHip(hipError, "hipSetDevice");
    isHipRuntimeApiInitialized = true;
}

__global__ void copyBufferKernel(uint32_t numElements, const float* __restrict__ ptrSrc, float* __restrict__ ptrDst) {
    uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < numElements) {
        ptrDst[idx] = ptrSrc[idx];
    }
}

double runTestsHipIndividual(
        hipStream_t stream, int numCopiesPerRun, bool measureUpload,
        uint32_t numElements, float* ptrSrc, float* ptrDst, float* hostPtr,
        const std::function<void()>& uploadDataCallback,
        void (*checkMemoryContentCallback)(uint32_t numElements, const void* hostPtr)) {
    dim3 blockDim(256, 1, 1);
    dim3 gridDim(sgl::uiceil(numElements, blockDim.x), 1, 1);

    const int numRuns = getNumRuns(numCopiesPerRun, numElements);

    double elapsedTimeNs = 0.0;
    std::string errorMessage;
    for (int it = 0; it < numRuns + 1; it++) {
        if (!measureUpload) {
            uploadDataCallback();
            errorCheckHip(hipStreamSynchronize(stream), "hipStreamSynchronize");
        }

        auto timeStart = std::chrono::high_resolution_clock::now();
        if (measureUpload) {
            uploadDataCallback();
        }
        for (int copyIdx = 0; copyIdx < numCopiesPerRun; copyIdx++) {
            copyBufferKernel<<<gridDim, blockDim, 0, stream>>>(numElements, ptrSrc, ptrDst);
        }
        errorCheckHip(hipStreamSynchronize(stream), "hipStreamSynchronize");
        auto timeStop = std::chrono::high_resolution_clock::now();
        auto elapsedTimeRunNs = std::chrono::duration_cast<std::chrono::nanoseconds>(timeStop - timeStart);
        if (it != 0) {
            // First run is warmup.
            elapsedTimeNs += double(elapsedTimeRunNs.count()) / double(numRuns * numCopiesPerRun);
        }
        errorCheckHip(hipMemcpyAsync(
                hostPtr, ptrDst, numElements * sizeof(float), hipMemcpyDeviceToHost, stream), "hipMemcpyAsync");
        errorCheckHip(hipStreamSynchronize(stream), "hipStreamSynchronize");

        // Check equality to expected values.
        checkMemoryContentCallback(numElements, hostPtr);
    }

    return elapsedTimeNs;
}

void runTestsHip(
        hipDevice_t hipDevice, uint32_t numElements,
        void (*checkMemoryContentCallback)(uint32_t numElements, const void* hostPtr)) {
    hipStream_t stream{};
    errorCheckHip(hipStreamCreateWithFlags(&stream, hipStreamNonBlocking), "hipStreamCreateWithFlags");

    hipDeviceProp_t deviceProp{};
    errorCheckHip(hipGetDeviceProperties(&deviceProp, hipDevice), "hipGetDeviceProperties");

    const size_t sizeInBytes = numElements * sizeof(float);

    float* ptrSrc = nullptr;
    float* ptrDst = nullptr;
    float* hostPtr = nullptr;
    errorCheckHip(hipMalloc(reinterpret_cast<void**>(&ptrDst), sizeInBytes), "hipMalloc");
    errorCheckHip(hipHostMalloc(reinterpret_cast<void**>(&hostPtr), sizeInBytes), "hipHostMalloc");

    auto* bufferHost = new float[numElements];
    for (uint32_t i = 0; i < numElements; i++) {
        bufferHost[i] = static_cast<float>(i);
    }

    std::cout << "Tested API: HIP" << std::endl;
    for (int configIdx = 0; configIdx < NUM_CONFIGS; configIdx++) {
        int numCopiesPerRun = configsNumCopiesPerRun[configIdx];
        bool measureUpload = configsMeasureUpload[configIdx];
        std::cout << " #Accesses: " << numCopiesPerRun;
        if (!measureUpload) {
            std::cout << " (upload excluded)";
        }
        std::cout << std::endl;
        {
            errorCheckHip(hipMalloc(reinterpret_cast<void**>(&ptrSrc), sizeInBytes), "hipMalloc");
            auto uploadDataCallback = [&]() {
                errorCheckHip(hipMemcpyAsync(
                        ptrSrc, bufferHost, sizeInBytes, hipMemcpyHostToDevice, stream), "hipMemcpyAsync");
            };
            double elapsedTimeNs = runTestsHipIndividual(
                    stream, numCopiesPerRun, measureUpload, numElements, ptrSrc, ptrDst, hostPtr,
                    uploadDataCallback, checkMemoryContentCallback);
            errorCheckHip(hipFree(ptrSrc), "hipFree");
            std::cout << "  Time copy hipMalloc: " << convertTimeToString(elapsedTimeNs, numElements) << std::endl;
        }
        {
            errorCheckHip(hipMallocManaged(reinterpret_cast<void**>(&ptrSrc), sizeInBytes), "hipMallocManaged");
            auto uploadDataCallback = [&]() {
                memcpy(ptrSrc, bufferHost, sizeInBytes);
            };
            double elapsedTimeNs = runTestsHipIndividual(
                    stream, numCopiesPerRun, measureUpload, numElements, ptrSrc, ptrDst, hostPtr,
                    uploadDataCallback, checkMemoryContentCallback);
            errorCheckHip(hipFree(ptrSrc), "hipFree");
            std::cout << "  Time copy hipMallocManaged: " << convertTimeToString(elapsedTimeNs, numElements) << std::endl;
        }
        {
            errorCheckHip(hipMallocManaged(reinterpret_cast<void**>(&ptrSrc), sizeInBytes), "hipMallocManaged");
            // hipMemAdvise makes things slower on an RTX 3090.
            //errorCheckHip(hipMemAdvise(ptrSrc, sizeInBytes, hipMemAdviseSetReadMostly, hipMemLocationTypeDevice), "hipMemPrefetchAsync");
            auto uploadDataCallback = [&]() {
                errorCheckHip(hipMemcpyAsync(
                        ptrSrc, bufferHost, sizeInBytes, hipMemcpyHostToDevice, stream), "hipMemcpyAsync");
                if (deviceProp.concurrentManagedAccess) {
                    errorCheckHip(hipMemPrefetchAsync_v2(
                            ptrSrc, sizeInBytes, hipMemLocation{hipMemLocationTypeDevice, hipDevice}, 0,
                            stream), "hipMemPrefetchAsync");
                }
            };
            double elapsedTimeNs = runTestsHipIndividual(
                    stream, numCopiesPerRun, measureUpload, numElements, ptrSrc, ptrDst, hostPtr,
                    uploadDataCallback, checkMemoryContentCallback);
            errorCheckHip(hipFree(ptrSrc), "hipFree");
            std::cout << "  Time copy hipMallocManaged2: " << convertTimeToString(elapsedTimeNs, numElements) << std::endl;
        }
        {
            errorCheckHip(hipHostMalloc(reinterpret_cast<void**>(&ptrSrc), sizeInBytes), "hipHostMalloc");
            float* ptrSrcDevice = nullptr;
            errorCheckHip(hipHostGetDevicePointer(
                    reinterpret_cast<void**>(&ptrSrcDevice), ptrSrc, 0), "hipHostGetDevicePointer");
            auto uploadDataCallback = [&]() {
                memcpy(ptrSrc, bufferHost, sizeInBytes);
            };
            double elapsedTimeNs = runTestsHipIndividual(
                    stream, numCopiesPerRun, measureUpload, numElements, ptrSrc, ptrDst, hostPtr,
                    uploadDataCallback, checkMemoryContentCallback);
            errorCheckHip(hipFreeHost(ptrSrc), "hipFreeHost");
            std::cout << "  Time copy hipHostMalloc: " << convertTimeToString(elapsedTimeNs, numElements) << std::endl;
        }
    }
    std::cout << std::endl;

    delete[] bufferHost;
    errorCheckHip(hipFree(ptrDst), "hipFree");
    errorCheckHip(hipFreeHost(hostPtr), "hipFreeHost");
    errorCheckHip(hipStreamDestroy(stream), "hipStreamDestroy");
}
