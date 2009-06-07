#ifndef _MAIN_H
#define	_MAIN_H

#include <WebToolkit.h>

class MyConfig
{
public:
	string ip;
	int port;
	string rootDir;
	string htmlTemplate;
	string favIcon;
	string bindWhat;
	string bindAs;
	MyConfig();
};

class MyApp:public IHttpRequestHandler,public INotFoundHandler
{
private:
	MyConfig config;
	Server server;
	string htmlTemplate;
	string folderTemplate;
	string fileTemplate;
public:
	MyApp();
	void Run();
	void HandleNotFound(HttpResponse* response);
	void Handle(HttpRequest* request,HttpResponse* response);
};

#endif

