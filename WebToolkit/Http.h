/*
	This file is a part of madfish-webtoolkit project.
	http://code.google.com/p/madfish-webtoolkit/

	(c) 2009 Alexander "Mad Fish" Chehovsky
	See License.txt for licensing information.
*/

/**
	\file Http.h
	\brief Definitions of Http-related classes.
*/

#pragma once
#ifndef _HTTP_H
#define	_HTTP_H

#include "Stream.h"

#include <string>
#include <stdexcept>
#include <map>
#include <sstream>

namespace WebToolkit
{

//! Http method. Only GET and POST are usable.
enum HttpMethod
{
	HttpGet,
	HttpPost
};

//! Http result for response header.
enum HttpResult
{
	HttpOK, //!< 200 OK
	HttpPartialContent, //!< 206 Partial Content
	HttpMovedPermanently, //!< 301 Moved Permanently
	HttpSeeOther, //!< 303 See Other
	HttpNotModified, //!< 304 Not Modified
	HttpBadRequest, //!< 400 Bad Request
	HttpForbidden, //!< 403 Forbidden
	HttpNotFound, //!< 404 Not Found
	HttpMethodNotAllowed, //!< 405 Method Not Allowed
	HttpInternalServerError, //!< 500 Internal Server Error
	HttpBadGateway //!< 502 Bad Gateway
};

//! Http constants
class Http
{
public:
	static const char* const methodStrings[];
	static const int resultCodes[];
	static const char* const resultStrings[];
	static const char* serverString;
};

/**
	\brief Http exception class.
	Allows to specify http result code.
*/
class HttpException:public std::runtime_error
{
public:
	HttpResult result;
	HttpException(HttpResult code,const std::string& st):result(code),std::runtime_error(st)
	{
	}
};

/**
	\brief Session object interface.
	If you want to store your object for session, you should implement this.
	This interface was introduced to make automatic destruction possible.
*/
class HttpSessionObject
{
public:
	virtual ~HttpSessionObject()=0;
};

//Request and response headers classes are suitable for both server-side and client-side usage.

/**
	\brief Request header class.
	Can perform parsing and generation of request header.
*/
class HttpRequestHeader
{
private:
	//! Parse cookies string.
	void ParseCookies(const std::string& st);
	//! Parse single header item string.
	void ParseHeaderItem(const std::string& name,const std::string& value);
public:
	/**
		HTTP method. Only GET or POST are supported, as the only usable ones. 
		See HttpMethod enum. GET is the default one.
	*/
	HttpMethod method;
	//! Host header value.
	std::string host;
	//! Requested resource.
	std::string resource;
	/**
		If-Modified-Since header value. HttpNotModified should be sent in the response 
		if content has not been modified since that time. Should be 0 if not used.
	*/
	time_t modifyTime;
	//! User-Agent header value.
	std::string userAgent;
	//! Content range. Should be -1 if not used.
	long long rangeFrom,rangeTo;
	//! Cookies as name-value pairs.
	std::map<std::string,std::string> cookies;
	//! The mime type of the content.
	std::string contentType;
	//! Content-Length header value. Should be 0 if not used.
	int contentLength;
	//! Allows to set some custom header.
	std::map<std::string,std::string> customHeaders;
	//! Is this connection persistent
	bool keepConnection;
	//! Constructs a new header with default values.
	HttpRequestHeader();
	//! Parse a single line of header data.
	void ParseLine(const std::string& line);
	//! Writes the entire header into string.
	std::string BuildHeader();
};

/**
	\brief Cookie in the response header.
*/
struct ResponseCookie
{
	//! Cookie's value
	std::string value;
	//! Cookie's expiration time.
	time_t expireTime;
};

/**
	\brief Response header class.
	Can perform parsing and generation of response header.
*/
class HttpResponseHeader
{
private:
	//! Parse cookies string.
	void ParseCookies(const std::string& st);
	//! Parse single header item string.
	void ParseHeaderItem(const std::string& name,const std::string& value);
public:
	//! Http result. See HttpResult enum.
	HttpResult result;
	//! Server header value.
	std::string server;
	//! The mime type of the content.
	std::string contentType;
	//! Redirect location.
	std::string location;
	//! Content-Length header value.
	long long contentLength;
	//! Content range. Used with HttpPartialContent result. Should be -1 if not used.
	long long rangeFrom,rangeTo,rangeTotal;
	//! Last-Modified header value. Should be 0 if not used.
	time_t modifyTime;
	//! Expires header value. Can be used to avoid subsequent requests to static content. Should be 0 if not used.
	time_t expireTime;
	//! Cookies as name-value pairs. Cookie's expiration time is additionally stored.
	std::map<std::string,ResponseCookie> cookies;
	//! Allows to set some custom header, e.g. X-Accel-Redirect for nginx.
	std::map<std::string,std::string> customHeaders;
	//! Constructs a new header with default values.
	HttpResponseHeader();
	//! Parse a single line of header data.
	void ParseLine(const std::string& line);
	//! Writes the entire header into string.
	std::string BuildHeader();
};

class HttpServerContext;

/**
	\brief File upload handling interface.
	You should implement this interface if you want to accept files.
*/
class FileUploadHandler
{
public:
	virtual void HandleFileUpload(HttpServerContext* context,const std::string& filename,CoreToolkit::InputStream* stream)=0;
	virtual ~FileUploadHandler(); //!< To allow automatic destruction
};

/**
	\brief Http handling interface.
	You should implement this interface for handling http requests, errors, etc.
	This is the interface for any classes that are working with http context.
*/
class HttpHandler
{
public:
	virtual void Handle(HttpServerContext* context)=0;
	virtual ~HttpHandler(); //!< To allow automatic destruction
};

class Server; //Trying not to take additional header dependencies.

/**
	\brief Http server context class.
	This is the "heart" of http processing, because reference to this is passed to all handlers.
	
	Class is abstract. It is fully implemented in ClientRequest class
	
	Why extend InputStream and OutputStream and not to embed stream pointers?
	Because content can be compressed on the fly - so let it be implemented in the Client class.
*/
class HttpServerContext:public CoreToolkit::InputStream,public CoreToolkit::OutputStream
{
private:
	//! Next number to assign for uri parameter.
	int nextUriNum;
	//! Next number to assign for host parameter.
	int nextHostNum;
protected:
	//! Parse string in "application/x-www-form-urlencoded" format as parameters.
	void ParseParameters(const std::string& st);
	//! Constructs a new server context.
	HttpServerContext(Server* s);
public:
	//! Server instance, to which this request happened.
	Server* server;
	//! Error handler.
	HttpHandler* errorHandler;
	//! File upload handler.
	FileUploadHandler* fileHandler;
	//! See related class.
	HttpRequestHeader requestHeader;
	//! See related class.
	HttpResponseHeader responseHeader;
	//! Response header already sent
	bool headerSent;
	//! Remote peer IP address. Can be used for identifying client by IP address.
	std::string clientIP;
	//! Parameters as name-value pairs.
	std::map<std::string,std::string> parameters;
	//! Associated session object, if present.
	HttpSessionObject* sessionObject;
	//! Generated output goes here.
	std::ostringstream responseBody;
	/**
		Call this when you need to process post data.
		Called automatically by dispatchers for Post resources.
	*/
	void ProcessPostData();
	/**
		Parse specified number of uri sections as parameters. num=0 means all.
		These parameters will have names "uri#", where # starts from 0.
	*/
	void ParseURIAsParameters(int num=0);
	/**
		Parse specified number of host sections as parameters. num=0 means all.
		These parameters will have names "host#", where # starts from 0.
	*/
	void ParseHostAsParameters(int num=0);
	//! Redirect (See Other)
	void Redirect(const std::string& location);
	//! Redirect (Moved Permanently)
	void RedirectPermanent(const std::string& location);
	//! Send headers
	void SendResponseHeader();
	//! Send response (headers and body)
	void SendResponse();
	//! Save object in the session.
	void StartSession(HttpSessionObject* sessionObject);
	//! Serve file. Set download=true to force downloading (do not open in browser).
	void ServeFile(const std::string& fileName,bool download=false);
};

}

#endif

