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
#include "graphics/DirectXExceptions.h"
#include "runtime/exceptions.h"
#include "runtime/StringUtils.h"

#include <string>

RendererException::RendererException(
    const std::wstring& title,
    const std::wstring& details,
    const std::wstring& context,
    const char * fileName,
    unsigned int lineNumber)
    : HailstormException(
        title,
        details,
        context,
        Utils::ConvertUtf8ToWideString(fileName),
        lineNumber)
{
}

DirectXException::DirectXException(
    unsigned long errorCode,
    const std::wstring& details,
    const std::wstring& context)
    : HailstormException(DirectXException::ErrorCodeToString(errorCode), details, context)
{
}

DirectXException::DirectXException(
    unsigned long errorCode,
    const std::wstring& details,
    const std::wstring& context,
    const char * fileName,
    unsigned int lineNumber)
    : HailstormException(
        DirectXException::ErrorCodeToString(errorCode),
        details,
        context,
        Utils::ConvertUtf8ToWideString(fileName),
        lineNumber)
{
}

std::wstring DirectXException::ErrorCodeToString(unsigned long errorCode)
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
        output = L"*** FAILED TO LOOK UP ERROR CODE (Non-zero error returned) ***";
    }
    else if (errorText == nullptr)
    {
        output = L"*** FAILED TO LOOK UP ERROR CODE (Message was null) ***";
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

ShaderCompileFailedException::ShaderCompileFailedException(
    unsigned long errorCode,
    const std::wstring& shaderFileName,
    const char * errorText)
    : DirectXException(
        errorCode,
        Utils::ConvertUtf8ToWideString(errorText),
        shaderFileName)
{
}