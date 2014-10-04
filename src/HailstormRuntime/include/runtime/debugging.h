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

#include <cassert>      // TODO: Remove this
                        // TODO: Change all assert to Assert

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
// Custom assertion handling
/////////////////////////////////////////////////////////////////////////////
#ifdef ASSERTS_ENABLED
#   define scott_assert(msg,cond)           \
    do                                      \
        {                                   \
        if ( !(cond) )                      \
            {                               \
            if ( App::raiseAssertion(msg,#cond,__FILE__,__LINE__) == \
            App::EAssertion_Halt ) \
            app_break;              \
            }                               \
        } while( 0 )

//#   define assert2(expr,msg) scott_assert(msg,expr)
//#   define assert_null(var) scott_assert("Pointer was expected to be null",#var##" == NULL")
//#   define assert_notNull(var) scott_assert("Pointer was expected to be non-null",#var##" != NULL")
#   define assert2(expr,msg) assert((expr) && (msg))
#   define assert_null(var) assert((var) == nullptr)
#   define assert_notNull(var) assert((var) != nullptr)
#   ifdef ASSERTS_VERIFY        // only enabled for full sanity checks
//#       define verify(expression)  scott_assert(NULL,expression)
#       define verify(expression)  assert(expression)
#   else
#       define verify(expression)  do { (void)sizeof(x); } while(0)
#   endif

//#   ifdef assert                // detect if the builtin assert has been defined
//#       undef assert            // remove the built in assert
//#   endif
//#   define assert(x) scott_assert(NULL,x)

#else
#   define assert(x)          do { (void)sizeof(x); } while(0)
#   define assert2(x,m)       do { (void)sizeof(x); } while(0)
#   define assert_null(x)     do { (void)sizeof(x); } while(0)
#   define assert_notNull(x)  do { (void)sizeof(x); } while(0)
#   define verify(x)          do { (void)sizeof(x); } while(0)
#endif


#endif