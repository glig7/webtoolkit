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

ImageBoard::ImageBoard():server()
{
	dispatcher.AddMapping("/index",HttpGet,new HttpHandlerConnector<ImageBoard>(this,&ImageBoard::Index),true);
	dispatcher.AddMapping("/post",HttpPost,new HttpHandlerConnector<ImageBoard>(this,&ImageBoard::Post),true,NULL,false,new HttpFileHandlerConnector<ImageBoard>(this,&ImageBoard::HandleUpload),true);
	dispatcher.AddMapping("/redirect",HttpGet,new Redirector("/index"),true);
	dispatcher.SetDefaultHandler("/redirect");
	server.RegisterHandler(&dispatcher);
}

void ImageBoard::Run()
{
	server.Run();
}

void ImageBoard::Index(HttpServerContext* context)
{
	context->responseBody<<"<html><body><h1>ImageBoard</h1>";
	context->responseBody<<"<h3>Add new entry:</h3>";
	context->responseBody<<"<form enctype=\"multipart/form-data\" method=\"post\" action=\"/post\">";
	context->responseBody<<"<table>";
	context->responseBody<<"<tr><td>Author:</td><td><input type=\"text\" name=\"author\"></td></tr>";
	context->responseBody<<"<tr><td>Image:</td><td><INPUT type=\"file\" name=\"image\"></td>";
	context->responseBody<<"<tr><td>Text:</td><td><textarea name=\"text\"></textarea></td>";
	context->responseBody<<"<tr><td><input type=\"submit\" value=\"Submit\"></td></tr>";
	context->responseBody<<"</table>";
	context->responseBody<<"</form>";
	context->responseBody<<"</body></html>";
}

void ImageBoard::Post(HttpServerContext* context)
{
	LOG(LogInfo)<<"author: "<<context->parameters["author"];
	LOG(LogInfo)<<"text: "<<context->parameters["text"];
	context->Redirect("/index");
}

void ImageBoard::HandleUpload(WebToolkit::HttpServerContext* context,const std::string& name,const std::string& filename,CoreToolkit::InputStream* stream)
{
	LOG(LogInfo)<<"name: "<<name;
	LOG(LogInfo)<<"filename: "<<filename;
	File file(filename,true);
	while(!stream->Eof())
		file.Write(stream->ReadSome());
}

int main()
{
	try
	{
		ImageBoard app;
		app.Run();
		Environment::WaitForTermination();
	}
	catch(exception& e)
	{
		LOG(LogError)<<e.what();
	}
}
