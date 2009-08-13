#ifndef _HTTP_H
#define	_HTTP_H

#include "Stream.h"

enum HttpMethod
{
	HttpGet,
	HttpPost
};

enum HttpResult
{
	HttpOK, //200 OK
	HttpPartialContent, //206 Partial Content
	HttpMovedPermanently, //301 Moved Permanently
	HttpSeeOther, //303 See Other
	HttpNotModified, //304 Not Modified
	HttpBadRequest, //400 Bad Request
	HttpForbidden, //403 Forbidden
	HttpNotFound, //404 Not Found
	HttpMethodNotAllowed, //405 Method Not Allowed
	HttpInternalServerError, //500 Internal Server Error
	HttpBadGateway //502 Bad Gateway
};

class Http
{
public:
	static const char* const methodStrings[];
	static const int resultCodes[];
	static const char* const resultStrings[];
	static const char* serverString;
};

class HttpException:public runtime_error
{
public:
	HttpResult result;
	HttpException(HttpResult code,const string& st):result(code),runtime_error(st)
	{
	}
};

//If you want to store your object for session, you should implement this.
//This interface was introduced to make automatic destruction possible.
class HttpSessionObject
{
public:
	virtual ~HttpSessionObject()=0;
};

//Request and response headers classes are suitable for both server-side and client-side usage.

class HttpRequestHeader
{
private:
	void ParseCookies(const string& st);
	void ParseHeaderItem(const string& name,const string& value);
public:
	HttpMethod method; //HTTP method. Only GET or POST are supported, as the only usable ones. See HttpMethod enum. GET is the default one.
	string host; //Host header value.
	string resource; //Requested resource.
	time_t modifyTime; //If-Modified-Since header value. HttpNotModified should be sent in the response if content has not been modified since that time. Should be 0 if not used.
	string userAgent; //User-Agent header value.
	i64 rangeFrom,rangeTo; //Content range. Should be -1 if not used.
	map<string,string> cookies; //Cookies as name-value pairs.
	string contentType; //The mime type of the content.
	int contentLength; //Content-Length header value. Should be 0 if not used.
	map<string,string> customHeaders; //Allows to set some custom header.
	bool keepConnection; //Is this connection persistent
	//
	HttpRequestHeader();
	void ParseLine(const string& line);
	string BuildHeader();
};

struct ResponseCookie
{
	string value;
	time_t expireTime;
};

class HttpResponseHeader
{
private:
	void ParseCookies(const string& st);
	void ParseHeaderItem(const string& name,const string& value);
public:
	HttpResult result; //Http result. See HttpResult enum.
	string server; //Server header value.
	string contentType; //The mime type of the content.
	string location; //Redirect location.
	i64 contentLength; //Content-Length header value.
	i64 rangeFrom,rangeTo,rangeTotal; //Content range. Used with HttpPartialContent result. Should be -1 if not used.
	time_t modifyTime; //Last-Modified header value. Should be 0 if not used.
	time_t expireTime; //Expires header value. Can be used to avoid subsequent requests to static content. Should be 0 if not used.
	map<string,ResponseCookie> cookies; //Cookies as name-value pairs. Cookie's expiration time is additionally stored.
	map<string,string> customHeaders; //Allows to set some custom header, e.g. X-Accel-Redirect for nginx.
	//
	HttpResponseHeader();
	void ParseLine(const string& line);
	string BuildHeader();
};

class HttpServerContext;

//You should implement this interface if you want to accept files.
class IFileUploadHandler
{
public:
	virtual void HandleFileUpload(HttpServerContext* context,const string& filename,InputStream* stream)=0;
	virtual ~IFileUploadHandler(); //To allow automatic destruction
};

//You should implement this interface for handling http requests, errors, etc.
//This is the interface for any classes that are working with http context.
class IHttpHandler
{
public:
	virtual void Handle(HttpServerContext* context)=0;
	virtual ~IHttpHandler(); //To allow automatic destruction
};

class Server; //Trying not to take additional header dependencies.

//This is the "heart" of http processing, because reference to this is passed to all handlers.
//
//Class is abstract. It is fully implemented in ClientRequest class
//
//Why extend InputStream and OutputStream and not to embed stream pointers?
//Because content can be compressed on the fly - so let it be implented in the Client class.
class HttpServerContext:public InputStream,public OutputStream
{
private:
	int nextUriNum; //Next number to assign for uri parameter.
	int nextHostNum; //Next number to assign for host parameter.
protected:
	void ParseParameters(const string& st);
	HttpServerContext(Server* s);
public:
	Server* server; //Server isntance, to which this request happened.
	IHttpHandler* errorHandler; //Error handler.
	IFileUploadHandler* fileHandler; //File upload handler.
	HttpRequestHeader requestHeader; //See related class.
	HttpResponseHeader responseHeader; //See related class.
	bool headerSent; //Response header already sent
	string clientIP; //Remote peer IP address. Can be used for identifying client by IP address.
	map<string,string> parameters; //Parameters as name-value pairs.
	HttpSessionObject* sessionObject; //Associated session object, if present.
	ostringstream responseBody; //Generated output goes here.
	//Call this when you need to process post data.
	//Called automatically by dispatchers for Post resources.
	void ProcessPostData();
	//Parse specifed number of uri sections (/*/) as parameters. num=0 means all.
	//These parameters will have names "uri#", where # starts from 0.
	void ParseURIAsParameters(int num=0);
	//Parse specifed number of host sections (.*.) as parameters. num=0 means all.
	//These parameters will have names "host#", where # starts from 0.
	void ParseHostAsParameters(int num=0);
	//Redirect (See Other)
	void Redirect(const string& location);
	//Redirect (Moved Permanently)
	void RedirectPermanent(const string& location);
	//Send headers
	void SendResponseHeader();
	//Send response (headers and body)
	void SendResponse();
	//Save object in the session.
	void StartSession(HttpSessionObject* sessionObject);
	//Serve file. Set download=true to force downloading (do not open in browser).
	void ServeFile(const string& fileName,bool download=false);
};

#endif

