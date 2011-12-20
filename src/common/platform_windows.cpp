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
#include "stdafx.h"
#include "common/platform_windows.h"
#include "common/platform.h"
#include "common/delete.h"
#include "gui/window.h"
#include "gui/aboutbox.h"
#include "gui/assertiondialog.h"
#include "gui/errordialog.h"

#include "resource.h"

#include <cassert>

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
    EAssertionStatus raiseAssertion( const char *pMessage,
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
    }

    /**
     * Generates a non-fatal error message that is displayed to the player, and
     * the player is allowed to choose whether to continue or quit.
     *
     * \param  message  The main error message
     * \param  details  (optional) Details about the problem
     */
    void raiseError( const std::string& message,
                     const std::string& details )
    {
        ErrorDialog error( message, details );
        error.show();

        if ( error.didUserPressQuit() )
        {
            App::quit( EPROGRAM_USER_ERROR, "User quit in response to error" );
        }
    }

    /**
     * Displays a fatal error message to the player before he/she is forced to
     * quit playing.
     *
     * \param  message  The main error message
     * \param  details  (optional) Details about the problem
     */
    void raiseFatalError( const std::string& message,
                          const std::string& details )
    {

    }

    /**
     * Performs windows specific tasks that need to happen before the game
     * starts up
     */
    void startup()
    {
    }

    /**
     * Quit the program with the requested status and reason
     */
    void quit( EProgramStatus programStatus, const std::string& message )
    {
        exit( programStatus );
    }
}

/////////////////////////////////////////////////////////////////////////////
// Application message loop
/////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    Window * pWindow = NULL;

    //
    // Grab a pointer to the Window* instance that is sending this message.
    //  Either a window was created, in which case we need to save the pointer,
    //  or we need to look up the saved value
    //
    if ( message == WM_NCCREATE )
    {
        // We need to intercept the WM_NCCREATE message, since it is the first
        // message that a newly create window will send.  Once we get it, we will
        // grab the encoded Window* pointer and use SetWindowLong to save it for
        // future use
        LPCREATESTRUCT cs = reinterpret_cast<LPCREATESTRUCT>( lParam );
        pWindow           = reinterpret_cast<Window*>( cs->lpCreateParams );
        assert( pWindow != NULL && "Failed to find window pointer");

        // Store the window pointer
        ::SetWindowLongPtr(
            hWnd,
            GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>( pWindow ) );

        // Also store the assigned HWND value
        pWindow->setWindowHandle( hWnd );
    }
    else
    {
        // Try to look up the pointer that is stored in the window's userdata
        // field
        pWindow = reinterpret_cast<Window*>( ::GetWindowLongPtr( hWnd, GWLP_USERDATA ) );
    }

    // Route the message to the correct window instance. If we could not decipher
    // the instance, then just let windows perform a default action
    if ( pWindow != NULL )
    {
        return pWindow->handleMessage( message, wParam, lParam );
    }
    else
    {
        return DefWindowProc( hWnd, message, wParam, lParam );
    }

    
}