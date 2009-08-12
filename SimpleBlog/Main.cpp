#include "Main.h"

SimpleBlog::SimpleBlog():server(8080,"0.0.0.0")
{
	dispatcher.AddMapping("/index",HttpGet,new HttpHandlerConnector<SimpleBlog>(this,&SimpleBlog::Index),true);
	dispatcher.AddMapping("/post",HttpPost,new HttpHandlerConnector<SimpleBlog>(this,&SimpleBlog::Post),true);
	dispatcher.AddMapping("/redirect",HttpGet,new Redirector("/index"),true);
	dispatcher.SetDefaultHandler("/redirect");
	server.RegisterHandler(&dispatcher);
}

void SimpleBlog::Run()
{
	server.Run();
}

void SimpleBlog::Index(HttpServerContext* context)
{
	context->responseBody<<"<html><body><h1>SimpleBlog</h1><h3>Blog entries:</h3>";
	{
		MutexLock lock(entriesMutex);
		for(size_t i=0;i<entries.size();i++)
		{
			context->responseBody<<"<div class=\"entry\">";
			context->responseBody<<"<div class=\"author\"><b>";
			context->responseBody<<entries[i].author;
			context->responseBody<<" wrote:";
			context->responseBody<<"</b></div>";
			context->responseBody<<"<div class=\"text\">";
			context->responseBody<<entries[i].text;
			context->responseBody<<"</div></div>";
		}
	}
	context->responseBody<<"<h3>Add new entry:</h3>";
	context->responseBody<<"<form method=\"post\" action=\"/post\">";
	context->responseBody<<"<table>";
	context->responseBody<<"<tr><td>Author:</td><td><input type=\"text\" name=\"author\"></td></tr>";
	context->responseBody<<"<tr><td>Text:</td><td><textarea name=\"text\"></textarea></td>";
	context->responseBody<<"<tr><td><input type=\"submit\" value=\"Submit\"></td></tr>";
	context->responseBody<<"</table>";
	context->responseBody<<"</form>";
	context->responseBody<<"</body></html>";
}

void SimpleBlog::Post(HttpServerContext* context)
{
	MutexLock lock(entriesMutex);
	BlogEntry e;
	e.author=context->parameters["author"];
	e.text=context->parameters["text"];
	entries.push_back(e);
	context->Redirect("/index");
}

int main()
{
	try
	{
		SimpleBlog app;
		app.Run();
		Environment::WaitForTermination();
	}
	catch(exception& e)
	{
		LOG(LogError)<<e.what();
	}
}
