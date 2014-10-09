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
#include "assertiondialog.h"
#include "version.h"
#include "runtime\StringUtils.h"
#include "Resource.h"

#include <string>
#include <sstream>
#include <algorithm>

#include <Commctrl.h>
#include <Shobjidl.h>

#include <wrl\wrappers\corewrappers.h>  // ComPtr.
#include <wrl\client.h>                 // ComPtr friends.

/**
 * Default constructor. The assertion dialog will try to guess the instance
 * and handle of the main window
 *
 * \param  expression  The expression that raised an assertion
 * \param  filename    Name of the file where the assertion was raised
 * \param  lineNumber  The line number where the assertion was raised
 */
AssertionDialog::AssertionDialog(const std::wstring& expression,
                                 const std::wstring& filename,
                                 unsigned int lineNumber)
    : mAppInstance(GetModuleHandle(NULL)),
      mWindowHandle(GetActiveWindow()),
      mMessage(),
      mExpression(expression),
      mFilename(filename),
      mLineNumber(lineNumber)
{
    mMessage = FormatMessage(L"The assertion was raised");
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
void AssertionDialog::SetMessage(const std::wstring& message)
{
    if (message.size() > 0)
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
bool AssertionDialog::Show() const
{
    AssertionDialog::EUserAction userAction;
    bool shouldStartDebugging = false;

    // Keep the dialog window up until the user has successfully saved the report, or chosen another action.
    bool keepGoing = true;

    while (keepGoing)
    {
        userAction = ShowDialog();

        if (userAction == EUSERACTION_SAVE)
        {
            keepGoing = (!SaveCrashDump());
        }
        else if (userAction == EUSERACTION_DEBUG)
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
AssertionDialog::EUserAction AssertionDialog::ShowDialog() const
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

    tc.cbSize = sizeof(tc);
    tc.hwndParent = mWindowHandle;
    tc.hInstance = mAppInstance;
    tc.dwFlags = TDF_USE_HICON_MAIN | TDF_USE_HICON_FOOTER | TDF_EXPAND_FOOTER_AREA | TDF_USE_COMMAND_LINKS;

    LoadIconWithScaleDown(
        NULL,
        MAKEINTRESOURCE(IDI_ERROR),  /*IDI_ERROR,*/
        GetSystemMetrics(SM_CXICON),
        GetSystemMetrics(SM_CYICON),
        &tc.hMainIcon);

    LoadIconWithScaleDown(
        NULL,
        IDI_INFORMATION,
        GetSystemMetrics(SM_CXSMICON),
        GetSystemMetrics(SM_CYSMICON),
        &tc.hFooterIcon);

    tc.pszWindowTitle = L"Application Assertion";
    tc.pszContent = mMessage.c_str();
    tc.pszMainInstruction = L"An internal assertion check has failed";
    tc.pszFooter = mExpression.c_str();

    tc.cButtons = ARRAYSIZE(cb);
    tc.pButtons = cb;
    tc.nDefaultButton = 0;

    // After all that tedious set up work, we can finally show off our fancy shmancy assertion dialog!
    int buttonPressed = 0;
    int commandPressed = 0;
    BOOL verificationChecked = false;

    TaskDialogIndirect(
        &tc,
        &buttonPressed,
        &commandPressed,
        &verificationChecked);

    // Return which action was selected
    switch (buttonPressed)
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
bool AssertionDialog::SaveCrashDump() const
{
    bool didSaveFile = false;

    // Create the file save as dialog
    Microsoft::WRL::ComPtr<IFileDialog> fileDialog;
    HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&fileDialog));

    if (!SUCCEEDED(hr))
    {
        return false;
    }

    // File types supported
    COMDLG_FILTERSPEC filters[] = { { L"Text Files", L"*.txt" } };

    // Configure the save dialog with useful defaults
    fileDialog->SetTitle(L"Save crash dump to a file");
    fileDialog->SetFileTypes(ARRAYSIZE(filters), filters);
    fileDialog->SetDefaultExtension(L"*.txt");
    fileDialog->SetFileName(L"crash details.txt");

    // Show the file save as dialog
    fileDialog->Show(mWindowHandle);

    // Find the name of the file they want to save the assertion report to.
    // If the user didn't give an appropriate response, simply leave the filename
    // to be null
    Microsoft::WRL::ComPtr<IShellItem> dialogResult;
    PWSTR filepath = nullptr;

    hr = fileDialog->GetResult(&dialogResult);

    if (SUCCEEDED(hr))
    {
        HRESULT hr = dialogResult->GetDisplayName(SIGDN_FILESYSPATH, &filepath);

        if (SUCCEEDED(hr))
        {
            std::string fileContents = Utils::ConvertWideStringToUtf8(GetFileReportString());

            // First we have to create the file
            HANDLE fileHandle = CreateFile(
                filepath,
                GENERIC_WRITE,
                0,
                nullptr,
                CREATE_ALWAYS,
                FILE_ATTRIBUTE_NORMAL,
                nullptr);

            // Convert the text file to UTF8 and then write the string to disk.
            WriteFile(
                fileHandle,
                fileContents.c_str(),
                static_cast<DWORD>(fileContents.size()),
                nullptr,
                nullptr);

            // Remember that we wrote the file out
            didSaveFile = true;
        }
    }

    // Now clean up. Release the dialog window and close the file stream.
    CoTaskMemFree(filepath);
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
std::wstring AssertionDialog::FormatMessage(const std::wstring& intro) const
{
    // Create a string stream and format the message
    //  typedef basic_ostringstream<wchar_t>
    std::wostringstream ss;

    ss
        << intro       << " in "
        << mFilename   << " on line "
        << mLineNumber << ". The failed expression was:\n"
        << mExpression;

    return ss.str();
}

/**
 * Creates the assertion report string that should be written out to a file
 * and presented to the developer
 */
std::wstring AssertionDialog::GetFileReportString() const
{
    std::wostringstream ss;

    ss << L"# Application assertion report data" << std::endl
       << L"APPLICATION: " << Version::APP_NAME << std::endl
       << L"LVERSION    : " << Version::VERSION_STRING << std::endl
       << L"BUILDSTRING: " << Version::BUILD_STRING << std::endl
#ifdef _WIN64
       << L"PLATFORM   : " << "WIN64 (x64)" << std::endl
#else
       << L"PLATFORM   : " << "WIN32 (x86)" << std::endl
#endif
       << L"EXPRESSION : " << mExpression << std::endl
       << L"SOURCEFILE : " << mFilename << std::endl
       << L"LINENUMBER : " << mLineNumber << std::endl
       << L"MESSAGE    : " << mMessage << std::endl;

    return ss.str();
}