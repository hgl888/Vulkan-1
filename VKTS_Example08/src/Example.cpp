/**
 * VKTS Examples - Examples for Vulkan using VulKan ToolS.
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

#include "Example.hpp"

Example::Example(const vkts::IInstanceSP& instance, const vkts::IPhysicalDeviceSP& physicalDevice, const vkts::IDeviceSP& device, const vkts::IQueueSP& queue) :
		IUpdateThread(), instance(instance), physicalDevice(physicalDevice), device(device), queue(queue), fence(), computeShaderModule(), commandPool(), image(), deviceImageObject(), imageView(), descriptorSetLayout(), descriptorPool(), descriptorSet(), pipelineLayout(), pipeline(), cmdBuffer()
{
}

Example::~Example()
{
}

vkts::IImageDataSP Example::gatherImageData() const
{
	VkResult result;

	auto fence = vkts::fenceCreate(device->getDevice(), 0);

	if (!fence.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create fence.");

		return vkts::IImageDataSP();
	}

	//

	auto imageData = vkts::imageDataCreate(VKTS_IMAGE_NAME, VKTS_IMAGE_LENGTH, VKTS_IMAGE_LENGTH, 1, 1.0f, 0.0f, 0.0f, 1.0f, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM);

	// Check, if we can use a linear tiled image for staging.
	if (physicalDevice->isImageTilingAvailable(VK_IMAGE_TILING_LINEAR, imageData->getFormat(), imageData->getImageType(), 0, imageData->getExtent3D(), imageData->getMipLevels(), 1, VK_SAMPLE_COUNT_1_BIT, imageData->getSize()))
	{
		vkts::IImageSP stageImage;
		vkts::IDeviceMemorySP stageDeviceMemory;

		if (!createTexture(stageImage, stageDeviceMemory, VK_IMAGE_TILING_LINEAR, VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 0))
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create stage image and device memory.");

			return vkts::IImageDataSP();
		}

		//

		cmdBuffer->reset();


		result = cmdBuffer->beginCommandBuffer(0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);

		if (result != VK_SUCCESS)
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not begin command buffer.");

			return vkts::IImageDataSP();
		}

		VkImageSubresourceRange imageSubresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

		// Prepare stage image for final layout etc.
		stageImage->cmdPipelineBarrier(cmdBuffer->getCommandBuffer(), VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, imageSubresourceRange);

		VkImageCopy imageCopy;

		imageCopy.srcSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
		imageCopy.srcOffset = {0, 0, 0};
		imageCopy.dstSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
		imageCopy.dstOffset = {0, 0, 0};
		imageCopy.extent = { VKTS_IMAGE_LENGTH, VKTS_IMAGE_LENGTH, 1u };

		// Copy form device to host visible image / memory. This command also sets the needed barriers.
		image->copyImage(cmdBuffer->getCommandBuffer(), stageImage, imageCopy);

		stageImage->cmdPipelineBarrier(cmdBuffer->getCommandBuffer(), VK_ACCESS_HOST_READ_BIT, VK_IMAGE_LAYOUT_GENERAL, imageSubresourceRange);

		result = cmdBuffer->endCommandBuffer();

		if (result != VK_SUCCESS)
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not end command buffer.");

			return VK_FALSE;
		}


		VkSubmitInfo submitInfo{};

		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		submitInfo.waitSemaphoreCount = 0;
		submitInfo.pWaitSemaphores = nullptr;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = cmdBuffer->getCommandBuffers();
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores = nullptr;

		result = queue->submit(1, &submitInfo, fence->getFence());

		if (result != VK_SUCCESS)
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not submit queue.");

			return vkts::IImageDataSP();
		}

		//

		result = fence->waitForFence(UINT64_MAX);

		if (result != VK_SUCCESS)
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not wait for fence.");

			return vkts::IImageDataSP();
		}

		//
		// Copy pixel data from device memory into image data memory.
		//

		VkImageSubresource imageSubresource;

		imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageSubresource.mipLevel = 0;
		imageSubresource.arrayLayer = 0;

		VkSubresourceLayout subresourceLayout;

		stageImage->getImageSubresourceLayout(subresourceLayout, imageSubresource);

		//

		result = stageDeviceMemory->mapMemory(0, VK_WHOLE_SIZE, 0);

		if (result != VK_SUCCESS)
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not map memory.");

			return vkts::IImageDataSP();
		}

		imageData->upload(stageDeviceMemory->getMemory(), 0, 0, subresourceLayout);

		if (!(stageDeviceMemory->getMemoryPropertyFlags() & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
		{
			result = stageDeviceMemory->invalidateMappedMemoryRanges(0, VK_WHOLE_SIZE);

			if (result != VK_SUCCESS)
			{
				vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not invalidate memory.");

				return VK_FALSE;
			}
		}

		stageDeviceMemory->unmapMemory();

		// Stage image and device memory are automatically destroyed.
	}
	else
	{
		// As an alternative, use the buffer.

		vkts::IBufferSP stageBuffer;
		vkts::IDeviceMemorySP stageDeviceMemory;

		VkBufferCreateInfo bufferCreateInfo{};

        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size = imageData->getSize();
        bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        bufferCreateInfo.flags = 0;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bufferCreateInfo.queueFamilyIndexCount = 0;
        bufferCreateInfo.pQueueFamilyIndices = nullptr;

        if (!createBuffer(stageBuffer, stageDeviceMemory, bufferCreateInfo, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
        {
    		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create buffer.");

			return vkts::IImageDataSP();
        }


		//

		cmdBuffer->reset();


		result = cmdBuffer->beginCommandBuffer(0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);

		if (result != VK_SUCCESS)
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not begin command buffer.");

			return vkts::IImageDataSP();
		}

		VkBufferImageCopy bufferImageCopy;

		bufferImageCopy.bufferOffset = 0;
		bufferImageCopy.bufferRowLength = VKTS_IMAGE_LENGTH;
		bufferImageCopy.bufferImageHeight = VKTS_IMAGE_LENGTH;
		bufferImageCopy.imageSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
		bufferImageCopy.imageOffset = {0, 0, 0};
		bufferImageCopy.imageExtent = {VKTS_IMAGE_LENGTH, VKTS_IMAGE_LENGTH, 1};

		image->copyImageToBuffer(cmdBuffer->getCommandBuffer(), stageBuffer, bufferImageCopy);


		result = cmdBuffer->endCommandBuffer();

		if (result != VK_SUCCESS)
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not end command buffer.");

			return VK_FALSE;
		}


		VkSubmitInfo submitInfo{};

		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		submitInfo.waitSemaphoreCount = 0;
		submitInfo.pWaitSemaphores = nullptr;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = cmdBuffer->getCommandBuffers();
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores = nullptr;

		result = queue->submit(1, &submitInfo, fence->getFence());

		if (result != VK_SUCCESS)
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not submit queue.");

			return vkts::IImageDataSP();
		}

		//

		result = fence->waitForFence(UINT64_MAX);

		if (result != VK_SUCCESS)
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not wait for fence.");

			return vkts::IImageDataSP();
		}

		//
		// Copy pixel data from device memory into image data memory.
		//

		VkSubresourceLayout subresourceLayout;

		subresourceLayout.offset = 0;
		subresourceLayout.size = stageBuffer->getSize();
		subresourceLayout.rowPitch = VKTS_IMAGE_LENGTH * 4 * sizeof(uint8_t);
		subresourceLayout.arrayPitch = VKTS_IMAGE_LENGTH * VKTS_IMAGE_LENGTH * 4 * sizeof(uint8_t);
		subresourceLayout.depthPitch = VKTS_IMAGE_LENGTH * VKTS_IMAGE_LENGTH * 4 * sizeof(uint8_t);

		result = stageDeviceMemory->mapMemory(0, VK_WHOLE_SIZE, 0);

		if (result != VK_SUCCESS)
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not map memory.");

			return vkts::IImageDataSP();
		}

		imageData->upload(stageDeviceMemory->getMemory(), 0, 0, subresourceLayout);

		if (!(stageDeviceMemory->getMemoryPropertyFlags() & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
		{
			result = stageDeviceMemory->invalidateMappedMemoryRanges(0, VK_WHOLE_SIZE);

			if (result != VK_SUCCESS)
			{
				vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not invalidate memory.");

				return VK_FALSE;
			}
		}

		stageDeviceMemory->unmapMemory();

		// Stage image and device memory are automatically destroyed.
	}

	// Fence is automatically destroyed.

	return imageData;
}

VkBool32 Example::buildCmdBuffer()
{
	VkResult result;

	//

	cmdBuffer = vkts::commandBuffersCreate(device->getDevice(), commandPool->getCmdPool(), VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);

	if (!cmdBuffer.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create command buffer.");

		return VK_FALSE;
	}

	result = cmdBuffer->beginCommandBuffer(0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);

	if (result != VK_SUCCESS)
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not begin command buffer.");

		return VK_FALSE;
	}

	//

	vkCmdBindPipeline(cmdBuffer->getCommandBuffer(), VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->getPipeline());

	vkCmdBindDescriptorSets(cmdBuffer->getCommandBuffer(), VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout->getPipelineLayout(), 0, 1, descriptorSet->getDescriptorSets(), 0, nullptr);

	VkImageSubresourceRange imageSubresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

	image->cmdPipelineBarrier(cmdBuffer->getCommandBuffer(), VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL, imageSubresourceRange);

	vkCmdDispatch(cmdBuffer->getCommandBuffer(), VKTS_IMAGE_LENGTH / VKTS_LOCAL_SIZE, VKTS_IMAGE_LENGTH / VKTS_LOCAL_SIZE, 1);

	//

	result = cmdBuffer->endCommandBuffer();

	if (result != VK_SUCCESS)
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not end command buffer.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildCmdPool()
{
	commandPool = vkts::commandPoolCreate(device->getDevice(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, queue->getQueueFamilyIndex());

	if (!commandPool.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not get command pool.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildPipeline()
{
	VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo{};

	pipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

	pipelineShaderStageCreateInfo.flags = 0;
	pipelineShaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	pipelineShaderStageCreateInfo.module = computeShaderModule->getShaderModule();
	pipelineShaderStageCreateInfo.pName = "main";
	pipelineShaderStageCreateInfo.pSpecializationInfo = nullptr;

	//

	VkComputePipelineCreateInfo computePipelineCreateInfo{};

	computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;

	computePipelineCreateInfo.flags = 0;
	computePipelineCreateInfo.stage = pipelineShaderStageCreateInfo;
	computePipelineCreateInfo.layout = pipelineLayout->getPipelineLayout();
	computePipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	computePipelineCreateInfo.basePipelineIndex = 0;

	pipeline = vkts::pipelineCreateCompute(device->getDevice(), VK_NULL_HANDLE, computePipelineCreateInfo);

	if (!pipeline.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create compute pipeline.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildPipelineLayout()
{
	const VkDescriptorSetLayout currentDescriptorSet = descriptorSetLayout->getDescriptorSetLayout();


	pipelineLayout = vkts::pipelineCreateLayout(device->getDevice(), 0, 1, &currentDescriptorSet, 0, nullptr);

	if (!pipelineLayout.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create pipeline layout.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildDescriptorSets()
{
	const VkDescriptorSetLayout currentDescriptorSetLayout = descriptorSetLayout->getDescriptorSetLayout();

	descriptorSet = vkts::descriptorSetsCreate(device->getDevice(), descriptorPool->getDescriptorPool(), 1, &currentDescriptorSetLayout);

	if (!descriptorSet.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not alloc descriptor sets.");

		return VK_FALSE;
	}

	//

	VkDescriptorImageInfo descriptorImageInfo{};

	descriptorImageInfo.sampler = VK_NULL_HANDLE;
	descriptorImageInfo.imageView = imageView->getImageView();
	descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	VkWriteDescriptorSet writeDescriptorSet{};

	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

	writeDescriptorSet.dstSet = descriptorSet->getDescriptorSets()[0];
	writeDescriptorSet.dstBinding = 0;
	writeDescriptorSet.dstArrayElement = 0;
	writeDescriptorSet.descriptorCount = 1;
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	writeDescriptorSet.pImageInfo = &descriptorImageInfo;
	writeDescriptorSet.pBufferInfo = nullptr;
	writeDescriptorSet.pTexelBufferView = nullptr;

	descriptorSet->updateDescriptorSets( 1, &writeDescriptorSet, 0, nullptr);

	return VK_TRUE;
}

VkBool32 Example::buildDescriptorSetPool()
{
	VkDescriptorPoolSize descriptorTypeCount{};

	descriptorTypeCount.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	descriptorTypeCount.descriptorCount = 1;

	descriptorPool = vkts::descriptorPoolCreate(device->getDevice(), VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT, 1, 1, &descriptorTypeCount);

	if (!descriptorPool.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create descriptor pool.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding descriptorSetLayoutBinding{};

	descriptorSetLayoutBinding.binding = 0;
	descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	descriptorSetLayoutBinding.descriptorCount = 1;
	descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	descriptorSetLayoutBinding.pImmutableSamplers = nullptr;

	descriptorSetLayout = vkts::descriptorSetLayoutCreate(device->getDevice(), 0, 1, &descriptorSetLayoutBinding);

	if (!descriptorSetLayout.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create descriptor set layout.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::createBuffer(vkts::IBufferSP& buffer, vkts::IDeviceMemorySP& deviceMemory, const VkBufferCreateInfo& bufferCreateInfo, const VkMemoryPropertyFlags memoryPropertyFlags) const
{

    VkResult result;

    //

    buffer = vkts::bufferCreate(device->getDevice(), bufferCreateInfo.flags, bufferCreateInfo.size, bufferCreateInfo.usage, bufferCreateInfo.sharingMode, bufferCreateInfo.queueFamilyIndexCount, bufferCreateInfo.pQueueFamilyIndices);

    if (!buffer.get())
    {
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create image.");

        return VK_FALSE;
    }

    //

    VkMemoryRequirements memoryRequirements;

    buffer->getBufferMemoryRequirements(memoryRequirements);

    //

    VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
    physicalDevice->getPhysicalDeviceMemoryProperties(physicalDeviceMemoryProperties);

    deviceMemory = vkts::deviceMemoryCreate(device->getDevice(), memoryRequirements, physicalDeviceMemoryProperties.memoryTypeCount, physicalDeviceMemoryProperties.memoryTypes, memoryPropertyFlags);

    if (!deviceMemory.get())
    {
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not allocate memory.");

		return VK_FALSE;
    }

    //

    result = vkBindBufferMemory(device->getDevice(), buffer->getBuffer(), deviceMemory->getDeviceMemory(), 0);

    if (result != VK_SUCCESS)
    {
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not bind buffer memory.");

		return VK_FALSE;
    }

    return VK_TRUE;
}


VkBool32 Example::createTexture(vkts::IImageSP& currentImage, vkts::IDeviceMemorySP& currentDeviceImageObject, const VkImageTiling imageTiling, const VkImageUsageFlags usage, const VkImageLayout initialLayout, const VkMemoryPropertyFlags memoryPropertyFlagBits, const VkAccessFlags accessMask) const
{
	VkResult result;

	//

	VkExtent3D extent = {VKTS_IMAGE_LENGTH, VKTS_IMAGE_LENGTH, 1};

	currentImage = vkts::imageCreate(device->getDevice(), 0, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, extent, 1, 1, VK_SAMPLE_COUNT_1_BIT, imageTiling, usage, VK_SHARING_MODE_EXCLUSIVE, 0, nullptr, initialLayout, accessMask);

	if (!currentImage.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create image.");

		return VK_FALSE;
	}

	//

	VkMemoryRequirements memoryRequirements;

	currentImage->getImageMemoryRequirements(memoryRequirements);

	//

	VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;

	physicalDevice->getPhysicalDeviceMemoryProperties(physicalDeviceMemoryProperties);

	currentDeviceImageObject = vkts::deviceMemoryCreate(device->getDevice(), memoryRequirements, VK_MAX_MEMORY_TYPES, physicalDeviceMemoryProperties.memoryTypes, memoryPropertyFlagBits);

	if (!currentDeviceImageObject.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not allocate memory.");

		return VK_FALSE;
	}

	// Bind image to memory.

	result = vkBindImageMemory(device->getDevice(), currentImage->getImage(), currentDeviceImageObject->getDeviceMemory(), 0);

	if (result != VK_SUCCESS)
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not bind image memory.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::destroyTexture(vkts::IImageSP& currentImage, vkts::IDeviceMemorySP& currentDeviceImageObject) const
{
	if (currentImage.get())
	{
		currentImage->destroy();

		currentDeviceImageObject->destroy();
	}

	return VK_TRUE;
}

VkBool32 Example::buildTexture()
{
	VkFormatProperties formatProperties;

	vkGetPhysicalDeviceFormatProperties(physicalDevice->getPhysicalDevice(), VK_FORMAT_R8G8B8A8_UNORM, &formatProperties);

	// Check, if storage image is possible.
	if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT))
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Format not supported.");

		return VK_FALSE;
	}

	if (!createTexture(image, deviceImageObject, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 0))
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create image.");

		return VK_FALSE;
	}

	//

	imageView = vkts::imageViewCreate(device->getDevice(), 0, image->getImage(), VK_IMAGE_VIEW_TYPE_2D, image->getFormat(), { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A }, { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

	if (!imageView.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create image view.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildShader()
{
	auto computeShaderBinary = vkts::fileLoadBinary(VKTS_COMPUTE_SHADER_NAME);

	if (!computeShaderBinary.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load compute shader: '%s'", VKTS_COMPUTE_SHADER_NAME);

		return VK_FALSE;
	}

	//

	computeShaderModule = vkts::shaderModuleCreate(VKTS_COMPUTE_SHADER_NAME, device->getDevice(), 0, computeShaderBinary->getSize(), (const uint32_t*)computeShaderBinary->getData());

	if (!computeShaderModule.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create compute shader module.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

//
// Vulkan initialization.
//
VkBool32 Example::init(const vkts::IUpdateThreadContext& updateContext)
{
	fence = vkts::fenceCreate(device->getDevice(), 0);

	if (!fence.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build fence.");

		return VK_FALSE;
	}

	if (!buildShader())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build shader.");

		return VK_FALSE;
	}

	if (!buildCmdPool())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build command pool.");

		return VK_FALSE;
	}

	//

	if (!buildTexture())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build texture.");

		return VK_FALSE;
	}

	//

	if (!buildDescriptorSetLayout())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build descriptor set layout.");

		return VK_FALSE;
	}

	if (!buildDescriptorSetPool())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build descriptor set pool.");

		return VK_FALSE;
	}

	if (!buildDescriptorSets())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build descriptor sets.");

		return VK_FALSE;
	}

	if (!buildPipelineLayout())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build pipeline layout.");

		return VK_FALSE;
	}

	if (!buildPipeline())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build pipeline.");

		return VK_FALSE;
	}

	if (!buildCmdBuffer())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build command buffer.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

//
// Vulkan update.
//
VkBool32 Example::update(const vkts::IUpdateThreadContext& updateContext)
{
	static VkBool32 executed = VK_FALSE;

	static VkBool32 informed = VK_FALSE;

	if (!executed)
	{
		VkSubmitInfo submitInfo{};

		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		submitInfo.waitSemaphoreCount = 0;
		submitInfo.pWaitSemaphores = nullptr;
		submitInfo.pWaitDstStageMask = nullptr;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = cmdBuffer->getCommandBuffers();
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores = nullptr;

		VkResult result = queue->submit(1, &submitInfo, fence->getFence());

		if (result != VK_SUCCESS)
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not submit queue.");

			return VK_FALSE;
		}

		//

		executed = VK_TRUE;
	}
	else
	{
		// Note: It is possible to wait (and block) for a fence.
		// 		 It is demonstrated, that the CPU can execute in parallel.
		if (fence->getStatus() == VK_SUCCESS)
		{
			vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Compute shader done.");

			//

			auto storedImage = gatherImageData();

			if (!storedImage.get())
			{
				vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No image data.");

				return VK_FALSE;
			}

			if (!vkts::imageDataSave(VKTS_IMAGE_NAME, storedImage))
			{
				vkts::logPrint(VKTS_LOG_WARNING, __FILE__, __LINE__, "Could not save image data.");

				return VK_FALSE;
			}

			//

			vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Saved image data as '%s'", VKTS_IMAGE_NAME);

			// Returning false requests quitting the application.
			return VK_FALSE;
		}
		else
		{
			if (!informed)
			{
				vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Compute shader ongoing.");

				informed = VK_TRUE;
			}

			// Avoid "empty while" loop, as nothing is done.
			std::this_thread::yield();
		}
	}

	return VK_TRUE;
}

//
// Vulkan termination.
//
void Example::terminate(const vkts::IUpdateThreadContext& updateContext)
{
	if (cmdBuffer.get())
	{
		cmdBuffer->destroy();

		cmdBuffer.reset();
	}

	if (pipeline.get())
	{
		pipeline->destroy();

		pipeline.reset();
	}

	if (pipelineLayout.get())
	{
		pipelineLayout->destroy();

		pipelineLayout.reset();
	}

	if (descriptorSet.get())
	{
		descriptorSet->destroy();

		descriptorSet.reset();
	}

	if (descriptorPool.get())
	{
		descriptorPool->destroy();

		descriptorPool.reset();
	}

	if (descriptorSetLayout.get())
	{
		descriptorSetLayout->destroy();

		descriptorSetLayout.reset();
	}

	if (imageView.get())
	{
		imageView->destroy();

		imageView.reset();
	}


	destroyTexture(image, deviceImageObject);

	image.reset();
	deviceImageObject.reset();


	if (commandPool.get())
	{
		commandPool->destroy();

		commandPool.reset();
	}

	if (computeShaderModule.get())
	{
		computeShaderModule->destroy();

		computeShaderModule.reset();
	}

	if (fence.get())
	{
		fence->destroy();

		fence.reset();
	}
}
