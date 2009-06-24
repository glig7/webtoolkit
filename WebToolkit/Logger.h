#ifndef _LOGGER_H
#define _LOGGER_H

#include "Thread.h"

#define LOG(level) \
	if((level)<=Log::reportingLevel) \
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

class ILogHandler
{
public:
	virtual void LogWrite(const string& st)=0;
};

class Log
{
private:
	static const char* const names[];
	static Mutex mutex;
	static ILogHandler* logHandler;
	ostringstream os;
public:
	static LogLevel reportingLevel;
	Log();
	~Log();
	std::ostringstream& Get(LogLevel level=LogInfo);
	static void SetReportingLevel(LogLevel logLevel);
	static void SetLogHandler(ILogHandler* handler);
};

#endif
