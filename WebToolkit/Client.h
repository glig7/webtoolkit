/*
	This file is a part of madfish-webtoolkit project.
	http://code.google.com/p/madfish-webtoolkit/

	(c) 2009 Alexander "Mad Fish" Chehovsky
	See License.txt for licensing information.
*/

#pragma once
#ifndef _CLIENT_H
#define	_CLIENT_H

namespace CoreToolkit
{
class Socket;
}
namespace WebToolkit
{

class Server;

class Client
{
private:
	Server* server;
	CoreToolkit::Socket* socket;
public:
	Client(Server* serv,CoreToolkit::Socket* sock);
	~Client();
	void Run();
};

}

#endif

