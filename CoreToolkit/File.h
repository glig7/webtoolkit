/*
	This file is a part of madfish-webtoolkit project.
	http://code.google.com/p/madfish-webtoolkit/

	(c) 2009 Alexander "Mad Fish" Chehovsky
	See License.txt for licensing information.
*/

#pragma once
#ifndef _FILE_H
#define	_FILE_H

#include "Stream.h"

#include <string>

#ifdef WIN32
#include <windows.h>
#else
#include <stdio.h>
#endif

namespace CoreToolkit
{

class File: public InputStream,public OutputStream,public SeekableStream
{
private:
#ifdef WIN32
	HANDLE f;
#else
	FILE* f;
#endif
public:
	File(const std::string& fileName,bool write);
	~File();
	int ReadSomeUnbuffered(void* buf,int len);
	int WriteSome(const void* buf,int len);
	void Seek(long long offset);
};

}

#endif
