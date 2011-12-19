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
#ifndef SCOTT_HAILSTORM_COMMON_DELETE_H
#define SCOTT_HAILSTORM_COMMON_DELETE_H

#include <map>
#include <vector>
#include <boost/checked_delete.hpp>
#include <algorithm>

/**
 * Deletes a pointer and then sets the address to NULL to prevent accidental
 * dereferences
 *
 * \param pointer The pointer to delete
 */
template<typename T>
void Delete( T& pointer )
{
    boost::checked_delete( pointer );
    pointer = NULL;
}

/**
 * Deletes an array pointer, and then sets the address to NULL to prevent
 * accidental dereferences
 *
 * \param arrayPointer The array pointer to delete
 */
template<typename T>
void DeleteArray( T& arrayPointer )
{
    boost::checked_array_delete( arrayPointer );
    arrayPointer = NULL;
}

/**
 * Deletes a generic container of pointer values by iterating through the
 * list, deleting each pointer element and then setting the size of the container
 * to zero.
 *
 * \param container A generic container holding pointers
 */
template<typename T>
void DeletePointerContainer( T& container )
{
    typename T::iterator itr;

    for ( itr = container.begin(); itr != container.end(); std::advance(itr,1) )
    {
        Delete<typename T::value_type>( *itr );
    }

    container.clear();
}

/**
 * Deletes an STL container of vectors, and resizes the container to zero.
 *
 * \param container A STL vector holding pointers
 */
template<typename T>
void DeleteVectorPointers( std::vector<T>& container )
{
    std::for_each( container.begin(), container.end(), Delete<T> );
    container.clear();
}

/**
 * Deletes an STL map of containers, and resizes the container to zero.
 *
 * \param container A STL map holding pointers
 */
template<typename T, typename U>
void DeleteMapPointers( std::map<T,U>& container )
{
    typename std::map<T,U>::iterator itr;

    for ( itr = container.begin(); itr != container.end(); ++itr )
    {
        boost::checked_delete( itr->second );
    }

    container.clear();
}

#endif