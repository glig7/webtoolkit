/*
	This file is a part of madfish-webtoolkit project.
	http://code.google.com/p/madfish-webtoolkit/

	(c) 2009 Alexander "Mad Fish" Chehovsky
	See License.txt for licensing information.
*/

#ifndef _CLIENT_H
#define	_CLIENT_H

class Server;
class Socket;

class Client
{
private:
	Server* server;
	Socket* socket;
public:
	Client(Server* serv,Socket* sock);
	~Client();
	void Run();
};

#endif

