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
#include "gui\window.h"
#include "common\platform_windows.h"
#include "resource.h"

#include <string>
#include <cassert>

/**
 * Default constructor
 */
Window::Window( HINSTANCE hInstance,
                const std::wstring& title,
                unsigned int width,
                unsigned int height )
    : mAppClassName( L"SCOTTS_WINDOWS_CLASS_0" ),
      mAppInstance( hInstance ),
      mWindowHandle( 0 ),
      mTitle( title ),
      mWidth( width ),
      mHeight( height )
{
}

Window::~Window()
{
}

void Window::create()
{
    WNDCLASSEX wcex;

    // Configure the window description struct
    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style			= CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc	= WndProc;
    wcex.cbClsExtra		= 0;
    wcex.cbWndExtra		= 0;
    wcex.hInstance		= mAppInstance;
    wcex.hIcon			= LoadIcon(mAppInstance, MAKEINTRESOURCE(IDI_DIRECTX));
    wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_DIRECTX);
    wcex.lpszClassName	= mAppClassName.c_str();
    wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    // Register the window class description
    ATOM result = RegisterClassEx(&wcex);
    assert( result != 0 );

    // Now create the window
    HWND tempHwnd = CreateWindowEx( WS_EX_OVERLAPPEDWINDOW,
                                    mAppClassName.c_str(),
                                    mTitle.c_str(),
                                    WS_OVERLAPPEDWINDOW,
                                    200,
                                    200,
                                    static_cast<int>( mWidth ),
                                    static_cast<int>( mHeight ),
                                    NULL,
                                    NULL,
                                    mAppInstance,
                                    this
    );

    // Verify that the window was created
    assert( mWindowHandle == tempHwnd );
    assert( mWindowHandle != NULL );

    // mark as created
    mCreated = true;
}

/**
 * Show the window
 */
void Window::show()
{
    assert( mCreated == true );

    ShowWindow( mWindowHandle, SW_RESTORE );
    UpdateWindow( mWindowHandle );
}

HWND Window::windowHandle() const
{
    return mWindowHandle;
}

void Window::setWindowHandle( HWND hWnd )
{
    assert( mCreated != true );
    mWindowHandle = hWnd;
}

HINSTANCE Window::appInstance() const
{
    return mAppInstance;
}

/**
 * Handle the window's message loop
 */
LRESULT Window::handleMessage( UINT message, WPARAM wParam, LPARAM lParam )
{
    return DefWindowProc( windowHandle(), message, wParam, lParam );
}