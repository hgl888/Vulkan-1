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
#include "Sampler.hpp"

namespace vkts
{

std::string VKTS_APIENTRY samplerGetName(const VkSamplerCreateInfo& samplerCreateInfo)
{
	// Currently, the following parameters are variable when creating a sampler. So, the name can be used as an identifier.

	return "SAMPLER_" + std::to_string((uint32_t)samplerCreateInfo.magFilter) + "_" + std::to_string((uint32_t)samplerCreateInfo.minFilter) + "_" + std::to_string((uint32_t)samplerCreateInfo.mipmapMode) + "_" + std::to_string((uint32_t)samplerCreateInfo.addressModeU) + "_" + std::to_string(samplerCreateInfo.maxLod);
}

ISamplerSP VKTS_APIENTRY samplerCreate(const VkDevice device, const VkSamplerCreateFlags flags, const VkFilter magFilter, const VkFilter minFilter, const VkSamplerMipmapMode mipmapMode, const VkSamplerAddressMode addressModeU, const VkSamplerAddressMode addressModeV, const VkSamplerAddressMode addressModeW, const float mipLodBias, const VkBool32 anisotropyEnable, const float maxAnisotropy, const VkBool32 compareEnable, const VkCompareOp compareOp, const float minLod, const float maxLod, const VkBorderColor borderColor, const VkBool32 unnormalizedCoordinates)
{
    if (!device)
    {
        return ISamplerSP();
    }

    VkResult result;

    VkSamplerCreateInfo samplerCreateInfo{};

    samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

    samplerCreateInfo.flags = flags;
    samplerCreateInfo.magFilter = magFilter;
    samplerCreateInfo.minFilter = minFilter;
    samplerCreateInfo.mipmapMode = mipmapMode;
    samplerCreateInfo.addressModeU = addressModeU;
    samplerCreateInfo.addressModeV = addressModeV;
    samplerCreateInfo.addressModeW = addressModeW;
    samplerCreateInfo.mipLodBias = mipLodBias;
    samplerCreateInfo.anisotropyEnable = anisotropyEnable;
    samplerCreateInfo.maxAnisotropy = maxAnisotropy;
    samplerCreateInfo.compareEnable = compareEnable;
    samplerCreateInfo.compareOp = compareOp;
    samplerCreateInfo.minLod = minLod;
    samplerCreateInfo.maxLod = maxLod;
    samplerCreateInfo.borderColor = borderColor;
    samplerCreateInfo.unnormalizedCoordinates = unnormalizedCoordinates;

    VkSampler sampler;

    result = vkCreateSampler(device, &samplerCreateInfo, nullptr, &sampler);

    if (result != VK_SUCCESS)
    {
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create sampler.");

        return ISamplerSP();
    }

    auto newInstance = new Sampler(device, flags, magFilter, minFilter, mipmapMode, addressModeU, addressModeV, addressModeW, mipLodBias, anisotropyEnable, maxAnisotropy, compareEnable, compareOp, minLod, maxLod, borderColor, unnormalizedCoordinates, sampler);

    if (!newInstance)
    {
        vkDestroySampler(device, sampler, nullptr);

        return ISamplerSP();
    }

    return ISamplerSP(newInstance);
}

ISamplerSP VKTS_APIENTRY samplerCreate(const VkDevice device, const VkSamplerCreateInfo& samplerCreateInfo)
{
	return samplerCreate(device, samplerCreateInfo.flags, samplerCreateInfo.magFilter, samplerCreateInfo.minFilter, samplerCreateInfo.mipmapMode, samplerCreateInfo.addressModeU, samplerCreateInfo.addressModeV, samplerCreateInfo.addressModeW, samplerCreateInfo.mipLodBias, samplerCreateInfo.anisotropyEnable, samplerCreateInfo.maxAnisotropy, samplerCreateInfo.compareEnable, samplerCreateInfo.compareOp, samplerCreateInfo.minLod, samplerCreateInfo.maxLod, samplerCreateInfo.borderColor, samplerCreateInfo.unnormalizedCoordinates);
}

}
