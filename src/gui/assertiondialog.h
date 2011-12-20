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
#ifndef SCOTT_ASSERTION_DIALOG_H
#define SCOTT_ASSERTION_DIALOG_H

#include <string>
#include <boost/utility.hpp>

/**
 * Assertion dialog is a class that encapsulates the user facing display and
 * handling of an application assertion. When created and called, it will 
 * display a dialog explaining the assertion that was triggered and then offer
 * the user several options for how they would like to continue.
 *
 * The dialog offers built in support for the player to save out a text report
 * of an assertion failure.
 */
class AssertionDialog : boost::noncopyable
{
public:
    AssertionDialog( const std::string& expression,
                     const std::string& filename,
                     unsigned int lineNumber );
    ~AssertionDialog();

    void setMessage( const std::string& message );
    bool show() const;

private:
    enum EUserAction
    {
        EUSERACTION_SAVE,
        EUSERACTION_DEBUG,
        EUSERACTION_QUIT
    };

protected:
    EUserAction showDialog() const;
    bool saveCrashDump() const;

    std::string getAssertionMessage( const std::string& intro ) const;
    std::string getFileReportString() const;

private:
    HINSTANCE mAppInstance;
    HWND mWindowHandle;
    std::string mExpression;
    std::string mFilename;
    unsigned int mLineNumber;
    std::string mMessage;
};

#endif