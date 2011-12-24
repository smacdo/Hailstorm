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