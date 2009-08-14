/*
	This file is a part of madfish-webtoolkit project.
	http://code.google.com/p/madfish-webtoolkit/

	(c) 2009 Alexander "Mad Fish" Chehovsky
	See License.txt for licensing information.
*/

#include "Stream.h"

using namespace std;

namespace CoreToolkit
{

InputStream::InputStream():eof(false)
{
}

bool InputStream::WaitUnbuffered(int timeout)
{
	return true;
}

int InputStream::FillBuffer()
{
	if(eof)
		return 0;
	char b[1024];
	int br=ReadSomeUnbuffered(b,sizeof(b));
	if(br==0)
		eof=true;
	else
		buffer.append(b,br);
	return br;
}

void InputStream::FillBuffer(int len)
{
	while(buffer.length()<static_cast<size_t>(len))
	{
		int br=FillBuffer();
		if(br==0)
			throw IOException("Unexpected end of stream");
	}
}

bool InputStream::Eof()
{
	return (eof)&&(buffer.empty());
}

int InputStream::ReadSomeFromBuffer(void* buf,int len)
{
	if(buffer.empty())
		return 0;
	int l=min<int>(buffer.length(),len);
	memcpy(buf,buffer.c_str(),l);
	buffer.erase(0,l);
	return l;
}

string InputStream::ReadSome()
{
	if(buffer.empty())
		FillBuffer();
	string t=buffer;
	buffer.clear();
	return t;
}

int InputStream::ReadSome(void* buf,int len)
{
	if(buffer.empty())
		FillBuffer();
	if(buffer.empty())
		return 0;
	return ReadSomeFromBuffer(buf,len);
}

bool InputStream::Wait(int timeout)
{
	if(!buffer.empty())
		return true;
	return WaitUnbuffered(timeout);
}

std::string InputStream::Read(int len)
{
	FillBuffer(len);
	string st=buffer.substr(0,len);
	buffer.erase(0,len);
	return st;
}

void InputStream::Read(void* buf,int len)
{
	FillBuffer(len);
	memcpy(buf,buffer.c_str(),len);
	buffer.erase(0,len);
}

bool InputStream::WaitForLine(int timeout)
{
	int newline=buffer.find('\n');
	int pos=buffer.length();
	while(newline==string::npos)
	{
		if(!WaitUnbuffered(timeout))
			return false;
		int br=FillBuffer();
		if(br==0)
			break;
		newline=buffer.find('\n',pos);
		pos=buffer.length();
	}
	return true;
}

std::string InputStream::ReadLine()
{
	int newline=buffer.find('\n');
	int pos=buffer.length();
	while(newline==string::npos)
	{
		int br=FillBuffer();
		if(br==0)
			break;
		newline=buffer.find('\n',pos);
		pos=buffer.length();
	}
	string r;
	if(newline==string::npos)
	{
		r=buffer;
		buffer.clear();
	}
	else
	{
		r=buffer.substr(0,newline);
		buffer.erase(0,newline+1);
	}
	if((!r.empty())&&(r[r.length()-1]=='\r'))
		r.resize(r.length()-1);
	return r;
}

void OutputStream::Write(const void* buf,int len)
{
	const char* t=reinterpret_cast<const char*>(buf);
	int bytesWritten;
	while(len>0)
	{
		bytesWritten=WriteSome(t,len);
		t+=bytesWritten;
		len-=bytesWritten;
	}
}

void OutputStream::Write(const std::string& st)
{
	Write(st.c_str(),st.length());
}

void OutputStream::WriteLine(const std::string& st)
{
	Write(st);
	Write("\n");
}

Filter::Filter(InputStream* source):sourceStream(source)
{
}

bool Filter::Wait(int timeout)
{
	return sourceStream->Wait(timeout);
}

}
