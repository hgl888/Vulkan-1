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

#ifndef VKTS_FN_BINARY_BUFFER_HPP_
#define VKTS_FN_BINARY_BUFFER_HPP_

#include <vkts/core/vkts_core.hpp>

namespace vkts
{

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL IBinaryBufferSP VKTS_APIENTRY binaryBufferCreate(const std::vector<uint8_t>& data);

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL IBinaryBufferSP VKTS_APIENTRY binaryBufferCreate(const uint32_t size);

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL IBinaryBufferSP VKTS_APIENTRY binaryBufferCreate(const uint8_t* data, const uint32_t size);

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL IBinaryBufferSP VKTS_APIENTRY binaryBufferCreate(const void* data, const uint32_t size);

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL IBinaryBufferSP VKTS_APIENTRY binaryBufferCreate(const uint32_t* data, const uint32_t size);

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL IBinaryBufferSP VKTS_APIENTRY binaryBufferCreate(const uint64_t* data, const uint32_t size);

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL IBinaryBufferSP VKTS_APIENTRY binaryBufferCreate(const float* data, const uint32_t size);

}

#endif /* VKTS_BINARY_BUFFER_HPP_ */
