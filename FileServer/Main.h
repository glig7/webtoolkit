/*
	This file is a part of madfish-webtoolkit project.
	http://code.google.com/p/madfish-webtoolkit/

	(c) 2009 Alexander "Mad Fish" Chehovsky
	See License.txt for licensing information.
*/

#pragma once
#ifndef _MAIN_H
#define	_MAIN_H

#include <WebToolkit.h>

#include <string>

class FileServerConfig
{
public:
	std::string ip;
	int port;
	std::string rootDir;
	std::string htmlTemplate;
	std::string favIcon;
	std::string bindWhat;
	std::string bindAs;
	int numWorkers;
	FileServerConfig();
};

class FileServer:public WebToolkit::HttpHandler
{
private:
	FileServerConfig config;
	WebToolkit::Server server;
	std::string htmlTemplate;
	std::string folderTemplate;
	std::string fileTemplate;
public:
	FileServer();
	void Run();
	void Handle(WebToolkit::HttpServerContext* context);
};

#endif

