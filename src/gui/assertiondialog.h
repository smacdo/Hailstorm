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
    AssertionDialog( const std::wstring& expression,
                     const std::wstring& filename,
                     unsigned int lineNumber );
    ~AssertionDialog();

    void setMessage( const std::wstring& message );
    bool show();

protected:
    std::wstring getAssertionMessage( const std::wstring& intro ) const;
    void saveCrashDump() const;

private:
    HINSTANCE mAppInstance;
    HWND mWindowHandle;
    std::wstring mExpression;
    std::wstring mFilename;
    unsigned int mLineNumber;
    std::wstring mMessage;
};

#endif