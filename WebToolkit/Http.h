#ifndef _HTTP_H
#define	_HTTP_H

class HttpRequest
{
public:
	bool isPost;
	string host;
	string resource;
	string userAgent;
	string clientIP;
	int clientPort;
	i64 rangeFrom,rangeTo;
	map<string,string> parameters;
	int postContentLength;
	string postContent;
	HttpRequest();
	void ParseLine(const string& line);
	void ParseParameters(const string& st);
	void ParseURIAsParameters();
	void ParseHostAsParameters();
};

class HttpResponse
{
private:
	bool headersSent;
	string result;
	string contentType;
	string location;
	i64 contentLength;
	i64 rangeFrom,rangeTo,rangeTotal;
	ostringstream body;
	time_t expireTime;
public:
	HttpResponse();
	void SetResultNotFound();
	void SetResultError();
	void Write(const string& buf);
	string BuildHeader();
	virtual void DirectSend(const char* buf,int len)=0;
	void DirectSend(const string& buf);
	void Send();
	void SetContentType(const string& st);
	void SetContentLength(i64 len);
	void Redirect(const string& st);
	void Clean();
	void SetContentRange(i64 from,i64 to,i64 total);
	void SetExpires(time_t t);
};

class IHttpRequestHandler
{
public:
	virtual void Handle(HttpRequest* request,HttpResponse* response)=0;
};

class INotFoundHandler
{
public:
	virtual void HandleNotFound(HttpResponse* response)=0;
};

class IErrorHandler
{
public:
	virtual void HandleError(const exception& e,HttpResponse* response)=0;
};

class HostDispatcher:public IHttpRequestHandler
{
protected:
	map<string,IHttpRequestHandler*> dispatchMap;
	IHttpRequestHandler* defaultHandler;
	bool autoParse;
public:
	HostDispatcher();
	void AddMapping(const string& st,IHttpRequestHandler* handler);
	void SetDefaultHandler(IHttpRequestHandler* handler);
	void EnableAutoParse();
	void Handle(HttpRequest* request,HttpResponse* response);
};

class URIDispatcher:public IHttpRequestHandler
{
protected:
	map<string,IHttpRequestHandler*> dispatchMap;
	IHttpRequestHandler* defaultHandler;
	bool autoParse;
public:
	URIDispatcher();
	void AddMapping(const string& st,IHttpRequestHandler* handler);
	void SetDefaultHandler(IHttpRequestHandler* handler);
	void EnableAutoParse();
	void Handle(HttpRequest* request,HttpResponse* response);
};

class RootIndexRedirector:public IHttpRequestHandler
{
protected:
	string indexURI;
public:
	RootIndexRedirector();
	void SetIndexURI(const string& uri);
	void Handle(HttpRequest* request,HttpResponse* response);
};

#endif

