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

#include "ImageData.hpp"

namespace vkts
{

void ImageData::reset()
{
    name = "";

    imageType = VK_IMAGE_TYPE_1D;

    format = VK_FORMAT_UNDEFINED;

    extent = { 0, 0, 0};

    mipLevels = 0;
    arrayLayers = 0;

    if (buffer.get())
    {
        buffer->reset();
    }

    BLOCK = VK_FALSE;
    UNORM = VK_FALSE;
    SFLOAT = VK_FALSE;
    SRGB = VK_FALSE;
    bytesPerChannel = 0;
    numberChannels = 0;

    allOffsets.clear();

    maxLuminance = 1.0f;
}

int32_t ImageData::getTexelLocation(float& fraction, const float a, const int32_t size, const VkSamplerAddressMode addressMode) const
{
	float unnormalizedA = a * (float)size;

	//

	fraction = fabsf(unnormalizedA - floorf(unnormalizedA));

	//

	int32_t coord = (int32_t)unnormalizedA;

	if (addressMode == VK_SAMPLER_ADDRESS_MODE_REPEAT)
	{
		coord = coord % size;

		//

		if (coord < 0)
		{
			coord += size;
		}
	}
	else if	(addressMode == VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT)
	{
		coord = (coord % (2 * size)) - size;

		if (coord < 0)
		{
			coord = -(1 + coord);
		}

		coord = (size - 1) - coord;

		//

		if (coord < 0)
		{
			coord = -coord;
		}
	}
	else if	(addressMode == VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE)
	{
		coord = glm::clamp(coord, 0, size - 1);
	}
	else if	(addressMode == VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER)
	{
		coord = glm::clamp(coord, -1, size);
	}
	else if	(addressMode == VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE)
	{
		if (coord < 0)
		{
			coord = -(1 + coord);
		}

		coord = glm::clamp(coord, 0, size - 1);
	}

	return coord;
}

int32_t ImageData::getCubeMapFace(float& s, float& t, const float x, const float y, const float z) const
{
	int32_t faceLayer = 0;

	float sc = 0.0f;
	float tc = 0.0f;
	float rc = 0.0f;

	float absX = fabsf(x);
	float absY = fabsf(y);
	float absZ = fabsf(z);

	if (absX > absY && absX > absZ)
	{
		if (x > 0.0f)
		{
			faceLayer = 0;

			sc = -z;
			tc = -y;
			rc = x;
		}
		else
		{
			faceLayer = 1;

			sc = z;
			tc = -y;
			rc = x;
		}
	}
	else if (absY >= absX && absY > absZ)
	{
		if (y > 0.0f)
		{
			faceLayer = 2;

			sc = x;
			tc = z;
			rc = y;
		}
		else
		{
			faceLayer = 3;

			sc = x;
			tc = -z;
			rc = y;
		}
	}
	else if (absZ >= absX && absZ >= absY)
	{
		if (z > 0.0f)
		{
			faceLayer = 4;

			sc = x;
			tc = -y;
			rc = z;
		}
		else
		{
			faceLayer = 5;

			sc = -x;
			tc = -y;
			rc = z;
		}
	}
	else
	{
		return -1;
	}

	s = 0.5f * sc / fabsf(rc) + 0.5f;
	t = 0.5f * tc / fabsf(rc) + 0.5f;

	return faceLayer;
}

ImageData::ImageData(const std::string& name, const VkImageType imageType, const VkFormat& format, const VkExtent3D& extent, const uint32_t mipLevels, const uint32_t arrayLayers, const std::vector<uint32_t>& allOffsets, const uint8_t* data, const uint32_t size, const float maxLuminance) :
    IImageData(), name(name), imageType(imageType), format(format), extent(extent), mipLevels(mipLevels), arrayLayers(arrayLayers), allOffsets(allOffsets), maxLuminance(maxLuminance)
{
    buffer = binaryBufferCreate(data, size);

    if (!buffer.get() || buffer->getSize() != size)
    {
        reset();

        return;
    }

    BLOCK = imageDataIsBLOCK(format);
    UNORM = imageDataIsUNORM(format);
    SFLOAT = imageDataIsSFLOAT(format);
    SRGB = imageDataIsSRGB(format);
    bytesPerChannel = imageDataGetBytesPerChannel(format);
    numberChannels = imageDataGetNumberChannels(format);
}

ImageData::ImageData(const std::string& name, const VkImageType imageType, const VkFormat& format, const VkExtent3D& extent, const uint32_t mipLevels, const uint32_t arrayLayers, const std::vector<uint32_t>& allOffsets, const IBinaryBufferSP& buffer, const float maxLuminance) :
    IImageData(), name(name), imageType(imageType), format(format), extent(extent), mipLevels(mipLevels), arrayLayers(arrayLayers), buffer(buffer), allOffsets(allOffsets), maxLuminance(maxLuminance)
{
    if (!this->buffer.get() || !this->buffer->getData())
    {
        reset();

        return;
    }

    BLOCK = imageDataIsBLOCK(format);
    UNORM = imageDataIsUNORM(format);
    SFLOAT = imageDataIsSFLOAT(format);
    SRGB = imageDataIsSRGB(format);
    bytesPerChannel = imageDataGetBytesPerChannel(format);
    numberChannels = imageDataGetNumberChannels(format);
}

ImageData::~ImageData()
{
    reset();
}

//
// IImageData
//

const std::string& ImageData::getName() const
{
    return name;
}

VkImageType ImageData::getImageType() const
{
    return imageType;
}

const VkFormat& ImageData::getFormat() const
{
    return format;
}

const VkExtent3D& ImageData::getExtent3D() const
{
    return extent;
}

uint32_t ImageData::getWidth() const
{
    return extent.width;
}

uint32_t ImageData::getHeight() const
{
    return extent.height;
}

uint32_t ImageData::getDepth() const
{
    return extent.depth;
}

uint32_t ImageData::getMipLevels() const
{
    return mipLevels;
}

uint32_t ImageData::getArrayLayers() const
{
    return arrayLayers;
}

const void* ImageData::getData() const
{
    if (buffer.get())
    {
        return buffer->getData();
    }

    return nullptr;
}

const uint8_t* ImageData::getByteData() const
{
    return (const uint8_t*)getData();
}

uint32_t ImageData::getSize() const
{
    if (buffer.get())
    {
        return buffer->getSize();
    }

    return 0;
}

VkBool32 ImageData::copy(void* data, const uint32_t mipLevel, const uint32_t arrayLayer, const VkSubresourceLayout& subresourceLayout) const
{
    if (!data || mipLevel >= mipLevels || arrayLayer >= arrayLayers || !getData())
    {
        return VK_FALSE;
    }

    //

	VkExtent3D currentExtent;
	uint32_t offset;

    getExtentAndOffset(currentExtent, offset, mipLevel, arrayLayer);

    //

    const uint8_t* currentSourceBuffer = &(static_cast<const uint8_t*>(buffer->getData())[offset]);

    uint8_t* currentTargetBuffer = static_cast<uint8_t*>(data);

    //

    if (BLOCK)
    {
    	uint32_t nextOffset;

    	if (!getExtentAndOffset(currentExtent, nextOffset, mipLevel + 1, arrayLayer))
    	{
        	if (!getExtentAndOffset(currentExtent, nextOffset, mipLevel, arrayLayer + 1))
        	{
        		nextOffset = getSize();
        	}
    	}

    	memcpy(&currentTargetBuffer[subresourceLayout.offset], &currentSourceBuffer[offset], nextOffset - offset);

    	return VK_TRUE;
    }

    if (bytesPerChannel == 0 || numberChannels == 0)
    {
        return VK_FALSE;
    }

    //

    const uint8_t* currentSourceChannel = nullptr;

    uint8_t* currentTargetChannel = nullptr;

    for (uint32_t z = 0; z < currentExtent.depth; z++)
    {
        for (uint32_t y = 0; y < currentExtent.height; y++)
        {
            currentSourceChannel = &currentSourceBuffer[y * currentExtent.width * numberChannels * bytesPerChannel + z * currentExtent.height * currentExtent.width * numberChannels * bytesPerChannel];

            // Do not calculate in array layer or mip level, as already accumulated in offset.
            currentTargetChannel = &currentTargetBuffer[y * subresourceLayout.rowPitch + z * subresourceLayout.depthPitch + subresourceLayout.offset];

            memcpy(currentTargetChannel, currentSourceChannel, numberChannels * bytesPerChannel * currentExtent.width);
        }
    }

    return VK_TRUE;
}

VkBool32 ImageData::upload(const void* data, const uint32_t mipLevel, const uint32_t arrayLayer, const VkSubresourceLayout& subresourceLayout) const
{
    if (!data || mipLevel >= mipLevels || arrayLayer >= arrayLayers || !getData())
    {
        return VK_FALSE;
    }

	VkExtent3D currentExtent;
	uint32_t offset;

    getExtentAndOffset(currentExtent, offset, mipLevel, arrayLayer);

    //

    if (subresourceLayout.size > (VkDeviceSize)(buffer->getSize() - offset))
    {
        return VK_FALSE;
    }

    const uint8_t* currentSourceBuffer = static_cast<const uint8_t*>(data);

    if (!buffer->seek(offset, VKTS_SEARCH_ABSOLUTE))
    {
        return VK_FALSE;
    }

    //

    if (BLOCK)
    {
    	uint32_t nextOffset;

    	if (!getExtentAndOffset(currentExtent, nextOffset, mipLevel + 1, arrayLayer))
    	{
        	if (!getExtentAndOffset(currentExtent, nextOffset, mipLevel, arrayLayer + 1))
        	{
        		nextOffset = getSize();
        	}
    	}

    	buffer->write(&currentSourceBuffer[subresourceLayout.offset], 1, nextOffset - offset);

		return VK_TRUE;
    }

    //

    if (bytesPerChannel == 0 || numberChannels == 0)
    {
        return VK_FALSE;
    }

    const uint8_t* currentSourceChannel = nullptr;

    for (uint32_t z = 0; z < currentExtent.depth; z++)
    {
        for (uint32_t y = 0; y < currentExtent.height; y++)
        {
            currentSourceChannel = &currentSourceBuffer[y * subresourceLayout.rowPitch + z * subresourceLayout.depthPitch + subresourceLayout.offset];

            buffer->write(currentSourceChannel, 1, numberChannels * bytesPerChannel * currentExtent.width);
        }
    }

    return VK_TRUE;
}

VkBool32 ImageData::isBLOCK() const
{
    return BLOCK;
}

VkBool32 ImageData::isUNORM() const
{
    return UNORM;
}

VkBool32 ImageData::isSFLOAT() const
{
    return SFLOAT;
}

VkBool32 ImageData::isSRGB() const
{
    return SRGB;
}

uint32_t ImageData::getBytesPerTexel() const
{
	return imageDataGetBytesPerTexel(format);
}

uint32_t ImageData::getBytesPerChannel() const
{
    return bytesPerChannel;
}

uint32_t ImageData::getNumberChannels() const
{
    return numberChannels;
}

const std::vector<uint32_t>& ImageData::getAllOffsets() const
{
    return allOffsets;
}

void ImageData::setTexel(const glm::vec4& rgba, const uint32_t x, const uint32_t y, const uint32_t z, const uint32_t mipLevel, const uint32_t arrayLayer)
{
    if (x >= extent.width || y >= extent.height || z >= extent.depth || mipLevel >= mipLevels || arrayLayer >= arrayLayers || BLOCK || !getData())
    {
        return;
    }

	VkExtent3D currentExtent;
	uint32_t offset;

    if (!getExtentAndOffset(currentExtent, offset, mipLevel, arrayLayer))
    {
    	return;
    }

    //

    if (!buffer->seek((int64_t)offset + (int64_t)numberChannels * (int64_t)bytesPerChannel * (int64_t)x + (int64_t)numberChannels * (int64_t)bytesPerChannel * (int64_t)y * (int64_t)extent.width + (int64_t)numberChannels * (int64_t)bytesPerChannel * (int64_t)z * (int64_t)extent.width * (int64_t)extent.height, VKTS_SEARCH_ABSOLUTE))
    {
    	return;
    }

    if (UNORM)
    {
        uint8_t texelStart[4];

        for (uint32_t currentChannelIndex = 0; currentChannelIndex < numberChannels; currentChannelIndex++)
        {
        	uint32_t currentTargetChannelIndex = currentChannelIndex;

            if (format == VK_FORMAT_B8G8R8_UNORM || format == VK_FORMAT_B8G8R8A8_UNORM || format == VK_FORMAT_B8G8R8_SRGB || format == VK_FORMAT_B8G8R8A8_SRGB)
            {
                if (currentChannelIndex == 0)
                {
                    currentTargetChannelIndex = 2;
                }
                else if (currentChannelIndex == 2)
                {
                    currentTargetChannelIndex = 0;
                }
            }

            texelStart[currentTargetChannelIndex] = (uint8_t)(rgba[currentChannelIndex] * 255.0f);
        }

        buffer->write(texelStart, 1, numberChannels);
    }
    else if (SFLOAT)
    {
        float texelStart[4];

        for (uint32_t currentChannelIndex = 0; currentChannelIndex < numberChannels; currentChannelIndex++)
        {
            texelStart[currentChannelIndex] = rgba[currentChannelIndex];
        }

        buffer->write(texelStart, sizeof(float), numberChannels);
    }
}

glm::vec4 ImageData::getTexel(const uint32_t x, const uint32_t y, const uint32_t z, const uint32_t mipLevel, const uint32_t arrayLayer) const
{
    if (x >= extent.width || y >= extent.height || z >= extent.depth || mipLevel >= mipLevels || arrayLayer >= arrayLayers || BLOCK || !getData())
    {
        return glm::vec4(NAN, NAN, NAN, NAN);
    }

	VkExtent3D currentExtent{0, 0, 0};
	uint32_t offset = 0;

    getExtentAndOffset(currentExtent, offset, mipLevel, arrayLayer);

    //

    if (!buffer->seek((int64_t)offset + (int64_t)numberChannels * (int64_t)bytesPerChannel * (int64_t)x + (int64_t)numberChannels * (int64_t)bytesPerChannel * (int64_t)y * (int64_t)extent.width + (int64_t)numberChannels * (int64_t)bytesPerChannel * (int64_t)z * (int64_t)extent.width * (int64_t)extent.height, VKTS_SEARCH_ABSOLUTE))
    {
    	 return glm::vec4(NAN, NAN, NAN, NAN);
    }

    glm::vec4 result(0.0f, 0.0f, 0.0f, 1.0f);

    if (UNORM)
    {
        uint8_t texelStart[4];

        buffer->read(texelStart, 1, numberChannels);

        for (uint32_t currentChannelIndex = 0; currentChannelIndex < numberChannels; currentChannelIndex++)
        {
        	uint32_t currentTargetChannelIndex = currentChannelIndex;

            if (format == VK_FORMAT_B8G8R8_UNORM || format == VK_FORMAT_B8G8R8A8_UNORM || format == VK_FORMAT_B8G8R8_SRGB || format == VK_FORMAT_B8G8R8A8_SRGB)
            {
                if (currentChannelIndex == 0)
                {
                    currentTargetChannelIndex = 2;
                }
                else if (currentChannelIndex == 2)
                {
                    currentTargetChannelIndex = 0;
                }
            }

            result[currentTargetChannelIndex] = (float)(texelStart[currentChannelIndex]) / 255.0f;
        }
    }
    else if (SFLOAT)
    {
        float texelStart[4];

        buffer->read(texelStart, sizeof(float), numberChannels);

        for (uint32_t currentChannelIndex = 0; currentChannelIndex < numberChannels; currentChannelIndex++)
        {
            result[currentChannelIndex] = texelStart[currentChannelIndex];
        }
    }

    return result;
}

glm::vec4 ImageData::getSample(const float x, const VkFilter filterX, const VkSamplerAddressMode addressModeX, const float y, const VkFilter filterY, const VkSamplerAddressMode addressModeY, const float z, const VkFilter filterZ, const VkSamplerAddressMode addressModeZ, const uint32_t mipLevel, const uint32_t arrayLayer) const
{
	if (BLOCK)
	{
		return glm::vec4(NAN, NAN, NAN, NAN);
	}

	glm::vec4 result(0.0f, 0.0f, 0.0f, 0.0f);

	//

	float dummy;

	float fractionX;
	float fractionY;
	float fractionZ;

	int32_t texelX = getTexelLocation(fractionX, x, (int32_t)extent.width, addressModeX);
	int32_t texelY = getTexelLocation(fractionY, y, (int32_t)extent.height, addressModeY);
	int32_t texelZ = getTexelLocation(fractionZ, z, (int32_t)extent.depth, addressModeZ);

	//

	float stepX = 1.0f / (float)extent.width;
	float stepY = 1.0f / (float)extent.height;
	float stepZ = 1.0f / (float)extent.depth;

	//

	float weightX = 1.0f - (fabsf(0.5f - fractionX) * 2.0f);
	float weightY = 1.0f - (fabsf(0.5f - fractionY) * 2.0f);
	float weightZ = 1.0f - (fabsf(0.5f - fractionZ) * 2.0f);

	//

	for (uint32_t currentZ = 0; currentZ < (uint32_t)filterZ + 1; currentZ++)
	{
		float currentWeightZ = 1.0f;

		//

		if (filterZ)
		{
			if (currentZ == 0)
			{
				currentWeightZ = weightZ;

				texelZ = getTexelLocation(dummy, z, (int32_t)extent.depth, addressModeZ);
			}
			else
			{
				currentWeightZ = (1.0f - weightZ);

				if (fractionZ >= 0.5f)
				{
					texelZ = getTexelLocation(dummy, z + stepZ, (int32_t)extent.depth, addressModeZ);
				}
				else
				{
					texelZ = getTexelLocation(dummy, z - stepZ, (int32_t)extent.depth, addressModeZ);
				}
			}
		}

		//

		for (uint32_t currentY = 0; currentY < (uint32_t)filterY + 1; currentY++)
		{
			float currentWeightY = 1.0f;

			//

			if (filterY)
			{
				if (currentY == 0)
				{
					currentWeightY = weightY;

					texelY = getTexelLocation(dummy, y, (int32_t)extent.height, addressModeY);
				}
				else
				{
					currentWeightY = (1.0f - weightY);

					if (fractionY >= 0.5f)
					{
						texelY = getTexelLocation(dummy, y + stepY, (int32_t)extent.height, addressModeY);
					}
					else
					{
						texelY = getTexelLocation(dummy, y - stepY, (int32_t)extent.height, addressModeY);
					}
				}
			}

			//

			for (uint32_t currentX = 0; currentX < (uint32_t)filterX + 1; currentX++)
			{
				float currentWeightX = 1.0f;

				//

				if (filterX)
				{
					if (currentX == 0)
					{
						currentWeightX = weightX;

						texelX = getTexelLocation(dummy, x, (int32_t)extent.width, addressModeX);
					}
					else
					{
						currentWeightX = (1.0f - weightX);

						if (fractionX >= 0.5f)
						{
							texelX = getTexelLocation(dummy, x + stepX, (int32_t)extent.width, addressModeX);
						}
						else
						{
							texelX = getTexelLocation(dummy, x - stepX, (int32_t)extent.width, addressModeX);
						}
					}
				}

				//

				result += getTexel((uint32_t)texelX, (uint32_t)texelY, (uint32_t)texelZ, mipLevel, arrayLayer) * currentWeightX * currentWeightY * currentWeightZ;
			}
		}
	}

	return result;
}

glm::vec4 ImageData::getSampleCubeMap(const float x, const float y, const float z, const VkFilter filter, const uint32_t mipLevel) const
{
	if (arrayLayers != 6 || extent.depth != 1 || std::isnan(x) || std::isnan(y) || std::isnan(z) || BLOCK)
	{
		return glm::vec4(NAN, NAN, NAN, NAN);
	}

	//

	glm::vec3 normalized = glm::normalize(glm::vec3(x, y ,z));

	//

	float s;
	float t;

	auto faceLayer = getCubeMapFace(s, t, normalized.x, normalized.y, normalized.z);

	if (faceLayer == -1)
	{
		return glm::vec4(NAN, NAN, NAN, NAN);
	}

	//

	glm::vec4 result = getSample(s, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, t, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, 0.5, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, mipLevel, faceLayer);

	if (filter == VK_FILTER_NEAREST)
	{
		return result;
	}

	// Get three more samples. Not seamless.

	float stepS = 1.0f / (float)extent.width;
	float stepT = 1.0f / (float)extent.height;

	float fractionS;
	float fractionT;

	getTexelLocation(fractionS, s, (int32_t)extent.width, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
	getTexelLocation(fractionT, t, (int32_t)extent.height, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);

	if (fractionS < 0.5f)
	{
		result += getSample(s - stepS, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, t, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, 0.5, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, mipLevel, faceLayer);

		if (fractionT < 0.5f)
		{
			result += getSample(s, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, t - stepT, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, 0.5, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, mipLevel, faceLayer);
			result += getSample(s - stepS, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, t - stepT, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, 0.5, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, mipLevel, faceLayer);
		}
		else
		{
			result += getSample(s, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, t + stepT, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, 0.5, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, mipLevel, faceLayer);
			result += getSample(s - stepS, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, t + stepT, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, 0.5, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, mipLevel, faceLayer);
		}
	}
	else
	{
		result += getSample(s + stepS, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, t, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, 0.5, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, mipLevel, faceLayer);

		if (fractionT < 0.5f)
		{
			result += getSample(s, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, t - stepT, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, 0.5, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, mipLevel, faceLayer);
			result += getSample(s + stepS, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, t - stepT, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, 0.5, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, mipLevel, faceLayer);
		}
		else
		{
			result += getSample(s, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, t + stepT, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, 0.5, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, mipLevel, faceLayer);
			result += getSample(s + stepS, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, t + stepT, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, 0.5, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, mipLevel, faceLayer);
		}
	}

	//

	return result * 0.25f;
}

VkBool32 ImageData::getExtentAndOffset(VkExtent3D& currentExtent, uint32_t& currentOffset, const uint32_t mipLevel, const uint32_t arrayLayer) const
{
	if (mipLevel >= mipLevels || arrayLayer >= arrayLayers)
	{
		return VK_FALSE;
	}

	currentExtent.width = glm::max(extent.width >> (mipLevel), 1u);
	currentExtent.height = glm::max(extent.height >> (mipLevel), 1u);
	currentExtent.depth = glm::max(extent.depth >> (mipLevel), 1u);

	currentOffset = allOffsets[arrayLayer * mipLevels + mipLevel];

	return VK_TRUE;
}

void ImageData::freeHostMemory()
{
	if (buffer.get())
	{
		buffer->reset();
	}

	buffer = IBinaryBufferSP();
}

VkBool32 ImageData::updateMaxLuminance()
{
	if (!getData())
	{
		return VK_FALSE;
	}

	maxLuminance = 0.0;

	for (uint32_t arrayLayer = 0; arrayLayer < getArrayLayers(); arrayLayer++)
	{
		for (uint32_t mipLevel = 0; mipLevel < getMipLevels(); mipLevel++)
		{
			for (uint32_t z = 0; z < getDepth(); z++)
			{
				for (uint32_t y = 0; y < getHeight(); y++)
				{
					for (uint32_t x = 0; x < getWidth(); x++)
					{
						maxLuminance = glm::max(maxLuminance, glm::dot(glm::vec3(getTexel(x, y, z, mipLevel, arrayLayer)), glm::vec3(0.2126f, 0.7152f, 0.0722f)));
					}
				}
			}
		}
	}

	return VK_TRUE;
}

float ImageData::getMaxLuminance() const
{
	return maxLuminance;
}

} /* namespace vkts */
