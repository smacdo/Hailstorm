/*
 * Copyright 2011 - 2014 Scott MacDonald
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
#include "HailstormRuntime.h"
#include "runtime/StringUtils.h"

#include "gui/DesktopConsole.h"
#include "gui/mainwindow.h"
#include "gui/aboutbox.h"
#include "gui/errordialog.h"

#include "resource.h"

#include <string>
#include <memory>

static const std::wstring MainWindowClassName = L"HailstormMainRenderWindow";

/**
 * Constructor
 */
MainWindow::MainWindow(HINSTANCE hInstance,
                       const std::wstring& title,
                       unsigned int width,
                       unsigned int height)
    : mWindowTitle(title),
      mAppInstance(hInstance),
      mWindowHandle(nullptr),
      mWindowWidth(width),
      mWindowHeight(height),
      mIsPaused(false),
      mIsResizing(false),
      mWasResized(false),
      mUserRequestedQuit(false),
      mIsMinimized(false),
      mConsole(new DesktopConsole(L"Hailstorm Debug Console", true))
{
}

/**
 * Destructor
 */
MainWindow::~MainWindow()
{
}

/**
 * Create and show the main window. This should be called immediately after constructing the instance.
 */
void MainWindow::Create()
{
    LOG_DEBUG("GUI") << "Creating the renderer main window";

    // Describe the window we want to create.
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = mAppInstance;
    wcex.hIcon = LoadIcon(mAppInstance, MAKEINTRESOURCE(IDI_DIRECTX));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCE(IDC_DIRECTX);
    wcex.lpszClassName = MainWindowClassName.c_str();
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    // Register the window class description
    ATOM result = RegisterClassEx(&wcex);
    assert(result != 0);

    // Need to find out how big to make the window, depending on the size of the back buffer.
    RECT windowRect = { 200, 200, 200 + Width(), 200 + Height() };
    AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW, TRUE, WS_EX_OVERLAPPEDWINDOW);

    // Actually create the window.
    HWND tempHwnd = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW,             // Window style: Overlapped window.
        MainWindowClassName.c_str(),        // Window class name.
        mWindowTitle.c_str(),               // Window caption.
        WS_OVERLAPPEDWINDOW,                // Overlapped window style.
        windowRect.left,                    // Window top left x position.
        windowRect.top,                     // Window top left yposition.
        windowRect.right - windowRect.left, // Window width.
        windowRect.bottom - windowRect.top, // Window height.
        nullptr,                            // This window does not have a parent window.
        nullptr,                            // Do not associate a menu with the window.
        mAppInstance,                       // Application instance.
        this);                              // Pass address of this class in creation event message (advanced).

    // Verify that the window was created.
    assert(mWindowHandle == tempHwnd);
    assert(mWindowHandle != nullptr);

    // Show the window.
    Show();
}

/**
 * Show the window
 */
void MainWindow::Show()
{
    LOG_DEBUG("GUI") << "Showing the main rendering window";
    Verify(mWindowHandle != 0);

    ShowWindow(mWindowHandle, SW_RESTORE);
    UpdateWindow(mWindowHandle);
}

/**
 *  Performs clean up work prior to exiting the main window. Called by the main window event handler.
 */
void MainWindow::Exit()
{
    LOG_DEBUG("GUI") << "Closing the main rendering window";

    mUserRequestedQuit = true;
    DestroyWindow(mWindowHandle);
}

/**
 * Show the about application message box
 */
void MainWindow::ShowAboutBox() const
{
    AboutBox about(mAppInstance, mWindowHandle);
    about.Show();
}

/**
 * Sets the window handle that represents the main window
 */
void MainWindow::SetWindowHandle( HWND hWnd )
{
    mWindowHandle = hWnd;
}

/**
 * Set if the window is in a paused state or not.
 */
void MainWindow::SetPaused(bool isPaused)
{
    LOG_NOTICE("GUI") << "Main rendering window has been " << (isPaused ? "paused" : "unpaused");
    mIsPaused = isPaused;
}

/**
 * Set if the window is minimized or not.
 */
void MainWindow::SetMinimized(bool isMinimized)
{
    LOG_NOTICE("GUI") << "Main rendering window has been " << (isMinimized ? "minimized" : "un-minimized");
    mIsMinimized = isMinimized;
}

/**
 * Set if the window is being resized.
 */
void MainWindow::SetIsWindowResizing(bool isResizing)
{
    if (mIsResizing != isResizing)
    {
        LOG_NOTICE("GUI") << "Main rendering window is " << (isResizing ? "resizing" : "no longer resizing");
        mIsResizing = isResizing;
    }
}

/**
 * Set if the window is in a resizing state or not.
 *  \param windowSize The current size of the window.
 */
void MainWindow::SetWindowResized(const Size& windowSize)
{
    LOG_NOTICE("GUI") << "Main window is was resized to " << windowSize.width << "x" << windowSize.height;
    Verify(windowSize.width > 0);           // TODO: Replace these with real exceptions.
    Verify(windowSize.height > 0);

    mWasResized = true;
    mWindowWidth = windowSize.width;
    mWindowHeight = windowSize.height;
}

/**
 * Pumps the windows event handler for any messages sent to this main window. Dispatches and processes those
 * messages. This method should be called at least once per update cycle by the game loop to maintain client ui
 * responsiveness.
 */
bool MainWindow::ProcessMessages()
{
    MSG message;
    bool keepGoing = true;

    while (keepGoing && PeekMessage(&message, mWindowHandle, 0, 0, PM_REMOVE) == TRUE)
    {
        if (message.message == WM_QUIT)
        {
            LOG_NOTICE("GUI") << "Quit event received.";
            keepGoing = false;
            mUserRequestedQuit = true;
        }

        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    return keepGoing;
}

/**
 * Handles window events that come from Windows.
 */
LRESULT MainWindow::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    HWND hWnd = WindowHandle();
    int wmId = 0, wmEvent = 0;

    switch (message)
    {
        case WM_COMMAND:
            wmId = LOWORD(wParam);
            wmEvent = HIWORD(wParam);

            // Parse the menu selections:
            switch (wmId)
            {
                case IDM_ABOUT:
                    ShowAboutBox();
                    break;
                case IDM_EXIT:
                    this->Exit();
                    break;
                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
            }
            break;

        case WM_ACTIVATE:
            SetPaused(LOWORD(wParam) == WA_INACTIVE);
            break;

        case WM_SIZE:
            if (wParam == SIZE_MINIMIZED)
            {
                SetPaused(true);
                SetMinimized(true);
            }
            else if (wParam == SIZE_MAXIMIZED)
            {
                // Unpause if we are coming out of a minimize
                if (IsMinimized())
                {
                    SetPaused(false);
                    SetMinimized(false);
                }

                SetWindowResized(Size{ LOWORD(lParam), HIWORD(lParam) });
            }
            else
            {
                // Unpause if we are coming out of a minimize
                if (IsMinimized())
                {
                    SetPaused(false);
                    SetMinimized(false);
                }

                SetWindowResized(Size{ LOWORD(lParam), HIWORD(lParam) });
            }
            break;

        case WM_POWERBROADCAST:
            if (wParam == PBT_APMQUERYSUSPEND)
            {
                // The application is about to be suspended. We should prepare for
                // this event by saving any data and then pausing the game loop
                // until we come out of suspension
            }
            else if (wParam == PBT_APMRESUMESUSPEND)
            {
                // The application is being resumed from suspension. We should
                // reload any saved data and unpause the game loop
            }

        case WM_ENTERMENULOOP:
            // Pause while user is in menu
            SetPaused(true);
            break;

        case WM_EXITMENULOOP:
            // Unpause after user has exited menu
            SetPaused(false);
            break;

        case WM_ENTERSIZEMOVE:
            SetIsWindowResizing(true);
            SetPaused(true);
            break;

        case WM_EXITSIZEMOVE:
            SetIsWindowResizing(false);
            SetPaused(false);
            break;

        case WM_CLOSE:
            PostQuitMessage(0);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Application message loop
/////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK MainWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    MainWindow * pMainWindow = nullptr;

    // Grab a pointer to the Window* instance that is sending this message. Either a window was created, in which case
    // we need to save the pointer, or we need to look up the saved value.
    if (message == WM_NCCREATE)
    {
        // We need to intercept the WM_NCCREATE message, since it is the first message that a newly created window will
        // send.  Once we get it, grab the encoded Window* pointer and use SetWindowLong to save it for future use.
        LPCREATESTRUCT cs = reinterpret_cast<LPCREATESTRUCT>(lParam);
        pMainWindow = reinterpret_cast<MainWindow*>(cs->lpCreateParams);
        Assert(pMainWindow != nullptr && "Failed to find window pointer");

        // Store the window pointer.
        ::SetWindowLongPtr(
            hWnd,
            GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(pMainWindow));

        // Also store the assigned HWND value.
        pMainWindow->SetWindowHandle(hWnd);
    }
    else
    {
        // Try to look up the pointer that is stored in the window's userdata field.
        pMainWindow =
            reinterpret_cast<MainWindow*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    // Route the message to the correct window instance. If we could not decipher the instance, then just let windows
    // perform a default action.
    if (pMainWindow != NULL)
    {
        return pMainWindow->HandleMessage(message, wParam, lParam);
    }
    else
    {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}