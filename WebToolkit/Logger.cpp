#include "Common.h"
#include "Logger.h"

Mutex Log::mutex;
LogLevel Log::reportingLevel=LogInfo;
ILogHandler* Log::logHandler=NULL;

const char* const Log::names[]={
	"",
	"ERRR",
	"WARN",
	"INFO",
	"VERB",
	"DEBG"
};

Log::Log()
{
}

Log::~Log()
{
	os<<endl;
	MutexLock lock(mutex);
	if(logHandler==NULL)
		fputs(os.str().c_str(),stderr);
	else
		logHandler->LogWrite(os.str());
}

void Log::SetReportingLevel(LogLevel logLevel)
{
    reportingLevel=logLevel;
}

std::ostringstream& Log::Get(LogLevel level)
{
    os<<names[level]<<"("<<hex<<Thread::GetCurrentThreadId()<<dec<<"): ";
    return os;
}

void Log::SetLogHandler(ILogHandler* handler)
{
	logHandler=handler;
}
