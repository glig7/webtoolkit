#ifndef _THREAD_H
#define	_THREAD_H

#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

class Thread
{
private:
#ifdef WIN32
	HANDLE thread;
	static DWORD WINAPI thread_func(LPVOID d);
#else
	pthread_t thread;
	static void* thread_func(void* d);
#endif
public:
	Thread();
	virtual ~Thread();
	virtual void Run();
	void Start();
	void Wait();
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

