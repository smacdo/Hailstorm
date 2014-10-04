/*
 * Copyright 2011 - 2014 Scott MacDonald
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
#include "runtime\exceptions.h"
#include "runtime\StringUtils.h"

#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Hailstorm exception base class.
///////////////////////////////////////////////////////////////////////////////////////////////////
HailstormException::HailstormException()
    : HailstormException(std::wstring(L"Unspecified exception"))
{
}

HailstormException::HailstormException(const char * message)
    : HailstormException(Utils::ConvertUtf8ToWideString(message))
{

}

HailstormException::HailstormException(const std::string& message)
    : HailstormException(Utils::ConvertUtf8ToWideString(message.c_str()))
{

}

HailstormException::HailstormException(const std::wstring& message)
    : HailstormException(message, L"", L"")
{

}

HailstormException::HailstormException(
    const std::wstring& message,
    const std::wstring& details,
    const std::wstring& context)
    : HailstormException(message, details, context, L"", 0)
{

}

HailstormException::HailstormException(
    const std::wstring& message,
    const std::wstring& details,
    const std::wstring& context,
    const std::wstring& filename,
    unsigned int lineNumber)
    : std::runtime_error(Utils::ConvertWideStringToUtf8(message)),
      mMessage(message),
      mDetails(details),
      mContext(context),
      mFileName(filename),
      mLineNumber(lineNumber)
{
}