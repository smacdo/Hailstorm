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

#include "common/platform.h"
#include "common/logging.h"

/////////////////////////////////////////////////////////////////////////////git s
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

    // Make sure we call any platform specific start up code first. This will allow
    // us to initialize all core libraries and anything else the specific platform
    // may require
    App::startup();
    LOG_NOTICE("WinMain") << 42 << " this is a string " << 5.0f << true;
    LOG_NOTICE("WinMain") << "Application has started";

    // Create a new window
    LOG_NOTICE("WinMain") << "Creating main application window";
    MainWindow mainWindow( hInstance, "Hailstorm Tech Demo", 800u, 600u );
    mainWindow.create();
    
    // Create the game client
    LOG_NOTICE("WinMain") << "Creating the game client";
    GameClient game( &mainWindow );

    // Run the game
    LOG_NOTICE("WinMain") << "Starting the game";
    return game.run();
}
