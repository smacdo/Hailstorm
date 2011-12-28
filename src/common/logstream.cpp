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
#include "common/logging.h"
#include "common/logging_impl.h"
#include "common/logging_stream.h"
#include <string>
#include <iostream>
#include <fstream>

LogStream::LogStream( std::ostream *pConsoleStream,
                      std::ofstream *pFileStream )
    : std::ostream( new DebugStreambuf<char> ),
      std::ios( 0 ),
      mpStreambuf( NULL ),
      mpConsoleStream( pConsoleStream ),
      mpFileStream( pFileStream )
{
    // i really don't like having to do this
    mpStreambuf = dynamic_cast<DebugStreambuf<char>* >( rdbuf() );
    assert( mpStreambuf != NULL && "Something really evil just happened" );

    // Assign the output streams to our internal debug stream
    if ( pConsoleStream != NULL )
    {
        mpStreambuf->setConsole( pConsoleStream->rdbuf() );
    }

    if ( pFileStream != NULL )
    {
        mpStreambuf->setFile( pFileStream->rdbuf() );
    }
}

/**
 * Log stream destructor. Closes all the streams that the debug stream object
 * is writing to
 */
LogStream::~LogStream()
{
    // Destroy the log stream's internal stream buffer
    delete mpStreambuf;

    // Now make sure all of our streams have been closed properly
    if ( mpConsoleStream != NULL )
    {
        (*mpConsoleStream) << std::endl;
    }

    if ( mpFileStream != NULL )
    {
        mpFileStream->close();
    }
}

/**
 * Starts a new log entry
 */
void LogStream::startLogEntry( const std::string& system, ELogLevel level )
{
    mpStreambuf->setLogLevel( level );
    mpStreambuf->setModule( system.c_str() );
}

/**
 * Ends the correct log entry
 */
void LogStream::endLogEntry()
{
    mpStreambuf->endLogEntry();
}