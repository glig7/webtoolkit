#include "Common.h"
#include "Thread.h"

#ifdef WIN32

DWORD WINAPI Thread::thread_func(LPVOID d)
{
	((Thread*)d)->Run();
	return 0;
}
#else

#include <signal.h>

void* Thread::thread_func(void* d)
{
	signal(SIGPIPE,SIG_IGN);
	((Thread*)d)->Run();
	return NULL;
}
#endif

Thread::Thread()
{
}

Thread::~Thread()
{
}

void Thread::Run()
{
}

void Thread::Start()
{
#ifdef WIN32
	CreateThread(NULL,0,Thread::thread_func,this,0,(LPDWORD)&thread);
#else
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
	pthread_create(&thread,&attr,Thread::thread_func,this);
	pthread_attr_destroy(&attr);
#endif
}

void Thread::Wait()
{
#ifdef WIN32
	WaitForSingleObject(thread,INFINITE);
#else
	pthread_join(thread,NULL);
#endif
}

Mutex::Mutex()
{
#ifdef WIN32
	mutex=CreateMutex(NULL,FALSE,NULL);
#else
	pthread_mutex_init(&mutex,NULL);
#endif
}

Mutex::~Mutex()
{
#ifdef WIN32
	CloseHandle(mutex);
#else
	pthread_mutex_destroy(&mutex);
#endif
}

void Mutex::Lock()
{
#ifdef WIN32
	WaitForSingleObject(mutex,INFINITE);
#else
	pthread_mutex_lock(&mutex);
#endif
}

void Mutex::Unlock()
{
#ifdef WIN32
	ReleaseMutex(mutex);
#else
	pthread_mutex_unlock(&mutex);
#endif
}

