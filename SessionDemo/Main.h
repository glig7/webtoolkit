/*
	This file is a part of madfish-webtoolkit project.
	http://code.google.com/p/madfish-webtoolkit/

	(c) 2009 Alexander "Mad Fish" Chehovsky
	See License.txt for licensing information.
*/

#pragma once
#ifndef _MAIN_H
#define	_MAIN_H

#include <WebToolkit.h>

class SessionObject:public WebToolkit::HttpSessionObject
{
public:
	int counter;
	SessionObject();
	~SessionObject();
};

class SessionDemo:public WebToolkit::HttpHandler
{
private:
	WebToolkit::Server server;
public:
	SessionDemo();
	void Run();
	void Handle(WebToolkit::HttpServerContext* context);
};

#endif

