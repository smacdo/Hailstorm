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
#include "gui/mainwindow.h"
#include "gui/aboutbox.h"
#include "gui/errordialog.h"
#include "resource.h"

/**
 * Constructor
 */
MainWindow::MainWindow( HINSTANCE hInstance,
                        const std::wstring& title,
                        unsigned int width,
                        unsigned int height )
    : Window( hInstance, title, width, height )
{

}

/**
 * Destructor
 */
MainWindow::~MainWindow()
{

}

/**
 * Show the about application message box
 */
void MainWindow::showAboutBox() const
{
    AboutBox about( appInstance(), windowHandle() );
    ErrorDialog error("oh noes this is an error");
    assert( false == true );
//    error.show();
//    about.show();
}

/**
 * Custom paint code
 */
void MainWindow::onPaint()
{
    PAINTSTRUCT ps;
    HDC hdc;
    HWND hWnd = windowHandle();

    hdc = BeginPaint( hWnd, &ps );
    // TODO: Add any drawing code here...
    EndPaint( hWnd, &ps );
}

/**
 * Message loop handler
 */
LRESULT MainWindow::handleMessage( UINT message, WPARAM wParam, LPARAM lParam )
{
    HWND hWnd = windowHandle();
    int wmId, wmEvent;

    switch ( message )
    {
        case WM_COMMAND:
            wmId    = LOWORD( wParam );
            wmEvent = HIWORD( wParam );

            // Parse the menu selections:
            switch ( wmId )
            {
                case IDM_ABOUT:
                    showAboutBox();
                    break;
                case IDM_EXIT:
                    this->exit();
                    break;
                default:
                    return DefWindowProc( hWnd, message, wParam, lParam );
            }
            break;

        case WM_CLOSE:
            PostQuitMessage( 0 );
            break;

        case WM_DESTROY:
            PostQuitMessage( 0 );
            break;

        default:
            return DefWindowProc( hWnd, message, wParam, lParam );
    }

    return 0;
}