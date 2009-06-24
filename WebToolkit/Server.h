#ifndef _SERVER_H
#define	_SERVER_H

#include "Socket.h"
#include "Client.h"
#include "Http.h"
#include "Thread.h"


class Server:public Singleton<Server>
{
private:
	Listener listener;
	IHttpRequestHandler* handler;
	INotFoundHandler* handlerNotFound;
	IErrorHandler* handlerError;
	Mutex workersMutex;
	volatile int workersCount;
	int listenerPort;
	string listenerIP;
public:
	volatile bool terminated;
	ThreadTasks<Socket*> tasks;
	Server(int port,const string& ip,int numWorkers=4);
	~Server();
	void Run();
	void Handle(HttpRequest* request,HttpResponse* response);
	void HandleNotFound(HttpResponse* response);
	void RegisterHandler(IHttpRequestHandler* handler);
	void RegisterNotFoundHandler(INotFoundHandler* handlerNotFound);
	void RegisterErrorHandler(IErrorHandler* handlerError);
	void OnWorkerAttach();
	void OnWorkerDetach();
	void ServeFile(const string& fileName,HttpRequest* request,HttpResponse* response,bool download=false);
	void StartWorkers(int numWorkers);
};

#endif
