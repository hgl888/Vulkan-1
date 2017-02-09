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

#ifndef VKTS_ITASK_HPP_
#define VKTS_ITASK_HPP_

#include <vkts/runtime/vkts_runtime.hpp>

namespace vkts
{

class ITask
{

    friend class TaskExecutor;

private:

    const uint64_t id;

    VkBool32 run()
    {
        return execute();
    }

protected:

    virtual VkBool32 execute() = 0;

public:

    ITask(const uint64_t id) :
    	id(id)
    {
    }

    virtual ~ITask()
    {
    }

    uint64_t getID()
    {
        return id;
    }
};

typedef std::shared_ptr<ITask> ITaskSP;

} /* namespace vkts */

#endif /* VKTS_ITASK_HPP_ */
