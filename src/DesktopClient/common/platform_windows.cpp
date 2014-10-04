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
#include "stdafx.h"
#include "common/platform_windows.h"
//#include "common/platform.h"
#include "runtime/delete.h"
#include "runtime/logging.h"
//#include "gui/mainwindow.h"
//#include "gui/aboutbox.h"
//#include "gui/assertiondialog.h"
//#include "gui/errordialog.h"

#include "resource.h"

#include <cassert>



/////////////////////////////////////////////////////////////////////////////
// Windows specific application helper functions                           //
/////////////////////////////////////////////////////////////////////////////
namespace WinApp
{
    /**
     * Converts an STL string into a WindowsNT wchar* wrapped inside of a
     * pretty wstring
     *
     * \param  str  The STL string you want to convert
     * \return A wstring representing the input
     */
    std::wstring ToWideString( const std::string& str )
    {
        // Find the length of the soon to be allocated wstring
        size_t slen = str.length();
        int len     = MultiByteToWideChar( CP_ACP, 0,
                                            str.c_str(),
                                            static_cast<int>(slen) + 1,
                                            0,
                                            0 );

        // Allocate space for the new wstring, and then convert the input
        wchar_t *buffer = new wchar_t[len];

        MultiByteToWideChar( CP_ACP, 0,
                                str.c_str(),
                                static_cast<int>(slen) + 1,
                                buffer,
                                len );

        // Now create the wstring, copy the temporary wchar* buffer into it and
        // then destroy the buffer before returning
        std::wstring result( buffer );
        DeleteArray( buffer );

        return result;
    }

    
}

/////////////////////////////////////////////////////////////////////////////
// Windows implementation of platform specific functions                   //
/////////////////////////////////////////////////////////////////////////////
namespace App
{
    /**
     * Generates a assertion reporting dialog (or console output) to show to the
     * player, before exiting the application
     *
     * \param  message     An accompanying assertion description (if provided)
     * \param  expression  String containing the expression text
     * \param  filename    Name of the file that generated the assertion
     * \param  lineNumber  Line that generated the assertion
     */
    /*EAssertionStatus raiseAssertion( const char *pMessage,
                                     const char *pExpression,
                                     const char *pFilename,
                                     unsigned int lineNumber )
    {
        // Configure the assertion dialog before displaying it
        AssertionDialog dialog( pExpression, pFilename, lineNumber );
        
        if ( pMessage != NULL )
        {
            dialog.setMessage( pMessage );
        }

        // Display the assertion to the user, and deal with their returned action
        if ( dialog.show() )
        {
            return EAssertion_Halt;
        }
        else
        {
            App::quit( EPROGRAM_ASSERT_FAILED,
                       "User quit after encountering assertion");
            return EAssertion_Continue;
        }
    }*/

    /**
     * Generates a non-fatal error message that is displayed to the player, and
     * the player is allowed to choose whether to continue or quit.
     *
     * \param  message  The main error message
     * \param  details  (optional) Details about the problem
     */
    /*void raiseError( const std::string& message,
                     const std::string& details )
    {
        ErrorDialog error( message, details );
        error.show();

        if ( error.didUserPressQuit() )
        {
            App::quit( EPROGRAM_USER_ERROR, "User quit in response to error" );
        }
    }*/

    /**
     * Displays a fatal error message to the player before he/she is forced to
     * quit playing.
     *
     * \param  message  The main error message
     * \param  details  (optional) Details about the problem
     */
    /*void raiseFatalError( const std::string& message,
                          const std::string& details )
    {
        ErrorDialog error( message, details );
        error.setIsFatal( true );
        error.show();

        if ( error.didUserPressQuit() )
        {
            App::quit( EPROGRAM_USER_ERROR, "User quit in response to error" );
        }
    }*/
}
