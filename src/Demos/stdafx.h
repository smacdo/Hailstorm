// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

// Windows
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <Windows.h>                    // Windows API
#include <wrl\wrappers\corewrappers.h>  // ComPtr
#include <wrl\client.h>                 // ComPtr and friends.

// Standard library
#include <memory>           // Smart pointers.


