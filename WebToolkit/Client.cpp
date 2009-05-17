#include "Common.h"
#include "Client.h"
#include "Server.h"

Client::Client(Socket* socket)
{
	this->socket=socket;
	Server::Instance().OnClientAttach();
}

Client::~Client()
{
	delete socket;
	Server::Instance().OnClientDetach();
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
		cout<<socket->remoteIP<<(request.isPost?" POST ":" GET ")<<request.resource<<endl;
		Server::Instance().Handle(&request,this);
		Send();
	}
	catch(exception& e)
	{
		cout<<e.what()<<endl;
	}
	delete this;
}

void Client::DirectSend(const char* buf,int len)
{
	socket->Write(buf,len);
}
