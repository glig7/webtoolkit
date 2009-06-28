#include "Common.h"
#include "Logger.h"
#include "Util.h"

Mutex Log::mutex;
LogLevel Log::reportingLevel=LogInfo;
ILogHandler* Log::logHandler=NULL;

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
	time_t t;
	time(&t);
	os<<Util::Timestamp(t)<<" ";
    os<<names[level]<<" ";
	if(level==LogDebug)
		os<<"(thread:"<<hex<<Thread::GetCurrentThreadId()<<dec<<") ";
    return os;
}

void Log::SetLogHandler(ILogHandler* handler)
{
	logHandler=handler;
}
