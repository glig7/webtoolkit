#include "Main.h"

HelloWorld::HelloWorld():server(80,"0.0.0.0")
{
	server.RegisterHandler(this);
}

void HelloWorld::Run()
{
	server.Run();
}

void HelloWorld::Handle(HttpRequest* request,HttpResponse* response)
{
	response->Write("<html><body><h1>Hello, world!</h1></body></html>");
}

int main()
{
	try
	{
		HelloWorld app;
		app.Run();
	}
	catch(exception& e)
	{
		cout<<e.what()<<endl;
	}
}
