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
			if(!socket->WaitForLine(2000))
				throw runtime_error("No data read in 2 seconds, closing connection");
			st=socket->ReadLine();
			if(st.empty())
				break;
			request.ParseLine(st);
		}
		ostringstream r;
		r<<socket->remoteIP<<(request.isPost?" POST ":" GET ")<<request.resource;
		Server::Instance().LogWrite(LogInfo,r.str());
		Server::Instance().Handle(&request,this);
		Send();
	}
	catch(exception& e)
	{
		Server::Instance().LogWrite(LogError,e.what());
	}
	delete this;
}

void Client::DirectSend(const char* buf,int len)
{
	socket->Write(buf,len);
}

void Client::Start()
{
#ifdef __UCLIBC__
	Run();
#else
	Thread::Start();
#endif
}

