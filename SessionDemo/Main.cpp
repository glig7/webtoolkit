#include "Main.h"

SessionObject::SessionObject():counter(0)
{
}

SessionObject::~SessionObject()
{
}

SessionDemo::SessionDemo():server(8080,"0.0.0.0")
{
	server.RegisterHandler(this);
}

void SessionDemo::Run()
{
	server.Run();
}

void SessionDemo::Handle(HttpRequest* request,HttpResponse* response)
{
	if(request->resource!="/")
	{
		Server::Instance().HandleNotFound(response);
		return;
	}
	if(request->sessionObject==NULL)
		Server::Instance().StartSession(new SessionObject(),request,response);
	SessionObject* sessionObject=static_cast<SessionObject*>(request->sessionObject);
	sessionObject->counter++;
	ostringstream r;
	r<<"<html><body><p>This page has been accessed ";
	r<<sessionObject->counter;
	r<<" times during this session.</p></body></html>";
	response->Write(r.str());
}

int main()
{
	try
	{
		SessionDemo app;
		app.Run();
	}
	catch(exception& e)
	{
		cout<<e.what()<<endl;
	}
}
