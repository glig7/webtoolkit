/*
	This file is a part of madfish-webtoolkit project.
	http://code.google.com/p/madfish-webtoolkit/

	(c) 2009 Alexander "Mad Fish" Chehovsky
	See License.txt for licensing information.
*/

#include "Main.h"

FileServerConfig::FileServerConfig():ip("0.0.0.0"),port(8080),numWorkers(16),rootDir("."),htmlTemplate("list.html")
{
	File in("FileServer.conf",false);
	while(!in.Eof())
	{
		string line=in.ReadLine();
		if((!line.empty())&&(line[0]!='#'))
		{
			vector<string> elements=Util::Extract(line);
			if(elements[0]=="ListenIP")
			{
				ip=elements[1];
				continue;
			}
			if(elements[0]=="Port")
			{
				port=atoi(elements[1].c_str());
				continue;
			}
			if(elements[0]=="RootDirectory")
			{
				rootDir=elements[1];
				if((rootDir[rootDir.size()-1]=='/')||(rootDir[rootDir.size()-1]=='\\'))
					rootDir.resize(rootDir.size()-1);
				continue;
			}
			if(elements[0]=="HTMLTemplate")
			{
				htmlTemplate=elements[1];
				continue;
			}
			if(elements[0]=="FavIcon")
			{
				favIcon=elements[1];
				continue;
			}
			if(elements[0]=="BindDirectory")
			{
				bindWhat=elements[1];
				bindAs=elements[2];
				continue;
			}
			if(elements[0]=="LogLevel")
			{
				if(elements[1]=="Debug")
					Log::SetReportingLevel(LogDebug);
				if(elements[1]=="Verbose")
					Log::SetReportingLevel(LogVerbose);
				if(elements[1]=="Info")
					Log::SetReportingLevel(LogInfo);
				if(elements[1]=="Warning")
					Log::SetReportingLevel(LogWarning);
				if(elements[1]=="Error")
					Log::SetReportingLevel(LogError);
				if(elements[1]=="Disabled")
					Log::SetReportingLevel(LogDisabled);
				continue;
			}
			if(elements[0]=="NumWorkers")
			{
				numWorkers=atoi(elements[1].c_str());
				continue;
			}
		}
	}
}

FileServer::FileServer():server(config.port,config.ip,config.numWorkers)
{
	server.RegisterHandler(this);
	htmlTemplate=FileUtils::ReadFile(config.htmlTemplate);
	size_t folderStart=htmlTemplate.find("<%folder%>");
	size_t folderEnd=htmlTemplate.find("<%/folder%>");
	folderTemplate=htmlTemplate.substr(folderStart+10,folderEnd-folderStart-10);
	htmlTemplate.erase(folderStart,folderEnd-folderStart+11);
	size_t fileStart=htmlTemplate.find("<%file%>");
	size_t fileEnd=htmlTemplate.find("<%/file%>");
	fileTemplate=htmlTemplate.substr(fileStart+8,fileEnd-fileStart-8);
	htmlTemplate.erase(fileStart,fileEnd-fileStart+9);
}

void FileServer::Run()
{
	server.Run();
}

bool compare(const DirectoryEntry& e1,const DirectoryEntry& e2)
{
	if(e1.isDirectory&&!e2.isDirectory)
		return true;
	if(!e1.isDirectory&&e2.isDirectory)
		return false;
	return Util::StringToLower(e1.name)<Util::StringToLower(e2.name);
}

void FileServer::Handle(HttpServerContext* context)
{
	if(context->requestHeader.resource=="/favicon.ico")
	{
		time_t t;
		time(&t);
		t+=86400;
		context->responseHeader.expireTime=t;
		context->ServeFile(config.favIcon);
		return;
	}
	string urlPath=Util::URLDecode(context->requestHeader.resource);
	string path=urlPath;
	if(!FileUtils::PathValid(path))
		throw HttpException(HttpNotFound,"Not found.");
	if((!config.bindAs.empty())&&(path.length()>2))
	{
		int slashPos=path.find('/',1);
		if(slashPos!=string::npos)
		{
			string firstDir=path.substr(1,slashPos-1);
			if(firstDir==config.bindAs)
			{
				path=config.bindWhat+path.substr(slashPos);
				time_t t;
				time(&t);
				t+=86400;
				context->responseHeader.expireTime=t;
				context->ServeFile(path);
				return;
			}
		}
	}
	path=config.rootDir+path;
	bool trailingSlash=(context->requestHeader.resource[context->requestHeader.resource.length()-1]=='/');
	if((!path.empty())&&(urlPath!="/"))
	{
		string path2=path;
		if(trailingSlash)
			path2.resize(path2.size()-1);
		CheckPathResult r=FileUtils::CheckPath(path2);
		switch(r)
		{
		case PathNotExist:
			throw HttpException(HttpNotFound,"Not found.");
		case PathIsFile:
			if(trailingSlash)
			{
				context->Redirect(context->requestHeader.resource.substr(0,context->requestHeader.resource.length()-1));
				return;
			}
			context->ServeFile(path);
			return;
		default:
			if(!trailingSlash)
			{
				context->Redirect(context->requestHeader.resource+'/');
				return;
			}
		}
	}
	string responseBody=htmlTemplate;
	Util::Substitute(responseBody,"<%pwd%>",urlPath);
	ostringstream list;
	vector<DirectoryEntry> dl=FileUtils::DirectoryList(path);
	sort(dl.begin(),dl.end(),compare);
	for(vector<DirectoryEntry>::iterator i=dl.begin();i!=dl.end();i++)
	{
		string link=Util::URLEncode(i->name);
		string visibleName=Util::HTMLEscape(i->name);
		if(i->isDirectory)
		{
			string t=folderTemplate;
			Util::Substitute(t,"<%link%>",link+"/");
			Util::Substitute(t,"<%name%>",visibleName);
			list<<t;
		}
		else
		{
			string t=fileTemplate;
			Util::Substitute(t,"<%link%>",link);
			Util::Substitute(t,"<%name%>",visibleName);
			ostringstream s;
			if(i->size<1024)
				s<<i->size<<" B";
			else
			{
				s<<fixed<<setprecision(1);
				float t=i->size/1024.0f;
				if(t<1024)
					s<<t<<" KiB";
				else
				{
					t/=1024;
					if(t<1024)
						s<<t<<" MiB";
					else
					{
						t/=1024;
						s<<t<<" GiB";
					}
				}
			}
			Util::Substitute(t,"<%size%>",s.str());
			list<<t;
		}
	}
	Util::Substitute(responseBody,"<%list%>",list.str());
	context->responseBody<<responseBody;
}

int main()
{
	try
	{
		FileServer app;
		app.Run();
		Environment::WaitForTermination();
	}
	catch(exception& e)
	{
		LOG(LogError)<<e.what();
	}
}

