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

#include <cstring>
#include <sycl/sycl.hpp>

#include "Common.hpp"
#include "TestSycl.hpp"

#define UUID_SIZE 16u

sycl::event copyBufferKernel(
        sycl::queue& syclQueue, uint32_t numElements, const float* ptrSrc, float* ptrDst,
        const sycl::event& lastCommandEvent) {
    auto event = syclQueue.submit([&](sycl::handler& cgh) {
        cgh.depends_on(lastCommandEvent);
        cgh.parallel_for<class CopyBufferKernel>(sycl::range<1>{numElements}, [=](sycl::id<1> it) {
            const auto index = it[0];
            ptrDst[index] = ptrSrc[index];
        });
    });
    return event;
}

double runTestsSyclIndividual(
        sycl::queue& syclQueue, int numCopiesPerRun, bool measureUpload,
        uint32_t numElements, float* ptrSrc, float* ptrDst, float* hostPtr,
        const std::function<sycl::event()>& uploadDataCallback,
        void (*checkMemoryContentCallback)(uint32_t numElements, const void* hostPtr)) {
    const int numRuns = getNumRuns(numCopiesPerRun, numElements);

    double elapsedTimeNs = 0.0;
    std::string errorMessage;
    for (int it = 0; it < numRuns + 1; it++) {
        if (!measureUpload) {
            sycl::event uploadDataEvent = uploadDataCallback();
            uploadDataEvent.wait_and_throw();
        }

        auto timeStart = std::chrono::high_resolution_clock::now();
        sycl::event dispatchKernelEvent;
        if (measureUpload) {
            dispatchKernelEvent = uploadDataCallback();
        }
        for (int copyIdx = 0; copyIdx < numCopiesPerRun; copyIdx++) {
            dispatchKernelEvent = copyBufferKernel(syclQueue, numElements, ptrSrc, ptrDst, dispatchKernelEvent);
        }
        dispatchKernelEvent.wait_and_throw();
        auto timeStop = std::chrono::high_resolution_clock::now();
        auto elapsedTimeRunNs = std::chrono::duration_cast<std::chrono::nanoseconds>(timeStop - timeStart);
        if (it != 0) {
            // First run is warmup.
            elapsedTimeNs += double(elapsedTimeRunNs.count()) / double(numRuns * numCopiesPerRun);
        }
        auto copyToHostEvent = syclQueue.memcpy(hostPtr, ptrDst, numElements * sizeof(float));
        copyToHostEvent.wait_and_throw();

        // Check equality to expected values.
        checkMemoryContentCallback(numElements, hostPtr);
    }

    return elapsedTimeNs;
}

void runTestsSycl(
        const uint8_t* deviceUuid, uint32_t numElements,
        void (*checkMemoryContentCallback)(uint32_t numElements, const void* hostPtr)) {
    sycl::device syclDevice;
    try {
        syclDevice = sycl::detail::select_device([deviceUuid](const sycl::device &testedDevice) -> int {
            sycl::detail::uuid_type uuid = testedDevice.get_info<sycl::ext::intel::info::device::uuid>();
            bool isSameUuid = true;
            for (int i = 0; i < static_cast<int>(UUID_SIZE); i++) {
                if (deviceUuid[i] != reinterpret_cast<const uint8_t*>(uuid.data())[i]) {
                    isSameUuid = false;
                    break;
                }
            }
            if (!isSameUuid) {
                return -1;
            }
            return 1;
        });
    } catch (sycl::exception const& e) {
        std::cout << "Skipping SYCL device: " << e.what() << std::endl << std::endl;
        return;
    }

    sycl::queue syclQueue{syclDevice, sycl::property::queue::in_order()};
    const size_t sizeInBytes = numElements * sizeof(float);

    float* ptrSrc = nullptr;
    auto* ptrDst = sycl::malloc_device<float>(numElements, syclQueue);
    auto* hostPtr = sycl::malloc_host<float>(numElements, syclQueue);
    auto* bufferHost = sycl::malloc_host<float>(numElements, syclQueue);

    for (uint32_t i = 0; i < numElements; i++) {
        bufferHost[i] = static_cast<float>(i);
    }

    std::cout << "Tested API: SYCL" << std::endl;
    for (int configIdx = 0; configIdx < NUM_CONFIGS; configIdx++) {
        int numCopiesPerRun = configsNumCopiesPerRun[configIdx];
        bool measureUpload = configsMeasureUpload[configIdx];
        std::cout << " #Accesses: " << numCopiesPerRun;
        if (!measureUpload) {
            std::cout << " (upload excluded)";
        }
        std::cout << std::endl;
        {
            ptrSrc = sycl::malloc_device<float>(numElements, syclQueue);
            auto uploadDataCallback = [&]() -> sycl::event {
                return syclQueue.memcpy(ptrSrc, bufferHost, sizeInBytes);
            };
            double elapsedTimeNs = runTestsSyclIndividual(
                    syclQueue, numCopiesPerRun, measureUpload, numElements, ptrSrc, ptrDst, hostPtr,
                    uploadDataCallback, checkMemoryContentCallback);
            sycl::free(ptrSrc, syclQueue);
            std::cout << "  Time copy malloc_device: " << convertTimeToString(elapsedTimeNs, numElements) << std::endl;
        }
        {
            ptrSrc = sycl::malloc_shared<float>(numElements, syclQueue);
            auto uploadDataCallback = [&]() {
                return syclQueue.memcpy(ptrSrc, bufferHost, sizeInBytes);
            };
            double elapsedTimeNs = runTestsSyclIndividual(
                    syclQueue, numCopiesPerRun, measureUpload, numElements, ptrSrc, ptrDst, hostPtr,
                    uploadDataCallback, checkMemoryContentCallback);
            sycl::free(ptrSrc, syclQueue);
            std::cout << "  Time copy malloc_shared: " << convertTimeToString(elapsedTimeNs, numElements) << std::endl;
        }
        {
            ptrSrc = sycl::malloc_host<float>(numElements, syclQueue);
            auto uploadDataCallback = [&]() {
                return syclQueue.memcpy(ptrSrc, bufferHost, sizeInBytes);
            };
            double elapsedTimeNs = runTestsSyclIndividual(
                    syclQueue, numCopiesPerRun, measureUpload, numElements, ptrSrc, ptrDst, hostPtr,
                    uploadDataCallback, checkMemoryContentCallback);
            sycl::free(ptrSrc, syclQueue);
            std::cout << "  Time copy malloc_host: " << convertTimeToString(elapsedTimeNs, numElements) << std::endl;
        }
    }
    std::cout << std::endl;

    sycl::free(ptrDst, syclQueue);
    sycl::free(hostPtr, syclQueue);
    sycl::free(bufferHost, syclQueue);
}
