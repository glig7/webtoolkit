#include "Common.h"
#include "Http.h"
#include "Util.h"
#include "Server.h"

HttpRequest::HttpRequest():rangeFrom(-1),rangeTo(-1),postContentLength(0)
{
}

void HttpRequest::ParseLine(const string& line)
{
	if(line.compare(0,3,"GET")==0)
	{
		isPost=false;
		size_t spacePos=line.find(' ',5);
		if(spacePos!=string::npos)
			resource=line.substr(4,spacePos-4);
		else
			resource=line.substr(4);
		size_t qPos=resource.find('?');
		if(qPos!=string::npos)
		{
			ParseParameters(resource.substr(qPos+1));
			resource.erase(qPos);
		}
	}
	if(line.compare(0,4,"POST")==0)
	{
		isPost=true;
		size_t spacePos=line.find(' ',6);
		if(spacePos!=string::npos)
			resource=line.substr(5,spacePos-5);
		else
			resource=line.substr(5);
	}
	if(line.compare(0,4,"Host")==0)
		host=line.substr(6);
	if(line.compare(0,10,"User-Agent")==0)
		userAgent=line.substr(12);
	if(line.compare(0,5,"Range")==0)
	{
		size_t eqpos=line.find('=');
		size_t mpos=line.find('-');
		rangeFrom=atoll(line.substr(eqpos+1,mpos-eqpos-1).c_str());
		string t=line.substr(mpos+1);
		if(!t.empty())
			rangeTo=atoll(t.c_str());
	}
	if(line.compare(0,14,"Content-Length")==0)
		postContentLength=atoi(line.substr(16).c_str());
}

void HttpRequest::ParseParameters(const string& st)
{
	string t=st;
	string p,l,r;
	size_t ampPos,eqPos;
	while(!t.empty())
	{
		ampPos=t.find('&');
		if(ampPos!=string::npos)
		{
			p=t.substr(0,ampPos);
			t.erase(0,ampPos+1);
		}
		else
		{
			p=t;
			t.clear();
		}
		eqPos=p.find('=');
		if(eqPos==string::npos)
			throw runtime_error("Malformed parameters in HTTP request");
		l=Util::URLDecode(p.substr(0,eqPos));
		r=Util::URLDecode(p.substr(eqPos+1));
		parameters[l]=r;
	}
}

void HttpRequest::ParseURIAsParameters()
{
	if(resource.empty())
		return;
	if(resource[0]=='/')
		resource.erase(0,1);
	size_t slashPos;
	string t;
	int i=0;
	while(!resource.empty())
	{
		slashPos=resource.find('/');
		if(slashPos!=string::npos)
		{
			t=resource.substr(0,slashPos);
			resource.erase(0,slashPos+1);
		}
		else
		{
			t=resource;
			resource.clear();
		}
		t=Util::URLDecode(t);
		ostringstream l;
		l<<"uri"<<i;
		parameters[l.str()]=t;
		i++;
	}
}

void HttpRequest::ParseHostAsParameters()
{
	if(host.empty())
		return;
	if(host[host.length()-1]=='.')
		resource.erase(host.length()-1);
	size_t dotPos;
	string t;
	int i=0;
	while(!host.empty())
	{
		dotPos=host.rfind('.');
		if(dotPos!=string::npos)
		{
			t=host.substr(dotPos+1);
			host.erase(dotPos);
		}
		else
		{
			t=host;
			host.clear();
		}
		ostringstream l;
		l<<"host"<<i;
		parameters[l.str()]=t;
		i++;
	}
}

HttpResponse::HttpResponse():result("HTTP/1.0 200 OK"),contentType("text/html"),headersSent(false),contentLength(0),rangeFrom(-1),expireTime(0)
{
}

void HttpResponse::SetResultNotFound()
{
	result="HTTP/1.0 404 Not Found";
}

void HttpResponse::SetResultError()
{
	result="HTTP/1.0 500 Internal Server Error";
}

void HttpResponse::Write(const string& buf)
{
	body<<buf;
}

string HttpResponse::BuildHeader()
{
	ostringstream r;
	r<<result<<endl;
	time_t timeNow;
	r<<"Date: "<<Util::MakeHTTPTime(time(&timeNow))<<endl;
	r<<"Server: madfish-webtoolkit"<<endl;
	if(!location.empty())
		r<<"Location: "<<location<<endl;
	else
	{
		if(expireTime!=0)
			r<<"Expires: "<<Util::MakeHTTPTime(expireTime)<<endl;
		if(contentLength==0)
			contentLength=body.str().size();
		r<<"Content-Length: "<<contentLength<<endl;
		if(rangeFrom!=-1)
			r<<"Content-Range: bytes "<<rangeFrom<<"-"<<rangeTo<<"/"<<rangeTotal<<endl;
		r<<"Content-Type: "<<contentType<<endl;
	}
	r<<"Connection: close"<<endl;
	r<<endl;
	return r.str();
}

void HttpResponse::DirectSend(const string& buf)
{
	DirectSend(buf.c_str(),buf.length());
}

void HttpResponse::Send()
{
	if(!headersSent)
	{
		DirectSend(BuildHeader());
		headersSent=true;
	}
	DirectSend(body.str());
	body.str("");
	body.clear();
}

void HttpResponse::SetContentType(const string& st)
{
	contentType=st;
}

void HttpResponse::SetContentLength(i64 len)
{
	contentLength=len;
}

void HttpResponse::Clean()
{
	body.str("");
	body.clear();
}

void HttpResponse::Redirect(const string& st)
{
	result="HTTP/1.0 301 Moved Permanently";
	location=st;
}

void HttpResponse::SetContentRange(i64 from,i64 to,i64 total)
{
	result="HTTP/1.0 206 Partial Content";
	rangeFrom=from;
	rangeTo=to;
	rangeTotal=total;
}

void HttpResponse::SetExpires(time_t t)
{
	expireTime=t;
}

HostDispatcher::HostDispatcher():defaultHandler(NULL),autoParse(false)
{
}

void HostDispatcher::AddMapping(const string& st,IHttpRequestHandler* handler)
{
	dispatchMap[st]=handler;
}

void HostDispatcher::SetDefaultHandler(IHttpRequestHandler* handler)
{
	defaultHandler=handler;
}

void HostDispatcher::EnableAutoParse()
{
	autoParse=true;
}

void HostDispatcher::Handle(HttpRequest* request,HttpResponse* response)
{
	for(map<string,IHttpRequestHandler*>::iterator iter=dispatchMap.begin();iter!=dispatchMap.end();iter++)
	{
		if((request->host.length()>=iter->first.length())&&(request->host.compare(request->host.length()-iter->first.length(),iter->first.length(),iter->first)==0))
		{
			request->host.erase(request->host.length()-iter->first.length());
			if(request->host[request->host.length()-1]=='.')
				request->host.resize(request->host.length()-1);
			if(autoParse)
				request->ParseHostAsParameters();
			iter->second->Handle(request,response);
			return;
		}
	}
	if(defaultHandler!=NULL)
	{
		if(autoParse)
			request->ParseHostAsParameters();
		defaultHandler->Handle(request,response);
	}
	else
		Server::Instance().HandleNotFound(response);
}

URIDispatcher::URIDispatcher():defaultHandler(NULL),autoParse(false)
{
}

void URIDispatcher::AddMapping(const string& st,IHttpRequestHandler* handler)
{
	dispatchMap[st]=handler;
}

void URIDispatcher::SetDefaultHandler(IHttpRequestHandler* handler)
{
	defaultHandler=handler;
}

void URIDispatcher::EnableAutoParse()
{
	autoParse=true;
}

void URIDispatcher::Handle(HttpRequest* request,HttpResponse* response)
{
	for(map<string,IHttpRequestHandler*>::iterator iter=dispatchMap.begin();iter!=dispatchMap.end();iter++)
	{
		if((request->resource.length()>=iter->first.length())&&(request->resource.compare(0,iter->first.length(),iter->first)==0))
		{
			request->resource.erase(0,iter->first.length());
			if(autoParse)
				request->ParseURIAsParameters();
			iter->second->Handle(request,response);
			return;
		}
	}
	if(defaultHandler!=NULL)
	{
		if(autoParse)
			request->ParseURIAsParameters();
		defaultHandler->Handle(request,response);
	}
	else
		Server::Instance().HandleNotFound(response);
}

RootIndexRedirector::RootIndexRedirector():indexURI("/index")
{
}

void RootIndexRedirector::SetIndexURI(const string& uri)
{
	indexURI=uri;
}

void RootIndexRedirector::Handle(HttpRequest* request,HttpResponse* response)
{
	if(request->resource=="/")
		response->Redirect(indexURI);
	else
		Server::Instance().HandleNotFound(response);
}
