/**
 * VKTS - VulKan ToolS.
 *
 * The MIT License (MIT)
 *
 * Copyright (c) since 2014 Norbert Nopper
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "PipelineCache.hpp"

namespace vkts
{

PipelineCache::PipelineCache(const VkDevice device, const VkPipelineCacheCreateFlags flags, const uint32_t initialDataSize, const void* initialData, const VkPipelineCache pipelineCache) :
    IPipelineCache(), device(device), pipelineCacheCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO, nullptr, flags, initialDataSize, initialData}, initialData(), pipelineCache(pipelineCache)
{
    if (initialDataSize > 0)
    {
        this->initialData = binaryBufferCreate((const uint8_t*) initialData, initialDataSize);

        if (this->initialData.get())
        {
            pipelineCacheCreateInfo.pInitialData = this->initialData->getData();
        }
    }
}

PipelineCache::~PipelineCache()
{
    destroy();
}

//
// IPipelineCache
//

const VkDevice PipelineCache::getDevice() const
{
    return device;
}

const VkPipelineCacheCreateInfo& PipelineCache::getPipelineCacheCreateInfo() const
{
    return pipelineCacheCreateInfo;
}

VkPipelineCacheCreateFlags PipelineCache::getFlags() const
{
    return pipelineCacheCreateInfo.flags;
}

size_t PipelineCache::getInitialDataSize() const
{
    return pipelineCacheCreateInfo.initialDataSize;
}

const void* PipelineCache::getInitialData() const
{
    return pipelineCacheCreateInfo.pInitialData;
}

const VkPipelineCache PipelineCache::getPipelineCache() const
{
    return pipelineCache;
}

//
// IDestroyable
//

void PipelineCache::destroy()
{
    if (pipelineCache)
    {
        vkDestroyPipelineCache(device, pipelineCache, nullptr);

        pipelineCache = VK_NULL_HANDLE;
    }
}

} /* namespace vkts */
