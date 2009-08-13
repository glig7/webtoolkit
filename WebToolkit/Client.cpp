#include "Common.h"
#include "Client.h"
#include "Logger.h"
#include "Http.h"
#include "Server.h"
#include "Socket.h"

class ClientRequest:public HttpServerContext
{
private:
	Socket* socket;
	int dataLeft;
protected:
	int ReadSomeUnbuffered(void* buf,int len);
public:
	ClientRequest(Server* server,Socket* s):HttpServerContext(server),socket(s)
	{
		clientIP=socket->remoteIP;
		errorHandler=server->defaultErrorHandler;
	}
	int WriteSome(const void* buf,int len)
	{
		return socket->WriteSome(buf,len);
	}
	bool Work();
};

int ClientRequest::ReadSomeUnbuffered(void* buf,int len)
{
	int t=min(len,dataLeft);
	socket->Read(buf,t);
	dataLeft-=t;
	if(dataLeft==0)
		eof=true;
	return t;
}

bool ClientRequest::Work()
{
	string st;
	for(;;)
	{
		if(!socket->Wait(5000))
			return false;
		st=socket->ReadLine();
		if(st.empty())
			break;
		requestHeader.ParseLine(st);
	}
	if(requestHeader.resource.empty())
		return requestHeader.keepConnection;
	LOG(LogInfo)<<clientIP<<" "<<Http::methodStrings[requestHeader.method]<<" "<<requestHeader.resource;
	if(requestHeader.cookies.find("sessiontoken")!=requestHeader.cookies.end())
		sessionObject=server->GetSessionObject(requestHeader.cookies["sessiontoken"]);
	size_t qpos=requestHeader.resource.find('?');
	if(qpos!=string::npos)
	{
		string parameters=requestHeader.resource.substr(qpos+1);
		requestHeader.resource.erase(qpos);
		ParseParameters(parameters);
	}
	dataLeft=requestHeader.contentLength;
	try
	{
		server->handler->Handle(this);
	}
	catch(HttpException& e)
	{
		responseHeader.result=e.result;
		responseBody.clear();
		responseBody<<e.what();
		LOG(LogError)<<e.what();
		if(errorHandler!=NULL)
			errorHandler->Handle(this);
	}
	catch(exception& e)
	{
		responseBody.clear();
		responseBody<<e.what();
		LOG(LogError)<<e.what();
		if(errorHandler!=NULL)
			errorHandler->Handle(this);
	}
	SendResponse();
	//Skip the rest of unused post data
	while(!Eof())
		ReadSome();
	LOG(LogVerbose)<<Http::resultStrings[responseHeader.result];
	return requestHeader.keepConnection;
}

Client::Client(Server* serv,Socket* sock):server(serv),socket(sock)
{
}

Client::~Client()
{
	delete socket;
}

void Client::Run()
{
	LOG(LogDebug)<<socket->remoteIP<<" Connection opened.";
	try
	{
		while(!socket->Eof())
		{
			ClientRequest clientRequest(server,socket);
			if(!clientRequest.Work())
				break;
		}
	}
	catch(exception& e)
	{
		LOG(LogError)<<e.what();
	}
	LOG(LogDebug)<<socket->remoteIP<<" Connection closed.";
}

