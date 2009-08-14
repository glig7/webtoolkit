/*
	This file is a part of madfish-webtoolkit project.
	http://code.google.com/p/madfish-webtoolkit/

	(c) 2009 Alexander "Mad Fish" Chehovsky
	See License.txt for licensing information.
*/

#pragma once
#ifndef _MAIN_H
#define	_MAIN_H

#include <Thread.h>
#include <WebToolkit.h>

#include <string>
#include <vector>

struct BlogEntry
{
	std::string author;
	std::string text;
};

class SimpleBlog
{
private:
	WebToolkit::Server server;
	WebToolkit::URIDispatcher dispatcher;
	CoreToolkit::Mutex entriesMutex;
	std::vector<BlogEntry> entries;
public:
	SimpleBlog();
	void Run();
	void Index(WebToolkit::HttpServerContext* context);
	void Post(WebToolkit::HttpServerContext* context);
};

#endif

