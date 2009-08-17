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

class ImageBoard
{
private:
	WebToolkit::Server server;
	WebToolkit::URIDispatcher dispatcher;
public:
	ImageBoard();
	void Run();
	void Index(WebToolkit::HttpServerContext* context);
	void Post(WebToolkit::HttpServerContext* context);
	void HandleUpload(WebToolkit::HttpServerContext* context,const std::string& name,const std::string& filename,CoreToolkit::InputStream* stream);
};

#endif

