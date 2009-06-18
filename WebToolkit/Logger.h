#ifndef _LOGGER_H
#define _LOGGER_H

#include "Thread.h"

#ifndef LOG_MAX_LEVEL
#define LOG_MAX_LEVEL LogDebug
#endif

#ifndef LOG_MIN_LEVEL
#define LOG_MIN_LEVEL LogError
#endif

#define LOG( level ) \
    if ( ( (level) > LOG_MAX_LEVEL ) || ( (level) < LOG_MIN_LEVEL ) ) ;\
    else if ( (level) > Log::ReportingLevel() ) ;\
    else Log().Get( (level) )

enum LogLevel
{
    LogError   = 0,
    LogWarning = 1,
    LogInfo    = 2,
    LogVerbose = 3,
    LogDebug   = 4
};

class Log
{
public:
    Log();
   ~Log();
    std::ostringstream& Get( LogLevel level = LogInfo );
public:
    static LogLevel& ReportingLevel();
    static FILE*&    LogFile();
private:
    static FILE*             logFile;
    static LogLevel          reportingLevel;
    static const char* const names[];
	static Mutex mutex;
private:
    Log( const Log& );
    Log& operator =( const Log& );
private:
    std::ostringstream os;
};

#endif
