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
      mHeight( height ),
      mUserQuit( false )
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
 * Starts the application exit process
 */
void Window::exit()
{
    mUserQuit = true;
    DestroyWindow( mWindowHandle );
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

bool  Window::didUserQuit() const
{
    return mUserQuit;
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
 * Returns the width of the window
 */
unsigned int Window::width() const
{
    return mWidth;
}

/**
 * Returns the height of the window
 */
unsigned int Window::height() const
{
    return mHeight;
}

/**
 * Process any incoming window messages
 */
bool Window::processMessages()
{
    MSG message;
    bool keepGoing = true;

    while ( keepGoing && PeekMessage( &message, mWindowHandle, 0, 0, PM_REMOVE ) == TRUE )
    {
        if ( message.message == WM_QUIT )
        {
            keepGoing = false;
            mUserQuit = true;
        }

        TranslateMessage( &message );
        DispatchMessage( &message );
    }

    return keepGoing;
}

/**
 * Handle the window's message loop
 */
LRESULT Window::handleMessage( UINT message, WPARAM wParam, LPARAM lParam )
{
    return DefWindowProc( windowHandle(), message, wParam, lParam );
}