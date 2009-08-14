/*
	This file is a part of madfish-webtoolkit project.
	http://code.google.com/p/madfish-webtoolkit/

	(c) 2009 Alexander "Mad Fish" Chehovsky
	See License.txt for licensing information.
*/

#ifndef _MAIN_H
#define	_MAIN_H

#include <WebToolkit.h>

struct BlogEntry
{
	string author;
	string text;
};

class SimpleBlog
{
private:
	Server server;
	URIDispatcher dispatcher;
	Mutex entriesMutex;
	vector<BlogEntry> entries;
public:
	SimpleBlog();
	void Run();
	void Index(HttpServerContext* context);
	void Post(HttpServerContext* context);
};

#endif

