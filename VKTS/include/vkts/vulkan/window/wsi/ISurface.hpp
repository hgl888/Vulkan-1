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

#ifndef VKTS_ISURFACE_HPP_
#define VKTS_ISURFACE_HPP_

#include <vkts/vulkan/window/vkts_window.hpp>

namespace vkts
{

class ISurface: public IDestroyable
{

public:

    ISurface() :
        IDestroyable()
    {
    }

    virtual ~ISurface()
    {
    }

    virtual const VkInstance getInstance() const = 0;

    virtual const VKTS_NATIVE_DISPLAY getNativeDisplay() const = 0;

    virtual const VKTS_NATIVE_WINDOW getNativeWindow() const = 0;

    virtual const VkSurfaceKHR getSurface() const = 0;

    virtual VkFormat getFormat(const VkPhysicalDevice physicalDevice, const uint32_t index = 0) const = 0;

    virtual VkResult getPhysicalDeviceSurfaceCapabilities(const VkPhysicalDevice physicalDevice, VkSurfaceCapabilitiesKHR& surfaceCapabilities) const = 0;

    virtual VkBool32 hasCurrentExtentChanged(const VkPhysicalDevice physicalDevice) const = 0;

    virtual const VkExtent2D& getCurrentExtent(const VkPhysicalDevice physicalDevice, const VkBool32 refresh = VK_TRUE) const = 0;

};

typedef std::shared_ptr<ISurface> ISurfaceSP;

} /* namespace vkts */

#endif /* VKTS_ISURFACE_HPP_ */
