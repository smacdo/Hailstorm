/*
 * Copyright 2014 Scott MacDonald
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
#include "runtime/StringUtils.h"

#include <string>
#include <algorithm>
#include <cctype>
#include <functional>

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>

std::wstring Utils::ConvertUtf8ToWideString(const std::string& input)
{
    // Deal with the trivial case of being passed an empty string.
    if (input.empty())
    {
        return std::wstring();
    }

    // Determine the length required to hold the converted wstring.
    int requiredLength = ::MultiByteToWideChar(
        CP_UTF8,
        MB_ERR_INVALID_CHARS,
        input.c_str(),
        static_cast<int>(input.length() * sizeof(char)),
        0,
        0);

    if (requiredLength == 0)
    {
        // do something here.
    }

    // Presize an empty wstring and convert the UTF8 string to wstring in place.
    std::wstring output(requiredLength, L'\0');
    int result = ::MultiByteToWideChar(
        CP_UTF8,
        MB_ERR_INVALID_CHARS,
        input.c_str(),
        static_cast<int>(input.length()),
        &output[0],
        static_cast<int>(output.length()));

    if (result == 0)
    {
        // do something here.
    }

    return output;
}

// TODO: Test MB_ERR_INVALID_CHARS and WC_NO_BEST_FIT_CHARS (look at docs)
std::string Utils::ConvertWideStringToUtf8(const std::wstring& input)
{
    // Deal with the trivial case of being passed an empty string.
    if (input.empty())
    {
        return std::string();
    }

    // Determine the length required to hold the converted string.
    int requiredLength = ::WideCharToMultiByte(
        CP_UTF8,
        WC_ERR_INVALID_CHARS,
        input.c_str(),
        static_cast<int>(input.length()),
        0,          // pointer to buffer that receives the converted string.
        0,          // size in bytes of the buffer above.
        nullptr,    // default character if converted char cannot be represented (must be null for UTF8).
        nullptr);   // pointer to flag that indicates default character used. (must be null for UTF8).

    if (requiredLength == 0)
    {
        // do something here.
    }

    // Presize an empty string and convert the wide string to a UTF8 string in place.
    std::string output(requiredLength, '\0');
    int result = ::WideCharToMultiByte(
        CP_UTF8,
        WC_ERR_INVALID_CHARS,
        input.c_str(),                                      // wide string pointer.
        static_cast<int>(input.length()),                   // wide string length in characters.
        &output[0],                                         // narrow output string pointer.
        static_cast<int>(output.length() * sizeof(char)),   // narrow output string length in bytes.
        nullptr,                                            // default character for invalid converted chars.
        nullptr);                                           // pointer to flag that is set if default char is used.

    if (result == 0)
    {
        // do something here.
    }

    return output;
}

std::wstring Utils::GetWinApiErrorText(unsigned long errorCode)
{
    return GetHResultErrorText(errorCode);
}

std::wstring Utils::GetHResultErrorText(unsigned long errorCode)
{
    LPTSTR errorText = nullptr;
    std::wstring output;

    DWORD result = FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM		    // Look at system message definitions when looking up this error code.
        | FORMAT_MESSAGE_ALLOCATE_BUFFER	// Allocate a string buffer for us. TODO: Don't use this because WinRT. 
        | FORMAT_MESSAGE_IGNORE_INSERTS,	// Don't use FormatMessage inserts. No point, and difficult to get right.
        nullptr,                            // No source for message definition, since it is a system code.
        errorCode,                          // The actual error code to look up.
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),  // Use the system default language.
        (LPTSTR)&errorText,                 // String that receives the error message.
        0,									// Allow windows to create a string buffer for us.
        nullptr);							// No message formatting parameters.

    if (result == 0)
    {
        DWORD formatMessageError = GetLastError();
        output = L"* FAILED TO LOOK UP ERROR CODE. LastError = " + std::to_wstring(formatMessageError);
    }
    else if (errorText == nullptr)
    {
        output = L"* FAILED TO LOOK UP ERROR CODE. Message was null. ";
    }
    else if (errorText != nullptr)
    {
        // Copy temporary buffer allocate by windows into the output string, and then release the windows
        // allocated string buffer.
        output = std::wstring(errorText);

        LocalFree(errorText);
        errorText = nullptr;
    }

    return output;
}