/*
 * Copyright (C) 2011 Scott MacDonald. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
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