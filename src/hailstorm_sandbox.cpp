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
#include "hailstorm_sandbox.h"
#include "gui/mainwindow.h"
#include "common/logging.h"
#include "gameclient.h"


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

    LOG_NOTICE("PLATFORM") << "Application has entered WinMain";

    // Create a new window
    MainWindow mainWindow( hInstance, L"Hailstorm Tech Demo", 800u, 600u );
    
    mainWindow.create();
    mainWindow.show();

    GameClient game( &mainWindow );
    return game.run();

 	// Load the menu accelerator table	
	/*HACCEL hAccelTable =
        LoadAccelerators( hInstance, MAKEINTRESOURCE(IDC_DIRECTX));

	// Main message loop:
    MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage( &msg);
		}
	}

	return (int) msg.wParam;*/
}
