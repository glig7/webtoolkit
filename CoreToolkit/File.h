/*
	This file is a part of madfish-webtoolkit project.
	http://code.google.com/p/madfish-webtoolkit/

	(c) 2009 Alexander "Mad Fish" Chehovsky
	See License.txt for licensing information.
*/

/**
	\file File.h
	\brief Definition of File class.
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

/**
	\brief File class.
	File is a stream. It implements InputStream, OutputStream, and SeekableStream interfaces.
	Supports Unicode and large file sizes.
*/
class File: public InputStream,public OutputStream,public SeekableStream
{
private:
#ifdef WIN32
	//! Win32 handle to the file.
	HANDLE f;
#else
	//! Standard file pointer.
	FILE* f;
#endif
public:
	/**
		Constructs new file object.
		\param[in] fileName Name of file to open in UTF-8 encoding.
		\param[in] write True for writing, false for reading.
	*/
	File(const std::string& fileName,bool write);
	~File();
	int ReadSomeUnbuffered(void* buf,int len);
	int WriteSome(const void* buf,int len);
	void Seek(long long offset);
};

}

#endif

