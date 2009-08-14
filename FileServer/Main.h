/*
	This file is a part of madfish-webtoolkit project.
	http://code.google.com/p/madfish-webtoolkit/

	(c) 2009 Alexander "Mad Fish" Chehovsky
	See License.txt for licensing information.
*/

#ifndef _MAIN_H
#define	_MAIN_H

#include <WebToolkit.h>

class FileServerConfig
{
public:
	string ip;
	int port;
	string rootDir;
	string htmlTemplate;
	string favIcon;
	string bindWhat;
	string bindAs;
	int numWorkers;
	FileServerConfig();
};

class FileServer:public IHttpHandler
{
private:
	FileServerConfig config;
	Server server;
	string htmlTemplate;
	string folderTemplate;
	string fileTemplate;
public:
	FileServer();
	void Run();
	void Handle(HttpServerContext* context);
};

#endif

