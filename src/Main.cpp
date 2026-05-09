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

#include <Math/Math.hpp>
#include <Utils/File/Logfile.hpp>
#include <Graphics/Vulkan/Utils/Instance.hpp>
#include <Graphics/Vulkan/Utils/Device.hpp>

#ifdef SUPPORT_CUDA
#include <Graphics/Vulkan/Utils/InteropCuda.hpp>
#endif

#include "Common.hpp"
#include "TestVulkan.hpp"
#ifdef SUPPORT_CUDA
#include "TestCuda.hpp"
#endif
#ifdef SUPPORT_HIP
#include "TestHip.hpp"
#endif
#ifdef SUPPORT_SYCL
#include "TestSycl.hpp"
#endif

#ifdef SUPPORT_CUDA
static bool isCudaInitialized = false;
#endif

void vulkanErrorCallbackHeadless() {
    std::cerr << "Application callback" << std::endl;
}

void checkMemoryContentCallback(const void* hostPtr) {
    std::string errorMessage;
    if (!checkIsArrayLinear(numElements, hostPtr, errorMessage)) {
        sgl::Logfile::get()->throwError("Memory content mismatched.");
    }
}

#ifdef SUPPORT_HIP
bool getMatchingHipDevice(sgl::vk::Device* device, hipDevice_t* hipDevice) {
    const VkPhysicalDeviceIDProperties& deviceIdProperties = device->getDeviceIDProperties();
    bool foundDevice = false;

    int numDevices = 0;
    hipError_t hipResult = hipGetDeviceCount(&numDevices);
    if (hipResult == hipErrorNoDevice) {
        std::cout << "No HIP device detected" << std::endl << std::endl;
        return false;
    }
    checkHipResult(hipResult, "Error in hipGetDeviceCount: ");

    for (int deviceIdx = 0; deviceIdx < numDevices; deviceIdx++) {
        hipDevice_t currDevice = 0;
        hipResult = hipDeviceGet(&currDevice, deviceIdx);
        checkHipResult(hipResult, "Error in hipDeviceGet: ");

        hipUUID currUuid = {};
        hipResult = hipDeviceGetUuid(&currUuid, currDevice);
        checkHipResult(hipResult, "Error in hipDeviceGetUuid: ");

        bool isSameUuid = true;
        for (int i = 0; i < 16; i++) {
            if (deviceIdProperties.deviceUUID[i] != reinterpret_cast<uint8_t*>(&currUuid.bytes)[i]) {
                isSameUuid = false;
                break;
            }
        }
        if (isSameUuid) {
            foundDevice = true;
            *hipDevice = currDevice;
            break;
        }
    }

    /*
     * hipDeviceGetUuid is not compatible with VkPhysicalDeviceIDProperties::deviceUUID:
     * https://github.com/ROCm/hipamd/issues/50
     * Use some reasonable fallback when no device could be matched.
     */
    if (!foundDevice) {
        if (device->getDeviceDriverId() != VK_DRIVER_ID_AMD_PROPRIETARY
                && device->getDeviceDriverId() != VK_DRIVER_ID_AMD_OPEN_SOURCE
                && device->getDeviceDriverId() != VK_DRIVER_ID_MESA_RADV) {
            return false;
        }
        if (numDevices == 1) {
            hipDevice_t currDevice = 0;
            hipResult = hipDeviceGet(&currDevice, 0);
            checkHipResult(hipResult, "Error in hipDeviceGet: ");
            foundDevice = true;
            *hipDevice = currDevice;
        } else {
            char deviceName[256];
            for (int deviceIdx = 0; deviceIdx < numDevices; deviceIdx++) {
                hipDevice_t currDevice = 0;
                hipResult = hipDeviceGet(&currDevice, deviceIdx);
                checkHipResult(hipResult, "Error in hipDeviceGet: ");

                memset(deviceName, 0, sizeof(deviceName));
                hipResult = hipDeviceGetName(deviceName, 255, currDevice);
                checkHipResult(hipResult, "Error in hipDeviceGetUuid: ");

                if (strcmp(device->getDeviceName(), deviceName) == 0) {
                    foundDevice = true;
                    *hipDevice = currDevice;
                }
            }
        }
    }

    return foundDevice;
}
#endif

void runTests(sgl::vk::Device*& device) {
    std::cout << "Running on " << device->getDeviceName() << std::endl;

    runTestsVulkan(device);

#ifdef SUPPORT_CUDA
    if (device->getDeviceDriverId() == VK_DRIVER_ID_NVIDIA_PROPRIETARY) {
        // Choose a CUDA device matching the Vulkan device using the CUDA driver API.
        if (!isCudaInitialized) {
            if (!sgl::initializeCudaDeviceApiFunctionTable()) {
                sgl::Logfile::get()->throwError("Error in main: sgl::initializeCudaDeviceApiFunctionTable() returned false.");
            }
            CUresult cuResult = sgl::g_cudaDeviceApiFunctionTable.cuInit(0);
            if (cuResult == CUDA_ERROR_NO_DEVICE) {
                sgl::Logfile::get()->throwError("No CUDA-capable device was found. Disabling CUDA interop support.");
            }
            sgl::checkCUresult(cuResult, "Error in cuInit: ");
            isCudaInitialized = true;
        }
        CUdevice cuDevice = 0;
        if (!sgl::vk::getMatchingCudaDevice(device, &cuDevice)) {
            sgl::Logfile::get()->throwError("Error in main: sgl::vk::getMatchingCudaDevice could not find a matching device.");
        }

        // Set the selected CUDA driver API device in the runtime API.
        setCudaDevice(cuDevice);
        runTestsCuda(cuDevice, checkMemoryContentCallback);
#endif

#ifdef SUPPORT_HIP
    if (device->getDeviceDriverId() == VK_DRIVER_ID_NVIDIA_PROPRIETARY
            || device->getDeviceDriverId() == VK_DRIVER_ID_MESA_NVK
            || device->getDeviceDriverId() == VK_DRIVER_ID_MESA_RADV
            || device->getDeviceDriverId() == VK_DRIVER_ID_AMD_OPEN_SOURCE
            || device->getDeviceDriverId() == VK_DRIVER_ID_AMD_PROPRIETARY) {
        hipDevice_t hipDevice;
        if (getMatchingHipDevice(device, &hipDevice)) {
            setHipDevice(hipDevice);
            runTestsHip(hipDevice, checkMemoryContentCallback);
        }
    }
#endif

#ifdef SUPPORT_SYCL
        runTestsSycl(device->getDeviceIDProperties().deviceUUID, checkMemoryContentCallback);
#endif
    }
}

int main() {
    sgl::Logfile::get()->createLogfile("LogfileTestGpuUnifiedMemory.html", "TestGpuUnifiedMemory");

    auto* instance = new sgl::vk::Instance;
#ifdef NDEBUG
    instance->createInstance({}, false);
#else
    instance->createInstance({}, true);
#endif
    instance->setDebugCallback(&vulkanErrorCallbackHeadless);

    sgl::vk::DeviceFeatures requestedDeviceFeatures{};
    std::vector<const char*> requiredDeviceExtensions = { VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME };

    std::vector<VkPhysicalDevice> physicalDevices = sgl::vk::enumeratePhysicalDevices(instance);
    std::vector<VkPhysicalDevice> suitablePhysicalDevices;
    VkPhysicalDeviceProperties physicalDeviceProperties{};
    for (auto& physicalDevice : physicalDevices) {
        sgl::vk::getPhysicalDeviceProperties(physicalDevice, physicalDeviceProperties);
        if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU) {
            continue;
        }
        if (sgl::vk::checkIsPhysicalDeviceSuitable(
                instance, physicalDevice, nullptr, requiredDeviceExtensions, requestedDeviceFeatures, true)) {
            suitablePhysicalDevices.push_back(physicalDevice);
        }
    }
    for (size_t i = 0; i < suitablePhysicalDevices.size(); i++) {
        if (i != 0) {
            std::cout << std::endl << "--------------------------------------------" << std::endl << std::endl;
        }
        auto physicalDevice = suitablePhysicalDevices.at(i);
        auto* device = new sgl::vk::Device;
        device->createDeviceHeadlessFromPhysicalDevice(
                instance, physicalDevice, requiredDeviceExtensions, {}, requestedDeviceFeatures, false);
        runTests(device);
        delete device;
    }

#ifdef SUPPORT_CUDA
    if (isCudaInitialized) {
        sgl::freeCudaDeviceApiFunctionTable();
    }
#endif

    delete instance;

    return 0;
}
