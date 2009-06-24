#include "Common.h"
#include "Server.h"
#include "Util.h"
#include "File.h"
#include "Logger.h"

template<> Server* Singleton<Server>::instance=NULL;

#ifdef WIN32
#include <windows.h>

BOOL WINAPI HandlerRoutine(DWORD dwCtrlType)
{
	Server::Instance().terminated=true;
	return TRUE;
}
#else
#include <signal.h>
#endif

Server::Server(int port,const string& ip,int numWorkers):listenerPort(port),listenerIP(ip),listener(port,ip),handler(NULL),handlerNotFound(NULL),handlerError(NULL),terminated(false),workersCount(0)
{
	instance=this;
#ifdef WIN32
	SetConsoleCtrlHandler(HandlerRoutine,TRUE);
#else
	sigset_t sigset;
	sigemptyset(&sigset);
	sigaddset(&sigset,SIGINT);
	sigaddset(&sigset,SIGHUP);
	sigaddset(&sigset,SIGTERM);
	sigprocmask(SIG_BLOCK,&sigset,NULL);
	signal(SIGPIPE,SIG_IGN);
#endif
	StartWorkers(numWorkers);
}

Server::~Server()
{
	LOG(LogInfo)<<"Server stopped";
}

void worker_thread(void* d)
{
	Server::Instance().OnWorkerAttach();
	for(;;)
	{
		Socket* socket=Server::Instance().tasks.Pop();
		if(!socket)
			break;
		Client client(socket);
		client.Run();
	}
	Server::Instance().OnWorkerDetach();
}

void Server::Run()
{
	LOG(LogInfo)<<"Started server on "<<listenerIP<<":"<<listenerPort;
#ifndef WIN32
	sigset_t sigset;
#endif
	while(!terminated)
	{
		if(listener.Wait(500))
		{
			Socket* socket=listener.Accept();
			if(workersCount!=0)
				tasks.Push(socket);
			else
			{
				Client client(socket);
				client.Run();
			}
		}
#ifndef WIN32
		sigpending(&sigset);
		if(sigismember(&sigset,SIGINT)||sigismember(&sigset,SIGTERM))
			terminated=true;
#endif
	}
	LOG(LogInfo)<<"Waiting for worker threads...";
	int t=workersCount;
	for(int i=0;i<t;i++)
		tasks.Push(NULL);
	while(workersCount!=0)
	{
	}
}

void Server::Handle(HttpRequest* request,HttpResponse* response)
{
	if(handler!=NULL)
	{
		try
		{
			handler->Handle(request,response);
		}
		catch(exception& e)
		{
			response->SetResultError();
			response->Clean();
			if(handlerError!=NULL)
				handlerError->HandleError(e,response);
			else
			{
				response->Write("<html><body><h1>500 Internal Server Error</h1><p>Exception: ");
				response->Write(e.what());
				response->Write("</p</body></html>");
			}
		}
	}
	else
		HandleNotFound(response);
}

void Server::HandleNotFound(HttpResponse* response)
{
	response->SetResultNotFound();
	if(handlerNotFound!=NULL)
		handlerNotFound->HandleNotFound(response);
	else
		response->Write("<html><body><h1>404 Not Found</h1></body></html>");
}

void Server::RegisterHandler(IHttpRequestHandler* handler)
{
	this->handler=handler;
}

void Server::RegisterNotFoundHandler(INotFoundHandler* handlerNotFound)
{
	this->handlerNotFound=handlerNotFound;
}

void Server::RegisterErrorHandler(IErrorHandler* handlerError)
{
	this->handlerError=handlerError;
}

void Server::OnWorkerAttach()
{
	MutexLock lock(workersMutex);
	workersCount++;
}

void Server::OnWorkerDetach()
{
	MutexLock lock(workersMutex);
	workersCount--;
}

void Server::ServeFile(const string& fileName,HttpRequest* request,HttpResponse* response,bool download)
{
	i64 size=Util::GetFileSize(fileName);
	if(size==-1)
	{
		HandleNotFound(response);
		return;
	}
	File in(fileName,false);
	i64 contentLength;
	if(request->rangeFrom==-1)
	{
		contentLength=size;
		response->SetContentLength(size);
	}
	else
	{
		if(request->rangeTo==-1)
			request->rangeTo=size-1;
		contentLength=request->rangeTo-request->rangeFrom+1;
		response->SetContentLength(contentLength);
		response->SetContentRange(request->rangeFrom,request->rangeTo,size);
		in.Seek(request->rangeFrom);
	}
	response->SetContentType("application/octet-stream");
	if(!download)
	{
		int slashpos=fileName.find('/');
		string t;
		if(slashpos!=string::npos)
			t=fileName.substr(slashpos+1);
		else
			t=fileName;
		int dotpos=t.rfind('.');
		if(dotpos!=string::npos)
		{
			string ext=t.substr(dotpos+1);
			if(ext=="txt"||ext=="cpp"||ext=="h")
				response->SetContentType("text/plain");
			if(ext=="htm"||ext=="html")
				response->SetContentType("text/html");
			if(ext=="xml")
				response->SetContentType("text/xml");
			if(ext=="css")
				response->SetContentType("text/css");
			if(ext=="js")
				response->SetContentType("application/javascript");
			if(ext=="gif")
				response->SetContentType("image/gif");
			if(ext=="jpg")
				response->SetContentType("image/jpeg");
			if(ext=="png")
				response->SetContentType("image/png");
		}
	}
	response->Send();
	char buf[1024];
	while(contentLength>0)
	{
		int br;
		if(contentLength<sizeof(buf))
			br=static_cast<int>(contentLength);
		else
			br=sizeof(buf);
		in.Read(buf,br);
		response->DirectSend(buf,br);
		contentLength-=br;
	}
}


void Server::StartWorkers(int numWorkers)
{
	for(int i=0;i<numWorkers;i++)
		Thread::StartThread(worker_thread,NULL);
}
