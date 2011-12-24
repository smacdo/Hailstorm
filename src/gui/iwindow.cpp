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
#include "gui\iwindow.h"
#include "common\platform_windows.h"
#include "resource.h"

#include <string>
#include <cassert>

/**
 * Default constructor
 */
IWindow::IWindow( const std::string& title,
                  unsigned int width,
                  unsigned int height )
    : mTitle( title ),
      mWidth( width ),
      mHeight( height ),
      mUserQuit( false )
{
}

IWindow::~IWindow()
{
}

bool  IWindow::didUserQuit() const
{
    return mUserQuit;
}

void IWindow::setUserQuit()
{
    mUserQuit = true;
}

/**
 * Returns the window title
 */
std::string IWindow::windowTitle() const
{
    return mTitle;
}

/**
 * Returns the width of the window
 */
unsigned int IWindow::width() const
{
    return mWidth;
}

/**
 * Returns the height of the window
 */
unsigned int IWindow::height() const
{
    return mHeight;
}