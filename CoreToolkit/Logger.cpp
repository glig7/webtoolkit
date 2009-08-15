/*
	This file is a part of madfish-webtoolkit project.
	http://code.google.com/p/madfish-webtoolkit/

	(c) 2009 Alexander "Mad Fish" Chehovsky
	See License.txt for licensing information.
*/

/**
	\file Logger.cpp
	\brief Implementation of Logger class.
*/

#include "Logger.h"
#include "Util.h"

#include <time.h>
#include <stdio.h>

using namespace std;

namespace CoreToolkit
{

Mutex Log::mutex;
LogLevel Log::reportingLevel=LogDebug;
LogHandler* Log::logHandler=NULL;

const char* const Log::names[]={
	"",
	"[error]",
	"[warning]",
	"[info]",
	"[verbose]",
	"[debug]"
};

Log::Log()
{
}

Log::~Log()
{
	os<<endl;
	MutexLock lock(mutex);
	if(logHandler==NULL)
	{
		fputs(os.str().c_str(),stderr);
		fflush(stderr);
	}
	else
		logHandler->LogWrite(os.str());
}

void Log::SetReportingLevel(LogLevel logLevel)
{
    reportingLevel=logLevel;
}

std::ostringstream& Log::Get(LogLevel level)
{
	time_t t;
	time(&t);
	os<<Util::Timestamp(t)<<" ";
	os<<"("<<hex<<Thread::GetCurrentThreadId()<<dec<<") ";
    os<<names[level]<<" ";
    return os;
}

void Log::SetLogHandler(LogHandler* handler)
{
	logHandler=handler;
}

}
