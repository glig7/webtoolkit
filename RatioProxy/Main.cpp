/*
	This file is a part of madfish-webtoolkit project.
	http://code.google.com/p/madfish-webtoolkit/

	(c) 2009 Alexander "Mad Fish" Chehovsky
	See License.txt for licensing information.
*/

#include "Main.h"

#include <stdlib.h>

#ifdef WIN32
#define atoll _atoi64
#endif

using namespace std;
using namespace CoreToolkit;
using namespace WebToolkit;

RatioProxyConfig::RatioProxyConfig():ip("0.0.0.0"),port(3456),numWorkers(4)
{
	File in("RatioProxy.conf",false);
	while(!in.Eof())
	{
		string line=in.ReadLine();
		if((!line.empty())&&(line[0]!='#'))
		{
			vector<string> elements=Util::Extract(line);
			if(elements[0]=="ListenIP")
			{
				ip=elements[1];
				continue;
			}
			if(elements[0]=="Port")
			{
				port=atoi(elements[1].c_str());
				continue;
			}
			if(elements[0]=="LogLevel")
			{
				if(elements[1]=="Debug")
					Log::SetReportingLevel(LogDebug);
				if(elements[1]=="Verbose")
					Log::SetReportingLevel(LogVerbose);
				if(elements[1]=="Info")
					Log::SetReportingLevel(LogInfo);
				if(elements[1]=="Warning")
					Log::SetReportingLevel(LogWarning);
				if(elements[1]=="Error")
					Log::SetReportingLevel(LogError);
				if(elements[1]=="Disabled")
					Log::SetReportingLevel(LogDisabled);
				continue;
			}
			if(elements[0]=="NumWorkers")
			{
				numWorkers=atoi(elements[1].c_str());
				continue;
			}
			if(elements[0]=="DownloadValueSource")
			{
				if(elements[1]=="Download")
					downloadValueSource=Download;
				else
					downloadValueSource=Upload;
				continue;
			}
			if(elements[0]=="UploadValueSource")
			{
				if(elements[1]=="Download")
					uploadValueSource=Download;
				else
					uploadValueSource=Upload;
				continue;
			}
			if(elements[0]=="DownloadMultiplier")
			{
				downloadMultiplier=static_cast<float>(atof(elements[1].c_str()));
				continue;
			}
			if(elements[0]=="UploadMultiplier")
			{
				uploadMultiplier=static_cast<float>(atof(elements[1].c_str()));
				continue;
			}
		}
	}
}

RatioProxy::RatioProxy():server(config.port,config.ip,config.numWorkers)
{
	server.RegisterHandler(this);
}

void RatioProxy::Run()
{
	server.Run();
}

vector<string> DetectParametersOrdering(const string& st)
{
	vector<string> r;
	string t=st;
	string p,name;
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
			continue;
		name=Util::URLDecode(p.substr(0,eqPos));
		r.push_back(name);
	}
	return r;}

void RatioProxy::Handle(HttpServerContext* context)
{
	//Parse request
	string url=context->requestHeader.resource;
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
	//Extract statistics and apply transformation
	string dst=context->parameters["downloaded"];
	string ust=context->parameters["uploaded"];
	long long d=atoll(dst.c_str());
	long long u=atoll(ust.c_str());
	long long nd,nu;
	if(config.downloadValueSource==Download)
		nd=static_cast<long long>((static_cast<float>(d)*config.downloadMultiplier)); //d=d*qd
	else
		nd=static_cast<long long>((static_cast<float>(u)*config.downloadMultiplier)); //d=u*qd
	if(config.uploadValueSource==Download)
		nu=static_cast<long long>((static_cast<float>(d)*config.uploadMultiplier)); //u=d*qu
	else
		nu=static_cast<long long>((static_cast<float>(u)*config.uploadMultiplier)); //u=u*qu
	//Use warning level here to allow disabling info level (we don't want to see resource strings in the output).
	LOG(LogWarning)<<hostnameFull<<" D: "<<Util::ToHumanReadableSize(d)<<" U: "<<Util::ToHumanReadableSize(u)<<" ND: "<<Util::ToHumanReadableSize(nd)<<" NU: "<<Util::ToHumanReadableSize(nu);
	context->parameters["downloaded"]=Util::NumToString(nd);
	context->parameters["uploaded"]=Util::NumToString(nu);
	//Make request to the real tracker
	requestHeader.userAgent=context->requestHeader.userAgent; //Use the same User-Agent
	requestHeader.customHeaders=context->requestHeader.customHeaders; //Use the same additional headers
	//New parameters.
	vector<string> ordering=DetectParametersOrdering(context->requestHeader.parametersString);
	for(size_t i=0;i<ordering.size();i++)
	{
		if(i==0)
			requestHeader.resource+="?";
		else
			requestHeader.resource+="&";
		requestHeader.resource+=Util::URLEncode(ordering[i])+"="+Util::URLEncode(context->parameters[ordering[i]]);
	}
	//Run the request
	try
	{
		HttpClient::RunRequest(hostname,port,requestHeader,requestData,responseHeader,responseData);
	}
	catch(exception& e)
	{
		throw HttpException(HttpBadGateway,e.what());
	}
	if(responseHeader.result!=HttpOK)
		throw HttpException(HttpBadGateway,"Request failed.");
	context->responseHeader.contentType=responseHeader.contentType; //Use the same Content-Type
	//Get rid of chunked transfer encoding (if present)
	//This is a workaround really, tracker use chunked encoding for HTTP/1.1 but normal for HTTP/1.0.
	if(responseHeader.customHeaders["Transfer-Encoding"]=="chunked")
	{
		vector<string> r=Util::DecodeChunks(responseData);
		if(!r.empty())
			responseData=r[0];
		else
			responseData.clear();
	}
	responseHeader.customHeaders.erase("Transfer-Encoding");
	context->responseHeader.customHeaders=responseHeader.customHeaders; //Use the same additional headers
	context->responseBody<<responseData;
}

int main()
{
	try
	{
		RatioProxy app;
		app.Run();
		Environment::WaitForTermination();
	}
	catch(exception& e)
	{
		LOG(LogError)<<e.what();
	}
}
