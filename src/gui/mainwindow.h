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
#ifndef SCOTT_HAILSTORM_GUI_MAINWINDOW_H
#define SCOTT_HAILSTORM_GUI_MAINWINDOW_H

#include "gui/window.h"
#include <string>

class MainWindow : public Window
{
public:
    MainWindow( HINSTANCE hInstance,
                const std::wstring& title,
                unsigned int width,
                unsigned int height );
    ~MainWindow();

    // Show the message box
    void showAboutBox() const;

    virtual LRESULT handleMessage( UINT message, WPARAM wParam, LPARAM lParam );

protected:
    
    virtual void onPaint();
};

#endif