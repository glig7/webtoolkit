/*
	This file is a part of madfish-webtoolkit project.
	http://code.google.com/p/madfish-webtoolkit/

	(c) 2009 Alexander "Mad Fish" Chehovsky
	See License.txt for licensing information.
*/

#include "Common.h"
#include "Socket.h"

#include <memory.h>

#ifdef WIN32
#include <Ws2tcpip.h>

struct WSInit
{
public:

	WSInit()
	{
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2,2),&wsaData);
	}

	~WSInit()
	{
		WSACleanup();
	}
} wsinit;
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#endif

BaseSocket::BaseSocket():sock(-1)
{
}

BaseSocket::BaseSocket(int sock)
{
	this->sock=sock;
}

BaseSocket::~BaseSocket()
{
	if(sock>=0)
#ifdef WIN32
		closesocket(sock);
#else
		close(sock);
#endif
}

bool BaseSocket::Wait(int timeout)
{
	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(sock,&readfds);
	timeval tv;
	tv.tv_sec=timeout/1000;
	tv.tv_usec=(timeout%1000)*1000;
	select(sock+1,&readfds,NULL,NULL,&tv);
	return FD_ISSET(sock,&readfds)!=0;
}

Socket::Socket(int sock)
{
	this->sock=sock;
}

Socket::~Socket()
{
}

int Socket::ReadSomeUnbuffered(void* buf,int len)
{
	int br=recv(sock,reinterpret_cast<char*>(buf),len,0);
	if(br<0)
		throw IOException("Socket read error");
	return br;
}

bool Socket::WaitUnbuffered(int timeout)
{
	return BaseSocket::Wait(timeout);
}

int Socket::WriteSome(const void* buf,int len)
{
	int bw=send(sock,reinterpret_cast<const char*>(buf),len,0);
	if(bw<0)
		throw IOException("Socket write error");
	return bw;
}

Listener::Listener(int portNumber,const string& ip)
{
	sock=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(sock<0)
		throw IOException("Failed to create socket");
	int val=1;
	setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,reinterpret_cast<const char*>(&val),sizeof(val));
	sockaddr_in stSockAddr;
	memset(&stSockAddr,0,sizeof (stSockAddr));
	stSockAddr.sin_family=AF_INET;
	stSockAddr.sin_port=htons(portNumber);
	stSockAddr.sin_addr.s_addr=inet_addr(ip.c_str());
	if(bind(sock,(const sockaddr*)&stSockAddr,sizeof (stSockAddr))<0)
		throw IOException("Failed to bind socket");
	if(listen(sock,100)<0)
		throw IOException("Failed to start listening");
}

Listener::~Listener()
{
}

Socket* Listener::Accept()
{
	sockaddr_storage remoteaddr;
	socklen_t addrlen=sizeof (remoteaddr);
	int clientSock=accept(sock,(sockaddr*)&remoteaddr,&addrlen);
	if(clientSock<0)
		throw IOException("accept() failed");
	Socket* s=new Socket(clientSock);
	s->remoteIP=inet_ntoa(((sockaddr_in*)&remoteaddr)->sin_addr);
	s->remotePort=((sockaddr_in*)&remoteaddr)->sin_port;
	return s;
}

