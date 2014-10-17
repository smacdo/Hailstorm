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
#include "bases\Initializable.h"

#include "runtime/StringUtils.h"
#include <string>

// Constructor.
Initializable::Initializable()
    : mIsInitialized(false)
{
}

// Destructor.
Initializable::~Initializable()
{
    // Empty.
}

// Internal helper that derived class calls to mark the instance as initialized.
void Initializable::SetIsInitialized()
{
    mIsInitialized = true;
}

// Destructor. Shutdown class if it has not already been shutdown.
bool Initializable::IsInitialized() const
{
    return mIsInitialized;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Exception that is thrown when a method is called on an non-initialized instance.
///////////////////////////////////////////////////////////////////////////////////////////////////
NotInitializedException::NotInitializedException(
    const std::wstring& className,
    const char * fileName,
    unsigned int lineNumber)
    : HailstormException(
        L"Object not initialized",
        L"The object was not initialized prior to being used",
        className,
        Utils::ConvertUtf8ToWideString(fileName),
        lineNumber)
{
}
