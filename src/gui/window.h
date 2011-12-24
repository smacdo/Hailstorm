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
#ifndef SCOTT_WINDOW_H
#define SCOTT_WINDOW_H

#include <string>
#include <boost/utility.hpp>

class Window : boost::noncopyable
{
public:
    Window( HINSTANCE hInstance,
            const std::wstring& windowTitle,
            unsigned int width,
            unsigned int height );
    virtual ~Window();
    void create();
    void show();
    bool didUserQuit() const;
    virtual bool processMessages();
    virtual LRESULT handleMessage( UINT message, WPARAM wParam, LPARAM lParam );

    // Starts the exit process
    void exit();

    HWND windowHandle() const;
    HINSTANCE appInstance() const;

    void setWindowHandle( HWND hWnd );
    unsigned int width() const;
    unsigned int height() const;

private:
    const std::wstring mAppClassName;
    std::wstring mTitle;
    unsigned int mWidth;
    unsigned int mHeight;
    HINSTANCE mAppInstance;
    HWND mWindowHandle;
    bool mCreated;
    bool mUserQuit;
};

#endif