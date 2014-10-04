/*
 * Copyright 2011 Scott MacDonald
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef SCOTT_HAILSTORM_DEBUGGING_H
#define SCOTT_HAILSTORM_DEBUGGING_H

#include "runtime/exceptions.h"

/////////////////////////////////////////////////////////////////////////////
// Platform specific debug trigger
/////////////////////////////////////////////////////////////////////////////
#if defined(_WIN32)
#   define app_break __debugbreak()
#else
#   define app_break __builtin_trap()
#endif

/////////////////////////////////////////////////////////////////////////////
// Debug mode flags
/////////////////////////////////////////////////////////////////////////////
#if defined(_DEBUG)
#   define ASSERTS_ENABLED = 1
#   define ASSERTS_VERIFY  = 1
#endif

/////////////////////////////////////////////////////////////////////////////
// Verification
/////////////////////////////////////////////////////////////////////////////
#define Verify(expr) \
    do { if (!(expr)) { throw AssertionFailedException(#expr, __FILE__, __LINE__); } } while (0)

#define VerifyNull(expr) \
    do { if ((expr)==nullptr) { throw IsNullAssertionFailedException(#expr, __FILE__, __LINE__); } } while (0)

#define VerifyNotNull(expr) \
    do { if ((expr)!=nullptr) { throw IsNotNullAssertionFailedException(#expr, __FILE__, __LINE__); } } while (0)

/////////////////////////////////////////////////////////////////////////////
// Custom assertion handling
/////////////////////////////////////////////////////////////////////////////
#ifdef ASSERTS_ENABLED
#   define scott_assert(AssertType,cond) \
        do { if (!(cond)) { throw AssertType(#cond, __FILE__, __LINE__); } } while (0)
#   define scott_assert2(msg,cond) \
        do { if (!(cond)) { throw AssertionFailedException(msg, #cond, __FILE__, __LINE__); } } while(0)

#   define Assert2(expr,msg) scott_assert2(msg,expr)
#   define AssertNull(var) scott_assert(IsNullAssertionFailedException, #var##" == nullptr")
#   define AssertNotNull(var) scott_assert(IsNotNullAssertionFailedException, #var##" != nullptr")

#   ifdef assert                // detect if the builtin assert has been defined
#       undef assert            // remove the built in assert
#   endif
#   define assert(x) scott_assert(AssertionFailedException,x)

#else
#   define assert(x)          do { (void)sizeof(x); } while(0)
#   define assert2(x,m)       do { (void)sizeof(x); } while(0)
#   define assert_null(x)     do { (void)sizeof(x); } while(0)
#   define assert_notNull(x)  do { (void)sizeof(x); } while(0)
#   define verify(x)          do { (void)sizeof(x); } while(0)
#endif


#endif