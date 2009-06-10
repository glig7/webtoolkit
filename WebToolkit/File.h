#ifndef _FILE_H
#define	_FILE_H

#ifdef WIN32
#include <windows.h>
#else
#include <stdio.h>
#endif

class File
{
private:
#ifdef WIN32
	HANDLE f;
#else
	FILE* f;
#endif
	bool eof;
	string linebuf;
public:
	File(const string& fileName,bool write);
	~File();
	size_t Read(char* buf,size_t len);
	string ReadSome();
	string ReadLine();
	bool Eof();
	void Write(const char* buf,size_t len);
	void Write(const string& st);
	void WriteLine(const string& st);
	void Seek(i64 offset);
};

#endif

