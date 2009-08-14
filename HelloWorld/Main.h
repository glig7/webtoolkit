/*
	This file is a part of madfish-webtoolkit project.
	http://code.google.com/p/madfish-webtoolkit/

	(c) 2009 Alexander "Mad Fish" Chehovsky
	See License.txt for licensing information.
*/

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

