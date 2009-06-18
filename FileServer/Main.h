#ifndef _MAIN_H
#define	_MAIN_H

#include <WebToolkit.h>

class FileServerConfig
{
public:
	string ip;
	int port;
	string rootDir;
	string htmlTemplate;
	string favIcon;
	string bindWhat;
	string bindAs;
	int numWorkers;
	FileServerConfig();
};

class FileServer:public IHttpRequestHandler,public INotFoundHandler
{
private:
	FileServerConfig config;
	Server server;
	string htmlTemplate;
	string folderTemplate;
	string fileTemplate;
public:
	FileServer();
	void Run();
	void HandleNotFound(HttpResponse* response);
	void Handle(HttpRequest* request,HttpResponse* response);
};

#endif

