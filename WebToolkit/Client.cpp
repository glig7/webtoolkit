#include "Common.h"
#include "Client.h"
#include "Server.h"
#include "Logger.h"

Client::Client(Socket* socket)
{
	this->socket=socket;
}

Client::~Client()
{
	delete socket;
}

void Client::Run()
{
	try
	{
		HttpRequest request;
		request.clientIP=socket->remoteIP;
		request.clientPort=socket->remotePort;
		string st;
		for(;;)
		{
			st=socket->ReadLine();
			if(st.empty())
				break;
			request.ParseLine(st);
		}
		if(request.cookies.find("sessiontoken")!=request.cookies.end())
			request.sessionObject=Server::Instance().GetSessionObject(request.cookies["sessiontoken"]);
		if(request.postContentLength!=0)
		{
			request.postContent=socket->BufferedRead(request.postContentLength);
			request.ParseParameters(request.postContent);
		}
		LOG(LogInfo)<<socket->remoteIP<<(request.isPost?" POST ":" GET ")<<request.resource;
		Server::Instance().Handle(&request,this);
		Send();
	}
	catch(exception& e)
	{
		LOG(LogError)<<e.what();
	}
}

void Client::DirectSend(const char* buf,int len)
{
	socket->Write(buf,len);
}
