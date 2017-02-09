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

#ifndef BUILDCOMMANDTASK_HPP_
#define BUILDCOMMANDTASK_HPP_

#include <vkts/vkts.hpp>

#define VKTS_NUMBER_DYNAMIC_UNIFORM_BUFFERS 4
#define VKTS_MAX_NUMBER_BUFFERS 3

class BuildCommandTask : public vkts::ITask
{

private:

    static vkts::OverwriteDraw* overwrite;

	const vkts::IUpdateThreadContext& updateContext;

	const vkts::IContextObjectSP contextObject;

    const vkts::SmartPointerVector<vkts::IGraphicsPipelineSP>& allGraphicsPipelines;

	const vkts::ISceneSP scene;

	const std::map<uint32_t, VkTsDynamicOffset>& dynamicOffsets;

	const uint32_t objectOffset;

	const uint32_t objectStep;

	VkCommandBufferInheritanceInfo* commandBufferInheritanceInfo;

    VkExtent2D extent;

    uint32_t usedBuffer;

	vkts::ICommandPoolSP commandPool;

	vkts::SmartPointerVector<vkts::ICommandBuffersSP> cmdBuffer;

protected:

	virtual VkBool32 execute() override;

public:

    static void setOverwrite(vkts::OverwriteDraw* overwrite);

	BuildCommandTask(const uint64_t id, const vkts::IUpdateThreadContext& updateContext, const vkts::IContextObjectSP& contextObject, const vkts::SmartPointerVector<vkts::IGraphicsPipelineSP>& allGraphicsPipelines, const vkts::ISceneSP& scene, const uint32_t buffers, const std::map<uint32_t, VkTsDynamicOffset>& dynamicOffsets, const uint32_t& objectOffset, const uint32_t& objectStep);
	virtual ~BuildCommandTask();

    void setCommandBufferInheritanceInfo(VkCommandBufferInheritanceInfo* commandBufferInheritanceInfo);

    void setExtent(const VkExtent2D& extent);

    void setUsedBuffer(const uint32_t usedBuffer);

    VkCommandBuffer getCommandBuffer() const;

};

typedef std::shared_ptr<BuildCommandTask> IBuildCommandTaskSP;

#endif /* BUILDCOMMANDTASK_HPP_ */
