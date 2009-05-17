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
	int rangeFrom,rangeTo;
	HttpRequest();
	void ParseLine(const string& line);
};

class HttpResponse
{
private:
	bool headersSent;
	string result;
	string contentType;
	string location;
	int contentLength;
	int rangeFrom,rangeTo,rangeTotal;
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
	void SetContentLength(int len);
	void Redirect(const string& st);
	void Clean();
	void SetContentRange(int from,int to,int total);
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

#endif

