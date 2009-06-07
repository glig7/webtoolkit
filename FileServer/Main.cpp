#include "Main.h"

FileServerConfig::FileServerConfig():ip("0.0.0.0"),port(8080),rootDir("."),htmlTemplate("list.html")
{
	File in("FileServer.conf",false,false);
	while(!in.Eof())
	{
		string line=in.ReadLine();
		if((!line.empty())&&(line[0]!='#'))
		{
			vector<string> elements=Util::Extract(line);
			if(elements[0]=="IP")
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
		}
	}
}

FileServer::FileServer():server(config.port,config.ip)
{
	server.RegisterHandler(this);
	server.RegisterNotFoundHandler(this);
	htmlTemplate=Util::ReadFile(config.htmlTemplate);
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

void FileServer::HandleNotFound(HttpResponse* response)
{
	response->Write("<html><body><h1>404 Not Found</h1><p>Sorry!</p></body></html>");
}

bool compare(const DirectoryEntry& e1,const DirectoryEntry& e2)
{
	if(e1.isDirectory&&!e2.isDirectory)
		return true;
	if(!e1.isDirectory&&e2.isDirectory)
		return false;
	return Util::StringToLower(e1.name)<Util::StringToLower(e2.name);
}

void FileServer::Handle(HttpRequest* request,HttpResponse* response)
{
	if(request->resource=="/favicon.ico")
	{
		Server::Instance().ServeFile(config.favIcon,request,response);
		return;
	}
	string urlPath=Util::URLDecode(request->resource);
	string path=urlPath;
	if(!Util::PathValid(path))
	{
		Server::Instance().HandleNotFound(response);
		return;
	}
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
				response->SetExpires(t);
				Server::Instance().ServeFile(path,request,response);
				return;
			}
		}
	}
	path=config.rootDir+path;
	bool trailingSlash=(request->resource[request->resource.length()-1]=='/');
	if((!path.empty())&&(path!="/"))
	{
		string path2=path;
		if(trailingSlash)
			path2.resize(path2.size()-1);
		CheckPathResult r=Util::CheckPath(path2);
		switch(r)
		{
		case PathNotExist:
			Server::Instance().HandleNotFound(response);
			return;
		case PathIsFile:
			if(trailingSlash)
			{
				response->Redirect(request->resource.substr(0,request->resource.length()-1));
				return;
			}
			Server::Instance().ServeFile(path,request,response);
			return;
		default:
			if(!trailingSlash)
			{
				response->Redirect(request->resource+'/');
				return;
			}
		}
	}
	string responseBody=htmlTemplate;
	Util::Substitute(responseBody,"<%pwd%>",urlPath);
	ostringstream list;
	vector<DirectoryEntry> dl=Util::DirectoryList(path);
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
			s<<i->size;
			Util::Substitute(t,"<%size%>",s.str());
			list<<t;
		}
	}
	Util::Substitute(responseBody,"<%list%>",list.str());
	response->Write(responseBody);
}

int main()
{
	try
	{
		FileServer app;
		app.Run();
	}
	catch(exception& e)
	{
		cout<<e.what()<<endl;
	}
}
