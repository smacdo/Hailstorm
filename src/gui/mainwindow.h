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