#ifndef _MAIN_H
#define	_MAIN_H

#include <WebToolkit.h>

class HelloWorld:public IHttpHandler
{
private:
	Server server;
public:
	HelloWorld();
	void Run();
	void Handle(HttpServerContext* context);
};

#endif

