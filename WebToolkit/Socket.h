#ifndef _SOCKET_H
#define	_SOCKET_H

#include "Stream.h"

class BaseSocket
{
protected:
	int sock;
public:
	string remoteIP;
	int remotePort;
	BaseSocket();
	BaseSocket(int socket);
	~BaseSocket();
	bool Wait(int timeout);
};

class Socket:public BaseSocket,public InputStream,public OutputStream
{
public:
	Socket(int sock);
	~Socket();
	int ReadSomeUnbuffered(void* buf,int len);
	bool WaitUnbuffered(int timeout);
	int WriteSome(const void* buf,int len);
	using InputStream::Wait;
};

class Listener:public BaseSocket
{
public:
	Listener(int portNumber,const string& ip);
	~Listener();
	Socket* Accept();
};

#endif

