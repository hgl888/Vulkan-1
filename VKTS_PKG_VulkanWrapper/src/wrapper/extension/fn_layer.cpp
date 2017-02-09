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

// 1024 layers in parallel are possible.
#define VKTS_MAX_LAYERS 1024

namespace vkts
{

static const char* g_instanceLayerNames[VKTS_MAX_LAYERS]{};
static uint32_t g_instanceLayerCount = 0;

uint32_t VKTS_APIENTRY layerGetNeededInstanceLayerCount()
{
    return g_instanceLayerCount;
}

const char** VKTS_APIENTRY layerGetNeededInstanceLayerNames()
{
    return g_instanceLayerNames;
}

VkBool32 VKTS_APIENTRY layerAddInstanceLayers(const char* layer)
{
	if (g_instanceLayerCount < VKTS_MAX_LAYERS)
	{
		g_instanceLayerNames[g_instanceLayerCount] = layer;
		g_instanceLayerCount++;

		return VK_TRUE;
	}

	return VK_FALSE;
}

}
