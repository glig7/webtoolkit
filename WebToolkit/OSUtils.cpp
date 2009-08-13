#include "Common.h"
#include "OSUtils.h"
#include "Thread.h"

bool Environment::terminated=false;

#ifdef WIN32
#include <windows.h>

BOOL WINAPI HandlerRoutine(DWORD dwCtrlType)
{
	Environment::terminated=true;
	return TRUE;
}
#else
#include <signal.h>

void signal_handler(int sig_num)
{
    signal(sig_num,signal_handler);
	Environment::terminated=true;
}
#endif

void Environment::Init()
{
#ifdef WIN32
	SetConsoleCtrlHandler(HandlerRoutine,TRUE);
#else
	signal(SIGINT,signal_handler);
	signal(SIGTERM,signal_handler);
	signal(SIGPIPE,SIG_IGN);
#endif
}

void Environment::WaitForTermination()
{
	while(!terminated)
		Thread::Sleep(200);
}


struct EnvironmentInit
{
	EnvironmentInit()
	{
		Environment::Init();
	}
} environmentInit;

