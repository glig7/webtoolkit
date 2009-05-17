#ifndef _SERVER_H
#define	_SERVER_H

#include "Socket.h"
#include "Client.h"
#include "Http.h"
#include "Thread.h"

class Server
{
private:
	static Server* instance;
	Listener listener;
	IHttpRequestHandler* handler;
	INotFoundHandler* handlerNotFound;
	Mutex clientsMutex;
	volatile int clientsCount;
public:
	volatile bool terminated;
	Server(int port);
	~Server();
	static Server& Instance();
	void Run();
	void Handle(HttpRequest* request,HttpResponse* response);
	void HandleNotFound(HttpResponse* response);
	void RegisterHandler(IHttpRequestHandler* handler);
	void RegisterNotFoundHandler(INotFoundHandler* handlerNotFound);
	void OnClientAttach();
	void OnClientDetach();
	void ServeFile(const string& fileName,HttpRequest* request,HttpResponse* response,bool download=false);
};

#endif
