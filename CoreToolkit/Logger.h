/*
	This file is a part of madfish-webtoolkit project.
	http://code.google.com/p/madfish-webtoolkit/

	(c) 2009 Alexander "Mad Fish" Chehovsky
	See License.txt for licensing information.
*/

#pragma once
#ifndef _LOGGER_H
#define _LOGGER_H

#include "Thread.h"

#include <string>
#include <sstream>

#define LOG(level) \
	if((level)<=CoreToolkit::Log::reportingLevel) \
		CoreToolkit::Log().Get((level))

namespace CoreToolkit
{

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
	virtual void LogWrite(const std::string& st)=0;
};

class Log
{
private:
	static const char* const names[];
	static Mutex mutex;
	static ILogHandler* logHandler;
	std::ostringstream os;
public:
	static LogLevel reportingLevel;
	Log();
	~Log();
	std::ostringstream& Get(LogLevel level);
	static void SetReportingLevel(LogLevel logLevel);
	static void SetLogHandler(ILogHandler* handler);
};

}

#endif

