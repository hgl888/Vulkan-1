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
#include "Framebuffer.hpp"

namespace vkts
{

IFramebufferSP VKTS_APIENTRY
framebufferCreate(const VkDevice device, const VkFramebufferCreateFlags flags, const VkRenderPass renderPass, const uint32_t attachmentCount, const VkImageView* attachments, const uint32_t width, const uint32_t height, const uint32_t layers)
{
    if (!device)
    {
        return IFramebufferSP();
    }

    VkResult result;

    VkFramebufferCreateInfo framebufferCreateInfo{};

    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;

    framebufferCreateInfo.flags = flags;
    framebufferCreateInfo.renderPass = renderPass;
    framebufferCreateInfo.attachmentCount = attachmentCount;
    framebufferCreateInfo.pAttachments = attachments;
    framebufferCreateInfo.width = width;
    framebufferCreateInfo.height = height;
    framebufferCreateInfo.layers = layers;

    VkFramebuffer framebuffer;

    result = vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &framebuffer);

    if (result != VK_SUCCESS)
    {
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create frame buffer.");

        return IFramebufferSP();
    }

    auto newInstance = new Framebuffer(device, flags, renderPass, attachmentCount, attachments, width, height, layers, framebuffer);

    if (!newInstance)
    {
        vkDestroyFramebuffer(device, framebuffer, nullptr);

        return IFramebufferSP();
    }

    return IFramebufferSP(newInstance);
}

}
