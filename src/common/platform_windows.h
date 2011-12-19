#ifndef SCOTT_PLATFORM_WINDOWS_H
#define SCOTT_PLATFORM_WINDOWS_H

#include "stdafx.h"

namespace WinApp
{
    std::wstring ToWideString( const std::string& str );
}

LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	DlgWndProc(HWND, UINT, WPARAM, LPARAM);

#endif