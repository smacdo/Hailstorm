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
#ifndef SCOTT_HAILSTORM_GUI_MAINWINDOW_H
#define SCOTT_HAILSTORM_GUI_MAINWINDOW_H

#include "host/RenderingWindow.h"
#include "runtime/Size.h"
#include <string>
#include <memory>

class DesktopConsole;

/**
 * MainWindow is an implementation of the Win32 desktop gui client. It implements all the work required to show,
 * display and allow the game client to interact with the desktop windows client.
 */
class MainWindow : public RenderingWindow       // TODO: Rename this to be more descriptive, "DesktopWindow"
{
public:
    MainWindow(
        HINSTANCE hInstance,
        const std::wstring& title,
        unsigned int width,
        unsigned int height);
    MainWindow(const MainWindow&) = delete;
    virtual ~MainWindow() ;

    MainWindow& operator =(const MainWindow&) = delete;

    // Create and show the main window. This should be called immediately after constructing the instance.
    void Create();

    // Shows the main window.
    void Show();

    // Pumps the windows event handler for any messages sent to this main window. Dispatches and processes those
    // messages. This method should be called at least once per update cycle by the game loop to maintain client ui
    // responsiveness.
    virtual bool ProcessMessages() override;

    // Handles window events that come from Windows.
    LRESULT HandleMessage(UINT message, WPARAM wParam, LPARAM lParam);

    // Show the message box
    void ShowAboutBox() const;

    // Get the window handle associated with this main window instance.
    virtual HWND WindowHandle() const override { return mWindowHandle; }

    // Get the window width.
    virtual unsigned int Width() const override { return mWindowWidth; }

    // Get the window height.
    virtual unsigned int Height() const override { return mWindowHeight; }

    // Get the size of the window.
    virtual Size WindowSize() const override { return Size(mWindowWidth, mWindowHeight); }

    // Get if the window was resized.
    virtual bool WasResized() const override { return mWasResized; }

    // Get if the window is being resized.
    virtual bool IsResizing() const override { return mIsResizing; }

    // Get if the window is in a paused state.
    virtual bool IsPaused() const override { return mIsPaused; }

    // Get if the window is in a minimzed state.
    virtual bool IsMinimized() const override { return mIsMinimized; }

    // Get if the window is closing.
    virtual bool IsClosing() const override { return mUserRequestedQuit; }

    // Helepr method that clear window resized flag.
    //  TODO: This is an inappropriate method, because it actually assist the DXRenderer, and doesn't have real
    //        reason to exist in this class other than convience.
    virtual void ClearResizedFlag() override { mWasResized = false; }

protected:
    // Set if the window is paused or not.
    void SetPaused(bool isPaused);

    // Set if the window is minimized or not.
    void SetMinimized(bool isMinimized);

    // Set if the window is resizing or not.
    void SetIsWindowResizing(bool isResizing);

    // Set the new resized window size.
    void SetWindowResized(const Size& windowSize);

private:
    // Associate the MainWindow instance to a window handle.
    // This should only be called once, and by the MainWindow static event handler.
    void SetWindowHandle(HWND hWnd);

    // Internal event handler. This method is called by Windows to inform the main window class of events.
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    // Performs clean up work prior to exiting the main window. Called by the main window event handler.
    void Exit();

private:
    const std::wstring mWindowTitle;
    HINSTANCE mAppInstance;
    HWND mWindowHandle;
    unsigned int mWindowWidth;
    unsigned int mWindowHeight;
    bool mIsPaused;
    bool mIsResizing;
    bool mWasResized;
    bool mUserRequestedQuit;
    bool mIsMinimized;
    std::unique_ptr<DesktopConsole> mConsole;
};

#endif