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
#ifndef SCOTT_HAILSTORM_GUI_ERRORDIALOG_H
#define SCOTT_HAILSTORM_GUI_ERRORDIALOG_H

#include <string>
#include <boost/noncopyable.hpp>

class ErrorDialog : boost::noncopyable
{
public:
    ErrorDialog( const std::string& message );
    ErrorDialog( const std::string& message, const std::string& details );
    ~ErrorDialog();

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