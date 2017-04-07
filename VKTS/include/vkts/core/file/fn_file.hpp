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

#ifndef VKTS_FN_FILE_HPP_
#define VKTS_FN_FILE_HPP_

#include <vkts/core/vkts_core.hpp>

namespace vkts
{

/**
 * Not thread Safe.
 */
VKTS_APICALL VkBool32 VKTS_APIENTRY fileInit();

/**
 * Not thread Safe.
 */
VKTS_APICALL void VKTS_APIENTRY fileSetBaseDirectory(const char* directory);

/**
 * Not thread Safe.
 */
VKTS_APICALL const char* VKTS_APIENTRY fileGetBaseDirectory();

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL IBinaryBufferSP VKTS_APIENTRY fileLoadBinary(const char* filename);

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL ITextBufferSP VKTS_APIENTRY fileLoadText(const char* filename);

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL VkBool32 VKTS_APIENTRY fileSaveBinary(const char* filename, const IBinaryBufferSP& buffer);

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL VkBool32 VKTS_APIENTRY fileSaveBinaryData(const char* filename, const void* data, const uint32_t size);

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL VkBool32 VKTS_APIENTRY fileSaveText(const char* filename, const ITextBufferSP& text);

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL VkBool32 VKTS_APIENTRY fileCreateDirectory(const char* directory);

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL VkBool32 VKTS_APIENTRY fileGetDirectory(char* directory, const char* filename);

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL VkBool32 VKTS_APIENTRY fileGetFilename(char* out, const char* filename);

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL VkBool32 VKTS_APIENTRY fileGetBasename(char* basename, const char* filename);

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL VkBool32 VKTS_APIENTRY fileIsAbsolutePath(const char* filename);

/**
 * Not thread Safe.
 */
VKTS_APICALL void VKTS_APIENTRY fileTerminate();

}

#endif /* VKTS_FN_FILE_HPP_ */
