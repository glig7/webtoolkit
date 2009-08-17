/*
	This file is a part of madfish-webtoolkit project.
	http://code.google.com/p/madfish-webtoolkit/

	(c) 2009 Alexander "Mad Fish" Chehovsky
	See License.txt for licensing information.
*/

/**
	\file File.cpp
	\brief Implementation of File class.
*/

#include "File.h"
#include "Util.h"
#include "FileUtils.h"

using namespace std;

namespace CoreToolkit
{

File::File(const std::string& fileName,bool write):f(NULL)
{
#ifdef WIN32
	DWORD dwDesiredAccess=write?GENERIC_WRITE:GENERIC_READ;
	DWORD dwShareMode=write?0:FILE_SHARE_READ;
	DWORD dwCreationDisposition=write?CREATE_ALWAYS:OPEN_EXISTING;
	f=CreateFileW(Util::UTF8Decode(FileUtils::AdjustPath(fileName)).c_str(),dwDesiredAccess,dwShareMode,NULL,dwCreationDisposition,FILE_FLAG_SEQUENTIAL_SCAN,NULL);
	if(f==INVALID_HANDLE_VALUE)
#else
	string t=write?"w":"r";
	t+="b";
	f=fopen(FileUtils::AdjustPath(fileName).c_str(),t.c_str());
	if(f==NULL)
#endif
		throw IOException("Failed to open file "+fileName);
}

File::~File()
{
#ifdef WIN32
	CloseHandle(f);
#else
	fclose(f);
#endif
}

int File::ReadSomeUnbuffered(void* buf,int len)
{
#ifdef WIN32
	int br;
	if(ReadFile(f,buf,len,reinterpret_cast<LPDWORD>(&br),NULL)==0)
#else
	int br=fread(buf,1,len,f);
	if(ferror(f)!=0)
#endif
		throw IOException("File read failed");
	if(br<len)
		eof=true;
	return br;
}

int File::WriteSome(const void* buf,int len)
{
#ifdef WIN32
	DWORD bw;
	if(WriteFile(f,buf,len,&bw,NULL)==0)
#else
	if(fwrite(buf,len,1,f)==0)
#endif
		throw IOException("File write failed");
	return len;
}

void File::Seek(long long offset)
{
#ifdef WIN32
	LARGE_INTEGER o;
	o.QuadPart=offset;
	if(SetFilePointerEx(f,o,NULL,FILE_BEGIN)!=0)
#else
	if(fseeko(f,offset,SEEK_SET)!=0)
#endif
		throw IOException("File seek failed");
	buffer.clear();
}

}
