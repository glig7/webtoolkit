#ifndef _HTTPHELPERS_H
#define	_HTTPHELPERS_H

#include "Http.h"

struct Mapping
{
	HttpMethod allowedMethod;
	IHttpHandler* handler;
	bool handlerAutoDelete;
	IHttpHandler* errorHandler;
	bool errorHandlerAutoDelete;
	IFileUploadHandler* fileHandler;
	bool fileHandlerAutoDelete;
};

class Dispatcher:public IHttpHandler
{
protected:
	map<string,Mapping> dispatchMap;
	string defaultHandler;
public:
	Dispatcher();
	~Dispatcher();
	void AddMapping(const string& st,HttpMethod allowedMethod,IHttpHandler* handler,bool handlerAutoDelete=false,IHttpHandler* errorHandler=NULL,bool errorHandlerAutoDelete=false,IFileUploadHandler* fileHandler=NULL,bool fileHandlerAutoDelete=false);
	void SetDefaultHandler(string defaultHandler);
	void Invoke(const string& what,HttpServerContext* context);
	virtual void Handle(HttpServerContext* context)=0;
};

class HostDispatcher:public Dispatcher
{
public:
	void Handle(HttpServerContext* context);
};

class URIDispatcher:public Dispatcher
{
public:
	void Handle(HttpServerContext* context);
};

class Redirector:public IHttpHandler
{
protected:
	string redirectURI;
public:
	Redirector(const string& uri);
	void Handle(HttpServerContext* context);
};

template<class T>
class HttpHandlerConnector:public IHttpHandler
{
public:
	typedef void (T::*HandlerFunction)(HttpServerContext* context);
	T* obj;
	HandlerFunction fn;
	HttpHandlerConnector(T* o,HandlerFunction f):obj(o),fn(f)
	{
	}
	void Handle(HttpServerContext* context)
	{
		(obj->*fn)(context);
	}
};

template<class T>
class HttpFileHandlerConnector:public IFileUploadHandler
{
public:
	typedef void (T::*HandlerFunction)(HttpServerContext* context,const string& filename,InputStream* stream);
	T* obj;
	HandlerFunction fn;
	HttpFileHandlerConnector(T* o,HandlerFunction f):obj(o),fn(f)
	{
	}
	void HandleFileUpload(HttpServerContext* context,const string& filename,InputStream* stream)
	{
		(obj->*fn)(context,filename,stream);
	}
};

#endif