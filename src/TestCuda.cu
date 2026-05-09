/*
* BSD 3-Clause License
 *
 * Copyright (c) 2026, Christoph Neuhauser
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
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
#include <Utils/File/Logfile.hpp>

#include "Common.hpp"
#include "TestCuda.hpp"

static bool isCudaRuntimeApiInitialized = false;

static void errorCheckCuda(cudaError_t cudaError, const char* name) {
    if (cudaError != cudaSuccess) {
        throw std::runtime_error(
            std::string() + "CUDA error (" + std::to_string(int(cudaError)) + ") in " + name + ": "
            + cudaGetErrorString(cudaError));
    }
}

bool getIsCudaRuntimeApiInitialized() {
    return isCudaRuntimeApiInitialized;
}

void setCudaDevice(CUdevice cuDevice) {
    cudaError_t cudaError = cudaSetDevice(cuDevice);
    errorCheckCuda(cudaError, "cudaSetDevice");
    cudaError = cudaFree(nullptr);
    errorCheckCuda(cudaError, "cudaSetDevice");
    isCudaRuntimeApiInitialized = true;
}

__global__ void copyBufferKernel(uint32_t numElements, const float* __restrict__ ptrSrc, float* __restrict__ ptrDst) {
    uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < numElements) {
        ptrDst[idx] = ptrSrc[idx];
    }
}

double runTestsCudaIndividual(
        cudaStream_t stream, int numCopiesPerRun, uint32_t numElements, float* ptrSrc, float* ptrDst, float* hostPtr,
        const std::function<void()>& uploadDataCallback) {
    dim3 blockDim(256, 1, 1);
    dim3 gridDim(sgl::uiceil(numElements, blockDim.x), 1, 1);

    const int numRuns = numCopiesPerRun <= 1 ? 10 : 1;

    double elapsedTimeMs = 0.0;
    std::string errorMessage;
    for (int it = 0; it < numRuns + 1; it++) {
        auto timeStart = std::chrono::high_resolution_clock::now();
        uploadDataCallback();
        for (int copyIdx = 0; copyIdx < numCopiesPerRun; copyIdx++) {
            copyBufferKernel<<<gridDim, blockDim, 0, stream>>>(numElements, ptrSrc, ptrDst);
        }
        errorCheckCuda(cudaStreamSynchronize(stream), "cudaStreamSynchronize");
        auto timeStop = std::chrono::high_resolution_clock::now();
        auto elapsedTimeRunNs = std::chrono::duration_cast<std::chrono::nanoseconds>(timeStop - timeStart);
        if (it != 0) {
            // First run is warmup.
            elapsedTimeMs += double(elapsedTimeRunNs.count()) * 1e-6 / double(numRuns * numCopiesPerRun);
        }
        errorCheckCuda(cudaMemcpyAsync(
                hostPtr, ptrDst, numElements * sizeof(float), cudaMemcpyDeviceToHost, stream), "cudaMemcpyAsync");
        errorCheckCuda(cudaStreamSynchronize(stream), "cudaStreamSynchronize");

        // Check equality to expected values.
        if (!checkIsArrayLinear(numElements, hostPtr, errorMessage)) {
            sgl::Logfile::get()->throwError("Memory content mismatched.");
        }
    }

    return elapsedTimeMs;
}

void runTestsCuda(CUdevice cuDevice) {
    cudaStream_t stream{};
    errorCheckCuda(cudaStreamCreateWithFlags(&stream, cudaStreamNonBlocking), "cudaStreamCreateWithFlags");

    cudaDeviceProp deviceProp{};
    errorCheckCuda(cudaGetDeviceProperties(&deviceProp, cuDevice), "cudaGetDeviceProperties");

    const size_t sizeInBytes = numElements * sizeof(float);

    float* ptrSrc = nullptr;
    float* ptrDst = nullptr;
    float* hostPtr = nullptr;
    errorCheckCuda(cudaMalloc(reinterpret_cast<void**>(&ptrDst), sizeInBytes), "cudaMalloc");
    errorCheckCuda(cudaMallocHost(reinterpret_cast<void**>(&hostPtr), sizeInBytes), "cudaMallocHost");

    auto* bufferHost = new float[numElements];
    for (uint32_t i = 0; i < numElements; i++) {
        bufferHost[i] = static_cast<float>(i);
    }

    std::cout << "Tested API: CUDA" << std::endl;
    for (int numCopiesPerRun : numCopiesPerRunConfigs) {
        std::cout << " #Accesses: " << numCopiesPerRun << std::endl;
        {
            errorCheckCuda(cudaMalloc(reinterpret_cast<void**>(&ptrSrc), sizeInBytes), "cudaMalloc");
            auto uploadDataCallback = [&]() {
                errorCheckCuda(cudaMemcpyAsync(
                        ptrSrc, bufferHost, sizeInBytes, cudaMemcpyHostToDevice, stream), "cudaMemcpyAsync");
            };
            double elapsedTimeMs = runTestsCudaIndividual(
                    stream, numCopiesPerRun, numElements, ptrSrc, ptrDst, hostPtr, uploadDataCallback);
            errorCheckCuda(cudaFree(ptrSrc), "cudaFree");
            std::cout << "  Time copy cudaMalloc: " << elapsedTimeMs << "ms" << std::endl;
        }
        {
            errorCheckCuda(cudaMallocManaged(reinterpret_cast<void**>(&ptrSrc), sizeInBytes), "cudaMallocManaged");
            auto uploadDataCallback = [&]() {
                memcpy(ptrSrc, bufferHost, sizeInBytes);
            };
            double elapsedTimeMs = runTestsCudaIndividual(
                    stream, numCopiesPerRun, numElements, ptrSrc, ptrDst, hostPtr, uploadDataCallback);
            errorCheckCuda(cudaFree(ptrSrc), "cudaFree");
            std::cout << "  Time copy cudaMallocManaged: " << elapsedTimeMs << "ms" << std::endl;
        }
        {
            errorCheckCuda(cudaMallocManaged(reinterpret_cast<void**>(&ptrSrc), sizeInBytes), "cudaMallocManaged");
            // cudaMemAdvise makes things slower on an RTX 3090.
            //errorCheckCuda(cudaMemAdvise(ptrSrc, sizeInBytes, cudaMemAdviseSetReadMostly, cudaMemLocationTypeDevice), "cudaMemAdvise");
            auto uploadDataCallback = [&]() {
                errorCheckCuda(cudaMemcpyAsync(
                        ptrSrc, bufferHost, sizeInBytes, cudaMemcpyHostToDevice, stream), "cudaMemcpyAsync");
                /*
                 * https://stackoverflow.com/questions/43430216/cudamemprefetchasync-returns-cudaerrorinvaliddevice-why
                 * seems to indicate that the device property concurrentManagedAccess is needed for
                 * cudaMemPrefetchAsync,
                 */
                if (deviceProp.concurrentManagedAccess) {
                    errorCheckCuda(cudaMemPrefetchAsync_v2(
                            ptrSrc, sizeInBytes, cudaMemLocation{cudaMemLocationTypeDevice, cuDevice}, 0,
                            stream), "cudaMemPrefetchAsync");
                }
            };
            double elapsedTimeMs = runTestsCudaIndividual(
                    stream, numCopiesPerRun, numElements, ptrSrc, ptrDst, hostPtr, uploadDataCallback);
            errorCheckCuda(cudaFree(ptrSrc), "cudaFree");
            std::cout << "  Time copy cudaMallocManaged2: " << elapsedTimeMs << "ms" << std::endl;
        }
        {
            errorCheckCuda(cudaMallocHost(reinterpret_cast<void**>(&ptrSrc), sizeInBytes), "cudaMallocHost");
            /*
             * cudaHostGetDevicePointer is not needed for devices with the attribute
             * cudaDevAttrCanUseHostPointerForRegisteredMem.
             *
             * It is unclear whether cudaMallocHost is OK to use. According to
             * https://docs.nvidia.com/cuda/cuda-runtime-api/group__CUDART__MEMORY.html, cudaHostAlloc is equivalent
             * to cudaMallocHost without any flags, and the flag cudaHostAllocMapped is needed for
             * cudaHostGetDevicePointer. For this, the cudaDeviceMapHost flag needs to be set (can be checked via
             * cudaGetDeviceFlags).
             */
            float* ptrSrcDevice = nullptr;
            errorCheckCuda(cudaHostGetDevicePointer(
                    reinterpret_cast<void**>(&ptrSrcDevice), ptrSrc, 0), "cudaHostGetDevicePointer");
            auto uploadDataCallback = [&]() {
                memcpy(ptrSrc, bufferHost, sizeInBytes);
            };
            double elapsedTimeMs = runTestsCudaIndividual(
                    stream, numCopiesPerRun, numElements, ptrSrcDevice, ptrDst, hostPtr, uploadDataCallback);
            errorCheckCuda(cudaFreeHost(ptrSrc), "cudaFreeHost");
            std::cout << "  Time copy cudaMallocHost: " << elapsedTimeMs << "ms" << std::endl;
        }
    }
    std::cout << std::endl;

    delete[] bufferHost;
    errorCheckCuda(cudaFree(ptrDst), "cudaFree");
    errorCheckCuda(cudaFreeHost(hostPtr), "cudaFreeHost");
    errorCheckCuda(cudaStreamDestroy(stream), "cudaStreamDestroy");
}
