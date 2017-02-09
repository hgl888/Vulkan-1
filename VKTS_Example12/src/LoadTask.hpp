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

#ifndef LOADTASK_HPP_
#define LOADTASK_HPP_

#include <vkts/vkts.hpp>

#define VKTS_SCENE_NAME "buster_drone/buster_drone.vkts"
//#define VKTS_SCENE_NAME "cerberus/cerberus.vkts"

#define VKTS_ENVIRONMENT_SCENE_NAME "primitives/sphere.vkts"

#define VKTS_NUMBER_DYNAMIC_UNIFORM_BUFFERS 3
#define VKTS_MAX_NUMBER_BUFFERS 3

class LoadTask : public vkts::ITask
{

private:

	const vkts::IContextObjectSP contextObject;

	const vkts::IRenderPassSP renderPass;
	const vkts::SmartPointerVector<vkts::IShaderModuleSP>& allBSDFVertexShaderModules;

	const vkts::IDescriptorSetLayoutSP environmentDescriptorSetLayout;

	vkts::ISceneRenderFactorySP& renderFactory;
	vkts::ISceneManagerSP& sceneManager;
	vkts::ISceneFactorySP& sceneFactory;
	vkts::ISceneSP& scene;

    vkts::ISceneRenderFactorySP& environmentRenderFactory;
	vkts::ISceneManagerSP& environmentSceneManager;
	vkts::ISceneFactorySP& environmentSceneFactory;
	vkts::ISceneSP& environmentScene;

	vkts::ICommandPoolSP commandPool;
	vkts::ICommandBuffersSP cmdBuffer;
	vkts::ICommandObjectSP commandObject;

protected:

	virtual VkBool32 execute() override;

public:

	LoadTask(const vkts::IContextObjectSP& contextObject, const vkts::IRenderPassSP& renderPass, const vkts::SmartPointerVector<vkts::IShaderModuleSP>& allBSDFVertexShaderModules, const vkts::IDescriptorSetLayoutSP environmentDescriptorSetLayout, vkts::ISceneRenderFactorySP& renderFactory, vkts::ISceneManagerSP& sceneManager, vkts::ISceneFactorySP& sceneFactory, vkts::ISceneSP& scene, vkts::ISceneRenderFactorySP& environmentRenderFactory, vkts::ISceneManagerSP& environmentSceneManager, vkts::ISceneFactorySP& environmentSceneFactory, vkts::ISceneSP& environmentScene);
	virtual ~LoadTask();

    VkCommandBuffer getCommandBuffer() const;

};

typedef std::shared_ptr<LoadTask> ILoadTaskSP;

#endif /* LOADTASK_HPP_ */
