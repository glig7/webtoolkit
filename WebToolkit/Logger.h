#ifndef _LOGGER_H
#define _LOGGER_H

#include "Thread.h"

#define LOG( level ) \
	if((level)<=Log::ReportingLevel()) \
		Log().Get((level))

enum LogLevel
{
	LogDisabled,
	LogError,
	LogWarning,
	LogInfo,
	LogVerbose,
	LogDebug
};

class Log
{
public:
	Log();
	~Log();
	std::ostringstream& Get(LogLevel level=LogInfo);
public:
	static LogLevel& ReportingLevel();
	static FILE*& LogFile();
private:
	static FILE* logFile;
	static LogLevel reportingLevel;
	static const char* const names[];
	static Mutex mutex;
private:
	Log(const Log&);
	Log& operator=(const Log&);
private:
	ostringstream os;
};

#endif
