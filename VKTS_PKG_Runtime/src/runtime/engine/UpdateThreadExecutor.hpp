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

#ifndef VKTS_UPDATETHREADEXECUTOR_HPP_
#define VKTS_UPDATETHREADEXECUTOR_HPP_

#include <vkts/runtime/vkts_runtime.hpp>

#include "ExecutorSync.hpp"
#include "UpdateThreadContext.hpp"

namespace vkts
{

class UpdateThreadExecutor
{

private:

    const int32_t index;

    ExecutorSync& executorSync;

    const IUpdateThreadSP updateThread;

    const UpdateThreadContextSP updateThreadContext;

    const PFN_dispatchFunction dispatchFunction;

public:

    UpdateThreadExecutor() = delete;
    UpdateThreadExecutor(const UpdateThreadExecutor& other) = delete;
    UpdateThreadExecutor(UpdateThreadExecutor&& other) = delete;
    UpdateThreadExecutor(const int32_t index, ExecutorSync& executorSync, const IUpdateThreadSP& updateThread, const UpdateThreadContextSP& updateThreadContext, const PFN_dispatchFunction dispatchFunction);
    ~UpdateThreadExecutor();

    UpdateThreadExecutor& operator =(const UpdateThreadExecutor& other) = delete;

    UpdateThreadExecutor& operator =(UpdateThreadExecutor && other) = delete;

    int32_t getIndex() const;

    void run() const;

};

typedef std::shared_ptr<UpdateThreadExecutor> UpdateThreadExecutorSP;

} /* namespace vkts */

#endif /* VKTS_UPDATETHREADEXECUTOR_HPP_ */
