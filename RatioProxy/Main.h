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

enum ValueSource
{
	Download,
	Upload
};

class RatioProxyConfig
{
public:
	std::string ip;
	int port;
	int numWorkers;
	ValueSource downloadValueSource;
	ValueSource uploadValueSource;
	float downloadMultiplier;
	float uploadMultiplier;
	RatioProxyConfig();
};

class RatioProxy:public WebToolkit::HttpHandler
{
private:
	RatioProxyConfig config;
	WebToolkit::Server server;
public:
	RatioProxy();
	void Run();
	void Handle(WebToolkit::HttpServerContext* context);
};

#endif

