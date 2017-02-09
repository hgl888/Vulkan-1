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
#include "PhysicalDevice.hpp"

namespace vkts
{

uint32_t VKTS_APIENTRY physicalDeviceGetCount(const VkInstance instance)
{
    if (!instance)
    {
        return 0;
    }

    uint32_t physicalDeviceCount = 0;

    VkResult result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

    if (result != VK_SUCCESS)
    {
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not get physical devices count.");

        return 0;
    }

    return physicalDeviceCount;
}

IPhysicalDeviceSP VKTS_APIENTRY physicalDeviceCreate(const VkInstance instance, const uint32_t index)
{
    if (!instance)
    {
        return IPhysicalDeviceSP();
    }

    uint32_t physicalDeviceCount;

    VkResult result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

    if (result != VK_SUCCESS)
    {
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not get physical devices count.");

        return IPhysicalDeviceSP();
    }

    if (index >= physicalDeviceCount)
    {
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Invalid physical device index %u >= %u.", index, physicalDeviceCount);

        return IPhysicalDeviceSP();
    }

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);

    result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, &physicalDevices[0]);

    if (result != VK_SUCCESS)
    {
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not enumerate physical devices.");

        return IPhysicalDeviceSP();
    }


    auto currentPhysicalDevice = IPhysicalDeviceSP(new PhysicalDevice(physicalDevices[index], index, instance));

    if (!currentPhysicalDevice.get())
    {
    	return IPhysicalDeviceSP();
    }

    VkPhysicalDeviceProperties physicalDeviceProperties{};

    currentPhysicalDevice->getPhysicalDeviceProperties(physicalDeviceProperties);

    logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Created physical device '%s'", physicalDeviceProperties.deviceName);

    return currentPhysicalDevice;
}

}
