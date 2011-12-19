#include "stdafx.h"
#include "assertiondialog.h"
#include "Resource.h"

#include <string>
#include <sstream>
#include <algorithm>

#include <Commctrl.h>
#include <Shobjidl.h>

/**
 * Default constructor. The assertion dialog will try to guess the instance
 * and handle of the main window
 *
 * \param  expression  The expression that raised an assertion
 * \param  filename    Name of the file where the assertion was raised
 * \param  lineNumber  The line number where the assertion was raised
 */
AssertionDialog::AssertionDialog( const std::wstring& expression,
                                  const std::wstring& filename,
                                  unsigned int lineNumber )
    : mAppInstance( GetModuleHandle(NULL) ),
      mWindowHandle( GetActiveWindow() ),
      mExpression( expression ),
      mFilename( filename ),
      mLineNumber( lineNumber ),
      mMessage()
{
    mMessage = getAssertionMessage( L"The assertion was raised" );
}

/**
 * Assertion dialog destructor
 */
AssertionDialog::~AssertionDialog()
{

}

/**
 * Sets a custom message that is displayed alongside the normal assertion
 * details
 */
void AssertionDialog::setMessage( const std::wstring& message )
{
    mMessage = message;
}

/**
 * Shows the "assertion failed" dialog box, and gives the user several options
 * for how to continue. Once chosen, this method will also execute that action
 * for the user (such as saving the dialog window) before continuing
 *
 * \return             True if the user wants to start debugging, or false
 *                     if they selected the option to quit
 */
bool AssertionDialog::show()
{
    // The buttons that will appear on the assertion dialog
    const TASKDIALOG_BUTTON cb[] =
    {
        { 0, L"Save crash information (default)\nYou can help the developers of "
             L"this program by e-mailing them this crash information after saving "
             L"it to your computer" },
        { 1, L"Fire up the debugger (advanced)\nTriggers a breakpoint in Visual "
             L"Studio and jumps to the offending line of code" },
        { 2, L"Quit the application\nOh no! We promise not to crash again :(" }
    };

    // Create a dialog to display the assertion information and configure
    // its options
    TASKDIALOGCONFIG tc = { 0 };

    tc.cbSize         = sizeof( tc );
    tc.hwndParent     = mWindowHandle;
    tc.hInstance      = mAppInstance;
    tc.dwFlags        = TDF_USE_HICON_MAIN | TDF_USE_HICON_FOOTER |
                        TDF_EXPAND_FOOTER_AREA | TDF_USE_COMMAND_LINKS;

    LoadIconWithScaleDown( NULL, MAKEINTRESOURCE(IDI_ERROR),  /*IDI_ERROR,*/
        GetSystemMetrics(SM_CXICON),
        GetSystemMetrics(SM_CYICON),
        &tc.hMainIcon );
    LoadIconWithScaleDown( NULL, IDI_INFORMATION,
        GetSystemMetrics(SM_CXSMICON),
        GetSystemMetrics(SM_CYSMICON),
        &tc.hFooterIcon);

    tc.pszWindowTitle     = L"Application Assertion";
    tc.pszContent         = mMessage.c_str();
    tc.pszMainInstruction = L"An internal assertion check has failed";
    tc.pszFooter          = mExpression.c_str();
    
    tc.cButtons           = ARRAYSIZE( cb );
    tc.pButtons           = cb;
    tc.nDefaultButton     = 0;

    // Try displaying the assertion dialog
    int buttonPressed        = 0;
    int commandPressed       = 0;
    BOOL verificationChecked = false;

    HRESULT result = TaskDialogIndirect( &tc,
                                         &buttonPressed,
                                         &commandPressed,
                                         &verificationChecked );

    // See what the user's response was. Should we save out a report, trigger
    // the debugger or just exit?
    if ( buttonPressed == 0 )
    {
        // Save the crash report out
        saveCrashDump();
    }

    // Now return whether or not the user wanted to trigger the debugger
    return true;
}

/**
 * Code that handles the saving of a crash dump
 */
void AssertionDialog::saveCrashDump() const
{
    HRESULT result;

    // Create the file save as dialog
    IFileDialog *pFileDialog = NULL;
    result = CoCreateInstance( CLSID_FileSaveDialog, 
                               NULL,
                               CLSCTX_INPROC_SERVER,
                               IID_PPV_ARGS( &pFileDialog ) );

    if (! SUCCEEDED(result) )
    {
        return;
    }

    // File types supported
    COMDLG_FILTERSPEC filters[] = { { L"Text Files", L"*.txt" } };

    // Configure the save dialog with useful defaults
    pFileDialog->SetTitle( L"Save crash dump to a file" );
    pFileDialog->SetFileTypes( ARRAYSIZE(filters), filters );
    pFileDialog->SetDefaultExtension(L"*.txt");
    pFileDialog->SetFileName( L"crash details.txt" );

    // Show the file save as dialog
    pFileDialog->Show(NULL);

    // Open up a file stream to the file the user wanted to create

    // Now clean up. Release the dialog window and close the file stream
    pFileDialog->Release();
}

/**
 * Generates a formatted assertion message that can be presented to the user
 * in the area where you normally display the message details. This formatted
 * message contains the file name, line number and expression that failed.
 *
 * \param   intro  Text to introduce the error
 * \return  A formatted assertion message to display the user
 */
std::wstring AssertionDialog::getAssertionMessage( const std::wstring& intro ) const
{
    // Create a string stream and format the message
    //  typedef basic_ostringstream<wchar_t>
    std::wostringstream ss;

    ss << intro       << " in "
       << mFilename   << " on line "
       << mLineNumber << ". The failed expression was:\n"
       << mExpression;

    return ss.str();
}