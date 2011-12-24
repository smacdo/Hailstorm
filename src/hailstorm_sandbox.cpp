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
#include "gui/mainwindow.h"
#include "common/logging.h"
#include "gameclient.h"


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

    LOG_NOTICE("PLATFORM") << "Application has entered WinMain";

    // Create a new window
    MainWindow mainWindow( hInstance, L"Hailstorm Tech Demo", 800u, 600u );
    
    mainWindow.create();
    
    GameClient game( &mainWindow );
    return game.run();
}
