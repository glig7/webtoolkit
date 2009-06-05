#include "Common.h"
#include "Http.h"
#include "Util.h"

HttpRequest::HttpRequest():rangeFrom(-1),rangeTo(-1)
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
	if(line.compare(0,6,"Range:")==0)
	{
		size_t eqpos=line.find('=');
		size_t mpos=line.find('-');
		rangeFrom=_atoi64(line.substr(eqpos+1,mpos-eqpos-1).c_str());
		string t=line.substr(mpos+1);
		if(!t.empty())
			rangeTo=_atoi64(t.c_str());
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

