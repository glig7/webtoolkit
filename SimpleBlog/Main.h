#ifndef _MAIN_H
#define	_MAIN_H

#include <WebToolkit.h>

class Index:public IHttpRequestHandler
{
public:
	void Handle(HttpRequest* request,HttpResponse* response);
};

class Post:public IHttpRequestHandler
{
public:
	void Handle(HttpRequest* request,HttpResponse* response);
};

struct BlogEntry
{
	string author;
	string text;
};

class SimpleBlog:public Singleton<SimpleBlog>
{
private:
	Server server;
	URIDispatcher dispatcher;
	RootIndexRedirector indexRedirector;
	Index index;
	Post post;
public:
	Mutex entriesMutex;
	vector<BlogEntry> entries;
	SimpleBlog();
	void Run();
};

#endif

