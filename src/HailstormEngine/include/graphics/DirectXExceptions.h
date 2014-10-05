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
#ifndef SCOTT_DIRECTX_EXCEPTIONS_H
#define SCOTT_DIRECTX_EXCEPTIONS_H

#include "runtime/exceptions.h"
#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Renderer exceptions.
//  TODO: Move these into another file.
///////////////////////////////////////////////////////////////////////////////////////////////////
class RendererException : public HailstormException
{
public:
    RendererException(
        const std::wstring& title,
        const std::wstring& details,
        const std::wstring& context,
        const char * fileName,
        unsigned int lineNumber);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// DirectX Exceptions.
///////////////////////////////////////////////////////////////////////////////////////////////////
class DirectXException : public HailstormException
{
public:
    DirectXException(unsigned long errorCode)
        : DirectXException(errorCode, L"", L"")
    {
    }

    DirectXException(
        unsigned long errorCode,
        const std::wstring& details,
        const std::wstring& context);

    DirectXException(
        unsigned long errorCode,
        const std::wstring& details,
        const std::wstring& context,
        const char * fileName,
        unsigned int lineNumber);

    static std::wstring ErrorCodeToString(unsigned long errorCode);
};

class ShaderCompileFailedException : public DirectXException
{
public:
    ShaderCompileFailedException(
        unsigned long errorCode,
        const std::wstring& shaderFileName,
        const char * errorText);
};

#endif