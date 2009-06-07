#ifndef _SERVER_H
#define	_SERVER_H

#include "Socket.h"
#include "Client.h"
#include "Http.h"
#include "Thread.h"

enum LogMessageType
{
	LogDebug,
	LogVerbose,
	LogInfo,
	LogWarning,
	LogError,
	LogDisabled
};

class ILogger
{
public:
	virtual void LogWrite(LogMessageType type,const string& message)=0;
};

class Server
{
private:
	static Server* instance;
	Listener listener;
	IHttpRequestHandler* handler;
	INotFoundHandler* handlerNotFound;
	ILogger* logger;
	LogMessageType logLevel;
	Mutex clientsMutex;
	volatile int clientsCount;
	int listenerPort;
	string listenerIP;
public:
	volatile bool terminated;
	Server(int port,const string& ip);
	~Server();
	static Server& Instance();
	void Run();
	void Handle(HttpRequest* request,HttpResponse* response);
	void HandleNotFound(HttpResponse* response);
	void RegisterHandler(IHttpRequestHandler* handler);
	void RegisterNotFoundHandler(INotFoundHandler* handlerNotFound);
	void LogWrite(LogMessageType type,const string& message);
	void RegisterLogger(ILogger* logger);
	void SetLogLevel(LogMessageType logLevel);
	void OnClientAttach();
	void OnClientDetach();
	void ServeFile(const string& fileName,HttpRequest* request,HttpResponse* response,bool download=false);
};

#endif
