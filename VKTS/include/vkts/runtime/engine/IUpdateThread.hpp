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

#ifndef VKTS_IUPDATETHREAD_HPP_
#define VKTS_IUPDATETHREAD_HPP_

#include <vkts/runtime/vkts_runtime.hpp>

namespace vkts
{

class IUpdateThread
{

public:

    IUpdateThread()
    {
    }

    virtual ~IUpdateThread()
    {
    }

    virtual VkBool32 init(const IUpdateThreadContext& updateContext) = 0;

    virtual VkBool32 update(const IUpdateThreadContext& updateContext) = 0;

    virtual void terminate(const IUpdateThreadContext& updateContext) = 0;

};

typedef std::shared_ptr<IUpdateThread> IUpdateThreadSP;

} /* namespace vkts */

#endif /* VKTS_IUPDATETHREAD_HPP_ */
