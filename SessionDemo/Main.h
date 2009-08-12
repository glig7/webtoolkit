#ifndef _MAIN_H
#define	_MAIN_H

#include <WebToolkit.h>

class SessionObject:public HttpSessionObject
{
public:
	int counter;
	SessionObject();
	~SessionObject();
};

class SessionDemo:public IHttpHandler
{
private:
	Server server;
public:
	SessionDemo();
	void Run();
	void Handle(HttpServerContext* context);
};

#endif

