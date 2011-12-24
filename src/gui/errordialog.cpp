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
#include "gui/errordialog.h"
#include "common/platform_windows.h"

#include "Resource.h"

#include <string>

#include <Commctrl.h>
#include <Shobjidl.h>

ErrorDialog::ErrorDialog( const std::string& message )
    : mAppInstance( GetModuleHandle(NULL) ),
      mWindowHandle( GetActiveWindow() ),
      mTitle( "Hailstorm Engine" ),
      mMainMessage( message ),
      mDetails(),
      mVerificationText(),
      mIsFatal( false ),
      mDidUserPressQuit( false ),
      mIsVerifyChecked( false )
{
}

ErrorDialog::ErrorDialog( const std::string& message, const std::string& details )
    : mAppInstance( GetModuleHandle(NULL) ),
      mWindowHandle( GetActiveWindow() ),
      mTitle( "Hailstorm Engine" ),
      mMainMessage( message ),
      mDetails( details ),
      mIsFatal( false ),
      mDidUserPressQuit( false ),
      mIsVerifyChecked( false )
{
}

ErrorDialog::~ErrorDialog()
{

}

void ErrorDialog::setIsFatal( bool isFatal )
{
    mIsFatal = isFatal;
}

bool ErrorDialog::isFatal() const
{
    return mIsFatal;
}

bool ErrorDialog::didUserPressQuit() const
{
    return mDidUserPressQuit;
}

void ErrorDialog::show()
{
    std::wstring title       = WinApp::ToWideString( mTitle );
    std::wstring mainMessage = WinApp::ToWideString( mMainMessage );
    std::wstring details     = WinApp::ToWideString( mDetails );

    // The buttons that will appear on the assertion dialog
    const TASKDIALOG_BUTTON cb[] =
    {
        { 0, L"Continue" },
        { 1, L"Quit" },
    };

    // Create a dialog to display the assertion information and configure
    // its options
    TASKDIALOGCONFIG tc = { 0 };

    tc.cbSize         = sizeof( tc );
    tc.hwndParent     = mWindowHandle;
    tc.hInstance      = mAppInstance;

    // Dialog flags
    tc.dwFlags        = TDF_USE_HICON_MAIN;

    if (! mIsFatal )
    {
        tc.dwFlags |= TDF_ALLOW_DIALOG_CANCELLATION;
    }

    // Load the appropriate icon for the dialog
    if ( mIsFatal )
    {
        LoadIconWithScaleDown( NULL, MAKEINTRESOURCE(IDI_ERROR),  /*IDI_ERROR,*/
            GetSystemMetrics(SM_CXICON),
            GetSystemMetrics(SM_CYICON),
            &tc.hMainIcon );
    }
    else
    {
        LoadIconWithScaleDown( NULL, MAKEINTRESOURCE(IDI_WARNING),  /*IDI_ERROR,*/
            GetSystemMetrics(SM_CXICON),
            GetSystemMetrics(SM_CYICON),
            &tc.hMainIcon );
    }

    // Set up dialog buttons
    if ( mIsFatal )
    {
        tc.cButtons           = 1;
        tc.pButtons           = &cb[1];
        tc.nDefaultButton     = 0;
    }
    else
    {
        tc.cButtons           = 2;
        tc.pButtons           = &cb[0];
        tc.nDefaultButton     = 0;
    }

    // Set up the error text and make sure it is presented to the user in
    // a nice way
    tc.pszWindowTitle     = title.c_str();

    if ( details.empty() )
    {
        tc.pszMainInstruction = L"An error has occurred";
        tc.pszContent         = mainMessage.c_str();
    }
    else
    {
        tc.pszMainInstruction = mainMessage.c_str();
        tc.pszContent         = details.c_str();
    }

    // Try displaying the assertion dialog
    int buttonPressed        = 0;
    int commandPressed       = 0;

    HRESULT result = TaskDialogIndirect( &tc,
        &buttonPressed,
        &commandPressed,
        &mIsVerifyChecked );

    // Did the user press the quit button?
    if ( commandPressed == 1 )
    {
        mDidUserPressQuit = true;
    }
}