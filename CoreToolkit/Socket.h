/*
	This file is a part of madfish-webtoolkit project.
	http://code.google.com/p/madfish-webtoolkit/

	(c) 2009 Alexander "Mad Fish" Chehovsky
	See License.txt for licensing information.
*/

#pragma once
#ifndef _SOCKET_H
#define	_SOCKET_H

#include "Stream.h"

#include <string>

namespace CoreToolkit
{

class BaseSocket
{
protected:
	int sock;
public:
	std::string remoteIP;
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
	Listener(int portNumber,const std::string& ip);
	~Listener();
	Socket* Accept();
};

}

#endif

