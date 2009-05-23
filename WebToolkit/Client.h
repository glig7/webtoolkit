#ifndef _CLIENT_H
#define	_CLIENT_H

#include "Socket.h"
#include "Thread.h"
#include "Http.h"

class Client:public Thread,public HttpResponse
{
private:
	Socket* socket;
public:
	Client(Socket* socket);
	~Client();
	void Run();
	void DirectSend(const char* buf,int len);
	void Start();
};

#endif

