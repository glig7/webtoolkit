#include "Common.h"
#include "Logger.h"

Mutex Log::mutex;
FILE* Log::logFile=stderr;
LogLevel Log::reportingLevel=LogInfo;

const char* const Log::names[]={
	"",
	"ERRR",
	"WARN",
	"INFO",
	"VERB",
	"DEBG"
};

FILE*& Log::LogFile()
{
    return logFile;
}

LogLevel& Log::ReportingLevel()
{
    return reportingLevel;
}

Log::Log()
{
}

Log::~Log()
{
	os<<endl;
	mutex.Lock();
	fputs(os.str().c_str(),logFile);
	fflush(logFile);
	mutex.Unlock();
}

std::ostringstream& Log::Get( LogLevel level )
{
    const size_t tabspace=(level>LogVerbose)?(level-LogVerbose):0;
    os<<names[level]<<"("<<hex<<Thread::GetCurrentThreadId()<<dec<<"): "<<string(tabspace,'\t');
    return os;
}
