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

#ifndef VKTS_IIMAGEDATA_HPP_
#define VKTS_IIMAGEDATA_HPP_

#include <vkts/image/vkts_image.hpp>

namespace vkts
{

class IImageData
{

public:

    IImageData()
    {
    }

    virtual ~IImageData()
    {
    }

    virtual const std::string& getName() const = 0;

    virtual VkImageType getImageType() const = 0;

    virtual const VkFormat& getFormat() const = 0;

    virtual const VkExtent3D& getExtent3D() const = 0;

    virtual uint32_t getWidth() const = 0;

    virtual uint32_t getHeight() const = 0;

    virtual uint32_t getDepth() const = 0;

    virtual uint32_t getMipLevels() const = 0;

    virtual uint32_t getArrayLayers() const = 0;

    virtual const void* getData() const = 0;

    virtual const uint8_t* getByteData() const = 0;

    virtual uint32_t getSize() const = 0;

    /**
     * Copy image data pixels into the given data pointer.
     */
    virtual VkBool32 copy(void* data, const uint32_t mipLevel, const uint32_t arrayLayer, const VkSubresourceLayout& subresourceLayout) const = 0;

    /**
     * Uploads pixel data into image data.
     */
    virtual VkBool32 upload(const void* data, const uint32_t mipLevel, const uint32_t arrayLayer, const VkSubresourceLayout& subresourceLayout) const = 0;

    virtual VkBool32 isBLOCK() const = 0;

    virtual VkBool32 isUNORM() const = 0;

    virtual VkBool32 isSFLOAT() const = 0;

    virtual VkBool32 isSRGB() const = 0;

    virtual uint32_t getBytesPerTexel() const = 0;

    virtual uint32_t getBytesPerChannel() const = 0;

    virtual uint32_t getNumberChannels() const = 0;

    virtual const std::vector<uint32_t>& getAllOffsets() const = 0;

    virtual void setTexel(const glm::vec4& rgba, const uint32_t x, const uint32_t y, const uint32_t z, const uint32_t mipLevel, const uint32_t arrayLayer) = 0;

    virtual glm::vec4 getTexel(const uint32_t x, const uint32_t y, const uint32_t z, const uint32_t mipLevel, const uint32_t arrayLayer) const = 0;

    virtual glm::vec4 getSample(const float x, const VkFilter filterX, const VkSamplerAddressMode addressModeX, const float y, const VkFilter filterY, const VkSamplerAddressMode addressModeY, const float z, const VkFilter filterZ, const VkSamplerAddressMode addressModeZ, const uint32_t mipLevel, const uint32_t arrayLayer) const = 0;

    virtual glm::vec4 getSampleCubeMap(const float x, const float y, const float z, const VkFilter filter, const uint32_t mipLevel) const = 0;

    virtual VkBool32 getExtentAndOffset(VkExtent3D& currentExtent, uint32_t& currentOffset, const uint32_t mipLevel, const uint32_t arrayLayer) const = 0;

    virtual void freeHostMemory() = 0;

    virtual VkBool32 updateMaxLuminance() = 0;

    virtual float getMaxLuminance() const = 0;

};

typedef std::shared_ptr<IImageData> IImageDataSP;

} /* namespace vkts */

#endif /* VKTS_IIMAGEDATA_HPP_ */
