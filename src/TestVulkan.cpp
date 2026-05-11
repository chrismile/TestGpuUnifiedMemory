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
#include <chrono>

#include <Math/Math.hpp>
#include <Utils/Memory.hpp>
#include <Utils/File/Logfile.hpp>
#include <Graphics/Vulkan/Shader/ShaderManager.hpp>
#include <Graphics/Vulkan/Render/Renderer.hpp>
#include <Graphics/Vulkan/Render/CommandBuffer.hpp>
#include <Graphics/Vulkan/Render/ComputePipeline.hpp>
#include <Graphics/Vulkan/Render/Data.hpp>
#include <Graphics/Vulkan/libs/VMA/vk_mem_alloc.h>

#include "Common.hpp"
#include "TestVulkan.hpp"

double runTestsVulkanIndividual(
        int numCopiesPerRun, bool measureUpload,
        sgl::vk::Renderer* renderer, const sgl::vk::FencePtr& fence, const sgl::vk::CommandBufferPtr& commandBuffer,
        const sgl::vk::ComputeDataPtr& computeData, uint32_t numElements,
        const std::function<void(VkCommandBuffer)>& uploadDataCallback,
        const sgl::vk::BufferPtr& bufferDst, const sgl::vk::BufferPtr& stagingBuffer) {
    const int numRuns = numCopiesPerRun <= 1 ? 10 : 1;

    double elapsedTimeMs = 0.0;
    std::string errorMessage;
    for (int it = 0; it < numRuns + 1; it++) {
        if (!measureUpload) {
            VkCommandBuffer commandBufferTmp = renderer->getDevice()->beginSingleTimeCommands();
            uploadDataCallback(commandBufferTmp);
            renderer->getDevice()->endSingleTimeCommands(commandBufferTmp);
        }

        auto timeStart = std::chrono::high_resolution_clock::now();
        renderer->pushCommandBuffer(commandBuffer);
        commandBuffer->setFence(fence);
        renderer->beginCommandBuffer();
        if (measureUpload) {
            uploadDataCallback(commandBuffer->getVkCommandBuffer());
        }
        for (int copyIdx = 0; copyIdx < numCopiesPerRun; copyIdx++) {
            renderer->dispatch(computeData, sgl::uiceil(numElements, 256u), 1, 1);
            if (copyIdx < numCopiesPerRun - 1) {
                renderer->insertBufferMemoryBarrier(
                        VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_WRITE_BIT,
                        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                        bufferDst);
            }
        }
        renderer->endCommandBuffer();
        renderer->submitToQueue();
        fence->wait();
        auto timeStop = std::chrono::high_resolution_clock::now();
        auto elapsedTimeRunNs = std::chrono::duration_cast<std::chrono::nanoseconds>(timeStop - timeStart);
        if (it != 0) {
            // First run is warmup.
            elapsedTimeMs += double(elapsedTimeRunNs.count()) * 1e-6 / double(numRuns * numCopiesPerRun);
        }
        fence->reset();

        // Check equality to expected values.
        auto commandBufferSingleTime = renderer->getDevice()->beginSingleTimeCommands();
        bufferDst->copyDataTo(stagingBuffer, commandBufferSingleTime);
        renderer->getDevice()->endSingleTimeCommands(commandBufferSingleTime);
        void* hostPtr = stagingBuffer->mapMemory();
        if (!checkIsArrayLinear(numElements, hostPtr, errorMessage)) {
            stagingBuffer->unmapMemory();
            sgl::Logfile::get()->throwError("Memory content mismatched.");
        }
        stagingBuffer->unmapMemory();
    }

    return elapsedTimeMs;
}

void runTestsVulkan(sgl::vk::Device* device) {
    auto* shaderManager = new sgl::vk::ShaderManagerVk(device);
    auto renderer = new sgl::vk::Renderer(device);

    const char* SHADER_STRING_COPY_BUFFER_COMPUTE_FMT = R"(
    #version 450 core
    layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;
    layout(binding = 0) uniform UniformBuffer {
        uint numElements;
    };
    layout(binding = 1, std430) restrict readonly buffer SrcBuffer {
        float srcBuffer[];
    };
    layout(binding = 2, std430) writeonly buffer DestBuffer {
        float destBuffer[];
    };
    void main() {
        uint idx = gl_GlobalInvocationID.x;
        if (idx < numElements) {
            destBuffer[idx] = srcBuffer[idx];
        }
    }
    )";
    auto shaderStages = shaderManager->compileComputeShaderFromStringCached(
            "CopyImageToBufferShader.Compute", SHADER_STRING_COPY_BUFFER_COMPUTE_FMT);

    const size_t sizeInBytes = numElements * sizeof(float);

    sgl::vk::BufferSettings bufferSettings{};
    bufferSettings.sizeInBytes = sizeInBytes;
    bufferSettings.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    auto uniformBuffer = std::make_shared<sgl::vk::Buffer>(device, bufferSettings);
    uniformBuffer->uploadData(sizeof(uint32_t), &numElements);
    bufferSettings.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    auto bufferDst = std::make_shared<sgl::vk::Buffer>(device, bufferSettings);
    bufferSettings.memoryUsage = VMA_MEMORY_USAGE_GPU_TO_CPU;
    bufferSettings.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    auto stagingBuffer = std::make_shared<sgl::vk::Buffer>(device, bufferSettings);
    sgl::vk::BufferPtr stagingBufferUpload;

    // Create command buffer.
    sgl::vk::CommandPoolType commandPoolType;
    commandPoolType.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    auto commandBuffer = std::make_shared<sgl::vk::CommandBuffer>(device, commandPoolType);
    auto fence = std::make_shared<sgl::vk::Fence>(device);

    sgl::vk::ComputePipelineInfo computePipelineInfo(shaderStages);
    sgl::vk::ComputePipelinePtr computePipeline = std::make_shared<sgl::vk::ComputePipeline>(
            device, computePipelineInfo);
    auto computeData = std::make_shared<sgl::vk::ComputeData>(renderer, computePipeline);
    computeData->setStaticBuffer(uniformBuffer, 0);
    computeData->setStaticBuffer(bufferDst, 2);

    bool testHostPointerImport = device->isDeviceExtensionSupported(VK_EXT_EXTERNAL_MEMORY_HOST_EXTENSION_NAME);
    sgl::vk::BufferPtr bufferSrcHost;
    float* bufferHost;
    if (testHostPointerImport) {
        bufferHost = static_cast<float*>(sgl::aligned_alloc(device->getMinImportedHostPointerAlignment(), sizeInBytes));
    } else {
        bufferHost = new float[numElements];
    }
    for (uint32_t i = 0; i < numElements; i++) {
        bufferHost[i] = static_cast<float>(i);
    }

    bufferSettings.sizeInBytes = sizeInBytes;
    bufferSettings.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    bufferSettings.memoryUsage = VMA_MEMORY_USAGE_UNKNOWN;
    const VkPhysicalDeviceMemoryProperties& memoryProperties = device->getMemoryProperties();

    std::vector<std::pair<VkMemoryPropertyFlagBits, std::string>> memPropFlagsPairs = {
        { VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, "device local" },
        { VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, "host visible" },
        { VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, "host coherent" },
        { VK_MEMORY_PROPERTY_HOST_CACHED_BIT, "host cached" },
        { VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT, "lazily allocated" },
        { VK_MEMORY_PROPERTY_PROTECTED_BIT, "protected" },
        { VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD, "device coherent" },
        { VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD, "device uncached" },
        { VK_MEMORY_PROPERTY_RDMA_CAPABLE_BIT_NV, "RDMA capable" },
    };
    std::cout << "Tested API: Vulkan" << std::endl;
    bool testHostPtrImportNext = false;
    for (int configIdx = 0; configIdx < NUM_CONFIGS; configIdx++) {
        int numCopiesPerRun = configsNumCopiesPerRun[configIdx];
        bool measureUpload = configsMeasureUpload[configIdx];
        std::cout << " #Accesses: " << numCopiesPerRun;
        if (!measureUpload) {
            std::cout << " (upload excluded)";
        }
        std::cout << std::endl;
        for (uint32_t memoryTypeIdx = 0; memoryTypeIdx < memoryProperties.memoryTypeCount; ) {
            const VkMemoryType& memoryType = memoryProperties.memoryTypes[memoryTypeIdx];
            std::string memoryTypeString;
            if (memoryType.propertyFlags != 0) {
                memoryTypeString = " (";
                auto numEntries = static_cast<int>(sgl::popcount(static_cast<uint32_t>(memoryType.propertyFlags)));
                int entryIdx = 0;
                for (const auto& memPropFlagsPair : memPropFlagsPairs) {
                    if ((memoryType.propertyFlags & memPropFlagsPair.first) != 0) {
                        memoryTypeString += memPropFlagsPair.second;
                        if (entryIdx != numEntries - 1) {
                            memoryTypeString += ", ";
                        }
                        entryIdx++;
                    }
                }
                if (testHostPtrImportNext) {
                    memoryTypeString += "; host pointer import";
                }
                memoryTypeString += ")";
            }

            //if ((memoryType.propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == 0) {
            //    memoryTypeIdx++;
            //    continue;
            //}
            uint32_t memoryTypeBits = 1u << memoryTypeIdx;
            VkMemoryRequirements memoryRequirements{};
            if (!device->getBufferSettingsMemoryRequirements(bufferSettings, memoryRequirements)
                    || (memoryRequirements.memoryTypeBits & memoryTypeBits) == 0) {
                memoryTypeIdx++;
                continue;
            }
            bufferSettings.useMemoryTypeBits = true;
            bufferSettings.memoryTypeBits = memoryTypeBits;
            auto bufferSrc = std::make_shared<sgl::vk::Buffer>(device, bufferSettings);
            computeData->setStaticBuffer(bufferSrc, 1);
            auto uploadDataCallback = [&](VkCommandBuffer commandBufferVk) {
                if (testHostPtrImportNext) {
                    bufferSrcHost = std::make_shared<sgl::vk::Buffer>(device);
                    bufferSrcHost->createFromHostPointer(bufferHost, sizeInBytes, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
                    bufferSrcHost->copyDataTo(bufferSrc, commandBufferVk);
                } else {
                    bufferSrc->uploadData(sizeInBytes, bufferHost, commandBufferVk, stagingBufferUpload);
                }
            };
            double elapsedTimeUs = runTestsVulkanIndividual(
                    numCopiesPerRun, measureUpload, renderer, fence, commandBuffer, computeData, numElements,
                    uploadDataCallback, bufferDst, stagingBuffer);
            std::cout << "  Time copy memory type " << memoryTypeIdx << memoryTypeString << ": " << elapsedTimeUs << "ms" << std::endl;

            if (testHostPtrImportNext) {
                testHostPtrImportNext = false;
                memoryTypeIdx++;
            } else if (testHostPointerImport && (memoryType.propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0
                    && (memoryType.propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == 0) {
                testHostPtrImportNext = true;
            } else {
                memoryTypeIdx++;
            }
        }
    }
    std::cout << std::endl;

    if (testHostPointerImport) {
        sgl::aligned_free(bufferHost);
    } else {
        delete[] bufferHost;
    }
    shaderStages.reset();
    computeData.reset();
    delete renderer;
    delete shaderManager;
}
