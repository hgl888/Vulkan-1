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

#ifndef VKTS_IINSTANCE_HPP_
#define VKTS_IINSTANCE_HPP_

#include <vkts/vulkan/wrapper/vkts_wrapper.hpp>

namespace vkts
{

class IInstance: public IDestroyable
{

public:

    IInstance() :
        IDestroyable()
    {
    }

    virtual ~IInstance()
    {
    }

    virtual const VkApplicationInfo& getApplicationInfo() const = 0;

    virtual const char* getApplicationName() const = 0;

    virtual uint32_t getApplicationVersion() const = 0;

    virtual const char* getEngineName() const = 0;

    virtual uint32_t getEngineVersion() const = 0;

    virtual uint32_t getApiVersion() const = 0;

    virtual const VkInstanceCreateInfo& getInstanceCreateInfo() const = 0;

    virtual VkInstanceCreateFlags getFlags() const = 0;

    // ApplicationInfo, see above
    virtual uint32_t getEnabledLayerCount() const = 0;

    virtual const char* const* getEnabledLayerNames() const = 0;

    virtual uint32_t getEnabledExtensionCount() const = 0;

    virtual const char* const* getEnabledExtensionNames() const = 0;

    virtual const VkInstance getInstance() const = 0;

};

typedef std::shared_ptr<IInstance> IInstanceSP;

} /* namespace vkts */

#endif /* VKTS_IINSTANCE_HPP_ */
