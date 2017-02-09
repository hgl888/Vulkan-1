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

#include <vkts/vkts.hpp>

#include "Example.hpp"

static VkInstance instance = VK_NULL_HANDLE;

static VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

static vkts::INativeDisplaySP display;

static vkts::INativeWindowSP window;

static vkts::ISurfaceSP surface;

static vkts::IVisualContextSP visualContext;

static VkDevice device = VK_NULL_HANDLE;

static uint32_t queueFamilyIndex = 0;

static VkQueue queue = VK_NULL_HANDLE;

static void terminateApp()
{
	visualContext.reset();

	if (device)
	{
		queue = VK_NULL_HANDLE;

		queueFamilyIndex = 0;

		//

		vkDestroyDevice(device, nullptr);

		device = VK_NULL_HANDLE;
	}

	//

	if (surface.get())
	{
		surface->destroy();
	}

	//

	if (window.get())
	{
		window->destroy();
	}

	if (display.get())
	{
		display->destroy();
	}

	vkts::visualTerminate();

	//

	physicalDevice = VK_NULL_HANDLE;

	if (instance != VK_NULL_HANDLE)
	{
		vkts::debugDestroyDebugReportCallback(instance);

		//

		vkDestroyInstance(instance, nullptr);

		instance = VK_NULL_HANDLE;
	}

	//

	vkts::engineTerminate();
}

int main(int argc, char* argv[])
{
	//
	// Engine initialization.
	//

	if (!vkts::engineInit(vkts::visualDispatchMessages))
	{
		terminateApp();

		return -1;
	}

	vkts::logSetLevel(VKTS_LOG_INFO);

	//

	VkResult result;

	//

	if (!vkts::wsiGatherNeededInstanceExtensions())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not gather needed instance extensions.");

		terminateApp();

		return -1;
	}

	//

	uint32_t validate = 0;

	if (vkts::parameterGetUInt32(validate, std::string("-v"), argc, argv))
	{
		if (validate)
		{
			if (!vkts::validationGatherNeededInstanceLayers())
			{
				vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not gather needed instance layers.");

				terminateApp();

				return -1;
			}
		}
	}

	uint32_t debug = 0;

	if (vkts::parameterGetUInt32(debug, std::string("-d"), argc, argv) || true)
	{
		if (debug)
		{
			if (!vkts::debugGatherNeededInstanceExtensions())
			{
				vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not gather needed instance layers.");

				terminateApp();

				return -1;
			}
		}
	}

	//

	VkApplicationInfo applicationInfo{};

	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

	applicationInfo.pApplicationName = VKTS_EXAMPLE_NAME;
	applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.pEngineName = VKTS_ENGINE_NAME;
	applicationInfo.engineVersion = VK_MAKE_VERSION(VKTS_ENGINE_MAJOR, VKTS_ENGINE_MINOR, VKTS_ENGINE_PATCH);
	applicationInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);

	VkInstanceCreateInfo instanceCreateInfo{};

	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

	instanceCreateInfo.flags = 0;
	instanceCreateInfo.pApplicationInfo = &applicationInfo;
	instanceCreateInfo.enabledLayerCount = vkts::layerGetNeededInstanceLayerCount();
	instanceCreateInfo.ppEnabledLayerNames = vkts::layerGetNeededInstanceLayerNames();
	instanceCreateInfo.enabledExtensionCount = vkts::extensionGetNeededInstanceExtensionCount();
	instanceCreateInfo.ppEnabledExtensionNames = vkts::extensionGetNeededInstanceExtensionNames();

	result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);

	if (result != VK_SUCCESS)
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create instance.");

		terminateApp();

		return -1;
	}

	//

	if (debug)
	{
		if (!vkts::debugInitInstanceExtensions(instance))
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not initialize debug extensions.");

			terminateApp();

			return -1;
		}

		if (!vkts::debugCreateDebugReportCallback(instance, debug))
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create debug callback.");

			terminateApp();

			return -1;
		}
	}

	//

	uint32_t physicalDeviceCount;

	result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

	if (result != VK_SUCCESS || physicalDeviceCount == 0)
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not get physical devices count.");

		terminateApp();

		return -1;
	}

	std::vector<VkPhysicalDevice> allPhysicalDevices(physicalDeviceCount);

	result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, &allPhysicalDevices[0]);

	if (result != VK_SUCCESS || physicalDeviceCount == 0)
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not enumerate physical devices.");

		terminateApp();

		return -1;
	}

	//

	uint32_t physicalDeviceIndex = 0;

	if (vkts::parameterGetUInt32(physicalDeviceIndex, std::string("-pd"), argc, argv))
	{
		if (physicalDeviceIndex >= physicalDeviceCount)
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Physical device %u does not exist.", physicalDeviceIndex);

			terminateApp();

			return -1;
		}
	}

	//

	physicalDevice = allPhysicalDevices[physicalDeviceIndex];

	if (!vkts::wsiGatherNeededDeviceExtensions(physicalDevice))
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not gather needed device extensions.");

		terminateApp();

		return -1;
	}

	//
	// Visual initialization.
	//

	if (!vkts::visualInit(instance, physicalDevice))
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not initialize visual.");

		terminateApp();

		return -1;
	}

	display = vkts::visualCreateDefaultDisplay().lock();

	if (!display.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create display.");

		terminateApp();

		return -1;
	}

	window = vkts::visualCreateWindow(display, VKTS_EXAMPLE_NAME, 1024, 768, VK_FALSE, VK_TRUE, VK_FALSE).lock();

	if (!window.get())
	{
		window = vkts::visualGetWindow(VKTS_DEFAULT_WINDOW_INDEX).lock();

		if (!window.get())
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create window.");

			terminateApp();

			return -1;
		}
	}

	//

	surface = vkts::wsiSurfaceCreate(instance, display->getNativeDisplay(), window->getNativeWindow());

	if (!surface.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create surface.");

		terminateApp();

		return -1;
	}

	//


	uint32_t physicalDeviceQueueFamilyPropertiesCount;

	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &physicalDeviceQueueFamilyPropertiesCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilyProperties(physicalDeviceQueueFamilyPropertiesCount);

	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &physicalDeviceQueueFamilyPropertiesCount, &queueFamilyProperties[0]);

	queueFamilyIndex = physicalDeviceQueueFamilyPropertiesCount;

	uint32_t queueIndex = 0;
	VkBool32 supported;

	for (uint32_t currentQueueFamilyIndex = 0; currentQueueFamilyIndex < physicalDeviceQueueFamilyPropertiesCount; currentQueueFamilyIndex++)
	{
		result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, currentQueueFamilyIndex, surface->getSurface(), &supported);

		if (result != VK_SUCCESS)
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not get physical device surface support.");

			terminateApp();

			return -1;
		}

		if (supported && (queueFamilyProperties[currentQueueFamilyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT) && queueFamilyProperties[currentQueueFamilyIndex].queueCount > queueIndex)
		{
			queueFamilyIndex = currentQueueFamilyIndex;

			break;
		}
	}

	if (queueFamilyIndex == physicalDeviceQueueFamilyPropertiesCount)
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not find queue family index.");

		terminateApp();

		return -1;
	}

	//

	float queuePriorities[1] = { 0.0f };

	VkDeviceQueueCreateInfo deviceQueueCreateInfo{};

	deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;

	deviceQueueCreateInfo.flags = 0;
	deviceQueueCreateInfo.queueFamilyIndex = queueFamilyIndex;
	deviceQueueCreateInfo.queueCount = 1;
	deviceQueueCreateInfo.pQueuePriorities = queuePriorities;

	VkDeviceCreateInfo deviceCreateInfo{};

	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	deviceCreateInfo.flags = 0;
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
	deviceCreateInfo.enabledLayerCount = 0;
	deviceCreateInfo.ppEnabledLayerNames = nullptr;
	deviceCreateInfo.enabledExtensionCount = vkts::extensionGetNeededDeviceExtensionCount();
	deviceCreateInfo.ppEnabledExtensionNames = vkts::extensionGetNeededDeviceExtensionNames();
	deviceCreateInfo.pEnabledFeatures = nullptr;

	result = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);

	if (result != VK_SUCCESS)
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create device.");

		terminateApp();

		return -1;
	}

	//

	vkGetDeviceQueue(device, queueFamilyIndex, queueIndex, &queue);

	//
	// Example setup.
	//

	visualContext = vkts::visualCreateContext();

	if (!visualContext.get())
	{
		terminateApp();

		return -1;
	}

	//

	// Single threaded application, so it is safe to pass display and window.
	vkts::IUpdateThreadSP example = vkts::IUpdateThreadSP(new Example(instance, physicalDevice, window->getIndex(), visualContext, surface, device, queueFamilyIndex, queue));

	if (!example.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create application.");

		terminateApp();

		return -1;
	}

	// This update thread is executed in the main loop. No additional thread is launched.
	vkts::engineAddUpdateThread(example);

	//
	// Execution.
	//

	vkts::engineRun();

	//
	// Termination.
	//

	terminateApp();

	return 0;
}
