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
#ifndef SCOTT_HAILSTORM_GUI_ERRORDIALOG_H
#define SCOTT_HAILSTORM_GUI_ERRORDIALOG_H

#include <string>

class ErrorDialog
{
public:
    ErrorDialog( const std::string& message );
    ErrorDialog( const std::string& message, const std::string& details );
    ErrorDialog(const ErrorDialog&) = delete;
    ~ErrorDialog();

    ErrorDialog& operator =(const ErrorDialog&) = delete;

    void setIsFatal( bool isFatal );
    bool isFatal() const;

    void show();
    bool didUserPressQuit() const;

protected:
    HINSTANCE mAppInstance;
    HWND mWindowHandle;
    std::string mTitle;
    std::string mMainMessage;
    std::string mDetails;
    std::string mVerificationText;
    bool mIsFatal;
    bool mDidUserPressQuit;
    BOOL mIsVerifyChecked;
};

#endif