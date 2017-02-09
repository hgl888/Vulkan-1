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

#ifndef VKTS_FN_IMAGE_DATA_INTERNAL_HPP_
#define VKTS_FN_IMAGE_DATA_INTERNAL_HPP_

#include <vkts/image/vkts_image.hpp>

namespace vkts
{

VKTS_APICALL glm::vec3 VKTS_APIENTRY imageDataGetScanVector(const uint32_t x, const uint32_t y, const uint32_t side, const float step, const float offset);

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL IImageDataSP VKTS_APIENTRY imageDataLoadGli(const std::string& name, const IBinaryBufferSP& buffer);

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL VkBool32 VKTS_APIENTRY imageDataSaveGli(const std::string& name, const IImageDataSP& imageData, const uint32_t mipLevel, const uint32_t arrayLayer);

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL IImageDataSP VKTS_APIENTRY imageDataLoadStb(const std::string& name, const IBinaryBufferSP& buffer);

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL VkBool32 VKTS_APIENTRY imageDataSaveStb(const std::string& name, const IImageDataSP& imageData, const uint32_t mipLevel, const uint32_t arrayLayer);

}

#endif /* VKTS_FN_IMAGE_DATA_INTERNAL_HPP_ */
