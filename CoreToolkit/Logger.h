/*
	This file is a part of madfish-webtoolkit project.
	http://code.google.com/p/madfish-webtoolkit/

	(c) 2009 Alexander "Mad Fish" Chehovsky
	See License.txt for licensing information.
*/

/**
	\file Logger.h
	\brief Declarations of Log class and LOG macro.
*/

#pragma once
#ifndef _LOGGER_H
#define _LOGGER_H

#include "Thread.h"

#include <string>
#include <sstream>

/**
	Use this for logging. For example:
	LOG(LogInfo)<<"Message";
*/
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

/**
	\brief Log handler interface
	Implement this to make your custom log handling.
*/
class LogHandler
{
public:
	//! Write single record to the log.
	virtual void LogWrite(const std::string& st)=0;
};

/**
	\brief Class for handling log recording.
	It's instance is created for each log record.
	It was introduced to perform actions before and after writing.
*/
class Log
{
private:
	//! Log level names.
	static const char* const names[];
	//! Mutex for accesing log.
	static Mutex mutex;
	//! Custom log handler.
	static LogHandler* logHandler;
	//! Temporary string stream for writing log record.
	std::ostringstream os;
public:
	//! Current reporting level.
	static LogLevel reportingLevel;
	//! Constructs the temporary object for log record handling.
	Log();
	//! Writes the accumulated log record.
	~Log();
	std::ostringstream& Get(LogLevel level);
	//! Changes current reporting level.
	static void SetReportingLevel(LogLevel logLevel);
	//! Set the custom log handler.
	static void SetLogHandler(LogHandler* handler);
};

}

#endif

