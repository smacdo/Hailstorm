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
#include "stdafx.h"
#include "gui/aboutbox.h"
#include "common/platform_windows.h"
#include "resource.h"

#include <cassert>

AboutBox::AboutBox( HINSTANCE hInstance, HWND hWnd )
    : mAppInstance( hInstance ),
      mParentWindow( hWnd )
{
}

AboutBox::~AboutBox()
{
}

void AboutBox::show()
{
    DialogBox( mAppInstance,
               MAKEINTRESOURCE(IDD_ABOUTBOX),
               mParentWindow,
               DlgWndProc );
}

// Message handler for about box.
INT_PTR CALLBACK DlgWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}