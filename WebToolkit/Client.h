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

