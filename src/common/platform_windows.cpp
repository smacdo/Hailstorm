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
#include "common/platform.h"
#include "common/delete.h"
#include "common/logging.h"
#include "gui/mainwindow.h"
#include "gui/aboutbox.h"
#include "gui/assertiondialog.h"
#include "gui/errordialog.h"

#include "resource.h"

#include <cassert>

// For console
#include <fcntl.h>
#include <io.h>

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

    /**
     * Creates the a console window using the Windows API, and optionally
     * redirects standard output/input to this console window
     */
    void CreateConsoleWindow( bool redirectStdout )
    {
        // --- I think we should break all this out into a separate class ---
        const int MAX_CONSOLE_LINES = 4096;
//        const int CONSOLE_COLS      = 100;

        intptr_t stdHandle = 0;
        int consoleHandle = 0;
        FILE * pFile      = NULL;

        // Create the console window
        AllocConsole();

        SetConsoleTitleW( L"Application Console" );

        // Set up the screen buffer to be big enough that we can scroll the text
        // area
        CONSOLE_SCREEN_BUFFER_INFOEX consoleInfo;
        GetConsoleScreenBufferInfoEx( GetStdHandle(STD_OUTPUT_HANDLE), &consoleInfo );

//        consoleInfo.dwSize.X = CONSOLE_COLS;
        consoleInfo.dwSize.Y = MAX_CONSOLE_LINES;

        SetConsoleScreenBufferSize( GetStdHandle(STD_OUTPUT_HANDLE), consoleInfo.dwSize );

        // Redirect unbuffered STDOUT to the console window
        stdHandle     = reinterpret_cast<intptr_t>( GetStdHandle( STD_OUTPUT_HANDLE ) );
        consoleHandle = _open_osfhandle( stdHandle, _O_TEXT );
        pFile         = _fdopen( consoleHandle, "w" );

        *stdout = *pFile;
        setvbuf( stdout, NULL, _IOLBF, 4096 );  // is this large enough?

        // Redirect unbuffered STDIN to the console
        stdHandle     = reinterpret_cast<intptr_t>( GetStdHandle( STD_INPUT_HANDLE ) );
        consoleHandle = _open_osfhandle( stdHandle, _O_TEXT );
        pFile         = _fdopen( consoleHandle, "r" );

        *stdin = *pFile;
        setvbuf( stdin, NULL, _IONBF, 0 );

        // Redirect unbuffered STDERR to the console
        stdHandle     = reinterpret_cast<intptr_t>( GetStdHandle( STD_ERROR_HANDLE ) );
        consoleHandle = _open_osfhandle( stdHandle, _O_TEXT );
        pFile         = _fdopen( consoleHandle, "w" );

        *stderr = *pFile;
        setvbuf( stderr, NULL, _IONBF, 0 );

        // Sync C++ cout/cin/cerr/clog with the new handles
        std::ios::sync_with_stdio();
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
        // We need a console window
        WinApp::CreateConsoleWindow( true );

        // Make sure the logging system is initialized first before any of the
        // other critical system services
        GlobalLog::start();
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
    MainWindow * pMainWindow = NULL;

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
        pMainWindow       = reinterpret_cast<MainWindow*>( cs->lpCreateParams );
        assert( pMainWindow != NULL && "Failed to find window pointer");

        // Store the window pointer
        ::SetWindowLongPtr(
            hWnd,
            GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>( pMainWindow ) );

        // Also store the assigned HWND value
        pMainWindow->setWindowHandle( hWnd );
    }
    else
    {
        // Try to look up the pointer that is stored in the window's userdata
        // field
        pMainWindow =
            reinterpret_cast<MainWindow*>( ::GetWindowLongPtr( hWnd, GWLP_USERDATA ) );
    }

    // Route the message to the correct window instance. If we could not decipher
    // the instance, then just let windows perform a default action
    if ( pMainWindow != NULL )
    {
        return pMainWindow->handleMessage( message, wParam, lParam );
    }
    else
    {
        return DefWindowProc( hWnd, message, wParam, lParam );
    }

    
}