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
#include "graphics/dxutils.h"
#include "graphics/dxrenderer.h"
#include "graphics/dxutils.h"
#include "runtime/logging.h"

#include <d3d10.h>
#include <d3dx10.h>

namespace DxUtils
{
    /**
     * Takes a DirectX HRESULT return code, and checks that it was successfully. This
     * method will create an error message that includes diagnostic information if
     * the value is not HRESULT, and then return false.
     *
     * \param  result  The HRESULT to check
     * \param  action  The action that the renderer was attempting to perform
     * \return         True if the check succeeded, false otherwise
     */
    bool CheckResult( HRESULT result, bool shouldExit, const std::string& action )
    {
       // Don't do anything if it worked
        if ( result == S_OK )
        {
            return true;
        }
/* 
        // Now figure out what the error status code is
        std::string errorText;

        switch ( result )
        {
        case D3D10_ERROR_FILE_NOT_FOUND:
            errorText = "File not found";
            break;

        case D3D10_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS:
            errorText = "Too many unique state objects";
            break;

        case D3DERR_INVALIDCALL:
            errorText = "Invalid call (is a parameter wrong or null?)";
            break;

        case D3DERR_WASSTILLDRAWING:
            errorText = "Previous blit is still transferring information";
            break;

        case E_FAIL:
            errorText = "Cannot create device w/ debug layer when not installed";
            break;

        case E_INVALIDARG:
            errorText = "An invalid argument was passed";
            break;

        case E_OUTOFMEMORY:
            errorText = "D3D could not allocate sufficient memory";
            break;

        case S_FALSE:
            errorText = "S_FALSE? WTF?";
            break;

        default:
            errorText = "NO_ERROR_TEXT_AVAILABLE";
            break;
        }

        // Raise the error with the application before returning false
        if ( shouldExit )
        {
            App::raiseFatalError( "Call to DirectX failed. Attempting: " + action,
                errorText );
            App::quit( App::EPROGRAM_FATAL_ERROR, "DirectX call failed" );
        }
        else
        {
            App::raiseError( "Failed to perform: " + action, errorText );
        }*/

        return false;
    }

}