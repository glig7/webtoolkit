#ifndef _THREAD_H
#define	_THREAD_H

#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

typedef void (*ThreadProc)(void* arg);

class Thread
{
public:
	static void StartThread(ThreadProc threadProc,void* arg);
};

class Mutex
{
private:
#ifdef WIN32
	HANDLE mutex;
#else
	pthread_mutex_t mutex;
#endif
public:
	Mutex();
	~Mutex();
	void Lock();
	void Unlock();
};

#endif

