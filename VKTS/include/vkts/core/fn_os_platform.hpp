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

#ifndef VKTS_FN_OS_PLATFORM_HPP_
#define VKTS_FN_OS_PLATFORM_HPP_

// see http://sourceforge.net/p/predef/wiki/OperatingSystems/

#if defined(_WIN32)

// Windows OS.

// Only Windows 7 and up is supported.
#if (WINVER < 0x0601)
#undef WINVER
#define WINVER 0x0601
#endif

#if (_WIN32_WINNT < 0x0601)
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>

#define VKTS_PRINTF(...) printf(__VA_ARGS__)

#elif defined(__ANDROID__)

// Android OS.

#include <android_native_app_glue.h>

#include <android/log.h>

#define VKTS_PRINTF(...) __android_log_print(ANDROID_LOG_VERBOSE, "VKTS", __VA_ARGS__)

#elif defined(__gnu_linux__)

// GNU/Linux OS.

#define VKTS_PRINTF(...) printf(__VA_ARGS__)

#else

// Unsupported OS platform!

#error "Unsupported OS platform detected!"

#endif

#endif /* VKTS_FN_OS_PLATFORM_HPP_ */
