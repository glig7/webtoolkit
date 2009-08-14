/*
	This file is a part of madfish-webtoolkit project.
	http://code.google.com/p/madfish-webtoolkit/

	(c) 2009 Alexander "Mad Fish" Chehovsky
	See License.txt for licensing information.
*/

#pragma once
#ifndef _SERVER_H
#define	_SERVER_H

#include "Socket.h"
#include "Http.h"
#include "Thread.h"

#include <string>
#include <map>

namespace WebToolkit
{

struct Session
{
	time_t whenToDelete;
	HttpSessionObject* object;
};

class Server
{
private:
	CoreToolkit::Listener listener;
	CoreToolkit::Mutex workersMutex;
	volatile int workersCount;
	volatile bool listenerThreadRunning;
	int listenerPort;
	std::string listenerIP;
	CoreToolkit::Mutex sessionsMutex;
	std::map<std::string,Session> sessions;
	int gcCounter;
	void OnWorkerAttach();
	void OnWorkerDetach();
	static void _ListenerThread(void* server);
	static void _WorkerThread(void* server);
	void ListenerThread();
	void WorkerThread();
public:
	HttpHandler* handler;
	HttpHandler* defaultErrorHandler;
	int gcPeriod,gcMaxLifeTime;
	volatile bool terminated;
	CoreToolkit::ThreadTasks<CoreToolkit::Socket*> tasks;
	Server(int port=8080,const std::string& ip="0.0.0.0",int numWorkers=16);
	~Server();
	void Run();
	void Terminate();
	void RegisterHandler(HttpHandler* handler);
	void RegisterDefaultErrorHandler(HttpHandler* handler);
	void ServeFile(const std::string& fileName,HttpServerContext* context,bool download=false);
	void StartWorkers(int numWorkers);
	void StartSession(HttpSessionObject* sessionObject,HttpServerContext* context);
	HttpSessionObject* GetSessionObject(const std::string& token);
};

}

#endif

