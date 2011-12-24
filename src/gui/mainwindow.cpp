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
    about.show();
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