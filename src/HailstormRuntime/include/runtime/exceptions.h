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
#ifndef SCOTT_EXCEPTIONS_H
#define SCOTT_EXCEPTIONS_H

#include <exception>
#include <string>

class HailstormException : public std::runtime_error
{
public:
    HailstormException();
    HailstormException(const char * message);
    HailstormException(const std::string& message);
    HailstormException(const std::wstring& message);
    HailstormException(
        const std::wstring& title,
        const std::wstring& details,
        const std::wstring& context);
    HailstormException(
        const std::wstring& titles,
        const std::wstring& details,
        const std::wstring& context,
        const std::wstring& filename,
        unsigned int lineNumber);

    std::wstring Title() const { return mTitle; }
    std::wstring Details() const { return mDetails; }
    std::wstring Context() const { return mContext; }
    std::wstring FileName() const { return mFileName; }
    unsigned int LineNumber() const { return mLineNumber; }

private:
    const std::wstring& mTitle;
    const std::wstring& mDetails;
    const std::wstring& mContext;
    const std::wstring& mFileName;
    unsigned int mLineNumber;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// Assertion failed.
///////////////////////////////////////////////////////////////////////////////////////////////////
class AssertionFailedException : public HailstormException
{
public:
    AssertionFailedException(
        const char * expression,
        const char * fileName,
        unsigned int lineNumber);

    AssertionFailedException(
        const std::wstring& message,
        const char * expression,
        const char * fileName,
        unsigned int lineNumber);

private:

};

class IsNullAssertionFailedException : public AssertionFailedException
{
public:
    IsNullAssertionFailedException(
        const char * expression,
        const char * fileName,
        unsigned int lineNumber);
};

class IsNotNullAssertionFailedException : public AssertionFailedException
{
public:
    IsNotNullAssertionFailedException(
        const char * expression,
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
        : HailstormException(L"DirectX Exception")
    {
    }

    DirectXException(unsigned long errorCode, const std::wstring& details, const std::wstring& context)
        : HailstormException(L"DirectX Exception", details, context)
    {
    }

private:

};

#endif