#include "Main.h"

template<> SimpleBlog* Singleton<SimpleBlog>::instance=NULL;

void Index::Handle(HttpRequest* request,HttpResponse* response)
{
	if(request->isPost)
	{
		Server::Instance().HandleNotFound(response);
		return;
	}
	response->Write("<html><body><h1>SimpleBlog</h1><h3>Blog entries:</h3>");
	SimpleBlog::Instance().entriesMutex.Lock();
	for(size_t i=0;i<SimpleBlog::Instance().entries.size();i++)
	{
		response->Write("<div class=\"entry\">");
		response->Write("<div class=\"author\"><b>");
		response->Write(SimpleBlog::Instance().entries[i].author);
		response->Write(" wrote:");
		response->Write("</b></div>");
		response->Write("<div class=\"text\">");
		response->Write(SimpleBlog::Instance().entries[i].text);
		response->Write("</div>");
		response->Write("</div>");
	}
	SimpleBlog::Instance().entriesMutex.Unlock();
	response->Write("<h3>Add new entry:</h3>");
	response->Write("<form method=\"post\" action=\"/post\">");
	response->Write("<table>");
	response->Write("<tr><td>Author:</td><td><input type=\"text\" name=\"author\"></td></tr>");
	response->Write("<tr><td>Text:</td><td><textarea name=\"text\"></textarea></td>");
	response->Write("<tr><td><input type=\"submit\" value=\"Submit\"></td></tr>");
	response->Write("</table>");
	response->Write("</form>");
	response->Write("</body></html>");
}

void Post::Handle(HttpRequest* request,HttpResponse* response)
{
	if(!request->isPost)
	{
		Server::Instance().HandleNotFound(response);
		return;
	}
	SimpleBlog::Instance().entriesMutex.Lock();
	BlogEntry e;
	e.author=request->parameters["author"];
	e.text=request->parameters["text"];
	SimpleBlog::Instance().entries.push_back(e);
	SimpleBlog::Instance().entriesMutex.Unlock();
	response->Redirect("/index");
}

SimpleBlog::SimpleBlog():server(8080,"0.0.0.0")
{
	dispatcher.AddMapping("/index",&index);
	dispatcher.AddMapping("/post",&post);
	dispatcher.SetDefaultHandler(&indexRedirector);
	server.RegisterHandler(&dispatcher);
}

void SimpleBlog::Run()
{
	server.Run();
}

int main()
{
	try
	{
		SimpleBlog app;
		app.Run();
	}
	catch(exception& e)
	{
		cout<<e.what()<<endl;
	}
}
