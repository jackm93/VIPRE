//
//  Log.cpp
//  vipre
//
//  Created by Christian Noon on 12/2/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#include <fstream>

#include <OpenThreads/ScopedLock>

#include <vipre/Environment.hpp>
#include <vipre/Log.hpp>

using namespace vipre;

Log::Log() :
    _isEnabled(true),
    _logLevel(vipre::WARNING_LVL),
    _logMutex(),
    _logStream(&std::cout)
{
    // Attempt to disable the entire log system based on the "VIPRE_LOG_ENABLED" environment variable
    String logEnabled = vipre::getEnvironmentVariable(VIPRE_LOG_ENABLED_ENVIRONMENT_VARIABLE);
    logEnabled.toLowerCase();
    if (logEnabled == "no" || logEnabled == "false" || logEnabled == "nope" || logEnabled == "disable")
    {
        _isEnabled = false;
        std::cout << "vipre: Setting VIPRE_LOG_ENABLED to NO" << std::endl;
        return;
    }

    // Attempt to set the log level based on the "VIPRE_LOG_LEVEL" environment variable
    String logLevel = vipre::getEnvironmentVariable(VIPRE_LOG_LEVEL_ENVIRONMENT_VARIABLE);
    if (logLevel == "ALWAYS_LVL")
    {
        _logLevel = vipre::ALWAYS_LVL;
        std::cout << "vipre: Setting VIPRE_LOG_LEVEL to vipre::ALWAYS_LVL" << std::endl;
    }
    else if (logLevel == "ERROR_LVL")
    {
        _logLevel = vipre::ERROR_LVL;
        std::cout << "vipre: Setting VIPRE_LOG_LEVEL to vipre::ERROR_LVL" << std::endl;
    }
    else if (logLevel == "WARNING_LVL")
    {
        _logLevel = vipre::WARNING_LVL;
        std::cout << "vipre: Setting VIPRE_LOG_LEVEL to vipre::WARNING_LVL" << std::endl;
    }
    else if (logLevel == "INFO_LVL")
    {
        _logLevel = vipre::INFO_LVL;
        std::cout << "vipre: Setting VIPRE_LOG_LEVEL to vipre::INFO_LVL" << std::endl;
    }
    else if (logLevel == "DEBUG_LVL")
    {
        _logLevel = vipre::DEBUG_LVL;
        std::cout << "vipre: Setting VIPRE_LOG_LEVEL to vipre::DEBUG_LVL" << std::endl;
    }
    else if (!logLevel.empty())
    {
        std::cout << "vipre: WARNING: Your VIPRE_LOG_LEVEL environment variable: [" << logLevel
            << "] does not match any of the possible options: [ ALWAYS_LVL | ERROR_LVL | WARNING_LVL | INFO_LVL | DEBUG_LVL ]" << std::endl;
    }
    else
    {
        // DO NOTHING
    }

    // Attempt to set the log file based on the "VIPRE_LOG_FILE" environment variable
    String logFile = vipre::getEnvironmentVariable(VIPRE_LOG_FILE_ENVIRONMENT_VARIABLE);
    if (!logFile.empty())
    {
        if (logFile == "stderr")
        {
            _logStream = &std::cerr;
        }
        else if (logFile != "stdout")
        {
            bool success = setLogFile(logFile);
            if (success)
            {
                std::cout << "vipre: Setting VIPRE_LOG_FILE to " << logFile << std::endl;
            }
            else
            {
                std::cout << "vipre: WARNING: Your VIPRE_LOG_FILE environment variable: ["
                    << logFile << "] could not be created or opened" << std::endl;
            }
        }
    }
}

Log::~Log()
{
    ;
}

void Log::setIsLogEnabled(bool enabled)
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_logMutex);
    _isEnabled = enabled;
}

bool Log::isLogEnabled(LogLevel level)
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_logMutex);
    
    // Returning false if disabled
    if (!_isEnabled)
    {
        return false;
    }

    return (level <= _logLevel);
}

void Log::setLogLevel(LogLevel level)
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_logMutex);
    _logLevel = level;
}

LogLevel Log::getLogLevel()
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_logMutex);
    return _logLevel;
}

bool Log::setLogFile(const String& filepath)
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_logMutex);

    // First try to open the file
    std::ofstream* logFile = new std::ofstream(filepath.c_str());
    if ( !(*logFile) )
    {
        delete logFile;
        logFile = NULL;
        return false;
    }

    // We successfully opened the file for writing, so switch log streams
    _logStream = logFile;
    return true;
}

void Log::setLogStream(std::ostream& stream)
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_logMutex);
    _logStream = &stream;
}

std::ostream& Log::getLogStream(LogLevel level, const String& prefix)
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_logMutex);

    // The generic stream to "buffer" output to for the output handler. This allows us to
    // avoid pointer dereferences until necessary.
    std::ostream& ostream = *_logStream;

    // Append the prefix if necessary
    if (!prefix.empty())
    {
        ostream << prefix;
    }

    return *_logStream;
}
