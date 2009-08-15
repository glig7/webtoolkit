/*
	This file is a part of madfish-webtoolkit project.
	http://code.google.com/p/madfish-webtoolkit/

	(c) 2009 Alexander "Mad Fish" Chehovsky
	See License.txt for licensing information.
*/

/**
	\file OSUtils.cpp
	\brief Implementation of functions for interaction with OS.
*/

#include "OSUtils.h"
#include "Thread.h"

#ifdef WIN32
#include <windows.h>
#else
#include <signal.h>
#endif

using namespace std;

namespace CoreToolkit
{

#ifdef WIN32
bool Environment::terminated=false;
#endif

#ifdef WIN32
BOOL WINAPI HandlerRoutine(DWORD dwCtrlType)
{
	Environment::terminated=true;
	return TRUE;
}
#endif

void Environment::Init()
{
#ifdef WIN32
	SetConsoleCtrlHandler(HandlerRoutine,TRUE);
#else
	//Block all signals
	sigset_t sigset;
	sigemptyset(&sigset);
	sigfillset(&sigset);
	sigprocmask(SIG_BLOCK,&sigset,NULL);
#endif
}

bool Environment::CheckForTermination()
{
#ifdef WIN32
	return terminated;
#else
	sigset_t sigset;
	sigpending(&sigset);
	return (sigismember(&sigset,SIGINT)||sigismember(&sigset,SIGTERM)||sigismember(&sigset,SIGQUIT));
#endif
}

void Environment::WaitForTermination()
{
#ifdef WIN32
	while(!terminated)
		Thread::Sleep(200);
#else
    sigset_t wait_mask;
    sigemptyset(&wait_mask);
    sigaddset(&wait_mask,SIGINT);
    sigaddset(&wait_mask,SIGQUIT);
    sigaddset(&wait_mask,SIGTERM);
    int sig = 0;
    sigwait(&wait_mask,&sig);
#endif
}

struct EnvironmentInit
{
	EnvironmentInit()
	{
		Environment::Init();
	}
} environmentInit;

}
