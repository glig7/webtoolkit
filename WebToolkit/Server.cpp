/*
	This file is a part of madfish-webtoolkit project.
	http://code.google.com/p/madfish-webtoolkit/

	(c) 2009 Alexander "Mad Fish" Chehovsky
	See License.txt for licensing information.
*/

#include "Server.h"
#include "Util.h"
#include "FileUtils.h"
#include "File.h"
#include "Logger.h"
#include "Client.h"

#include <time.h>

using namespace std;
using namespace CoreToolkit;

namespace WebToolkit
{

Server::Server(int port,const std::string& ip,int numWorkers):listenerPort(port),listenerIP(ip),listener(port,ip),handler(NULL),defaultErrorHandler(NULL),terminated(false),workersCount(0),gcPeriod(100),gcMaxLifeTime(1800),gcCounter(0),listenerThreadRunning(false)
{
	StartWorkers(numWorkers);
}

Server::~Server()
{
	Terminate();
	for(map<string,Session>::iterator iter=sessions.begin();iter!=sessions.end();iter++)
		delete iter->second.object;
}

void Server::_ListenerThread(void* server)
{
	reinterpret_cast<Server*>(server)->ListenerThread();
}

void Server::_WorkerThread(void* server)
{
	reinterpret_cast<Server*>(server)->WorkerThread();
}

void Server::ListenerThread()
{
	listenerThreadRunning=true;
	while(!terminated)
	{
		if(listener.Wait(500))
		{
			if(terminated)
				break;
			Socket* socket=listener.Accept();
			if(workersCount!=0)
				tasks.Push(socket);
			else
			{
				Client client(this,socket);
				client.Run();
			}
		}
	}
	listenerThreadRunning=false;
}

void Server::WorkerThread()
{
	OnWorkerAttach();
	while(!terminated)
	{
		Socket* socket=tasks.Pop();
		if(terminated)
			break;
		Client client(this,socket);
		client.Run();
	}
	OnWorkerDetach();
}

void Server::Run()
{
	Thread::StartThread(_ListenerThread,this);
	LOG(LogInfo)<<"Started server on "<<listenerIP<<":"<<listenerPort;
}

void Server::Terminate()
{
	if((workersCount==0)&&(!listenerThreadRunning))
		return;
	terminated=true;
	LOG(LogInfo)<<"Waiting for worker threads...";
	int t=workersCount;
	for(int i=0;i<t;i++)
		tasks.Push(NULL);
	while((workersCount!=0)||(listenerThreadRunning))
		Thread::Sleep(200);
	LOG(LogInfo)<<"Server stopped";
	terminated=false;
}

void Server::RegisterHandler(HttpHandler* handler)
{
	this->handler=handler;
}

void Server::RegisterDefaultErrorHandler(HttpHandler* handler)
{
	this->defaultErrorHandler=handler;
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

void Server::ServeFile(const std::string& fileName,HttpServerContext* context,bool download)
{
	long long size=FileUtils::GetFileSize(fileName);
	if(size==-1)
		throw HttpException(HttpNotFound,"Requested file not found.");
	time_t modifyTime=FileUtils::GetFileModifyTime(fileName);
	if((context->requestHeader.modifyTime!=0)&&(modifyTime<=context->requestHeader.modifyTime))
	{
		context->responseHeader.result=HttpNotModified;
		return;
	}
	context->responseHeader.modifyTime=modifyTime;
	File in(fileName,false);
	long long contentLength;
	if(context->requestHeader.rangeFrom==-1)
	{
		contentLength=size;
		context->responseHeader.contentLength=size;
	}
	else
	{
		if(context->requestHeader.rangeTo==-1)
			context->requestHeader.rangeTo=size-1;
		contentLength=context->requestHeader.rangeTo-context->requestHeader.rangeFrom+1;
		context->responseHeader.contentLength=contentLength;
		context->responseHeader.rangeFrom=context->requestHeader.rangeFrom;
		context->responseHeader.rangeTo=context->requestHeader.rangeTo;
		context->responseHeader.rangeTotal=size;
		in.Seek(context->requestHeader.rangeFrom);
	}
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
			context->responseHeader.contentType=Util::MimeType(ext);
		}
	}
	else
		context->responseHeader.contentType="application/octet-stream";
	context->SendResponseHeader();
	char buf[1024];
	while(contentLength>0)
	{
		int br;
		if(contentLength<sizeof(buf))
			br=static_cast<int>(contentLength);
		else
			br=sizeof(buf);
		in.Read(buf,br);
		context->Write(buf,br);
		contentLength-=br;
	}
}

void Server::StartWorkers(int numWorkers)
{
	for(int i=0;i<numWorkers;i++)
		Thread::StartThread(_WorkerThread,this);
}

void Server::StartSession(HttpSessionObject* sessionObject,HttpServerContext* context)
{
	MutexLock lock(sessionsMutex);
	time_t t;
	time(&t);
	gcCounter++;
	if(gcCounter>=gcPeriod)
	{
		gcCounter=0;
		queue<string> q;
		for(map<string,Session>::iterator iter=sessions.begin();iter!=sessions.end();iter++)
		{
			if(t>iter->second.whenToDelete)
			{
				delete iter->second.object;
				q.push(iter->first);
			}
		}
		while(!q.empty())
		{
			sessions.erase(q.front());
			q.pop();
		}
	}
	Session s;
	s.object=sessionObject;
	s.whenToDelete=t+gcMaxLifeTime;
	string token;
	for(;;)
	{
		token=Util::GenerateRandomString(64);
		if(sessions.find(token)==sessions.end())
			break;
	}
	sessions[token]=s;
	context->sessionObject=sessionObject;
	context->responseHeader.cookies["sessiontoken"].value=token;
	context->responseHeader.cookies["sessiontoken"].expireTime=0;
}

HttpSessionObject* Server::GetSessionObject(const std::string& token)
{
	MutexLock lock(sessionsMutex);
	if(sessions.find(token)==sessions.end())
		return NULL;
	time_t t;
	time(&t);
	sessions[token].whenToDelete=t+gcMaxLifeTime;
	return sessions[token].object;
}

}
