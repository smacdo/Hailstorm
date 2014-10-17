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
    return Utils::GetHResultErrorText(errorCode);
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