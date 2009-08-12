#ifndef _SERVER_H
#define	_SERVER_H

#include "Socket.h"
#include "Http.h"
#include "Thread.h"

struct Session
{
	time_t whenToDelete;
	HttpSessionObject* object;
};

class Server
{
private:
	Listener listener;
	Mutex workersMutex;
	volatile int workersCount;
	volatile bool listenerThreadRunning;
	int listenerPort;
	string listenerIP;
	Mutex sessionsMutex;
	map<string,Session> sessions;
	int gcCounter;
	void OnWorkerAttach();
	void OnWorkerDetach();
	static void _ListenerThread(void* server);
	static void _WorkerThread(void* server);
	void ListenerThread();
	void WorkerThread();
public:
	IHttpHandler* handler;
	IHttpHandler* defaultErrorHandler;
	int gcPeriod,gcMaxLifeTime;
	volatile bool terminated;
	ThreadTasks<Socket*> tasks;
	Server(int port=8080,const string& ip="0.0.0.0",int numWorkers=16);
	~Server();
	void Run();
	void Terminate();
	void RegisterHandler(IHttpHandler* handler);
	void RegisterDefaultErrorHandler(IHttpHandler* handler);
	void ServeFile(const string& fileName,HttpServerContext* context,bool download=false);
	void StartWorkers(int numWorkers);
	void StartSession(HttpSessionObject* sessionObject,HttpServerContext* context);
	HttpSessionObject* GetSessionObject(const string& token);
};

#endif

