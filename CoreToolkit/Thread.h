/*
	This file is a part of madfish-webtoolkit project.
	http://code.google.com/p/madfish-webtoolkit/

	(c) 2009 Alexander "Mad Fish" Chehovsky
	See License.txt for licensing information.
*/

#pragma once
#ifndef _THREAD_H
#define	_THREAD_H

#include <queue>

#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

namespace CoreToolkit
{

typedef void (*ThreadProc)(void* arg);

class Thread
{
public:
	static void StartThread(ThreadProc threadProc,void* arg);
    static unsigned int GetCurrentThreadId();
	static void Sleep(int ms);
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

class MutexLock
{
private:
	Mutex& m;
public:
	MutexLock(Mutex& mutex):m(mutex)
	{
		m.Lock();
	}
	~MutexLock()
	{
		m.Unlock();
	}
};

template<class T>
class ThreadTasks
{
private:
	std::queue<T> q;
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

}

#endif

