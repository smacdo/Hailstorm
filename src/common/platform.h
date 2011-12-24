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
#ifndef SCOTT_COMMON_PLATFORM_H
#define SCOTT_COMMON_PLATFORM_H

#include <string>

/**
 * The app namespace is a collection of application level utility functions.
 * These functions tend to be used throughout the code base, and are heavily
 * dependent on the underlying operating system API to work properly.
 */
namespace App
{
    /**
     * Enumeration that is used by the custom debug/assertion code in the
     * Hailstorm engine.
     */
    enum EAssertionStatus
    {
        EAssertion_Halt = 0,        //! System halt which triggers the debugger
        EAssertion_Continue = 1     //! Skip debugger, keep running
    };

    enum EProgramStatus
    {
        EPROGRAM_OK = 0,
        EPROGRAM_ASSERT_FAILED = 1,
        EPROGRAM_FATAL_ERROR   = 2,
        EPROGRAM_USER_ERROR    = 3
    };

    /**
     * Platform specific start up
     */
    void startup();

    /**
     * Causes the application to immediately display the given message and then
     * exit.
     */
    void quit( EProgramStatus programStatus, const std::string& message );

    /**
     * Application will raise a custom assertion. By doing this, the application
     * will display a message to the user containing information on the assertion
     * and then it will give the player a list of actions to perform. The
     * function must indicate when it returns if the debugger should be triggered
     * before exiting
     *
     * \param  message     A custom assertion message to display (default NULL)
     * \param  expression  The expression causing the assertion
     * \param  filename    Source code file containing the expression
     * \param  linenumber  The line number of the expression
     * \return             EAssertion_Halt to trigger the debugger, otherwise
     *                     EAssertion_Continue
     */
    EAssertionStatus raiseAssertion( const char* message,
        const char* expression,
        const char* filename,
        unsigned int linenumber );

    /**
     * Displays a generic system error
     */
    void raiseError( const std::string& message,
                     const std::string& details = "" );

    /**
     * Displays a generic fatal error
     */
    void raiseFatalError( const std::string& message,
                          const std::string& details = "" );
}

#endif