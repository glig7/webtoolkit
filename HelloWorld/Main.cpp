#include "Main.h"

HelloWorld::HelloWorld():server()
{
	server.RegisterHandler(this);
}

void HelloWorld::Run()
{
	server.Run();
}

void HelloWorld::Handle(HttpServerContext* context)
{
	context->responseBody<<"<html><body><h1>Hello, world!</h1></body></html>";
}

int main()
{
	try
	{
		HelloWorld app;
		app.Run();
		Environment::WaitForTermination();
	}
	catch(exception& e)
	{
		LOG(LogError)<<e.what();
	}
}

