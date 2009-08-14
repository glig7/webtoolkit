/*
	This file is a part of madfish-webtoolkit project.
	http://code.google.com/p/madfish-webtoolkit/

	(c) 2009 Alexander "Mad Fish" Chehovsky
	See License.txt for licensing information.
*/

#pragma once
#ifndef _STREAM_H
#define	_STREAM_H

#include <string>
#include <stdexcept>

namespace CoreToolkit
{

class IOException:public std::runtime_error
{
public:
	IOException(const std::string& st):std::runtime_error(st)
	{
	}
};

class InputStream
{
private:
	int FillBuffer();
	void FillBuffer(int len);
protected:
	bool eof; //Means end of data in the source, but NOT in the buffer.
	std::string buffer;
	int ReadSomeFromBuffer(void* buf,int len);
	virtual int ReadSomeUnbuffered(void* buf,int len)=0;
	virtual bool WaitUnbuffered(int timeout);
public:
	InputStream();
	//Wait until input is available. Returns true if the stream is ready for reading.
	//You can specify 0 timeout to probe stream for data.
	//
	//Success (true returned) means that you can read some data with ReadSome() and it won't block.
	bool Wait(int timeout);
	//Check for end of stream state.
	//No data from the source is available, and the buffer is empty.
	bool Eof();
	//Read some input.
	std::string ReadSome();
	//Read some input.
	int ReadSome(void* buf,int len);
	//Read specific number of bytes.
	std::string Read(int len);
	//Read specific number of bytes. Adapted for reading binary objects.
	void Read(void* buf,int len);
	//Wait until line can be read.
	bool WaitForLine(int timeout);
	//Read line from the stream. Line ends with '\n'. If '\r' is also present, it is removed.
	std::string ReadLine();
};

class OutputStream
{
public:
	//Write some output.
	//You should override this method in derived classes.
	//Return value is the actual number of bytes written.
	virtual int WriteSome(const void* buf,int len)=0;
	//Write data to the stream.
	void Write(const std::string& st);
	//Write data to the stream. Adapted for writing binary objects.
	void Write(const void* buf,int len);
	//Write line of text to the stream. Differs from Write(string) only by adding '\n' after writing.
	void WriteLine(const std::string& st);
};

class SeekableStream
{
public:
	//Seek to the specified position in the stream.
	//Don't forget to clean the buffer.
	virtual void Seek(long long offset)=0;
};

class Filter:public InputStream
{
protected:
	InputStream* sourceStream;
public:
	Filter(InputStream* source);
	bool Wait(int timeout);
};

}

#endif

