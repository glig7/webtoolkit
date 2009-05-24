#ifndef _SOCKET_H
#define	_SOCKET_H

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

class Socket:public BaseSocket
{
private:
	string linebuf;
public:
	Socket(int sock);
	~Socket();
	string Read();
	bool WaitForLine(int timeout);
	string ReadLine();
	void Write(const char* buf,int len);
	void Write(const string& buf);
};

class Listener:public BaseSocket
{
public:
	Listener(int portNumber);
	~Listener();
	Socket* Accept();
};

#endif

