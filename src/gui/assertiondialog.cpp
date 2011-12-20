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
#include "assertiondialog.h"
#include "version.h"
#include "common/platform_windows.h"
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
AssertionDialog::AssertionDialog( const std::string& expression,
                                  const std::string& filename,
                                  unsigned int lineNumber )
    : mAppInstance( GetModuleHandle(NULL) ),
      mWindowHandle( GetActiveWindow() ),
      mExpression( expression ),
      mFilename( filename ),
      mLineNumber( lineNumber ),
      mMessage()
{
    mMessage = getAssertionMessage( "The assertion was raised" );
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
void AssertionDialog::setMessage( const std::string& message )
{
    if ( message.size() > 0 )
    {
        mMessage = message;
    }
}

/**
 * Shows the "assertion failed" dialog box, and gives the user several options
 * for how to continue. Once chosen, this method will also execute that action
 * for the user (such as saving the dialog window) before continuing
 *
 * \return             True if the user wants to start debugging, or false
 *                     if they selected the option to quit
 */
bool AssertionDialog::show() const
{
    AssertionDialog::EUserAction userAction;
    bool shouldStartDebugging = false;

    // Keep the dialog window up until the user has successfully saved the
    // report, or chosen another action
    bool keepGoing = true;

    while ( keepGoing )
    {
        userAction = showDialog();
        
        if ( userAction == EUSERACTION_SAVE )
        {
            keepGoing = (! saveCrashDump() );
        }
        else if ( userAction == EUSERACTION_DEBUG )
        {
            keepGoing = false;
            shouldStartDebugging = true;
        }
        else
        {
            keepGoing = false;
        }
    }

    // Let the caller know if the user wanted to start the debugger
    return shouldStartDebugging;
}

/**
 * Presents the user with the assertion failed dialog box, and returns the
 * action that the user chose to perform
 */
AssertionDialog::EUserAction AssertionDialog::showDialog() const
{
    // Convert our string parameters to window's wstring format
    std::wstring expression = WinApp::ToWideString( mExpression );
    std::wstring message    = WinApp::ToWideString( mMessage );

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
    tc.pszContent         = message.c_str();
    tc.pszMainInstruction = L"An internal assertion check has failed";
    tc.pszFooter          = expression.c_str();
    
    tc.cButtons           = ARRAYSIZE( cb );
    tc.pButtons           = cb;
    tc.nDefaultButton     = 0;

    // After all that tedious set up work, we can finally show off our fancy
    // shmancy assertion dialog!
    int buttonPressed        = 0;
    int commandPressed       = 0;
    BOOL verificationChecked = false;

    HRESULT result = TaskDialogIndirect( &tc,
                                         &buttonPressed,
                                         &commandPressed,
                                         &verificationChecked );

    // Return which action was selected
    switch ( buttonPressed )
    {
        case 0:
            return EUSERACTION_SAVE;
        case 1:
            return EUSERACTION_DEBUG;
        case 2:
            return EUSERACTION_QUIT;
        default:
            return EUSERACTION_QUIT;
    }
}

/**
 * This method handles the actual details of saving an assertion report. It will
 * bring up the "File save as..." dialog and then write the report out to
 * whichever file the user selects
 *
 * \return  True if the user saved the file, false if they aborted the action
 */
bool AssertionDialog::saveCrashDump() const
{
    HRESULT result;
    bool didSaveFile = false;

    // Create the file save as dialog
    IFileDialog *pFileDialog = NULL;
    result = CoCreateInstance( CLSID_FileSaveDialog, 
                               NULL,
                               CLSCTX_INPROC_SERVER,
                               IID_PPV_ARGS( &pFileDialog ) );

    if (! SUCCEEDED(result) )
    {
        return false;
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

    // Find the name of the file they want to save the assertion report to.
    // If the user didn't give an appropriate response, simply leave the filename
    // to be null
    IShellItem *pDialogResult = NULL;
    PWSTR       pFilePath     = NULL;

    pFileDialog->GetResult( &pDialogResult );

    if ( pDialogResult != NULL )
    {
        pDialogResult->GetDisplayName( SIGDN_FILESYSPATH, &pFilePath );
    }

    if ( pFilePath != NULL )
    {
        std::string fileContents = getFileReportString();

        // First we have to create the file
        HANDLE fileHandle = CreateFile( pFilePath,
                                        GENERIC_WRITE,
                                        0,
                                        NULL,
                                        CREATE_ALWAYS,
                                        FILE_ATTRIBUTE_NORMAL,
                                        NULL );

        // Now we can write the assertion details out to disk
        DWORD numBytesWritten = 0;

        WriteFile( fileHandle,
                   fileContents.c_str(),
                   fileContents.size(),
                   &numBytesWritten,
                   NULL );

        // Remember that we wrote the file out
        didSaveFile = true;
    }

    // Now clean up. Release the dialog window and close the file stream
    pFileDialog->Release();
    return didSaveFile;
}

/**
 * Generates a formatted assertion message that can be presented to the user
 * in the area where you normally display the message details. This formatted
 * message contains the file name, line number and expression that failed.
 *
 * \param   intro  Text to introduce the error
 * \return  A formatted assertion message to display the user
 */
std::string AssertionDialog::getAssertionMessage( const std::string& intro ) const
{
    // Create a string stream and format the message
    //  typedef basic_ostringstream<wchar_t>
    std::ostringstream ss;

    ss << intro       << " in "
       << mFilename   << " on line "
       << mLineNumber << ". The failed expression was:\n"
       << mExpression;

    return ss.str();
}

/**
 * Creates the assertion report string that should be written out to a file
 * and presented to the developer
 */
std::string AssertionDialog::getFileReportString() const
{
    std::ostringstream ss;

    ss << "# Application assertion report data"             << "\r\n"
       << "APPLICATION: "       << Version::APP_NAME        << "\r\n"
       << "VERSION    : "       << Version::VERSION_STRING  << "\r\n"
       << "BUILDSTRING: "       << Version::BUILD_STRING    << "\r\n"
#ifdef _WIN64
       << "PLATFORM   : "       << "WIN64 (x64)"            << "\r\n"
#else
       << "PLATFORM   : "       << "WIN32 (x86)"            << "\r\n"
#endif
       << "MESSAGE    : "       << mMessage                 << "\r\n"
       << "EXPRESSION : "       << mExpression              << "\r\n"
       << "SOURCEFILE : "       << mFilename                << "\r\n"
       << "LINENUMBER : "       << mLineNumber              << std::endl;

    return ss.str();
}