#ifndef _FILE_H
#define	_FILE_H

#include <stdio.h>

class File
{
private:
	FILE* f;
	string linebuf;
public:
	File(const string& fileName,bool write,bool binary);
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

