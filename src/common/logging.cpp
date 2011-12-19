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
#include "stdafx.h"
#include "common/logging.h"

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
 * \param  consoleStream  Output stream to use for console output
 * \param  fileStream     Output stream to use for logfile output
 * \param  logLevel       The level at which this entry was emitted
 * \param  system         Name of the component or subsystem
 */
LogEntry::LogEntry( std::ostream& consoleStream,
                    std::ostream& fileStream,
                    ELogLevel logLevel,
                    const std::string& system )
    : mConsoleStream(consoleStream),
      mFileStream(fileStream)
      
{
    // Write console output header
    mConsoleStream
        << "["  << LOG_LEVEL_NAMES[logLevel]
        << "; " << system << "] ";
}

/**
 * Null stream constructor. Takes in a reference to a (hopefully)
 * nullstream object, and doesn't print anything useful
 *
 * \param  nullStream  Reference to a null stream object
 */
LogEntry::LogEntry( std::ostream& nullStream )
    : mConsoleStream( nullStream ),
      mFileStream( nullStream )
{
}

/**
 * LogEntry destructor. Cleans up and emits end of line notices to the
 * log entry's streams
 */
LogEntry::~LogEntry()
{
    mConsoleStream << std::endl;
}

Log::Log()
    : mOutputFile(),
      mNullStream( new NullStreamBuf ),
      mConsoleStream( std::cout ),
      mFileStream( mNullStream ),
      mMinimumLogLevel( ELOG_DEBUG )
{
}

Log::~Log()
{
    if ( mOutputFile.is_open() )
    {
        mOutputFile.close();
    }
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
    if ( mMinimumLogLevel <= ELOG_TRACE )
    {
        return LogEntry( mConsoleStream, mFileStream, ELOG_TRACE, system );
    }
    else
    {
        return LogEntry( mNullStream );
    }
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
    if ( mMinimumLogLevel <= ELOG_DEBUG )
    {
        return LogEntry( mConsoleStream, mFileStream, ELOG_DEBUG, system );
    }
    else
    {
        return LogEntry( mNullStream );
    }
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
    if ( mMinimumLogLevel <= ELOG_INFO )
    {
        return LogEntry( mConsoleStream, mFileStream, ELOG_INFO, system );
    }
    else
    {
        return LogEntry( mNullStream );
    }
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
    if ( mMinimumLogLevel <= ELOG_NOTICE )
    {
        return LogEntry( mConsoleStream, mFileStream, ELOG_NOTICE, system );
    }
    else
    {
        return LogEntry( mNullStream );
    }
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
    if ( mMinimumLogLevel <= ELOG_WARN )
    {
        return LogEntry( mConsoleStream, mFileStream, ELOG_WARN, system );
    }
    else
    {
        return LogEntry( mNullStream );
    }
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
    if ( mMinimumLogLevel <= ELOG_ERROR )
    {
        return LogEntry( mConsoleStream, mFileStream, ELOG_ERROR, system );
    }
    else
    {
        return LogEntry( mNullStream );
    }
}

/**
 * Initializes the global log
 */
void GlobalLog::start()
{

}

/**
 * Return a reference to the global log
 */
Log& GlobalLog::getInstance()
{
    return mLog;
}