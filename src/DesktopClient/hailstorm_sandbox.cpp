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
#include "graphics/dxrenderer.h"
#include "runtime/logging.h"
#include "camera/RotationalCamera.h"

#include "demos/WaterLandscapeDemoScene.h"

// Let VC++ know we are compiling for Windows Vista and newer
#ifndef _WIN32_WINNT
#define _WIN32_WINNT   0x0600 // Vista
#endif

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

    // A camra is important! We can't see without one, and what kind of graphics demo would this be if we couldn't
    // see anything??
    std::shared_ptr<RotationalCamera> camera(new RotationalCamera());

    // Make sure the logging system is initialized first before any of the
    // other critical system services
    GlobalLog::start();

    // Create a new window
    LOG_NOTICE("WinMain") << "Application has started";
    LOG_NOTICE("WinMain") << "Creating main application window";

    std::shared_ptr<MainWindow> mainWindow(new MainWindow(hInstance, L"Hailstorm Tech Demo", 800u, 600u));
    mainWindow->Create();
    mainWindow->Show();
   
    // Create the DirectX renderer.
    LOG_NOTICE("WinMain") << "Creating DirectX renderer.";
    std::unique_ptr<DXRenderer> renderer(new DXRenderer(camera, mainWindow));

    // Create the game client
    LOG_NOTICE("WinMain") << "Creating the game client";
    std::unique_ptr<GameClient> game(new GameClient(camera, mainWindow, std::move(renderer)));

    // Run the game
    LOG_NOTICE("WinMain") << "Starting the game";
    game->Run(new WaterLandscapeDemoScene(camera));

    return EXIT_SUCCESS;
}