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
#ifndef SCOTT_COMMON_LOGGING_IMPLEMENTATION_INL_H
#define SCOTT_COMMON_LOGGING_IMPLEMENTATION_INL_H
#include "common/logging.h"
#include "common/logging_impl.h"
#include <sstream>
#include <ctime>
#include <cassert>

#include <ostream>
#include <fstream>

/**
 * The debug stream buffer constructor. Takes a pointer to a stream buffer
 * that acts as a backing store.
 */
template<typename C, typename T>
DebugStreambuf<C,T>::DebugStreambuf()
    : BUFFER_SIZE( 16 ),
      mpInternalBuffer( new char[BUFFER_SIZE] ),
      mpConsoleBuffer( NULL ),
      mpFileBuffer( NULL ),
      mDefaultLogLevel( ELOGLEVEL_INFO ),
      mDefaultModuleName( "App" ),
      mConsoleThreshold( ELOGLEVEL_DEBUG ),
      mFileThreshold( ELOGLEVEL_DEBUG ),
      mAtLineStart( true ),
      mLogLevel( mDefaultLogLevel ),
      mModuleName( mDefaultModuleName )
{
//    setp( &mpInternalBuffer[0], &mpInternalBuffer[BUFFER_SIZE] );
}

/**
 * The debug stream buffer constructor. Takes a pointer to a stream buffer
 * that acts as a backing store.
 */
template<typename C, typename T>
DebugStreambuf<C,T>::DebugStreambuf( std::basic_streambuf<C,T>* pConsoleBuffer,
                                     std::basic_filebuf<C,T>* pFileBuffer )
    : BUFFER_SIZE( 16 ),
      mpInternalBuffer( new char[BUFFER_SIZE] ),
      mpConsoleBuffer( pConsoleBuffer ),
      mpFileBuffer( pFileBuffer ),
      mDefaultLogLevel( ELOGLEVEL_INFO ),
      mDefaultModuleName( "App" ),
      mConsoleThreshold( ELOGLEVEL_INFO ),
      mFileThreshold( ELOGLEVEL_DEBUG ),
      mAtLineStart( true ),
      mLogLevel( mDefaultLogLevel ),
      mModuleName( mDefaultModuleName )
{
//    setp( &mpInternalBuffer[0], &mpInternalBuffer[BUFFER_SIZE] );
}

/**
 * Destructor
 */
template<typename C, typename T>
DebugStreambuf<C,T>::~DebugStreambuf()
{
    boost::checked_array_delete( mpConsoleBuffer );
}

/**
 * Set the minimum severity level required before a log entry gets written
 * to the user's console
 */
template<typename C, typename T>
void DebugStreambuf<C,T>::setConsoleThreshold( ELogLevel logLevel )
{
    mConsoleThreshold = logLevel;
}

/**
 * Set the minimum severity level required before a log entry gets written
 * to the log file
 */
template<typename C, typename T>
void DebugStreambuf<C,T>::setFileThreshold( ELogLevel logLevel )
{
    mFileThreshold = logLevel;
}

/**
 * Sets the name of the module that will be generating the next log entry
 */
template<typename C, typename T>
void DebugStreambuf<C,T>::setModule( const char* module )
{
    mModuleName = module;
}

/**
 * Sets the severity level of the next log entry
 */
template<typename C, typename T>
void DebugStreambuf<C,T>::setLogLevel( ELogLevel level )
{
    mLogLevel = level;
}


/**
 * Set the debug stream's console output to be a pointer to the given stream
 * buffer. Passing in NULL will disable the console output.
 *
 * \param  pConsoleBuffer  Pointer to a basic_streambuf
 */
template<typename C, typename T>
void DebugStreambuf<C,T>::setConsole( std::basic_streambuf<C,T> *pConsoleBuffer )
{
    mpConsoleBuffer = pConsoleBuffer;
}

/**
 * Sets the debug stream's file output to be a pointer to the given stream
 * buffer. Passing in NULL will disable the debug log's file output.
 *
 * \param  pFileBuffer  Pointer to a basic_filebuf
 */
template<typename C, typename T>
void DebugStreambuf<C,T>::setFile( std::basic_filebuf<C,T> *pFileBuffer )
{
    mpFileBuffer = pFileBuffer;
}

/**
 * Overridden stream handling method. xsputn is called by the underlying
 * basic_streambuf class whenever the streambuf receives input data.
 *
 * The method takes all of character output, appends any unnecessary log entry
 * text and then routes everything to our console and file stream_bufs.
 *
 * \param  pSequence  The character sequence to write
 * \param  size       The size of the character sequence
 * \return            Number of characters written. Zero if there was a problem
 */
template<typename C, typename T>
std::streamsize DebugStreambuf<C,T>::xsputn( const C* pSequence, std::streamsize size )
{
    bool didWrite = true;
    bool isAtLineStart = mAtLineStart;

    //
    // Write log data to the user's console
    //
    if ( mpConsoleBuffer != NULL && mLogLevel >= mConsoleThreshold )
    {
        // Write out the log header to the console
        if ( mAtLineStart )
        {
            writeEntryHeaderConsole();
            isAtLineStart = false;
        }

        // Now write the actual string to the console
        mpConsoleBuffer->sputn( pSequence, size );
    }

    //
    // Now write the log contents to the file log
    //
    if ( mpFileBuffer != NULL && mLogLevel >= mFileThreshold )
    {
        // Write out the log header to the console
        if ( mAtLineStart )
        {
            writeEntryHeaderFile();
            isAtLineStart = false;
        }

        // Now write the actual string to the console
        mpFileBuffer->sputn( pSequence, size );
    }

    // Make sure we if we wrote a log entry that we don't do so again until
    // the start of a new log entry
    mAtLineStart = isAtLineStart;
    return (didWrite ? size : 0u );
}

/**
 * Writes a string directly to the debug buffer
 */
template<typename C, typename T>
void DebugStreambuf<C,T>::write( const std::basic_string<C,T>& str )
{
    sputn( str.c_str(), str.size() );
}

/**
 * Writes the log entry's "header" text out to the console. Thsi text
 * consists of the current date/time, entry module and severity level.
 *
 * \return  True if the header was written to the console, false otherwise
 */
template<typename C, typename T>
bool DebugStreambuf<C,T>::writeEntryHeaderConsole() const
{
    assert( mpConsoleBuffer != NULL );

    // Get the string form of the log level as well as the current
    // time before writing the log entry header
    const size_t TIME_STR_LEN = 10;
    char timeString[TIME_STR_LEN];

    time_t currentTime  = std::time( NULL );

    std::strftime( timeString, TIME_STR_LEN,
                   "%H:%M:%S",
                   localtime(&currentTime) );

    // Generate the log entry header before we hand it over to our
    // underlying buffer object
    const size_t BUFFER_LEN = 32;
    char bufferString[BUFFER_LEN];

    int length = _snprintf( bufferString, BUFFER_LEN,
                            "%8s [%c %-12s] ",
                            timeString,
                            getLogLevelString( mLogLevel )[0],
                            mModuleName );

    // Now write the header text out to the underlying buffer
    mpConsoleBuffer->sputn( bufferString, length );
    return true;
}

/**
 * Writes the log entry's "header" text out to the file log. This text
 * consists of the current date/time, entry module and severity level.
 *
 * \return  True if the header was written to the file, false otherwise
 */
template<typename C, typename T>
bool DebugStreambuf<C,T>::writeEntryHeaderFile() const
{
    assert( mpFileBuffer != NULL );

    // Get the string form of the log level as well as the current
    // time before writing the log entry header
    const size_t TIME_STR_LEN = 20;
    char timeString[TIME_STR_LEN];

    time_t currentTime  = std::time( NULL );

    std::strftime( timeString, TIME_STR_LEN,
                   "%Y.%m.%d-%H:%M:%S",
                   localtime(&currentTime) );

    // Generate the log entry header before we hand it over to our
    // underlying buffer object
    const size_t BUFFER_LEN = 48;
    char bufferString[BUFFER_LEN];

    int length = _snprintf( bufferString, BUFFER_LEN,
                            "%s %s %s ",
                            timeString,
                            getLogLevelString( mLogLevel ),
                            mModuleName );

    // Now write the header text out to the underlying buffer
    mpFileBuffer->sputn( bufferString, length );
    return true;
}

/**
 * Returns the string name of a given severity level
 */
template<typename C, typename T>
const char* DebugStreambuf<C,T>::getLogLevelString( ELogLevel level ) const
{
    switch ( level )
    {
        case ELOGLEVEL_TRACE:
            return "TRACE";
        case ELOGLEVEL_DEBUG:
            return "DBG";
        case ELOGLEVEL_INFO:
            return "INFO";
        case ELOGLEVEL_NOTICE:
            return "NOTE";
        case ELOGLEVEL_WARN:
            return "WARN";
        case ELOGLEVEL_ERROR:
            return "ERROR";
        case ELOGLEVEL_FATAL:
            return "FATAL";
        default:
            return "???";
    }
}

/**
 * Called when the logger has finished emitting a single log entry. This
 * method then resets our internal state in preparation for the next log
 * entry
 */
template<typename C,typename T>
void DebugStreambuf<C,T>::endLogEntry()
{
    // Reset current log entry state
    mAtLineStart = true;
    mLogLevel    = mDefaultLogLevel;
    mModuleName  = mDefaultModuleName;

    // Make sure we emit any buffered log data before attempting to sync
    // our output sources
    pubsync();

    // Sync output
    if ( mpConsoleBuffer != NULL  && mLogLevel >= mConsoleThreshold )
    {
        mpConsoleBuffer->sputc( '\n' );
        mpConsoleBuffer->pubsync();
    }

    if ( mpFileBuffer != NULL && mLogLevel >= mFileThreshold )
    {
        mpFileBuffer->sputc( '\n' );
        mpFileBuffer->pubsync();
    }
}

#endif