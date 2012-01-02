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
#ifndef SCOTT_PLATFORM_WINDOWS_H
#define SCOTT_PLATFORM_WINDOWS_H

#include "stdafx.h"

namespace WinApp
{
    std::wstring ToWideString( const std::string& str );
    void CreateConsoleWindow( bool redirectStdout );
}

LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	DlgWndProc(HWND, UINT, WPARAM, LPARAM);

#endif