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
	friend class CondVar;
public:
	Mutex();
	~Mutex();
	void Lock();
	void Unlock();
};

class CondVar
{
private:
#ifdef WIN32
	HANDLE condVarEvent;
#else
	pthread_cond_t condVar;
#endif
public:
	CondVar();
	~CondVar();
	void Wait(Mutex& externalMutex);
	void Signal();
};

template<class T>
class ThreadTasks
{
private:
	queue<T> q;
	Mutex queueMutex;
	CondVar notEmpty;
public:
	void Push(const T& e)
	{
		queueMutex.Lock();
		q.push(e);
		notEmpty.Signal();
		queueMutex.Unlock();
	}
	T Pop()
	{
		queueMutex.Lock();
		while(q.empty())
			notEmpty.Wait(queueMutex);
		T e=q.front();
		q.pop();
		queueMutex.Unlock();
		return e;
	}
};

#endif

