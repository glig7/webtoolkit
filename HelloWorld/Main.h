#ifndef _MAIN_H
#define	_MAIN_H

#include <WebToolkit.h>

class HelloWorld:public IHttpRequestHandler
{
private:
	Server server;
public:
	HelloWorld();
	void Run();
	void Handle(HttpRequest* request,HttpResponse* response);
};

#endif

