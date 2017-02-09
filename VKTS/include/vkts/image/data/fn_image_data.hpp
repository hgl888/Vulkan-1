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

#ifndef VKTS_FN_IMAGE_DATA_HPP_
#define VKTS_FN_IMAGE_DATA_HPP_

#include <vkts/image/vkts_image.hpp>

namespace vkts
{

typedef IImageDataSP (VKTS_APIENTRY *PFN_imageDataLoadFunction)(const char* filename);
typedef VkBool32 (VKTS_APIENTRY *PFN_imageDataSaveFunction)(const char* filename, const IImageDataSP& imageData, const uint32_t mipLevel, const uint32_t arrayLayer);

/**
 *
 *
 * @ThreadSafe
 */
VKTS_APICALL VkBool32 VKTS_APIENTRY imageDataIsDepthStencil(const VkFormat format);

/**
 * Only supported BLOCKs are returned as true.
 *
 * @ThreadSafe
 */
VKTS_APICALL VkBool32 VKTS_APIENTRY imageDataIsBLOCK(const VkFormat format);

/**
 * Only supported UNORMs and non compressed formats are returned as true.
 *
 * @ThreadSafe
 */
VKTS_APICALL VkBool32 VKTS_APIENTRY imageDataIsUNORM(const VkFormat format);

/**
 * Only supported SFLOATs and non compressed formats are returned as true.
 *
 * @ThreadSafe
 */
VKTS_APICALL VkBool32 VKTS_APIENTRY imageDataIsSFLOAT(const VkFormat format);

/**
 * Only supported SRGBs and non compressed formats are returned as true.
 *
 * @ThreadSafe
 */
VKTS_APICALL VkBool32 VKTS_APIENTRY imageDataIsSRGB(const VkFormat format);

/**
 * Only for supported formats the bytes per texel is returned.
 *
 * @ThreadSafe
 */
VKTS_APICALL uint32_t VKTS_APIENTRY imageDataGetBytesPerTexel(const VkFormat format);

/**
 * Only for supported and non compressed formats the bytes per channel is returned.
 *
 * @ThreadSafe
 */
VKTS_APICALL uint32_t VKTS_APIENTRY imageDataGetBytesPerChannel(const VkFormat format);

/**
 * Only for supported and non compressed formats the number of channels is returned.
 *
 * @ThreadSafe
 */
VKTS_APICALL uint32_t VKTS_APIENTRY imageDataGetNumberChannels(const VkFormat format);

/**
 * Only for supported and non compressed formats the number of channels is returned.
 *
 * @ThreadSafe
 */
VKTS_APICALL std::string VKTS_APIENTRY imageDataGetColorName(const VkFormat format, const glm::vec4& color);

//

VKTS_APICALL void VKTS_APIENTRY imageDataSetLoadFunction(const PFN_imageDataLoadFunction loadFunction, const VkBool32 fallback = VK_TRUE);

VKTS_APICALL void VKTS_APIENTRY imageDataSetSaveFunction(const PFN_imageDataSaveFunction saveFunction, const VkBool32 fallback = VK_TRUE);

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL IImageDataSP VKTS_APIENTRY imageDataLoad(const char* filename);

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL IImageDataSP VKTS_APIENTRY imageDataLoadRaw(const char* filename, const uint32_t width, const uint32_t height, const VkFormat format);

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL VkBool32 VKTS_APIENTRY imageDataSave(const char* filename, const IImageDataSP& imageData, const uint32_t mipLevel = 0, const uint32_t arrayLayer = 0);

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL IImageDataSP VKTS_APIENTRY imageDataCreate(const std::string& name, const uint32_t width, const uint32_t height, const uint32_t depth, const VkImageType imageType, const VkFormat& format);

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL IImageDataSP VKTS_APIENTRY imageDataCreate(const std::string& name, const uint32_t width, const uint32_t height, const uint32_t depth, const glm::vec4& color, const VkImageType imageType, const VkFormat& format);

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL IImageDataSP VKTS_APIENTRY imageDataCreate(const std::string& name, const uint32_t width, const uint32_t height, const uint32_t depth, const float red, const float green, const float blue, const float alpha, const VkImageType imageType, const VkFormat& format);

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL IImageDataSP VKTS_APIENTRY imageDataConvert(const IImageDataSP& sourceImage, const VkFormat targetFormat, const std::string& name, const enum VkTsImageDataType targetImageDataType = VKTS_NON_COLOR_DATA, const enum VkTsImageDataType sourceImageDataType = VKTS_NON_COLOR_DATA, const glm::vec4& factor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), const std::array<VkBool32, 3>& mirror = {VK_FALSE, VK_FALSE, VK_FALSE});

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL IImageDataSP VKTS_APIENTRY imageDataCopy(const IImageDataSP& sourceImage, const std::string& name);

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL IImageDataSP VKTS_APIENTRY imageDataMerge(const SmartPointerVector<IImageDataSP>& sourceImages, const std::string& name, const uint32_t mipLevels, const uint32_t arrayLayers);

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL SmartPointerVector<IImageDataSP> VKTS_APIENTRY imageDataMipmap(const IImageDataSP& sourceImage, const VkBool32 addSourceAsCopy, const std::string& name);

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL SmartPointerVector<IImageDataSP> VKTS_APIENTRY imageDataCubemap(const IImageDataSP& sourceImage, const uint32_t length, const std::string& name);

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL SmartPointerVector<IImageDataSP> VKTS_APIENTRY imageDataPrefilterCookTorrance(const IImageDataSP& sourceImage, const uint32_t samples, const std::string& name);

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL SmartPointerVector<IImageDataSP> VKTS_APIENTRY imageDataPrefilterOrenNayar(const IImageDataSP& sourceImage, const uint32_t samples, const std::string& name);

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL SmartPointerVector<IImageDataSP> VKTS_APIENTRY imageDataPrefilterLambert(const IImageDataSP& sourceImage, const uint32_t samples, const std::string& name);

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL IImageDataSP VKTS_APIENTRY imageDataEnvironmentBRDF(const uint32_t length, const uint32_t samples, const std::string& name);

}

#endif /* VKTS_FN_IMAGE_DATA_HPP_ */
