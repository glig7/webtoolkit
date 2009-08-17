/*
	This file is a part of madfish-webtoolkit project.
	http://code.google.com/p/madfish-webtoolkit/

	(c) 2009 Alexander "Mad Fish" Chehovsky
	See License.txt for licensing information.
*/

/**
	\file Http.cpp
	\brief Implementation of Http-related classes.
*/

#include "Http.h"
#include "Util.h"
#include "Server.h"

#include <time.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#define atoll _atoi64
#endif

using namespace std;
using namespace CoreToolkit;

namespace WebToolkit
{

const char* const Http::methodStrings[]={
	"GET",
	"POST"
};

const int Http::resultCodes[]={
	200,
	206,
	301,
	303,
	304,
	400,
	403,
	404,
	405,
	500,
	502,
	0
};

const char* const Http::resultStrings[]={
	"200 OK",
	"206 Partial Content",
	"301 Moved Permanently",
	"303 See Other",
	"304 Not Modified",
	"400 Bad Request",
	"403 Forbidden",
	"404 Not Found",
	"405 Method Not Allowed",
	"500 Internal Server Error",
	"502 Bad Gateway"
};

const char* Http::serverString="madfish-webtoolkit/1.1";

//HttpRequestHeader methods

HttpRequestHeader::HttpRequestHeader():method(HttpGet),modifyTime(0),rangeFrom(-1),rangeTo(-1),contentLength(0),userAgent(Http::serverString),keepConnection(false)
{
}

void HttpRequestHeader::ParseHeaderItem(const std::string& name,const std::string& value)
{
	//Some additional verifications applied to ensure nothing wrong happens later.
	if(name=="Host")
	{
		host=value;
		return;
	}
	if(name=="User-Agent")
	{
		userAgent=value;
		return;
	}
	if(name=="Range")
	{
		size_t eqpos=value.find('=');
		size_t mpos=value.find('-');
		if((eqpos==string::npos)||(mpos==string::npos))
			throw HttpException(HttpBadRequest,"Malformed header.");
		rangeFrom=atoll(value.substr(eqpos+1,mpos-eqpos-1).c_str());
		string t=value.substr(mpos+1);
		if(!t.empty())
		{
			rangeTo=atoll(t.c_str());
			if(rangeTo<rangeFrom)
				throw HttpException(HttpBadRequest,"Malformed header.");
		}
		return;
	}
	if(name=="Content-Length")
	{
		contentLength=atoi(value.c_str());
		if(contentLength<0)
			throw HttpException(HttpBadRequest,"Malformed header.");
		return;
	}
	if(name=="Cookie")
	{
		ParseCookies(value);
		return;
	}
	if(name=="If-Modified-Since")
	{
		modifyTime=Util::ParseHTTPTime(value);
		return;
	}
	if(name=="Content-Type")
	{
		contentType=value;
		return;
	}
	if(name=="Connection")
	{
		string t=Util::StringToLower(value);
		if(t.find("keep-alive")!=string::npos)
			keepConnection=true;
		if(t.find("close")!=string::npos)
			keepConnection=false;
		return;
	}
	customHeaders[name]=value;
}

void HttpRequestHeader::ParseLine(const std::string& line)
{
	size_t colonPos=line.find(':');
	if(colonPos==string::npos)
	{
		vector<string> items=Util::Extract(line);
		if(items[0]==Http::methodStrings[HttpPost])
			method=HttpPost;
		else
		{
			if(items[0]!=Http::methodStrings[HttpGet])
				throw HttpException(HttpMethodNotAllowed,"Only POST and GET methods are supported");
		}
		//Parameters from resource will be extracted later.
		//They are not related to the header itself.
		resource=items[1];
		if(items[2]!="HTTP/1.0")
			keepConnection=true;
	}
	else
	{
		string name=line.substr(0,colonPos);
		if(line.length()<colonPos+3)
			throw HttpException(HttpBadRequest,"Unexpected end of line in the header");
		string value=line.substr(colonPos+2);
		ParseHeaderItem(name,value);
	}
}

std::string HttpRequestHeader::BuildHeader()
{
	ostringstream r;
	r<<Http::methodStrings[method]<<" "<<resource<<" "<<"HTTP/1.1"<<endl;
	r<<"Host: "<<host<<endl;
	r<<"User-Agent: "<<userAgent<<endl;
	if(!contentType.empty())
	r<<"Content-Type: "<<contentType<<endl;
	if(contentLength!=0)
		r<<"Content-Length: "<<contentLength<<endl;
	if(rangeFrom!=-1)
	{
		r<<"Range: bytes="<<rangeFrom<<"-";
		if(rangeTo!=-1)
			r<<rangeTo;
		r<<endl;
	}
	if(modifyTime!=0)
		r<<"If-Modified-Since: "<<Util::MakeHTTPTime(modifyTime)<<endl;
	if(!cookies.empty())
	{
		r<<"Cookie: ";
		for(map<string,string>::iterator iter=cookies.begin();iter!=cookies.end();iter++)
		{
			if(iter!=cookies.begin())
				r<<"; ";
			r<<iter->first<<"="<<iter->second;
		}
		r<<endl;
	}
	for(map<string,string>::iterator iter=customHeaders.begin();iter!=customHeaders.end();iter++)
		r<<iter->first<<": "<<iter->second<<endl;
	r<<"Connection: keep-alive"<<endl;
	r<<endl;
	return r.str();
}

void HttpRequestHeader::ParseCookies(const std::string& st)
{
	string t=st;
	string p,l,r;
	size_t sepPos,eqPos;
	while(!t.empty())
	{
		sepPos=t.find(';');
		if(sepPos!=string::npos)
		{
			p=t.substr(0,sepPos);
			t.erase(0,sepPos+1);
		}
		else
		{
			p=t;
			t.clear();
		}
		Util::Trim(p);
		eqPos=p.find('=');
		if(eqPos==string::npos)
			throw runtime_error("Malformed cookie in HTTP request");
		l=Util::StringToLower(p.substr(0,eqPos));
		r=Util::StringToLower(p.substr(eqPos+1));
		cookies[l]=r;
	}
}

//HttpResponseHeader methods

HttpResponseHeader::HttpResponseHeader():result(HttpOK),contentLength(0),rangeFrom(-1),modifyTime(0),expireTime(0),server(Http::serverString)
{
}

void HttpResponseHeader::ParseHeaderItem(const std::string& name,const std::string& value)
{
	//Some additional verifications applied to ensure nothing wrong happens later.
	if(name=="Server")
	{
		server=value;
		return;
	}
	if(name=="Content-Length")
	{
		contentLength=atoi(value.c_str());
		if(contentLength<0)
			throw HttpException(HttpBadRequest,"Malformed header.");
		return;
	}
	if(name=="Content-Type")
	{
		contentType=value;
		return;
	}
	if(name=="Location")
	{
		location=value;
		return;
	}
	if(name=="Content-Range")
	{
		size_t sppos=value.find(' ');
		size_t mpos=value.find('-');
		size_t spos=value.find('/');
		if((sppos==string::npos)||(mpos==string::npos)||(spos==string::npos))
			throw HttpException(HttpBadRequest,"Malformed header.");
		rangeFrom=atoll(value.substr(sppos+1,mpos-sppos-1).c_str());
		rangeTo=atoll(value.substr(mpos+1,spos-mpos-1).c_str());
		rangeTotal=atoll(value.substr(spos+1).c_str());
		if((rangeFrom<0)||(rangeTo<0)||(rangeTotal<0)||(rangeTo<rangeFrom)||(rangeFrom>rangeTotal)||(rangeTo>rangeTotal))
			throw HttpException(HttpBadRequest,"Malformed header.");
		return;
	}
	if(name=="Last-Modified")
	{
		modifyTime=Util::ParseHTTPTime(value);
		return;
	}
	if(name=="Expires")
	{
		expireTime=Util::ParseHTTPTime(value);
		return;
	}
	if(name=="Set-Cookie")
	{
		ParseCookies(value);
		return;
	}
	customHeaders[name]=value;
}

void HttpResponseHeader::ParseLine(const std::string& line)
{
	size_t colonPos=line.find(':');
	if(colonPos==string::npos)
	{
		vector<string> items=Util::Extract(line);
		int code=atoi(items[1].c_str());
		for(int i=0;Http::resultCodes[i]!=0;i++)
		{
			if(Http::resultCodes[i]==code)
			{
				result=static_cast<HttpResult>(i);
				break;
			}
		}
	}
	else
	{
		string name=line.substr(0,colonPos);
		if(line.length()<colonPos+3)
			throw HttpException(HttpBadRequest,"Unexpected end of line in the header");
		string value=line.substr(colonPos+2);
		ParseHeaderItem(name,value);
	}
}

std::string HttpResponseHeader::BuildHeader()
{
	ostringstream r;
	r<<"HTTP/1.1 "<<Http::resultStrings[result]<<endl;
	time_t timeNow;
	r<<"Date: "<<Util::MakeHTTPTime(time(&timeNow))<<endl;
	r<<"Server: "<<server<<endl;
	if(!location.empty())
		r<<"Location: "<<location<<endl;
	else
	{
		if(modifyTime!=0)
			r<<"Last-Modified: "<<Util::MakeHTTPTime(expireTime)<<endl;
		if(expireTime!=0)
			r<<"Expires: "<<Util::MakeHTTPTime(expireTime)<<endl;
	}
	r<<"Content-Length: "<<contentLength<<endl;
	if(rangeFrom!=-1)
		r<<"Content-Range: bytes "<<rangeFrom<<"-"<<rangeTo<<"/"<<rangeTotal<<endl;
	r<<"Content-Type: "<<contentType<<endl;
	if(!cookies.empty())
	{
		time_t t;
		time(&t);
		for(map<string,ResponseCookie>::iterator iter=cookies.begin();iter!=cookies.end();iter++)
		{
			r<<"Set-Cookie: ";
			r<<iter->first<<"="<<iter->second.value;
			if(iter->second.expireTime!=0)
				r<<"; expires="<<Util::MakeHTTPTime(iter->second.expireTime);
			r<<"; path=/"<<endl;
		}
	}
	for(map<string,string>::iterator iter=customHeaders.begin();iter!=customHeaders.end();iter++)
		r<<iter->first<<": "<<iter->second<<endl;
	r<<"Connection: keep-alive"<<endl;
	r<<endl;
	return r.str();
}

void HttpResponseHeader::ParseCookies(const std::string& st)
{
	size_t eqpos=st.find("name=");
	eqpos+=4;
	size_t scpos=st.find(';',eqpos);
	size_t eq2pos=st.find("expires=");
	eq2pos+=7;
	size_t sc2pos=st.find(';',eq2pos);
	string name=st.substr(0,eqpos);
	cookies[name].value=st.substr(eqpos+1,scpos-eqpos-1);
	if(eq2pos!=string::npos)
		cookies[name].expireTime=Util::ParseHTTPTime(st.substr(eq2pos+1,sc2pos-eq2pos-1));
	else
		cookies[name].expireTime=0;
}

//HttpServerContext methods

HttpServerContext::HttpServerContext(Server* s):server(s),nextUriNum(0),nextHostNum(0),fileHandler(NULL),errorHandler(NULL),headerSent(false),sessionObject(NULL)
{
}

//"multipart/form-data" form handler helper class
class MultipartHelper:public Filter
{
private:
	string helperBuffer;
	string delimiter;
	HttpServerContext* context;
	FileUploadHandler* handler;
	bool Gather();
public:
	MultipartHelper(InputStream* source,const string& d,HttpServerContext* c,FileUploadHandler* h):Filter(source),delimiter(d),context(c),handler(h)
	{
	}
	int ReadSomeUnbuffered(void* buf,int len);
	void Process();
};

bool MultipartHelper::Gather()
{
	string st=sourceStream->ReadSome();
	helperBuffer+=st;
	return !st.empty();
}

int MultipartHelper::ReadSomeUnbuffered(void* buf,int len)
{
	//Read until next delimiter or until end of stream.
	while(helperBuffer.length()<=delimiter.length())
	{
		if(!Gather())
			break;
	}
	if(helperBuffer.empty())
		return 0;
	size_t dpos=helperBuffer.find(delimiter);
	if(dpos==string::npos)
	{
		int l=min<int>(helperBuffer.length()-delimiter.length(),len);
		memcpy(buf,helperBuffer.c_str(),l);
		helperBuffer.erase(0,l);
		return l;
	}
	else
	{
		int l=min<int>(dpos,len);
		memcpy(buf,helperBuffer.c_str(),l);
		helperBuffer.erase(0,l);
		return l;
	}
}

void MultipartHelper::Process()
{
	size_t dpos;
	for(;;)
	{
		if(Gather())
		{
			dpos=helperBuffer.find(delimiter);
			if(dpos!=string::npos)
				break;
		}
		else
			throw HttpException(HttpBadRequest,"No delimiters found.");
	}
	helperBuffer.erase(0,dpos+delimiter.size());
	while(helperBuffer.size()<2)
	{
		if(!Gather())
			throw HttpException(HttpBadRequest,"No delimiters found.");
	}
	//Determine line ending style.
	string lineEnding;
	if(helperBuffer[0]=='\n')
		lineEnding="\n";
	else
		lineEnding=helperBuffer.substr(0,2);
	delimiter=lineEnding+"--"+delimiter;
	//Now delimiter string is ready for actual usage
	for(;;)
	{
		if(helperBuffer.empty())
			Gather();
		//No actual reason to throw exception here
		if(helperBuffer.empty())
			return;
		//multipart/form-data has the following structure:
		//
		//--delimiter
		//data
		//--delimiter
		//data
		//--delimiter--      <-finish
		if(helperBuffer[0]=='-')
			return;
		//From now on, we use our own InputStream implementation to read
		//until next delimiter.
		//Reset it's state and skip to next line.
		eof=false;
		ReadLine();
		//Well, now we're at the data headers.
		string name;
		string filename;
		for(;;)
		{
			string st=ReadLine();
			if(st.empty())
				break;
			size_t namepos=st.find("name=\"");
			if(namepos!=string::npos)
			{
				namepos+=5;
				size_t name2pos=st.find('\"',namepos+1);
				if(name2pos==string::npos)
					throw HttpException(HttpBadRequest,"Malformed header in multipart/form-data");
				name=st.substr(namepos+1,name2pos-namepos-1);
				size_t filenamepos=st.find("filename=\"",name2pos+1);
				if(filenamepos!=string::npos)
				{
					filenamepos+=9;
					size_t filename2pos=st.find('\"',filenamepos+1);
					if(filename2pos==string::npos)
						throw HttpException(HttpBadRequest,"Malformed header in multipart/form-data");
					filename=st.substr(filenamepos+1,filename2pos-filenamepos-1);
				}
			}
		}
		//Yeah, finally - we're exactly at the data.
		if(!filename.empty())
		{
			if(handler)
				handler->HandleFileUpload(context,name,filename,this);
		}
		else
		{
			string value;
			while(!Eof())
				value+=ReadSome();
			context->parameters[name]=value;
		}
		//Skip delimiter
		helperBuffer.erase(0,delimiter.size());
	}
}
//

void HttpServerContext::ProcessPostData()
{
	if(requestHeader.contentLength==0)
		return;
	if(requestHeader.contentType.find("application/x-www-form-urlencoded")!=string::npos)
		ParseParameters(Read(requestHeader.contentLength));
	else
	{
		size_t eqpos=requestHeader.contentType.find("boundary=");
		if(eqpos==string::npos)
			throw HttpException(HttpBadRequest,"Malformed header for multipart data");
		string delimiter=requestHeader.contentType.substr(eqpos+9);
		MultipartHelper helper(this,delimiter,this,fileHandler);
		helper.Process();
	}
}

void HttpServerContext::ParseURIAsParameters(int num)
{
	if(requestHeader.resource.empty())
		return;
	if(requestHeader.resource[0]=='/')
		requestHeader.resource.erase(0,1);
	size_t slashPos;
	string t;
	for(int i=0;(num==0)||(i<num);i++)
	{
		if(requestHeader.resource.empty())
			break;
		slashPos=requestHeader.resource.find('/');
		if(slashPos!=string::npos)
		{
			t=requestHeader.resource.substr(0,slashPos);
			requestHeader.resource.erase(0,slashPos+1);
		}
		else
		{
			t=requestHeader.resource;
			requestHeader.resource.clear();
		}
		t=Util::URLDecode(t);
		ostringstream l;
		l<<"uri"<<nextUriNum;
		parameters[l.str()]=t;
		nextUriNum++;
	}
}

void HttpServerContext::ParseHostAsParameters(int num)
{
	if(requestHeader.host.empty())
		return;
	if(requestHeader.host[requestHeader.host.length()-1]=='.')
		requestHeader.host.erase(requestHeader.host.length()-1);
	size_t dotPos;
	string t;
	for(int i=0;(num==0)||(i<num);i++)
	{
		if(requestHeader.host.empty())
			break;
		dotPos=requestHeader.host.rfind('.');
		if(dotPos!=string::npos)
		{
			t=requestHeader.host.substr(dotPos+1);
			requestHeader.host.erase(dotPos);
		}
		else
		{
			t=requestHeader.host;
			requestHeader.host.clear();
		}
		ostringstream l;
		l<<"host"<<nextHostNum;
		parameters[l.str()]=t;
		nextHostNum++;
	}
}

void HttpServerContext::ParseParameters(const std::string& st)
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

void HttpServerContext::Redirect(const std::string& location)
{
	responseHeader.result=HttpSeeOther;
	responseHeader.location=location;
	responseBody<<"<html><body><a href=\""<<location<<"\">"<<location<<"</a></body></html>";
}

void HttpServerContext::RedirectPermanent(const std::string& location)
{
	responseHeader.result=HttpMovedPermanently;
	responseHeader.location=location;
	responseBody<<"<html><body><a href=\""<<location<<"\">"<<location<<"</a></body></html>";
}

void HttpServerContext::SendResponseHeader()
{
	if(headerSent)
		return;
	Write(responseHeader.BuildHeader());
	headerSent=true;
}

void HttpServerContext::SendResponse()
{
	if(headerSent)
		return;
	responseHeader.contentLength=responseBody.str().length();
	SendResponseHeader();
	Write(responseBody.str());
}

void HttpServerContext::StartSession(HttpSessionObject* sessionObject)
{
	server->StartSession(sessionObject,this);
}

void HttpServerContext::ServeFile(const std::string& fileName,bool download)
{
	server->ServeFile(fileName,this,download);
}

HttpSessionObject::~HttpSessionObject()
{
}

FileUploadHandler::~FileUploadHandler()
{
}

HttpHandler::~HttpHandler()
{
}

}
