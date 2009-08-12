#include "Main.h"

SessionObject::SessionObject():counter(0)
{
}

SessionObject::~SessionObject()
{
}

SessionDemo::SessionDemo():server()
{
	server.RegisterHandler(this);
}

void SessionDemo::Run()
{
	server.Run();
}

void SessionDemo::Handle(HttpServerContext* context)
{
	if(context->requestHeader.resource!="/")
		throw HttpException(HttpNotFound,"Not Found!");
	if(context->sessionObject==NULL)
		context->StartSession(new SessionObject());
	SessionObject* sessionObject=static_cast<SessionObject*>(context->sessionObject);
	sessionObject->counter++;
	context->responseBody<<"<html><body><p>This page has been accessed ";
	context->responseBody<<sessionObject->counter;
	context->responseBody<<" times during this session.</p></body></html>";
}

int main()
{
	try
	{
		SessionDemo app;
		app.Run();
		Environment::WaitForTermination();
	}
	catch(exception& e)
	{
		LOG(LogError)<<e.what();
	}
}

