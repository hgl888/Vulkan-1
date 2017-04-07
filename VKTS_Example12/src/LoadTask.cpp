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

#include "LoadTask.hpp"

VkBool32 LoadTask::execute()
{
	commandPool = vkts::commandPoolCreate(contextObject->getDevice()->getDevice(), 0, contextObject->getQueue()->getQueueFamilyIndex());

	if (!commandPool.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not get command pool.");

		return VK_FALSE;
	}

	//

	cmdBuffer = vkts::commandBuffersCreate(contextObject->getDevice()->getDevice(), commandPool->getCmdPool(), VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);

	if (!cmdBuffer.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create command buffer.");

		return VK_FALSE;
	}

	//

	commandObject = vkts::commandObjectCreate(cmdBuffer);

	if (!commandObject.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create command object.");

		return VK_FALSE;
	}

	//

	auto result = cmdBuffer->beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);

	if (result != VK_SUCCESS)
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not begin command buffer.");

		return VK_FALSE;
	}


	//

	renderFactory = vkts::sceneRenderFactoryCreate(vkts::IDescriptorSetLayoutSP(), renderPass, vkts::IPipelineCacheSP(), VKTS_MAX_NUMBER_BUFFERS);

	if (!renderFactory.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create data factory.");

		return VK_FALSE;
	}

	//

	sceneFactory = vkts::sceneFactoryCreate(renderFactory, VK_TRUE);

	if (!sceneFactory.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create factory.");

		return VK_FALSE;
	}

	//

	sceneManager = vkts::sceneManagerCreate(VK_FALSE, contextObject, commandObject);

	if (!sceneManager.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create cache.");

		return VK_FALSE;
	}

	if (allBSDFVertexShaderModules.size() == 4)
	{
		sceneManager->addVertexShaderModule(VKTS_VERTEX_BUFFER_TYPE_VERTEX | VKTS_VERTEX_BUFFER_TYPE_NORMAL | VKTS_VERTEX_BUFFER_TYPE_TEXCOORD, allBSDFVertexShaderModules[0]);
		sceneManager->addVertexShaderModule(VKTS_VERTEX_BUFFER_TYPE_VERTEX | VKTS_VERTEX_BUFFER_TYPE_NORMAL, allBSDFVertexShaderModules[1]);
		sceneManager->addVertexShaderModule(VKTS_VERTEX_BUFFER_TYPE_VERTEX | VKTS_VERTEX_BUFFER_TYPE_TANGENTS | VKTS_VERTEX_BUFFER_TYPE_TEXCOORD, allBSDFVertexShaderModules[2]);
		sceneManager->addVertexShaderModule(VKTS_VERTEX_BUFFER_TYPE_VERTEX | VKTS_VERTEX_BUFFER_TYPE_TANGENTS | VKTS_VERTEX_BUFFER_TYPE_TEXCOORD | VKTS_VERTEX_BUFFER_TYPE_BONES, allBSDFVertexShaderModules[3]);
	}

	//

	vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Loading '%s'", VKTS_SCENE_NAME);

	//

	scene = vkts::sceneLoad(VKTS_SCENE_NAME, sceneManager, sceneFactory, VK_TRUE);

	if (!scene.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load scene.");

		return VK_FALSE;
	}

	//
	//

	environmentRenderFactory = vkts::sceneRenderFactoryCreate(environmentDescriptorSetLayout, vkts::IRenderPassSP(), vkts::IPipelineCacheSP(), VKTS_MAX_NUMBER_BUFFERS);

	if (!environmentRenderFactory.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create data factory.");

		return VK_FALSE;
	}

	//

	environmentSceneFactory = vkts::sceneFactoryCreate(environmentRenderFactory);

	if (!environmentSceneFactory.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create factory.");

		return VK_FALSE;
	}

	//

	environmentSceneManager = vkts::sceneManagerCreate(VK_FALSE, contextObject, commandObject);

	if (!environmentSceneManager.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create cache.");

		return VK_FALSE;
	}

	//

	vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Loading '%s'", VKTS_ENVIRONMENT_SCENE_NAME);

	//

	environmentScene = vkts::sceneLoad(VKTS_ENVIRONMENT_SCENE_NAME, environmentSceneManager, environmentSceneFactory, VK_TRUE);

	if (!environmentScene.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load scene.");

		return VK_FALSE;
	}

	//

	result = cmdBuffer->endCommandBuffer();

	if (result != VK_SUCCESS)
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not end command buffer.");

		return VK_FALSE;
	}

	vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Number objects: %d", scene->getNumberObjects());

	return VK_TRUE;
}

LoadTask::LoadTask(const vkts::IContextObjectSP& contextObject, const vkts::IRenderPassSP& renderPass, const vkts::SmartPointerVector<vkts::IShaderModuleSP>& allBSDFVertexShaderModules, const vkts::IDescriptorSetLayoutSP environmentDescriptorSetLayout, vkts::ISceneRenderFactorySP& renderFactory, vkts::ISceneManagerSP& sceneManager, vkts::ISceneFactorySP& sceneFactory, vkts::ISceneSP& scene, vkts::ISceneRenderFactorySP& environmentRenderFactory, vkts::ISceneManagerSP& environmentSceneManager, vkts::ISceneFactorySP& environmentSceneFactory, vkts::ISceneSP& environmentScene) :
	ITask(0), contextObject(contextObject), renderPass(renderPass), allBSDFVertexShaderModules(allBSDFVertexShaderModules), environmentDescriptorSetLayout(environmentDescriptorSetLayout), renderFactory(renderFactory), sceneManager(sceneManager), sceneFactory(sceneFactory), scene(scene), environmentRenderFactory(environmentRenderFactory), environmentSceneManager(environmentSceneManager), environmentSceneFactory(environmentSceneFactory), environmentScene(environmentScene), commandPool(), cmdBuffer(), commandObject()
{
}

LoadTask::~LoadTask()
{
	if (commandObject.get())
	{
		commandObject->destroy();
	}

	if (cmdBuffer.get())
	{
		cmdBuffer->destroy();
	}

	if (commandPool.get())
	{
		commandPool->destroy();
	}
}

VkCommandBuffer LoadTask::getCommandBuffer() const
{
	if (cmdBuffer.get())
	{
		return cmdBuffer->getCommandBuffer();
	}

	return VK_NULL_HANDLE;
}
