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
#include <iostream>
#include <ostream>
#include <fstream>
#include <string>

Log GlobalLog::mLog;

const char* LOG_LEVEL_NAMES[ELogLevel_Count] =
{
    "TRACE",
    "DEBUG",
    "INFO",
    "NOTICE",
    "WARN",
    "ERROR"
};

/**
 * Log entry constructor. Takes two output streams and assigns them to
 * be the console output, and file output streams. Will also print a short
 * entry header consisting of the log level and component before returning
 *
 * \param  debugStream
 */
LogEntry::LogEntry( LogStream* debugStream )
    : mDebugStream( debugStream )      
{
}

/**
 * LogEntry destructor. Cleans up and emits end of line notices to the
 * log entry's streams
 */
LogEntry::~LogEntry()
{
    mDebugStream->endLogEntry();
}

/**
 * Log object constructor. Creates a new log with an associated log stream,
 * but does not set any console or file output.
 */
Log::Log()
    : mDebugStream( new LogStream(NULL,NULL) )
{

}

/**
 * Log object constructor. Creates a new log with an associated log stream and
 * associates the given console/file streams to the log stream
 */
Log::Log( std::ostream *pConsoleStream, std::ofstream* pFileStream )
    : mDebugStream( new LogStream(pConsoleStream, pFileStream) )
{
}

/**
 * Log destructor. Destroys the log stream associated with this log object
 */
Log::~Log()
{
    boost::checked_delete( mDebugStream );
}

/**
 * Writes a trace entry to the program's log, and a stream that can be
 * used to append additional information to the entry
 *
 * \param  system  The name of the system or component writing the entry
 * \return A LogEntry object that can be used to append additional
 *         information to the log entry
 */
LogEntry Log::trace( const std::string& system ) const
{
    mDebugStream->startLogEntry( system, ELOGLEVEL_INFO );
    return LogEntry( mDebugStream );
}

/**
 * Writes a debug entry to the program's log, and a stream that can be
 * used to append additional information to the entry
 *
 * \param  system  The name of the system or component writing the entry
 * \return A LogEntry object that can be used to append additional
 *         information to the log entry
 */
LogEntry Log::debug( const std::string& system ) const
{
    mDebugStream->startLogEntry( system, ELOGLEVEL_DEBUG );
    return LogEntry( mDebugStream );
}

/**
 * Writes an info entry to the program's log, and a stream that can be
 * used to append additional information to the entry
 *
 * \param  system  The name of the system or component writing the entry
 * \return A LogEntry object that can be used to append additional
 *         information to the log entry
 */
LogEntry Log::info( const std::string& system ) const
{
    mDebugStream->startLogEntry( system, ELOGLEVEL_INFO );
    return LogEntry( mDebugStream );
}

/**
 * Writes a notice entry to the program's log, and a stream that can be
 * used to append additional information to the entry
 *
 * \param  system  The name of the system or component writing the entry
 * \return A LogEntry object that can be used to append additional
 *         information to the log entry
 */
LogEntry Log::notice( const std::string& system ) const
{
    mDebugStream->startLogEntry( system, ELOGLEVEL_NOTICE );
    return LogEntry( mDebugStream );
}

/**
 * Writes a warning entry to the program's log, and a stream that can be
 * used to append additional information to the entry
 *
 * \param  system  The name of the system or component writing the entry
 * \return A LogEntry object that can be used to append additional
 *         information to the log entry
 */
LogEntry Log::warn( const std::string& system ) const
{
    mDebugStream->startLogEntry( system, ELOGLEVEL_WARN );
    return LogEntry( mDebugStream );
}

/**
 * Sets the output stream used for console window output
 */
void Log::setConsoleStream( std::ostream *pConsoleStream )
{
    if ( pConsoleStream == NULL )
    {
        warn("Logging") << "Attaching a null console stream...";
    }

    mDebugStream->setConsoleStream( pConsoleStream );
}

/**
 * Sets the output stream used for file output
 */
void Log::setFileStream( std::ofstream *pFileStream )
{
    if ( pFileStream == NULL )
    {
        warn("Logging") << "Attaching a null file stream...";
    }

    mDebugStream->setFileStream( pFileStream );
}

/**
 * Writes an error entry to the program's log, and a stream that can be
 * used to append additional information to the entry
 *
 * \param  system  The name of the system or component writing the entry
 * \return A LogEntry object that can be used to append additional
 *         information to the log entry
 */
LogEntry Log::error( const std::string& system ) const
{
    mDebugStream->startLogEntry( system, ELOGLEVEL_ERROR );
    return LogEntry( mDebugStream );
}

/**
 * Initializes the global log
 */
void GlobalLog::start()
{
    // Attach the windows console to the log stream (We've already started a
    // console window before this)
    mLog.setConsoleStream( &std::cout );

    // Generate a new date time string to use as the log's name

}

/**
 * Return a reference to the global log
 */
Log& GlobalLog::getInstance()
{
    return mLog;
}