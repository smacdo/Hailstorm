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
#ifndef SCOTT_HAILSTORM_VERSION_H
#define SCOTT_HAILSTORM_VERSION_H

namespace Version
{
    const char * APP_NAME       = "Hailstorm Engine";
    const char * BUILD_STRING   = "NO_BUILD_STRING";
    const char * VERSION_STRING = "0.1.0";
    const int VERSION_MAJOR     = 0;
    const int VERSION_MINOR     = 1;
    const int VERSION_PATCH     = 0;
    const int VERSION_PARTS     = 3;
    const int VERSION[VERSION_PARTS] = { VERSION_MAJOR,
                                         VERSION_MINOR,
                                         VERSION_PATCH };
}

#endif