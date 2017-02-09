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

#include <vkts/window/vkts_window.hpp>

#include "fn_visual_internal.hpp"

#include "fn_visual_display_internal.hpp"

#define VKTS_WINDOWS_MAX_WINDOWS 64

namespace vkts
{

static VkInstance g_instance = VK_NULL_HANDLE;

static VkPhysicalDevice g_physicalDevice = VK_NULL_HANDLE;

static std::vector<VkDisplayPropertiesKHR> g_allDisplayProperties;


typedef struct _VKTS_NATIVE_DISPLAY_CONTAINER
{

	uint32_t width;
	uint32_t height;

	VkDisplayModeKHR oldMode;

    NativeDisplaySP display;

} VKTS_NATIVE_DISPLAY_CONTAINER;

static std::map<int32_t, VKTS_NATIVE_DISPLAY_CONTAINER*> g_allDisplays;

static NativeDisplaySP g_defaultDisplay = nullptr;


typedef struct _VKTS_NATIVE_WINDOW_CONTAINER
{

	VkDisplaySurfaceCreateInfoKHR displaySurfaceCreateInfo;

    NativeWindowSP window;

} VKTS_NATIVE_WINDOW_CONTAINER;

static std::map<int32_t, VKTS_NATIVE_WINDOW_CONTAINER*> g_allWindows;

static int32_t g_numberWindows = 0;

static uint64_t g_windowBits = 0;

VkBool32 VKTS_APIENTRY _visualInit(const VkInstance instance, const VkPhysicalDevice physicalDevice)
{
    if (!instance || !physicalDevice)
    {
    	return VK_FALSE;
    }

    //

    // Native displays and planes are already initialized or this is done automativally.

	//

	VkResult result;

	uint32_t propertyCount;

    //

	result = vkGetPhysicalDeviceDisplayPropertiesKHR(physicalDevice, &propertyCount, nullptr);

	if (result != VK_SUCCESS || propertyCount == 0)
	{
		return VK_FALSE;
	}

	g_allDisplayProperties = std::vector<VkDisplayPropertiesKHR>(propertyCount);

	result = vkGetPhysicalDeviceDisplayPropertiesKHR(physicalDevice, &propertyCount, &g_allDisplayProperties[0]);

	if (result != VK_SUCCESS)
	{
		g_allDisplayProperties.clear();

		return VK_FALSE;
	}

	//

	if (!_visualInitDisplay(instance, physicalDevice))
	{
		g_allDisplayProperties.clear();

		return VK_FALSE;
	}

	//

    g_instance = instance;

    g_physicalDevice = physicalDevice;

    return VK_TRUE;
}

VkBool32 VKTS_APIENTRY _visualDispatchMessages()
{
    return _visualDispatchMessagesDisplay();
}

//

int32_t VKTS_APIENTRY _visualGetNumberDisplays()
{
	return (int32_t)g_allDisplayProperties.size();
}

static VkDisplayModeKHR _visualInitDisplay(const int32_t displayIndex)
{
	if (displayIndex < 0 || displayIndex >= (int32_t)g_allDisplayProperties.size())
	{
		return VK_NULL_HANDLE;
	}

	//

	VkResult result;

	uint32_t propertyCount;

	//

	result = vkGetDisplayModePropertiesKHR(g_physicalDevice, g_allDisplayProperties[displayIndex].display, &propertyCount, nullptr);

	if (result != VK_SUCCESS || propertyCount == 0)
	{
		return VK_NULL_HANDLE;
	}

	std::vector<VkDisplayModePropertiesKHR> allDisplayModeProperties = std::vector<VkDisplayModePropertiesKHR>(propertyCount);

	result = vkGetDisplayModePropertiesKHR(g_physicalDevice, g_allDisplayProperties[displayIndex].display, &propertyCount, &allDisplayModeProperties[0]);

	if (result != VK_SUCCESS)
	{
		return VK_NULL_HANDLE;
	}

	uint32_t bestMatch = 0;
	uint32_t refreshRate = 0;

	for (uint32_t i = 0; i < propertyCount; i++)
	{
		if (allDisplayModeProperties[i].parameters.visibleRegion.width == g_allDisplayProperties[displayIndex].physicalResolution.width && allDisplayModeProperties[i].parameters.visibleRegion.height == g_allDisplayProperties[displayIndex].physicalResolution.height && allDisplayModeProperties[i].parameters.refreshRate > refreshRate)
		{
			bestMatch = i;

			refreshRate = allDisplayModeProperties[i].parameters.refreshRate;
		}
	}

	if (refreshRate == 0)
	{
		return VK_NULL_HANDLE;
	}

	//
    
    return allDisplayModeProperties[bestMatch].displayMode;
}

INativeDisplayWP VKTS_APIENTRY _visualCreateDefaultDisplay()
{
	if (g_defaultDisplay.get())
	{
		return g_defaultDisplay;
	}

	if (_visualGetNumberDisplays() == 0)
	{
		return INativeDisplayWP();
	}

	//

	auto currentDisplayMode = _visualInitDisplay(0);

	if (currentDisplayMode == VK_NULL_HANDLE)
	{
		return INativeDisplayWP();
	}

	//

	g_defaultDisplay = NativeDisplaySP(new NativeDisplay(0, VK_TRUE, g_allDisplayProperties[0].physicalResolution.width, g_allDisplayProperties[0].physicalResolution.height, g_allDisplayProperties[0].display));

    if (!g_defaultDisplay.get())
    {
        return INativeDisplayWP();
    }

    VKTS_NATIVE_DISPLAY_CONTAINER* currentDisplayContainer = new VKTS_NATIVE_DISPLAY_CONTAINER();

    if (!currentDisplayContainer)
    {
        _visualDestroyDisplay(g_defaultDisplay);

        return INativeDisplayWP();
    }

    currentDisplayContainer->width = g_allDisplayProperties[0].physicalResolution.width;
    currentDisplayContainer->height = g_allDisplayProperties[0].physicalResolution.height;
    currentDisplayContainer->oldMode = currentDisplayMode;
    currentDisplayContainer->display = g_defaultDisplay;

    g_allDisplays[0] = currentDisplayContainer;

    return g_defaultDisplay;
}

INativeDisplayWP VKTS_APIENTRY _visualCreateDisplay(const int32_t displayIndex)
{
    if (displayIndex < 0 || displayIndex >= _visualGetNumberDisplays())
    {
        return INativeDisplayWP();
    }

    auto walkerDisplayContainer = g_allDisplays.find(displayIndex);

    if (walkerDisplayContainer != g_allDisplays.end())
    {
        if (walkerDisplayContainer->second)
        {
            return walkerDisplayContainer->second->display;
        }

        return INativeDisplayWP();
    }

    //

	auto currentDisplayMode = _visualInitDisplay(displayIndex);

	if (currentDisplayMode == VK_NULL_HANDLE)
	{
		return INativeDisplayWP();
	}

    //

	auto currentDisplay = NativeDisplaySP(new NativeDisplay(displayIndex, displayIndex == 0, g_allDisplayProperties[displayIndex].physicalResolution.width, g_allDisplayProperties[displayIndex].physicalResolution.height, g_allDisplayProperties[displayIndex].display));

    if (!currentDisplay.get())
    {
        return INativeDisplayWP();
    }

    VKTS_NATIVE_DISPLAY_CONTAINER* currentDisplayContainer = new VKTS_NATIVE_DISPLAY_CONTAINER();

    if (!currentDisplayContainer)
    {
        _visualDestroyDisplay(currentDisplay);

        return INativeDisplayWP();
    }

    currentDisplayContainer->width = g_allDisplayProperties[displayIndex].physicalResolution.width;
    currentDisplayContainer->height = g_allDisplayProperties[displayIndex].physicalResolution.height;
    currentDisplayContainer->oldMode = currentDisplayMode;
    currentDisplayContainer->display = currentDisplay;

    g_allDisplays[displayIndex] = currentDisplayContainer;

    if (currentDisplay->isDefault())
    {
    	g_defaultDisplay = currentDisplay;
    }

    return currentDisplay;
}

NativeDisplaySP VKTS_APIENTRY _visualGetDisplay(const int32_t displayIndex)
{
    if (displayIndex < 0 || displayIndex >= _visualGetNumberDisplays())
    {
        return NativeDisplaySP();
    }

    auto foundDisplayContainer = g_allDisplays.find(displayIndex);

    if (foundDisplayContainer != g_allDisplays.end())
    {
        return foundDisplayContainer->second->display;
    }

    return NativeDisplaySP();
}

const SmartPointerVector<NativeDisplaySP>& VKTS_APIENTRY _visualGetActiveDisplays()
{
    static SmartPointerVector<NativeDisplaySP> displayList;

    displayList.clear();

    for (const auto currentPair : g_allDisplays)
    {
        displayList.append(currentPair.second->display);
    }

    return displayList;
}

void VKTS_APIENTRY _visualDestroyDisplay(const NativeDisplaySP& display)
{
    if (!display.get())
    {
        return;
    }

    auto walkerDisplayContainer = g_allDisplays.find(display->getIndex());

    if (walkerDisplayContainer == g_allDisplays.end())
    {
        return;
    }

    if (display.get() == g_defaultDisplay.get())
    {
        g_defaultDisplay = NativeDisplaySP();
    }

    if (walkerDisplayContainer->second)
    {
    	walkerDisplayContainer->second->display = NativeDisplaySP();

        delete walkerDisplayContainer->second;

        walkerDisplayContainer->second = nullptr;
    }

    g_allDisplays.erase(walkerDisplayContainer);
}

//

VkBool32 VKTS_APIENTRY _visualGetWindowCapabilities(VkTsWindowCapabilites& windowCapabilites)
{
	windowCapabilites.titleSetable = VK_FALSE;
	windowCapabilites.widthSetable = VK_TRUE;
	windowCapabilites.heightSetable = VK_TRUE;
	windowCapabilites.fullscreenSetable = VK_TRUE;
	windowCapabilites.resizeSetable = VK_FALSE;
	windowCapabilites.gameCursorSetable = VK_FALSE;

	windowCapabilites.isTitleVisible = VK_FALSE;
	//
	//
	windowCapabilites.isFullscreen = VK_FALSE;
	windowCapabilites.isResizable = VK_FALSE;
	windowCapabilites.isGameCursor = VK_FALSE;

	return VK_TRUE;
}

INativeWindowWP VKTS_APIENTRY _visualCreateWindow(const INativeDisplayWP& display, const char* title, const int32_t width, const int32_t height, const VkBool32 fullscreen, const VkBool32 resize, const VkBool32 gameCursor)
{
	auto currentNativeDisplay = display.lock();

	if (!currentNativeDisplay.get())
	{
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No native display.");

		return INativeWindowWP();
	}

	if (currentNativeDisplay->getIndex() >= _visualGetNumberDisplays())
	{
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Invalid native display.");

        return INativeWindowWP();
	}

    //

    uint32_t currentWindowBit = 1;
    int32_t currentWindowIndex = 0;

    if (g_numberWindows == VKTS_WINDOWS_MAX_WINDOWS)
    {
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Maximum windows reached.");

        return INativeWindowWP();
    }

    while (currentWindowBit & g_windowBits)
    {
        currentWindowBit = currentWindowBit << 1;

        currentWindowIndex++;
    }

    //

	VkResult result;

	uint32_t propertyCount;

	//

	VkDisplayModeKHR displayMode = g_allDisplays[currentNativeDisplay->getIndex()]->oldMode;

    if (fullscreen)
    {
		auto currentDisplayContainer = g_allDisplays[currentNativeDisplay->getIndex()];

		result = vkGetDisplayModePropertiesKHR(g_physicalDevice, g_allDisplayProperties[currentNativeDisplay->getIndex()].display, &propertyCount, nullptr);

		if (result != VK_SUCCESS || propertyCount == 0)
		{
	        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not get display mode properties count.");

	        return INativeWindowWP();
		}

		std::vector<VkDisplayModePropertiesKHR> allDisplayModeProperties = std::vector<VkDisplayModePropertiesKHR>(propertyCount);

		result = vkGetDisplayModePropertiesKHR(g_physicalDevice, g_allDisplayProperties[currentNativeDisplay->getIndex()].display, &propertyCount, &allDisplayModeProperties[0]);

		if (result != VK_SUCCESS)
		{
	        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not get display mode properties.");

			return INativeWindowWP();
		}

		uint32_t bestMatch = 0;
		uint32_t refreshRate = 0;

		for (uint32_t i = 0; i < propertyCount; i++)
		{
			if (allDisplayModeProperties[i].parameters.visibleRegion.width == (uint32_t)width && allDisplayModeProperties[i].parameters.visibleRegion.height == (uint32_t)height && allDisplayModeProperties[i].parameters.refreshRate > refreshRate)
			{
				bestMatch = i;

				refreshRate = allDisplayModeProperties[i].parameters.refreshRate;
			}
		}

		if (refreshRate == 0)
		{
	        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Invalid refresh rate.");

			return INativeWindowWP();
		}

		//
        
        displayMode = allDisplayModeProperties[bestMatch].displayMode;

		currentDisplayContainer->display->setDimension(glm::uvec2(allDisplayModeProperties[bestMatch].parameters.visibleRegion.width, allDisplayModeProperties[bestMatch].parameters.visibleRegion.height));

        // Resolution is changed automatically during Vulkan API call.
    }

	//
	// Find plane.
	//

	result = vkGetPhysicalDeviceDisplayPlanePropertiesKHR(g_physicalDevice, &propertyCount, nullptr);

	if (result != VK_SUCCESS || propertyCount == 0)
	{
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not get physical device display plane properties.");

		return INativeWindowWP();
	}

	std::vector<VkDisplayPlanePropertiesKHR> allDisplayPlaneProperties = std::vector<VkDisplayPlanePropertiesKHR>(propertyCount);

	result = vkGetPhysicalDeviceDisplayPlanePropertiesKHR(g_physicalDevice, &propertyCount, &allDisplayPlaneProperties[0]);

	if (result != VK_SUCCESS)
	{
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not get physical device display plane properties.");

		return INativeWindowWP();
	}

	//

	VkDisplayKHR currentDisplay = VK_NULL_HANDLE;
	uint32_t currentPlaneIndex = 0;
	uint32_t currentPlaneStackIndex = 0;
	VkExtent2D currentExtent = {(uint32_t)width, (uint32_t)height};

	// Go through all planes.
	for (uint32_t planeIndex = 0; planeIndex < propertyCount; planeIndex++)
	{
		// Check, if plane is attached to a display.
		if (allDisplayPlaneProperties[planeIndex].currentDisplay != VK_NULL_HANDLE)
		{
			uint32_t displayCount;

			result = vkGetDisplayPlaneSupportedDisplaysKHR(g_physicalDevice, planeIndex, &displayCount, nullptr);

			if (result != VK_SUCCESS|| displayCount == 0)
			{
		        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not get display plane supported displays count.");

				return INativeWindowWP();
			}

			std::vector<VkDisplayKHR> allDisplays = std::vector<VkDisplayKHR>(displayCount);

			result = vkGetDisplayPlaneSupportedDisplaysKHR(g_physicalDevice, planeIndex, &displayCount, &allDisplays[0]);

			if (result != VK_SUCCESS)
			{
		        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not get display plane supported displays.");

				return INativeWindowWP();
			}

			for (uint32_t displayIndex = 0; displayIndex < displayCount; displayIndex++)
			{
				// Check, if plane can be used on requested display.
				if (allDisplays[displayIndex] == g_allDisplayProperties[currentNativeDisplay->getIndex()].display)
				{
					VkDisplayPlaneCapabilitiesKHR displayPlaneCapabilities;

					result = vkGetDisplayPlaneCapabilitiesKHR(g_physicalDevice, displayMode, planeIndex, &displayPlaneCapabilities);

					if (result != VK_SUCCESS)
					{
				        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not get display plane capabilities.");

						return INativeWindowWP();
					}

					// Check, if plane fulfills extent requirements.

					if (displayPlaneCapabilities.minDstExtent.width > (uint32_t)width || displayPlaneCapabilities.minDstExtent.height > (uint32_t)height)
					{
				        logPrint(VKTS_LOG_DEBUG, __FILE__, __LINE__, "Extent to small.");

						continue;
					}

					if (displayPlaneCapabilities.maxDstExtent.width < (uint32_t)width || displayPlaneCapabilities.maxDstExtent.height < (uint32_t)height)
					{
				        logPrint(VKTS_LOG_DEBUG, __FILE__, __LINE__, "Extent to large.");

						continue;
					}

					//

					currentPlaneIndex = planeIndex;

					currentDisplay = allDisplayPlaneProperties[planeIndex].currentDisplay;

					currentPlaneStackIndex = allDisplayPlaneProperties[planeIndex].currentStackIndex;

					break;
				}
			}
		}

		if (currentDisplay != VK_NULL_HANDLE)
		{
			break;
		}
	}

	if (currentDisplay == VK_NULL_HANDLE)
	{
        logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "No plane found. Falling back to plane with current resolution.");

		currentPlaneIndex = 0;

		currentDisplay = allDisplayPlaneProperties[currentPlaneIndex].currentDisplay;

		currentPlaneStackIndex = allDisplayPlaneProperties[currentPlaneIndex].currentStackIndex;

		VkDisplayPlaneCapabilitiesKHR displayPlaneCapabilities;

		result = vkGetDisplayPlaneCapabilitiesKHR(g_physicalDevice, displayMode, currentPlaneIndex, &displayPlaneCapabilities);

		if (result != VK_SUCCESS)
		{
	        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not get display plane capabilities.");

			return INativeWindowWP();
		}

		currentExtent = displayPlaneCapabilities.maxDstExtent;
	}

	//

	auto currentNativeWindowContainer = new VKTS_NATIVE_WINDOW_CONTAINER();

	if (!currentNativeWindowContainer)
	{
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create native window container.");

		return INativeWindowWP();
	}

	auto currentNativeWindow = NativeWindowSP(new NativeWindow(display, &currentNativeWindowContainer->displaySurfaceCreateInfo, currentWindowIndex, title, (int32_t)currentExtent.width, (int32_t)currentExtent.height, fullscreen, VK_FALSE, gameCursor));

	memset(&currentNativeWindowContainer->displaySurfaceCreateInfo, 0, sizeof(VkDisplaySurfaceCreateInfoKHR));

	currentNativeWindowContainer->displaySurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_DISPLAY_SURFACE_CREATE_INFO_KHR;

	currentNativeWindowContainer->displaySurfaceCreateInfo.flags = 0;
	currentNativeWindowContainer->displaySurfaceCreateInfo.displayMode = displayMode;
	currentNativeWindowContainer->displaySurfaceCreateInfo.planeIndex = currentPlaneIndex;
	currentNativeWindowContainer->displaySurfaceCreateInfo.planeStackIndex = currentPlaneStackIndex;
	currentNativeWindowContainer->displaySurfaceCreateInfo.transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	currentNativeWindowContainer->displaySurfaceCreateInfo.globalAlpha = 1.0f;
	currentNativeWindowContainer->displaySurfaceCreateInfo.alphaMode = VK_DISPLAY_PLANE_ALPHA_OPAQUE_BIT_KHR;
	currentNativeWindowContainer->displaySurfaceCreateInfo.imageExtent = currentExtent;


	currentNativeWindowContainer->window = currentNativeWindow;

    g_allWindows[currentNativeWindow->getIndex()] = currentNativeWindowContainer;

    //

    _visualSetDisplayExtent(glm::ivec2((int32_t)currentExtent.width, (int32_t)currentExtent.height));

    //

    g_windowBits |= currentWindowBit;

    return currentNativeWindow;
}

NativeWindowSP VKTS_APIENTRY _visualGetWindow(const int32_t windowIndex)
{
	auto currentWindow = g_allWindows.find(windowIndex);

	if (currentWindow != g_allWindows.end())
	{
		return currentWindow->second->window;
	}

    return NativeWindowSP();
}

const SmartPointerVector<NativeWindowSP>& VKTS_APIENTRY _visualGetActiveWindows()
{
    static SmartPointerVector<NativeWindowSP> windowList;

    windowList.clear();

    for (const auto currentPair : g_allWindows)
    {
    	windowList.append(currentPair.second->window);
    }

    return windowList;
}

void VKTS_APIENTRY _visualDestroyWindow(const NativeWindowSP& window)
{
    if (!window.get())
    {
        return;
    }

    if (window->isFullscreen())
    {
        // Resize back happens when Vulkan is quit.
    }

    auto currentWindow = g_allWindows.find(window->getIndex());

    if (currentWindow != g_allWindows.end())
    {
    	auto currentWindowContainer = currentWindow->second;

    	currentWindowContainer->window = NativeWindowSP();

		g_allWindows.erase(currentWindow);

		delete currentWindowContainer;
    }
}

//

void VKTS_APIENTRY _visualTerminate()
{
	_visualTerminateDisplay();

	//

    while (g_allWindows.size() > 0)
    {
        _visualDestroyWindow(g_allWindows[0]->window);
    }
    g_allWindows.clear();

	//

    auto walkerDisplayContainer = g_allDisplays.begin();

    while (walkerDisplayContainer != g_allDisplays.end())
    {
        if (walkerDisplayContainer->second)
        {
            _visualDestroyDisplay(walkerDisplayContainer->second->display);
        }

        walkerDisplayContainer = g_allDisplays.begin();
    }

    g_allDisplays.clear();

    g_allDisplayProperties.clear();

	//

	g_physicalDevice = VK_NULL_HANDLE;

	g_instance = VK_NULL_HANDLE;

    //

    // Native display and planes not needed to be shut down.
}

//

void VKTS_APIENTRY _visualKeySet(const int32_t keyIndex, const VkBool32 pressed)
{
    for (const auto currentPair : g_allWindows)
    {
        currentPair.second->window->getKeyInput().setKey(keyIndex, pressed);
    }
}

void VKTS_APIENTRY _visualMouseSetButton(const int32_t buttonIndex, const VkBool32 pressed)
{
    for (const auto currentPair : g_allWindows)
    {
        currentPair.second->window->getMouseInput().setButton(buttonIndex, pressed);
    }
}

void VKTS_APIENTRY _visualMouseSetLocation(const glm::ivec2& location)
{
    for (const auto currentPair : g_allWindows)
    {
        currentPair.second->window->getMouseInput().setLocation(location);
    }
}

void VKTS_APIENTRY _visualMouseSetMouseWheel(const int32_t mosueWheel)
{
    for (const auto currentPair : g_allWindows)
    {
        currentPair.second->window->getMouseInput().setMouseWheel(mosueWheel);
    }
}

void VKTS_APIENTRY _visualGamepadSetButton(const int32_t gamepadIndex, const int32_t buttonIndex, const VkBool32 pressed)
{
    for (const auto currentPair : g_allWindows)
    {
    	currentPair.second->window->getGamepadInput(gamepadIndex).setButton(buttonIndex, pressed);
    }
}

void VKTS_APIENTRY _visualGamepadSetAxis(const int32_t gamepadIndex, const int32_t axisIndex, const float value)
{
    for (const auto currentPair : g_allWindows)
    {
    	currentPair.second->window->getGamepadInput(gamepadIndex).setAxis(axisIndex, value);
    }
}

//

void VKTS_APIENTRY _visualTouchpadSetLocationX(const int32_t slotIndex, const int32_t x, const int32_t rangeX, const int32_t rangeY)
{
	for (const auto currentPair : g_allWindows)
    {
		if (currentPair.second->window->getNativeDisplay().lock()->getWidth() > currentPair.second->window->getNativeDisplay().lock()->getHeight() && rangeX > rangeY)
		{
			currentPair.second->window->getTouchpadInput().setLocationX(slotIndex, x * currentPair.second->window->getNativeDisplay().lock()->getWidth() / rangeX);
		}
		else
		{
			currentPair.second->window->getTouchpadInput().setLocationY(slotIndex, x * currentPair.second->window->getNativeDisplay().lock()->getHeight() / rangeX);
		}
    }
}

void VKTS_APIENTRY _visualTouchpadSetLocationY(const int32_t slotIndex, const int32_t y, const int32_t rangeX, const int32_t rangeY)
{
	for (const auto currentPair : g_allWindows)
    {
		if (currentPair.second->window->getNativeDisplay().lock()->getWidth() > currentPair.second->window->getNativeDisplay().lock()->getHeight() && rangeX > rangeY)
		{
			currentPair.second->window->getTouchpadInput().setLocationY(slotIndex, y * currentPair.second->window->getNativeDisplay().lock()->getHeight() / rangeY);
		}
		else
		{
			currentPair.second->window->getTouchpadInput().setLocationX(slotIndex, y * currentPair.second->window->getNativeDisplay().lock()->getWidth() / rangeY);
		}
    }
}

void VKTS_APIENTRY _visualTouchpadSetPressed(const int32_t slotIndex, const VkBool32 pressed)
{
	for (const auto currentPair : g_allWindows)
    {
		currentPair.second->window->getTouchpadInput().setPressed(slotIndex, pressed);
    }
}


}
