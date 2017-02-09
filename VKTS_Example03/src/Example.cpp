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

Example::Example(const vkts::IInstanceSP& instance, const vkts::IPhysicalDeviceSP& physicalDevice, const int32_t windowIndex, const vkts::IVisualContextSP& visualContext, const vkts::ISurfaceSP& surface, const vkts::IDeviceSP& device, const vkts::IQueueSP& queue) :
		IUpdateThread(), instance(instance), physicalDevice(physicalDevice), windowIndex(windowIndex), visualContext(visualContext), surface(surface), device(device), queue(queue), commandPool(nullptr), imageAcquiredSemaphore(nullptr), renderingCompleteSemaphore(nullptr), image(nullptr), deviceImageObject(nullptr), sampler(nullptr), imageView(nullptr), descriptorSetLayout(VK_NULL_HANDLE), descriptorPool(VK_NULL_HANDLE), descriptorSet(VK_NULL_HANDLE), vertexBuffer(nullptr), deviceMemoryVertexBuffer(nullptr), vertexShaderModule(nullptr), fragmentShaderModule(nullptr), pipelineLayout(nullptr), swapchain(nullptr), renderPass(nullptr), pipeline(nullptr), swapchainImagesCount(0), swapchainImageView(), framebuffer(), cmdBuffer()
{
}

Example::~Example()
{
}

VkBool32 Example::createTexture(vkts::IImageSP& currentImage, vkts::IDeviceMemorySP& currentDeviceImageObject, const vkts::IImageDataSP& imageData, const VkImageTiling imageTiling, const VkImageUsageFlags usage, const VkImageLayout initialLayout, const VkMemoryPropertyFlags memoryPropertyFlagBits, const VkAccessFlags accessMask) const
{
	VkResult result;

	//

	currentImage = vkts::imageCreate(device->getDevice(), 0, imageData->getImageType(), imageData->getFormat(), imageData->getExtent3D(), 1, 1, VK_SAMPLE_COUNT_1_BIT, imageTiling, usage, VK_SHARING_MODE_EXCLUSIVE, 0, nullptr, initialLayout, accessMask);

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

	// Upload data if host visible.

	if (memoryPropertyFlagBits & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
	{
		VkImageSubresource imageSubresource;
		imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageSubresource.mipLevel = 0;
		imageSubresource.arrayLayer = 0;

		VkSubresourceLayout subresourceLayout;

		currentImage->getImageSubresourceLayout(subresourceLayout, imageSubresource);

		result = currentDeviceImageObject->mapMemory(0, VK_WHOLE_SIZE, 0);

		if (result != VK_SUCCESS)
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not map memory.");

			return VK_FALSE;
		}

		imageData->copy(currentDeviceImageObject->getMemory(), 0, 0, subresourceLayout);

		if (!(currentDeviceImageObject->getMemoryPropertyFlags() & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
		{
			result = currentDeviceImageObject->flushMappedMemoryRanges(0, VK_WHOLE_SIZE);

			if (result != VK_SUCCESS)
			{
				vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not flush memory.");

				return VK_FALSE;
			}
		}

		currentDeviceImageObject->unmapMemory();
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

VkBool32 Example::buildCmdBuffer(const int32_t usedBuffer)
{
	VkResult result;

	cmdBuffer[usedBuffer] = vkts::commandBuffersCreate(device->getDevice(), commandPool->getCmdPool(), VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);

	if (!cmdBuffer[usedBuffer].get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create command buffer.");

		return VK_FALSE;
	}

	result = cmdBuffer[usedBuffer]->beginCommandBuffer(0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);

	if (result != VK_SUCCESS)
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not begin command buffer.");

		return VK_FALSE;
	}

    //

    swapchain->cmdPipelineBarrier(cmdBuffer[usedBuffer]->getCommandBuffer(), VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, usedBuffer);

    //

	VkClearColorValue clearColorValue{};

	clearColorValue.float32[0] = 0.0f;
	clearColorValue.float32[1] = 0.0f;
	clearColorValue.float32[2] = 0.0f;
	clearColorValue.float32[3] = 1.0f;

	VkClearValue clearValues[1] = { clearColorValue };

	VkRenderPassBeginInfo renderPassBeginInfo{};

	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

	renderPassBeginInfo.renderPass = renderPass->getRenderPass();
	renderPassBeginInfo.framebuffer = framebuffer[usedBuffer]->getFramebuffer();
	renderPassBeginInfo.renderArea.offset.x = 0;
	renderPassBeginInfo.renderArea.offset.y = 0;
	renderPassBeginInfo.renderArea.extent = swapchain->getImageExtent();
	renderPassBeginInfo.clearValueCount = 1;
	renderPassBeginInfo.pClearValues = clearValues;

	cmdBuffer[usedBuffer]->cmdBeginRenderPass(&renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(cmdBuffer[usedBuffer]->getCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getPipeline());

	vkCmdBindDescriptorSets(cmdBuffer[usedBuffer]->getCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout->getPipelineLayout(), 0, 1, &descriptorSet, 0, nullptr);

	VkViewport viewport{};

	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float) swapchain->getImageExtent().width;
	viewport.height = (float) swapchain->getImageExtent().height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	vkCmdSetViewport(cmdBuffer[usedBuffer]->getCommandBuffer(), 0, 1, &viewport);

	VkRect2D scissor{};

	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent = swapchain->getImageExtent();

	vkCmdSetScissor(cmdBuffer[usedBuffer]->getCommandBuffer(), 0, 1, &scissor);

	VkDeviceSize offsets[1] =
	{ 0 };
	VkBuffer buffers[1] = { vertexBuffer->getBuffer() };

	vkCmdBindVertexBuffers(cmdBuffer[usedBuffer]->getCommandBuffer(), 0, 1, buffers, offsets);

	vkCmdDraw(cmdBuffer[usedBuffer]->getCommandBuffer(), 4, 1, 0, 0);

	cmdBuffer[usedBuffer]->cmdEndRenderPass();

    //

    swapchain->cmdPipelineBarrier(cmdBuffer[usedBuffer]->getCommandBuffer(), VK_ACCESS_MEMORY_READ_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, usedBuffer);

    //

	result = cmdBuffer[usedBuffer]->endCommandBuffer();

	if (result != VK_SUCCESS)
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not end command buffer.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildFramebuffer(const int32_t usedBuffer)
{
	auto imageView = swapchainImageView[usedBuffer]->getImageView();

	framebuffer[usedBuffer] = vkts::framebufferCreate(device->getDevice(), 0,  renderPass->getRenderPass(), 1, &imageView, swapchain->getImageExtent().width, swapchain->getImageExtent().height, 1);

	if (!framebuffer[usedBuffer].get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create frame buffer.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildSwapchainImageView(const int32_t usedBuffer)
{
	VkComponentMapping channelMapping = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
	VkImageSubresourceRange imageSubresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

	swapchainImageView[usedBuffer] = vkts::imageViewCreate(device->getDevice(), 0, swapchain->getAllSwapchainImages()[usedBuffer], VK_IMAGE_VIEW_TYPE_2D, swapchain->getImageFormat(), channelMapping, imageSubresourceRange);

	if (!swapchainImageView[usedBuffer].get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create color attachment view.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::updateDescriptorSets()
{
	VkDescriptorImageInfo descriptorImageInfo{};

	descriptorImageInfo.sampler = sampler->getSampler();
	descriptorImageInfo.imageView = imageView->getImageView();
	descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	VkWriteDescriptorSet writeDescriptorSet{};

	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

	writeDescriptorSet.dstSet = descriptorSet;
	writeDescriptorSet.dstBinding = 0;
	writeDescriptorSet.dstArrayElement = 0;
	writeDescriptorSet.descriptorCount = 1;
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	writeDescriptorSet.pImageInfo = &descriptorImageInfo;
	writeDescriptorSet.pBufferInfo = nullptr;
	writeDescriptorSet.pTexelBufferView = nullptr;

	vkUpdateDescriptorSets(device->getDevice(), 1, &writeDescriptorSet, 0, nullptr);

	return VK_TRUE;
}

VkBool32 Example::buildTexture(const vkts::ICommandBuffersSP& cmdBuffer, vkts::IImageSP& stageImage, vkts::IDeviceMemorySP& stageDeviceImageObject)
{
	auto imageData = vkts::imageDataLoad(VKTS_TEXTURE_NAME);

	if (!imageData.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load image data: '%s'", VKTS_TEXTURE_NAME);

		return VK_FALSE;
	}

	//

	VkFormatProperties formatProperties;

	vkGetPhysicalDeviceFormatProperties(physicalDevice->getPhysicalDevice(), imageData->getFormat(), &formatProperties);

	VkImageTiling imageTiling = VK_IMAGE_TILING_LINEAR;
	VkMemoryPropertyFlags memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	VkImageLayout initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
	VkAccessFlags accessMask = VK_ACCESS_HOST_WRITE_BIT;

	// Check, how to upload image data.
	if (!(formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT))
	{
		if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT))
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Format not supported.");

			return VK_FALSE;
		}

		imageTiling = VK_IMAGE_TILING_OPTIMAL;
		memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		initialLayout = VK_IMAGE_LAYOUT_GENERAL;
		accessMask = 0;
	}

	//

	if (!createTexture(image, deviceImageObject, imageData, imageTiling, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, initialLayout, memoryPropertyFlags, accessMask))
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create image.");

		return VK_FALSE;
	}

	//

	VkImageMemoryBarrier imageMemoryBarrier{};

	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

	imageMemoryBarrier.srcAccessMask = 0;			// Defined later.
	imageMemoryBarrier.dstAccessMask = 0;			// Defined later.
	imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;// Defined later.
	imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_UNDEFINED;// Defined later.
	imageMemoryBarrier.srcQueueFamilyIndex = 0;
	imageMemoryBarrier.dstQueueFamilyIndex = 0;
	imageMemoryBarrier.image = VK_NULL_HANDLE;		// Defined later.
	imageMemoryBarrier.subresourceRange = {	VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

	//

	imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageMemoryBarrier.image = image->getImage();

	cmdBuffer->cmdPipelineBarrier(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

	//

	// If the image is only accessible by the device ...
	if (memoryPropertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
	{
		// ... create texture with host visibility. This texture contains the pixel data.
		if (!createTexture(stageImage, stageDeviceImageObject, imageData, VK_IMAGE_TILING_LINEAR, VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VK_ACCESS_HOST_WRITE_BIT))
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create image.");

			return VK_FALSE;
		}

		// Prepare source image layout for copy.

		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		imageMemoryBarrier.image = stageImage->getImage();

		cmdBuffer->cmdPipelineBarrier(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

		// Prepare target image layout for copy.

		imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageMemoryBarrier.image = image->getImage();

		cmdBuffer->cmdPipelineBarrier(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

		// Copy image data by command.

		VkImageCopy imageCopy;

		imageCopy.srcSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};

		imageCopy.srcOffset = {	0, 0, 0};

		imageCopy.dstSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
		imageCopy.dstOffset = {	0, 0, 0};
		imageCopy.extent = { imageData->getWidth(), imageData->getHeight(), imageData->getDepth()};

		vkCmdCopyImage(cmdBuffer->getCommandBuffer(), stageImage->getImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image->getImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopy);

		// Switch back to original layout.
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
		imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		cmdBuffer->cmdPipelineBarrier(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
	}

	//

	sampler = vkts::samplerCreate(device->getDevice(), 0, VK_FILTER_NEAREST, VK_FILTER_NEAREST, VK_SAMPLER_MIPMAP_MODE_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, 0.0f, VK_FALSE, 1.0f, VK_FALSE, VK_COMPARE_OP_NEVER, 0.0f, 0.0f, VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE, VK_FALSE);

	if (!sampler.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create sampler.");

		return VK_FALSE;
	}

	imageView = vkts::imageViewCreate(device->getDevice(), 0, image->getImage(), VK_IMAGE_VIEW_TYPE_2D, image->getFormat(), { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A }, { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

	if (!imageView.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create image view.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildPipeline()
{
	VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo[VKTS_SHADER_STAGE_COUNT]{};

	pipelineShaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

	pipelineShaderStageCreateInfo[0].flags = 0;
	pipelineShaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	pipelineShaderStageCreateInfo[0].module = vertexShaderModule->getShaderModule();
	pipelineShaderStageCreateInfo[0].pName = "main";
	pipelineShaderStageCreateInfo[0].pSpecializationInfo = nullptr;

	pipelineShaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

	pipelineShaderStageCreateInfo[1].flags = 0;
	pipelineShaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	pipelineShaderStageCreateInfo[1].module = fragmentShaderModule->getShaderModule();
	pipelineShaderStageCreateInfo[1].pName = "main";
	pipelineShaderStageCreateInfo[1].pSpecializationInfo = nullptr;

	//

	VkTsVertexBufferType vertexBufferType = VKTS_VERTEX_BUFFER_TYPE_VERTEX | VKTS_VERTEX_BUFFER_TYPE_TEXCOORD;


	VkVertexInputBindingDescription vertexInputBindingDescription{};

	vertexInputBindingDescription.binding = 0;
	vertexInputBindingDescription.stride = (4 + 2) * sizeof(float);
	vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	VkVertexInputAttributeDescription vertexInputAttributeDescription[2]{};

	vertexInputAttributeDescription[0].location = 0;
	vertexInputAttributeDescription[0].binding = 0;
	vertexInputAttributeDescription[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	vertexInputAttributeDescription[0].offset = 0;

	vertexInputAttributeDescription[1].location = 1;
	vertexInputAttributeDescription[1].binding = 0;
	vertexInputAttributeDescription[1].format = VK_FORMAT_R32G32_SFLOAT;
	vertexInputAttributeDescription[1].offset = 4 * sizeof(float);

	VkPipelineVertexInputStateCreateInfo pipelineVertexInputCreateInfo{};

	pipelineVertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	pipelineVertexInputCreateInfo.flags = 0;
	pipelineVertexInputCreateInfo.vertexBindingDescriptionCount = 1;
	pipelineVertexInputCreateInfo.pVertexBindingDescriptions = &vertexInputBindingDescription;
	pipelineVertexInputCreateInfo.vertexAttributeDescriptionCount = 2;
	pipelineVertexInputCreateInfo.pVertexAttributeDescriptions = vertexInputAttributeDescription;

	//

	VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{};

	pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

	pipelineInputAssemblyStateCreateInfo.flags = 0;
	pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
	pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

	//

	VkViewport viewport{};

	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float) swapchain->getImageExtent().width;
	viewport.height = (float) swapchain->getImageExtent().height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};

	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent = swapchain->getImageExtent();

	VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo{};

	pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

	pipelineViewportStateCreateInfo.flags = 0;
	pipelineViewportStateCreateInfo.viewportCount = 1;
	pipelineViewportStateCreateInfo.pViewports = &viewport;
	pipelineViewportStateCreateInfo.scissorCount = 1;
	pipelineViewportStateCreateInfo.pScissors = &scissor;

	//

	VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo{};

	pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

	pipelineRasterizationStateCreateInfo.flags = 0;
	pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
	pipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	pipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
	pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	pipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
	pipelineRasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
	pipelineRasterizationStateCreateInfo.depthBiasClamp = 0.0f;
	pipelineRasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
	pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;

	//

	VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo{};

	pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;

	pipelineMultisampleStateCreateInfo.flags = 0;
	pipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	pipelineMultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
	pipelineMultisampleStateCreateInfo.minSampleShading = 0.0f;
	pipelineMultisampleStateCreateInfo.pSampleMask = nullptr;
	pipelineMultisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
	pipelineMultisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

	//

	VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState{};

	pipelineColorBlendAttachmentState.blendEnable = VK_FALSE;
	pipelineColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	pipelineColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	pipelineColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
	pipelineColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	pipelineColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	pipelineColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
	pipelineColorBlendAttachmentState.colorWriteMask = 0xf;

	VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo{};

	pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

	pipelineColorBlendStateCreateInfo.flags = 0;
	pipelineColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
	pipelineColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	pipelineColorBlendStateCreateInfo.attachmentCount = 1;
	pipelineColorBlendStateCreateInfo.pAttachments = &pipelineColorBlendAttachmentState;
	pipelineColorBlendStateCreateInfo.blendConstants[0] = 0.0f;
	pipelineColorBlendStateCreateInfo.blendConstants[1] = 0.0f;
	pipelineColorBlendStateCreateInfo.blendConstants[2] = 0.0f;
	pipelineColorBlendStateCreateInfo.blendConstants[3] = 0.0f;

	//

	VkDynamicState dynamicState[VKTS_NUMBER_DYNAMIC_STATES] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

	VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};

	pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;

	pipelineDynamicStateCreateInfo.flags = 0;
	pipelineDynamicStateCreateInfo.dynamicStateCount = VKTS_NUMBER_DYNAMIC_STATES;
	pipelineDynamicStateCreateInfo.pDynamicStates = dynamicState;

	//

	VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};

	graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

	graphicsPipelineCreateInfo.flags = 0;
	graphicsPipelineCreateInfo.stageCount = VKTS_SHADER_STAGE_COUNT;
	graphicsPipelineCreateInfo.pStages = pipelineShaderStageCreateInfo;
	graphicsPipelineCreateInfo.pVertexInputState = &pipelineVertexInputCreateInfo;
	graphicsPipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo;
	graphicsPipelineCreateInfo.pTessellationState = nullptr;
	graphicsPipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
	graphicsPipelineCreateInfo.pRasterizationState = &pipelineRasterizationStateCreateInfo;
	graphicsPipelineCreateInfo.pMultisampleState = &pipelineMultisampleStateCreateInfo;
	graphicsPipelineCreateInfo.pDepthStencilState = nullptr;
	graphicsPipelineCreateInfo.pColorBlendState = &pipelineColorBlendStateCreateInfo;
	graphicsPipelineCreateInfo.pDynamicState = &pipelineDynamicStateCreateInfo;
	graphicsPipelineCreateInfo.layout = pipelineLayout->getPipelineLayout();
	graphicsPipelineCreateInfo.renderPass = renderPass->getRenderPass();
	graphicsPipelineCreateInfo.subpass = 0;
	graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	graphicsPipelineCreateInfo.basePipelineIndex = 0;

	pipeline = vkts::pipelineCreateGraphics(device->getDevice(), VK_NULL_HANDLE, graphicsPipelineCreateInfo, vertexBufferType);

	if (!pipeline.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create graphics pipeline.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildRenderPass()
{
	VkAttachmentDescription attachmentDescription{};

	attachmentDescription.flags = 0;
	attachmentDescription.format = swapchain->getImageFormat();
	attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
	attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachmentReference{};

	colorAttachmentReference.attachment = 0;
	colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpassDescription{};

	subpassDescription.flags = 0;
	subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDescription.inputAttachmentCount = 0;
	subpassDescription.pInputAttachments = nullptr;
	subpassDescription.colorAttachmentCount = 1;
	subpassDescription.pColorAttachments = &colorAttachmentReference;
	subpassDescription.pResolveAttachments = nullptr;
	subpassDescription.pDepthStencilAttachment = nullptr;
	subpassDescription.preserveAttachmentCount = 0;
	subpassDescription.pPreserveAttachments = nullptr;

	renderPass = vkts::renderPassCreate(device->getDevice(), 0, 1, &attachmentDescription, 1, &subpassDescription, 0, nullptr);

	if (!renderPass.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create render pass.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildPipelineLayout()
{
	pipelineLayout = vkts::pipelineCreateLayout(device->getDevice(), 0, 1, &descriptorSetLayout, 0, nullptr);

	if (!pipelineLayout.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create pipeline layout.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildDescriptorSets()
{
	VkResult result;

	//

	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};

	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;

	descriptorSetAllocateInfo.descriptorPool = descriptorPool;
	descriptorSetAllocateInfo.descriptorSetCount = 1;
	descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;

	result = vkAllocateDescriptorSets(device->getDevice(), &descriptorSetAllocateInfo, &descriptorSet);

	if (result != VK_SUCCESS)
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not alloc descriptor sets.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildDescriptorSetPool()
{
	VkResult result;

	//

	VkDescriptorPoolSize descriptorTypeCount{};

	descriptorTypeCount.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorTypeCount.descriptorCount = 1;

	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};

	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;

	descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	descriptorPoolCreateInfo.maxSets = 1;
	descriptorPoolCreateInfo.poolSizeCount = 1;
	descriptorPoolCreateInfo.pPoolSizes = &descriptorTypeCount;

	result = vkCreateDescriptorPool(device->getDevice(), &descriptorPoolCreateInfo, nullptr, &descriptorPool);

	if (result != VK_SUCCESS)
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create descriptor pool.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildDescriptorSetLayout()
{
	VkResult result;

	//

	VkDescriptorSetLayoutBinding descriptorSetLayoutBinding{};

	descriptorSetLayoutBinding.binding = 0;
	descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorSetLayoutBinding.descriptorCount = 1;
	descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	descriptorSetLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};

	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

	descriptorSetLayoutCreateInfo.flags = 0;
	descriptorSetLayoutCreateInfo.bindingCount = 1;
	descriptorSetLayoutCreateInfo.pBindings = &descriptorSetLayoutBinding;

	result = vkCreateDescriptorSetLayout(device->getDevice(), &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout);

	if (result != VK_SUCCESS)
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create descriptor set layout.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildShader()
{
	auto vertexShaderBinary = vkts::fileLoadBinary(VKTS_VERTEX_SHADER_NAME);

	if (!vertexShaderBinary.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load vertex shader: '%s'", VKTS_VERTEX_SHADER_NAME);

		return VK_FALSE;
	}

	auto fragmentShaderBinary = vkts::fileLoadBinary(VKTS_FRAGMENT_SHADER_NAME);

	if (!fragmentShaderBinary.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load fragment shader: '%s'", VKTS_FRAGMENT_SHADER_NAME);

		return VK_FALSE;
	}

	//

	vertexShaderModule = vkts::shaderModuleCreate(VKTS_VERTEX_SHADER_NAME, device->getDevice(), 0, vertexShaderBinary->getSize(), (uint32_t*)vertexShaderBinary->getData());

	if (!vertexShaderModule.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create vertex shader module.");

		return VK_FALSE;
	}

	fragmentShaderModule = vkts::shaderModuleCreate(VKTS_FRAGMENT_SHADER_NAME, device->getDevice(), 0, fragmentShaderBinary->getSize(), (uint32_t*)fragmentShaderBinary->getData());

	if (!fragmentShaderModule.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create fragment shader module.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildVertexBuffer()
{
	// Window clip origin is upper left.
	static const float vertices[4 * (4 + 2)] = { -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f };

	VkResult result;

	vertexBuffer = vkts::bufferCreate(device->getDevice(), 0, sizeof(vertices), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE, 0, nullptr);

	if (!vertexBuffer.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create vertex buffer.");

		return VK_FALSE;
	}

	VkMemoryRequirements memoryRequirements;

	vertexBuffer->getBufferMemoryRequirements(memoryRequirements);

	VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;

	physicalDevice->getPhysicalDeviceMemoryProperties(physicalDeviceMemoryProperties);

	deviceMemoryVertexBuffer = vkts::deviceMemoryCreate(device->getDevice(), memoryRequirements, VK_MAX_MEMORY_TYPES, physicalDeviceMemoryProperties.memoryTypes, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	if (!deviceMemoryVertexBuffer.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not allocate memory.");

		return VK_FALSE;
	}

	result = deviceMemoryVertexBuffer->upload(0, 0, vertices, sizeof(vertices));

	if (result != VK_SUCCESS)
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not upload data.");

		return VK_FALSE;
	}

	result = vertexBuffer->bindBufferMemory(deviceMemoryVertexBuffer->getDeviceMemory(), 0);

	if (result != VK_SUCCESS)
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not bind buffer memory.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildResources( const vkts::IUpdateThreadContext& updateContext)
{
	VkResult result;

	//

	auto lastSwapchain = swapchain;

	VkSwapchainKHR oldSwapchain = lastSwapchain.get() ? lastSwapchain->getSwapchain() : VK_NULL_HANDLE;

	swapchain = vkts::wsiSwapchainCreate(physicalDevice->getPhysicalDevice(), device->getDevice(), 0, surface->getSurface(), VKTS_NUMBER_BUFFERS, 1, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_SHARING_MODE_EXCLUSIVE, 0, nullptr, VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, VK_PRESENT_MODE_FIFO_KHR, VK_TRUE, oldSwapchain);

	if (!swapchain.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create swap chain.");

		return VK_FALSE;
	}

	//

    swapchainImagesCount = (uint32_t)swapchain->getAllSwapchainImages().size();

    if (swapchainImagesCount == 0)
    {
        vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not get swap chain images count.");

        return VK_FALSE;
    }

    swapchainImageView = vkts::SmartPointerVector<vkts::IImageViewSP>(swapchainImagesCount);
    framebuffer = vkts::SmartPointerVector<vkts::IFramebufferSP>(swapchainImagesCount);
    cmdBuffer = vkts::SmartPointerVector<vkts::ICommandBuffersSP>(swapchainImagesCount);

    //

	if (lastSwapchain.get())
	{
		lastSwapchain->destroy();
	}

	//

	if (!buildRenderPass())
	{
		return VK_FALSE;
	}

	if (!buildPipeline())
	{
		return VK_FALSE;
	}

	//

	vkts::IImageSP stageImage;
	vkts::IDeviceMemorySP stageDeviceImageObject;

	vkts::ICommandBuffersSP updateCmdBuffer = vkts::commandBuffersCreate(device->getDevice(), commandPool->getCmdPool(), VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);

	if (!updateCmdBuffer.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create command buffer.");

		return VK_FALSE;
	}

	result = updateCmdBuffer->beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);

	if (result != VK_SUCCESS)
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not begin command buffer.");

		return VK_FALSE;
	}

	VkBool32 doUpdateDescriptorSets = VK_FALSE;

	if (!image.get())
	{
		if (!buildTexture(updateCmdBuffer, stageImage, stageDeviceImageObject))
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build texture.");

			return VK_FALSE;
		}

		doUpdateDescriptorSets = VK_TRUE;
	}

	result = updateCmdBuffer->endCommandBuffer();

	if (result != VK_SUCCESS)
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not end command buffer.");

		return VK_FALSE;
	}

	VkSubmitInfo submitInfo{};

	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = nullptr;
	submitInfo.commandBufferCount = updateCmdBuffer->getCommandBufferCount();
	submitInfo.pCommandBuffers = updateCmdBuffer->getCommandBuffers();
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = nullptr;

	result = queue->submit(1, &submitInfo, VK_NULL_HANDLE);

	if (result != VK_SUCCESS)
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not submit queue.");

		return VK_FALSE;
	}

	result = queue->waitIdle();

	if (result != VK_SUCCESS)
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not wait for idle queue.");

		return VK_FALSE;
	}

	updateCmdBuffer->destroy();

	if (stageImage.get() || stageDeviceImageObject.get())
	{
		destroyTexture(stageImage, stageDeviceImageObject);
	}

	//

	if (doUpdateDescriptorSets)
	{
		if (!updateDescriptorSets())
		{
			return VK_FALSE;
		}
	}

	for (int32_t i = 0; i < (int32_t)swapchainImagesCount; i++)
	{
		if (!buildSwapchainImageView(i))
		{
			return VK_FALSE;
		}

		if (!buildFramebuffer(i))
		{
			return VK_FALSE;
		}

		if (!buildCmdBuffer(i))
		{
			return VK_FALSE;
		}
	}

	return VK_TRUE;
}

void Example::terminateResources(const vkts::IUpdateThreadContext& updateContext)
{
	if (device.get())
	{
		for (int32_t i = 0; i < (int32_t)swapchainImagesCount; i++)
		{
			if (cmdBuffer[i].get())
			{
				cmdBuffer[i]->destroy();
			}

			if (framebuffer[i].get())
			{
				framebuffer[i]->destroy();
			}

			if (swapchainImageView[i].get())
			{
				swapchainImageView[i]->destroy();
			}
		}

		if (pipeline.get())
		{
			pipeline->destroy();
		}

		if (renderPass.get())
		{
			renderPass->destroy();
		}
	}
}

//
// Vulkan initialization.
//
VkBool32 Example::init(const vkts::IUpdateThreadContext& updateContext)
{
	if (!visualContext->isWindowAttached(windowIndex))
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not get window.");

		return VK_FALSE;
	}

	//

	surface->hasCurrentExtentChanged(physicalDevice->getPhysicalDevice());

	//

	commandPool = vkts::commandPoolCreate(device->getDevice(), 0, queue->getQueueFamilyIndex());

	if (!commandPool.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not get command pool.");

		return VK_FALSE;
	}

    //

    imageAcquiredSemaphore = vkts::semaphoreCreate(device->getDevice(), 0);

    if (!imageAcquiredSemaphore.get())
    {
        vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create semaphore.");

        return VK_FALSE;
    }

    renderingCompleteSemaphore = vkts::semaphoreCreate(device->getDevice(), 0);

    if (!renderingCompleteSemaphore.get())
    {
        vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create semaphore.");

        return VK_FALSE;
    }

    //

	if (!buildVertexBuffer())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build vertex buffer.");

		return VK_FALSE;
	}

	if (!buildShader())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build shader.");

		return VK_FALSE;
	}

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
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build pipeline cache.");

		return VK_FALSE;
	}

	//

	if (!buildResources(updateContext))
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build resources.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

//
// Vulkan update.
//
VkBool32 Example::update(const vkts::IUpdateThreadContext& updateContext)
{

	VkResult result = VK_SUCCESS;

	//

	if (surface->hasCurrentExtentChanged(physicalDevice->getPhysicalDevice()))
	{
		const auto& currentExtent = surface->getCurrentExtent(physicalDevice->getPhysicalDevice(), VK_FALSE);

		if (currentExtent.width == 0 || currentExtent.height == 0)
		{
			return VK_TRUE;
		}

		result = VK_ERROR_OUT_OF_DATE_KHR;
	}

	//

	uint32_t currentBuffer;

	if (result == VK_SUCCESS)
	{
		result = swapchain->acquireNextImage(UINT64_MAX, imageAcquiredSemaphore->getSemaphore(), VK_NULL_HANDLE, currentBuffer);
	}

	if (result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR)
	{
        VkSemaphore waitSemaphores = imageAcquiredSemaphore->getSemaphore();
        VkSemaphore signalSemaphores = renderingCompleteSemaphore->getSemaphore();


        VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

		VkSubmitInfo submitInfo{};

		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &waitSemaphores;
        submitInfo.pWaitDstStageMask = &waitDstStageMask;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = cmdBuffer[currentBuffer]->getCommandBuffers();
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &signalSemaphores;

		result = queue->submit(1, &submitInfo, VK_NULL_HANDLE);

		if (result != VK_SUCCESS)
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not submit queue.");

			return VK_FALSE;
		}

        waitSemaphores = renderingCompleteSemaphore->getSemaphore();

        VkSwapchainKHR swapchains = swapchain->getSwapchain();

        result = swapchain->queuePresent(queue->getQueue(), 1, &waitSemaphores, 1, &swapchains, &currentBuffer, nullptr);

		if (result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR)
		{
			result = queue->waitIdle();

			if (result != VK_SUCCESS)
			{
				vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not wait for idle queue.");

				return VK_FALSE;
			}
		}
		else
		{
			if (result == VK_ERROR_OUT_OF_DATE_KHR)
			{
				terminateResources(updateContext);

				if (!buildResources(updateContext))
				{
					vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build resources.");

					return VK_FALSE;
				}
			}
			else
			{
				vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not present queue.");

				return VK_FALSE;
			}
		}
	}
	else
	{
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			terminateResources(updateContext);

			if (!buildResources(updateContext))
			{
				vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build resources.");

				return VK_FALSE;
			}
		}
		else
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not acquire next image.");

			return VK_FALSE;
		}
	}

    return VK_TRUE;
}

//
// Vulkan termination.
//
void Example::terminate(const vkts::IUpdateThreadContext& updateContext)
{
	if (device.get())
	{
		terminateResources(updateContext);

		//

		if (swapchain.get())
		{
			swapchain->destroy();
		}

		if (pipelineLayout.get())
		{
			pipelineLayout->destroy();
		}

		if (vertexShaderModule.get())
		{
			vertexShaderModule->destroy();
		}

		if (fragmentShaderModule.get())
		{
			fragmentShaderModule->destroy();
		}

		if (vertexBuffer.get())
		{
			vertexBuffer->destroy();
		}

		if (deviceMemoryVertexBuffer.get())
		{
			deviceMemoryVertexBuffer->destroy();
		}

		if (descriptorSet != VK_NULL_HANDLE)
		{
			vkFreeDescriptorSets(device->getDevice(), descriptorPool, 1, &descriptorSet);

			descriptorSet = VK_NULL_HANDLE;
		}

		if (descriptorPool != VK_NULL_HANDLE)
		{
			vkDestroyDescriptorPool(device->getDevice(), descriptorPool, nullptr);

			descriptorPool = VK_NULL_HANDLE;
		}

		if (descriptorSetLayout != VK_NULL_HANDLE)
		{
			vkDestroyDescriptorSetLayout(device->getDevice(), descriptorSetLayout, nullptr);

			descriptorSetLayout = VK_NULL_HANDLE;
		}

		if (imageView.get())
		{
			imageView->destroy();
		}

		if (sampler.get())
		{
			sampler->destroy();
		}

		destroyTexture(image, deviceImageObject);

        if (renderingCompleteSemaphore.get())
        {
            renderingCompleteSemaphore->destroy();
        }

        if (imageAcquiredSemaphore.get())
        {
            imageAcquiredSemaphore->destroy();
        }

		if (commandPool.get())
		{
			commandPool->destroy();
		}
	}
}
