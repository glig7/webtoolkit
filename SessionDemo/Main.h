/*
	This file is a part of madfish-webtoolkit project.
	http://code.google.com/p/madfish-webtoolkit/

	(c) 2009 Alexander "Mad Fish" Chehovsky
	See License.txt for licensing information.
*/

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

