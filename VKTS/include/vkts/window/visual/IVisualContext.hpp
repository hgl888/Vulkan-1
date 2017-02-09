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

#ifndef VKTS_IVISUALCONTEXT_HPP_
#define VKTS_IVISUALCONTEXT_HPP_

#include <vkts/window/vkts_window.hpp>

namespace vkts
{

class IVisualContext
{

public:

    IVisualContext()
    {
    }

    virtual ~IVisualContext()
    {
    }

    //
    // Display functions.
    //

    virtual VkBool32 isDisplayAttached(const int32_t displayIndex) const = 0;

    virtual const std::vector<int32_t>& getAttachedDisplayIndices() const = 0;

    virtual const glm::uvec2& getDisplayDimension(const int32_t displayIndex) const = 0;

    virtual VKTS_NATIVE_DISPLAY getNativeDisplay(const int32_t displayIndex) const = 0;

    //
    // Window functions.
    //

    virtual VkBool32 isWindowAttached(const int32_t windowIndex) const = 0;

    virtual const std::vector<int32_t>& getAttachedWindowIndices() const = 0;

    virtual const glm::uvec2& getWindowDimension(const int32_t windowIndex) const = 0;

    virtual VKTS_NATIVE_WINDOW getNativeWindow(const int32_t windowIndex) const = 0;

    //
    // Input functions
    //

    virtual VkBool32 getKey(const int32_t windowIndex, const int32_t keyIndex) const = 0;

    virtual VkBool32 getMouseButton(const int32_t windowIndex, const int32_t buttonIndex) const = 0;

    virtual const glm::ivec2& getMouseLocation(const int32_t windowIndex) const = 0;

    virtual VkBool32 isGameMouse(const int32_t windowIndex) const = 0;

    virtual int32_t getMouseWheel(const int32_t windowIndex) const = 0;

    virtual VkBool32 getGamepadButton(const int32_t windowIndex, const int32_t gamepadIndex, const int32_t buttonIndex) const = 0;

    virtual float getGamepadAxis(const int32_t windowIndex, const int32_t gamepadIndex, const int32_t axisIndex) const = 0;

    virtual VkBool32 getTouchpadPressed(const int32_t windowIndex, const int32_t slotIndex) const = 0;

    virtual const glm::ivec2& getTouchpadLocation(const int32_t windowIndex, const int32_t slotIndex) const = 0;

};

typedef std::shared_ptr<IVisualContext> IVisualContextSP;

}
/* namespace vkts */

#endif /* VKTS_IVISUALCONTEXT_HPP_ */
