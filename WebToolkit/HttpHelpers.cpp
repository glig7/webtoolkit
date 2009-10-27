/*
	This file is a part of madfish-webtoolkit project.
	http://code.google.com/p/madfish-webtoolkit/

	(c) 2009 Alexander "Mad Fish" Chehovsky
	See License.txt for licensing information.
*/

#include "HttpHelpers.h"
#include "Server.h"

#include <stdlib.h>

using namespace std;
using namespace CoreToolkit;

namespace WebToolkit
{

Dispatcher::Dispatcher()
{
}

Dispatcher::~Dispatcher()
{
	for(map<string,Mapping>::iterator iter=dispatchMap.begin();iter!=dispatchMap.end();iter++)
	{
		if(iter->second.handlerAutoDelete)
			delete iter->second.handler;
		if(iter->second.errorHandlerAutoDelete)
			delete iter->second.errorHandler;
		if(iter->second.fileHandlerAutoDelete)
			delete iter->second.fileHandler;
	}
}

void Dispatcher::AddMapping(const std::string& st,HttpMethod allowedMethod,HttpHandler* handler,bool handlerAutoDelete,HttpHandler* errorHandler,bool errorHandlerAutoDelete,FileUploadHandler* fileHandler,bool fileHandlerAutoDelete)
{
	dispatchMap[st].allowedMethod=allowedMethod;
	dispatchMap[st].handler=handler;
	dispatchMap[st].handlerAutoDelete=handlerAutoDelete;
	dispatchMap[st].errorHandler=errorHandler;
	dispatchMap[st].errorHandlerAutoDelete=errorHandlerAutoDelete;
	dispatchMap[st].fileHandler=fileHandler;
	dispatchMap[st].fileHandlerAutoDelete=fileHandlerAutoDelete;
}

void Dispatcher::SetDefaultHandler(std::string defaultHandler)
{
	this->defaultHandler=defaultHandler;
}

void Dispatcher::Invoke(const std::string& what,HttpServerContext* context)
{
	if(dispatchMap.find(what)==dispatchMap.end())
		throw logic_error("Incorrect default handler");
	Mapping& mapping=dispatchMap[what];
	if(mapping.errorHandler!=NULL)
		context->errorHandler=mapping.errorHandler;
	if(mapping.fileHandler!=NULL)
		context->fileHandler=mapping.fileHandler;
	if(mapping.allowedMethod!=context->requestHeader.method)
		throw HttpException(HttpMethodNotAllowed,string("This page should be accessed using ")+Http::methodStrings[mapping.allowedMethod]);
	if(mapping.allowedMethod==HttpPost)
		context->ProcessPostData();
	mapping.handler->Handle(context);
}

void HostDispatcher::Handle(HttpServerContext* context)
{
	for(map<string,Mapping>::iterator iter=dispatchMap.begin();iter!=dispatchMap.end();iter++)
	{
		if((context->requestHeader.host.length()>=iter->first.length())&&(context->requestHeader.host.compare(context->requestHeader.host.length()-iter->first.length(),iter->first.length(),iter->first)==0))
		{
			context->requestHeader.host.erase(context->requestHeader.host.length()-iter->first.length());
			if(context->requestHeader.host[context->requestHeader.host.length()-1]=='.')
				context->requestHeader.host.resize(context->requestHeader.host.length()-1);
			Invoke(iter->first,context);
			return;
		}
	}
	if(!defaultHandler.empty())
		Invoke(defaultHandler,context);
	else
		throw HttpException(HttpNotFound,"Not found.");
}

void URIDispatcher::Handle(HttpServerContext* context)
{
	for(map<string,Mapping>::iterator iter=dispatchMap.begin();iter!=dispatchMap.end();iter++)
	{
		if((context->requestHeader.resource.length()>=iter->first.length())&&(context->requestHeader.resource.compare(0,iter->first.length(),iter->first)==0))
		{
			context->requestHeader.resource.erase(0,iter->first.length());
			Invoke(iter->first,context);
			return;
		}
	}
	if(!defaultHandler.empty())
		Invoke(defaultHandler,context);
	else
		throw HttpException(HttpNotFound,"Not found.");
}

Redirector::Redirector(const std::string& uri):redirectURI(uri)
{
}

void Redirector::Handle(HttpServerContext* context)
{
	if(context->requestHeader.resource=="/")
		context->Redirect(redirectURI);
	else
		throw HttpException(HttpNotFound,"Not found.");
}

void HttpClient::RunRequest(const std::string& hostname,int port,HttpRequestHeader& requestHeader,const std::string& requestData,HttpResponseHeader& responseHeader,std::string& responseData)
{
	Socket s(hostname,port);
	string rhs=requestHeader.BuildHeader();
	s.Write(rhs);
	s.Write(requestData);
	string st;
	for(;;)
	{
		if(!s.Wait(5000))
			throw IOException("No data in 5 seconds");
		st=s.ReadLine();
		if(st.empty())
			break;
		responseHeader.ParseLine(st);
	}
	//We're using connection: close (this is by default)
	//So, just read until the end of stream
	char buf[1024];
	while(!s.Eof())
	{
		int bytesRead=s.ReadSome(buf,sizeof(buf));
		responseData.append(buf,bytesRead);
	}
}

std::string HttpClient::Get(const std::string& url)
{
	size_t slashPos=url.find('/',7);
	string hostnameFull=url.substr(7,slashPos-7);
	string hostname=hostnameFull;
	string resource=url.substr(slashPos);
	size_t colonPos=hostname.find(':');
	int port=80;
	if(colonPos!=string::npos)
	{
		string portString=hostname.substr(colonPos+1);
		hostname.resize(colonPos);
		port=atoi(portString.c_str());
	}
	HttpRequestHeader requestHeader;
	HttpResponseHeader responseHeader;
	string requestData;
	string responseData;
	requestHeader.host=hostnameFull;
	requestHeader.method=HttpGet;
	requestHeader.resource=resource;
	RunRequest(hostname,port,requestHeader,requestData,responseHeader,responseData);
	if(responseHeader.result!=HttpOK)
		throw HttpException(responseHeader.result,"Failed to get data.");
	return responseData;
}

}
