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
#ifndef SCOTT_HAILSTORM_GUI_ERRORDIALOG_H
#define SCOTT_HAILSTORM_GUI_ERRORDIALOG_H

#include <string>

class ErrorDialog
{
public:
    ErrorDialog(const std::wstring& message);
    ErrorDialog(const std::wstring& message, const std::wstring& details);
    ErrorDialog(const ErrorDialog&) = delete;
    ~ErrorDialog();

    ErrorDialog& operator =(const ErrorDialog&) = delete;

    void SetIsFatal(bool isFatal);
    bool IsFatal() const;

    void Show();
    bool DidUserPressQuit() const;

protected:
    HINSTANCE mAppInstance;
    HWND mWindowHandle;
    std::wstring mTitle;
    std::wstring mMainMessage;
    std::wstring mDetails;
    std::wstring mVerificationText;
    bool mIsFatal;
    bool mDidUserPressQuit;
    BOOL mIsVerifyChecked;
};

#endif