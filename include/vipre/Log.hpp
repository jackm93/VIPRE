//
//  Log.hpp
//  vipre
//
//  Created by Christian Noon on 12/2/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#ifndef VIPRE_LOG_HPP
#define VIPRE_LOG_HPP

#define VIPRE_LOG_ENABLED_ENVIRONMENT_VARIABLE      "VIPRE_LOG_ENABLED"
#define VIPRE_LOG_FILE_ENVIRONMENT_VARIABLE         "VIPRE_LOG_FILE"
#define VIPRE_LOG_LEVEL_ENVIRONMENT_VARIABLE        "VIPRE_LOG_LEVEL"

/** Defines the vipre prefix for logging. */
#define viprePrefix     "[vipre] "

#include <OpenThreads/Mutex>

#include <vipre/Export.hpp>
#include <vipre/String.hpp>

namespace vipre {

/** The various levels of log message verbosity. */
enum LogLevel
{
    ALWAYS_LVL,
    ERROR_LVL,
    WARNING_LVL,
    INFO_LVL,
    DEBUG_LVL
};

/**
 * The log class handles all logging for vipre and all the extension libraries.
 *
 * This logging system supports the following:
 *
 *    - Five different log levels
 *    - Output redirection to files (a custom std::ofstream)
 *    - Thread-safe access to the stream buffer
 *    - Disabling the log altogether
 *
 * The following environment variables can be used to configure the log at runtime:
 *
 *    - VIPRE_LOG_ENABLED: Disables the log system if set to any of the following:
 *        * [ NO | FALSE | NOPE | DISABLE ]
 *    - VIPRE_LOG_FILE: Redirects the output from the log to the specified file:
 *        * /Users/cnoon/Desktop/logOutput.txt
 *    - VIPRE_LOG_LEVEL: Defines the maximum output level for the log:
 *        * [ ALWAYS_LVL | ERROR_LVL | WARNING_LVL | INFO_LVL | DEBUG_LVL ]
 *
 * Using the log is straight-forward. There are several different convenience macros
 * provided to make this as simple as possible. The easiest way to use the log is with
 * the vipreLog() macro. It will return the std::ostream to direct output to. It is
 * used just like std::cout. Here is a simple example:
 *
 *    - vipreLog() << "this is my output" << std::endl;
 *
 * Sometimes, it is useful to be able to prefix your output with something like a
 * namespace, application name, error message, etc. For this purpose, the following
 * macros exist:
 *
 *    - vipreLogALWAYS(prefix)
 *    - vipreLogERROR(prefix)
 *    - vipreLogWARNING(prefix)
 *    - vipreLogINFO(prefix)
 *    - vipreLogDEBUG(prefix)
 *
 * Here are some common examples of different logging messages:
 *
 *    - vipreLogERROR("vipre: ") << "ERROR: We have a problem here" << std::endl;
 *    - vipreLogALWAYS("vipre: ") << "Initializing the logging system" << std::endl;
 *
 * As you can see, it is just like using std::cout, except with a bunch of extra benefits!
 */
class VIPRE_EXPORT Log
{
public:

    /** Create a singleton instance. */
    static Log* instance() { static Log log; return &log; }

    /** Sets whether the log is enabled. */
    void setIsLogEnabled(bool enabled);

    /** Returns whether the log is enabled. */
    bool isLogEnabled(LogLevel level);

    /** Sets the log level. */
    void setLogLevel(LogLevel level);

    /** Returns the log level. */
    LogLevel getLogLevel();

    /**
     * Sets the output stream to a std::ofstream created from the provided filepath.
     * NOTE: This will not change the current log stream if the file cannot be opened successfully.
     *
     * @return - whether the stream was replaced
     */
    bool setLogFile(const String& filepath);

    /** Switches the current output stream to the provided stream. */
    void setLogStream(std::ostream& stream);

    /** Returns the log stream output (use like std::cout). */
    std::ostream& getLogStream(LogLevel level, const String& prefix = "");

protected:

    /**
     * Constructor.
     *
     * Initializes the log level by attempting to grab the VIPRE_LOG_LEVEL environment
     * variables from the user's environment.
     */
    Log();

    /** Destructor. */
    ~Log();

    /** Instance member variables. */
    bool _isEnabled;
    LogLevel _logLevel;
    OpenThreads::Mutex _logMutex;
    std::ostream* _logStream;
};

/**
 * Logs a message without a prefix. Below are a couple examples:
 *
 * vipreLog(vipre::Log::WARNING) << "This is an example of a WARNING message" << std::endl;
 * - Would log: "This is an example of a WARNING message"
 *
 * vipreLog(vipre::Log::ERROR) << "ERROR: Look at what happened!" << std::endl;
 * - Would log: "ERROR: Look at what happened!"
 */
#define vipreLog(level) if ((LOGGER()->isLogEnabled(level)) && level <= LOGGER()->getLogLevel()) LOGGER()->getLogStream(level)

/**
 * Logs a message with the defined prefix. Below are a couple examples:
 *
 * vipreLogERROR("vipre: ") << "ERROR: Look at what happened!" << std::endl;
 * - Would log: "vipre: ERROR: Look at what happened!"
 *
 * vipreLogERROR(viprePrefix) << "This output uses the preprocessor definition viprePrefix" << std::endl;
 * - Would log: "vipre: This output uses the preprocessor definition viprePrefix"
 */
#define vipreLogALWAYS(prefix) vipreLogWithPrefix(vipre::ALWAYS_LVL, prefix)
#define vipreLogERROR(prefix) vipreLogWithPrefix(vipre::ERROR_LVL, prefix)
#define vipreLogWARNING(prefix) vipreLogWithPrefix(vipre::WARNING_LVL, prefix)
#define vipreLogINFO(prefix) vipreLogWithPrefix(vipre::INFO_LVL, prefix)
#define vipreLogDEBUG(prefix) vipreLogWithPrefix(vipre::DEBUG_LVL, prefix)

/**
 * DO NOT USE THESE MACROS!!!
 *
 * Only for internal vipre library use, please use the methods above for logging.
 */
#define LOGGER() vipre::Log::instance()
#define vipreLogWithPrefix(level, prefix) if ((LOGGER()->isLogEnabled(level)) && level <= LOGGER()->getLogLevel()) LOGGER()->getLogStream(level, prefix)

}   // End of vipre namespace

#endif  // End of VIPRE_LOG_HPP
