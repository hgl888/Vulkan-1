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

#include <vkts/vulkan/wrapper/vkts_wrapper.hpp>
#include "Fence.hpp"

namespace vkts
{

IFenceSP VKTS_APIENTRY fenceCreate(const VkDevice device, const VkFenceCreateFlags flags)
{
    if (!device)
    {
        return IFenceSP();
    }

    VkResult result;

    VkFenceCreateInfo fenceCreateInfo{};

    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    fenceCreateInfo.flags = flags;

    VkFence fence;

    result = vkCreateFence(device, &fenceCreateInfo, nullptr, &fence);

    if (result != VK_SUCCESS)
    {
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create fence.");

        return IFenceSP();
    }

    auto newInstance = new Fence(device, flags, fence);

    if (!newInstance)
    {
        vkDestroyFence(device, fence, nullptr);

        return IFenceSP();
    }

    return IFenceSP(newInstance);
}

VkResult VKTS_APIENTRY fenceWaitFor(const VkDevice device, const std::vector<VkFence>& fences, const VkBool32 waitAll, const uint64_t timeout)
{
    if (!device || fences.size() == 0)
    {
        return VK_NOT_READY;
    }

    return vkWaitForFences(device, (uint32_t) fences.size(), &fences[0], waitAll, timeout);
}

}
