#include "Common.h"
#include "Socket.h"

#include <memory.h>

#ifdef WIN32
#pragma comment(lib,"ws2_32")
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
	tv.tv_sec=0;
	tv.tv_usec=timeout*1000;
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

string Socket::Read()
{
	char buf[1024];
	int bytesRead=recv(sock,buf,sizeof(buf),0);
	if(bytesRead<0)
		throw runtime_error("Socket read error");
	return string(buf,bytesRead);
}

bool Socket::WaitForLine(int timeout)
{
	int newline=linebuf.find('\n');
	int pos=linebuf.length();
	while(newline==string::npos)
	{
		if(!Wait(timeout))
			return false;
		string t=Read();
		if(t.empty())
			break;
		linebuf+=t;
		newline=linebuf.find('\n',pos);
		pos=linebuf.length();
	}
	return true;
}

string Socket::ReadLine()
{
	int newline=linebuf.find('\n');
	int pos=linebuf.length();
	while(newline==string::npos)
	{
		string t=Read();
		if(t.empty())
			break;
		linebuf+=t;
		newline=linebuf.find('\n',pos);
		pos=linebuf.length();
	}
	string r;
	if(newline==string::npos)
	{
		r=linebuf;
		linebuf.clear();
	}
	else
	{
		r=linebuf.substr(0,newline);
		linebuf.erase(0,newline+1);
	}
	if((!r.empty())&&(r[r.length()-1]=='\r'))
		r.resize(r.length()-1);
	return r;
}

void Socket::Write(const char* buf,int len)
{
	int bytesWritten;
	while(len>0)
	{
		bytesWritten=send(sock,buf,len,0);
		if(bytesWritten<0)
			throw runtime_error("Socket write error");
		buf+=bytesWritten;
		len-=bytesWritten;
	}
}

void Socket::Write(const string& buf)
{
	Write(buf.c_str(),buf.length());
}

Listener::Listener(int portNumber,const string& ip)
{
	sock=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(sock<0)
		throw runtime_error("Failed to create socket");
	int val=1;
	setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&val,sizeof(val));
	sockaddr_in stSockAddr;
	memset(&stSockAddr,0,sizeof (stSockAddr));
	stSockAddr.sin_family=AF_INET;
	stSockAddr.sin_port=htons(portNumber);
	stSockAddr.sin_addr.s_addr=inet_addr(ip.c_str());
	if(bind(sock,(const sockaddr*)&stSockAddr,sizeof (stSockAddr))<0)
		throw runtime_error("Failed to bind socket");
	if(listen(sock,10)<0)
		throw runtime_error("Failed to start listening");
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
		throw runtime_error("accept() failed");
	Socket* s=new Socket(clientSock);
	s->remoteIP=inet_ntoa(((sockaddr_in*)&remoteaddr)->sin_addr);
	s->remotePort=((sockaddr_in*)&remoteaddr)->sin_port;
	return s;
}
