/*
 * Copyright (C) 2011 Scott MacDonald. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SCOTT_HAILSTORM_DEBUGGING_H
#define SCOTT_HAILSTORM_DEBUGGING_H

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

#   define assert2(expr,msg) scott_assert(msg,expr)
#   define assert_null(var) scott_assert("Pointer was expected to be null",#var##" == NULL")
#   define assert_notNull(var) scott_assert("Pointer was expected to be non-null",#var##" != NULL")

#   ifdef ASSERTS_VERIFY        // only enabled for full sanity checks
#       define verify(expression)  scott_assert(NULL,expression)
#   else
#       define verify(expression)  do { (void)sizeof(x); } while(0)
#   endif

#   ifdef assert                // detect if the builtin assert has been defined
#       undef assert            // remove the built in assert
#   endif
#   define assert(x) scott_assert(NULL,x)

#else
#   define assert(x)          do { (void)sizeof(x); } while(0)
#   define assert2(x)         do { (void)sizeof(x); } while(0)
#   define assert_null(x)     do { (void)sizeof(x); } while(0)
#   define assert_notNull(x)  do { (void)sizeof(x); } while(0)
#   define verify(x)          do { (void)sizeof(x); } while(0)
#endif


#endif