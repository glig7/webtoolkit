/*
	This file is a part of madfish-webtoolkit project.
	http://code.google.com/p/madfish-webtoolkit/

	(c) 2009 Alexander "Mad Fish" Chehovsky
	See License.txt for licensing information.
*/

#include "Main.h"

using namespace std;
using namespace CoreToolkit;
using namespace WebToolkit;

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
