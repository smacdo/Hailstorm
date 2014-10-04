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
#include "hailstorm_sandbox.h"
#include "gameclient.h"
#include "gui/mainwindow.h"

#include "runtime/logging.h"

// For console
#include <fcntl.h>
#include <io.h>

// Let VC++ know we are compiling for Windows Vista and newer
#ifndef _WIN32_WINNT
#define _WIN32_WINNT   0x0600 // Vista
#endif

void CreateConsoleWindow(bool redirectStdout);

/////////////////////////////////////////////////////////////////////////////
// Application entry point
/////////////////////////////////////////////////////////////////////////////
int APIENTRY _tWinMain( HINSTANCE hInstance,
                        HINSTANCE hPrevInstance,
                        LPTSTR    lpCmdLine,
                        int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    // Enable Visual Studio's debug heap and various memory checking features
    int flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

    flags |= _CRTDBG_LEAK_CHECK_DF;         // Turn on leak-checking bit.
    flags &= ~_CRTDBG_CHECK_CRT_DF;         // Turn off CRT block checking bit.
    _CrtSetDbgFlag(flags);                  // Now set debug flags           

    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);

    // We need a console window
    CreateConsoleWindow(true);

    // Make sure the logging system is initialized first before any of the
    // other critical system services
    GlobalLog::start();

    // Create a new window
    LOG_NOTICE("WinMain") << "Application has started";
    LOG_NOTICE("WinMain") << "Creating main application window";

    MainWindow mainWindow( hInstance, "Hailstorm Tech Demo", 800u, 600u );
    mainWindow.create();
    
    // Create the game client
    LOG_NOTICE("WinMain") << "Creating the game client";
    GameClient game( &mainWindow );

    // Run the game
    LOG_NOTICE("WinMain") << "Starting the game";
    game.run();

    return EXIT_SUCCESS;
}

/**
* Creates the a console window using the Windows API, and optionally
* redirects standard output/input to this console window
*/
void CreateConsoleWindow(bool redirectStdout)
{
    // --- I think we should break all this out into a separate class ---
    const int MAX_CONSOLE_LINES = 4096;
    //        const int CONSOLE_COLS      = 100;

    intptr_t stdHandle = 0;
    int consoleHandle = 0;
    FILE * pFile = NULL;

    // Create the console window
    AllocConsole();

    SetConsoleTitleW(L"Application Console");

    // Set up the screen buffer to be big enough that we can scroll the text
    // area
    CONSOLE_SCREEN_BUFFER_INFOEX consoleInfo;
    GetConsoleScreenBufferInfoEx(GetStdHandle(STD_OUTPUT_HANDLE), &consoleInfo);

    //        consoleInfo.dwSize.X = CONSOLE_COLS;
    consoleInfo.dwSize.Y = MAX_CONSOLE_LINES;

    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), consoleInfo.dwSize);

    // Redirect unbuffered STDOUT to the console window
    stdHandle = reinterpret_cast<intptr_t>(GetStdHandle(STD_OUTPUT_HANDLE));
    consoleHandle = _open_osfhandle(stdHandle, _O_TEXT);
    pFile = _fdopen(consoleHandle, "w");

    *stdout = *pFile;
    setvbuf(stdout, NULL, _IOLBF, 4096);  // is this large enough?

    // Redirect unbuffered STDIN to the console
    stdHandle = reinterpret_cast<intptr_t>(GetStdHandle(STD_INPUT_HANDLE));
    consoleHandle = _open_osfhandle(stdHandle, _O_TEXT);
    pFile = _fdopen(consoleHandle, "r");

    *stdin = *pFile;
    setvbuf(stdin, NULL, _IONBF, 0);

    // Redirect unbuffered STDERR to the console
    stdHandle = reinterpret_cast<intptr_t>(GetStdHandle(STD_ERROR_HANDLE));
    consoleHandle = _open_osfhandle(stdHandle, _O_TEXT);
    pFile = _fdopen(consoleHandle, "w");

    *stderr = *pFile;
    setvbuf(stderr, NULL, _IONBF, 0);

    // Sync C++ cout/cin/cerr/clog with the new handles
    std::ios::sync_with_stdio();
}