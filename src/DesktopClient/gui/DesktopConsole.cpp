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
#include "DesktopConsole.h"

#include <memory>
#include <string>
#include <fcntl.h>
#include <io.h>

// Constants.
static const int MaxConsoleLines = 4096;
static const int ConsoleCols = 120;
static const size_t DefaultStdOutBufferSize = 4096;

DesktopConsole::DesktopConsole(
    const std::wstring& windowTitle,
    bool redirectStandardInputAndOutput)
{
    Initialize(windowTitle, redirectStandardInputAndOutput);
}

DesktopConsole::~DesktopConsole()
{
    FreeConsole();
}

void DesktopConsole::Initialize(
    const std::wstring& windowTitle,
    bool redirectStandardInputAndOutput)
{
//    FILE * pFile = nullptr;

    // Create the console window and set the window title.
    if (!AllocConsole())
    {
        throw WindowsApiException(GetLastError(), L"AllocConsole", __FILE__, __LINE__);
    }

    if (!SetConsoleTitle(windowTitle.c_str()))
    {
        throw WindowsApiException(GetLastError(), L"SetConsoleTitle", __FILE__, __LINE__);
    }

    // Resize screen buffer to be big enough that we can scroll the text area.
    CONSOLE_SCREEN_BUFFER_INFOEX consoleInfo = { 0 };
    consoleInfo.dwSize.X = ConsoleCols;
    consoleInfo.dwSize.Y = MaxConsoleLines;

    if (!SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), consoleInfo.dwSize))
    {
        throw WindowsApiException(GetLastError(), L"SetConsoleScreenBufferSize", __FILE__, __LINE__);
    }

    // Redirect unbuffered STDOUT to the console window
    intptr_t stdHandle = reinterpret_cast<intptr_t>(GetStdHandle(STD_OUTPUT_HANDLE));
    int consoleHandle = _open_osfhandle(stdHandle, _O_TEXT);
    FILE * pFile = _fdopen(consoleHandle, "w");

    *stdout = *pFile;
    setvbuf(stdout, nullptr, _IOLBF, DefaultStdOutBufferSize);

    // Redirect unbuffered STDIN to the console
    stdHandle = reinterpret_cast<intptr_t>(GetStdHandle(STD_INPUT_HANDLE));
    consoleHandle = _open_osfhandle(stdHandle, _O_TEXT);
    pFile = _fdopen(consoleHandle, "r");

    *stdin = *pFile;
    setvbuf(stdin, nullptr, _IONBF, 0);

    // Redirect unbuffered STDERR to the console
    stdHandle = reinterpret_cast<intptr_t>(GetStdHandle(STD_ERROR_HANDLE));
    consoleHandle = _open_osfhandle(stdHandle, _O_TEXT);
    pFile = _fdopen(consoleHandle, "w");

    *stderr = *pFile;
    setvbuf(stderr, nullptr, _IONBF, 0);

    // Sync C++ cout/cin/cerr/clog with the new handles
    std::ios::sync_with_stdio();
}

void DesktopConsole::FreeConsole()
{
    if (!::FreeConsole())
    {
        throw WindowsApiException(GetLastError(), L"FreeConsole", __FILE__, __LINE__);
    }
}