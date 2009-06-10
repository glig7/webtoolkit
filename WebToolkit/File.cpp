#include "Common.h"
#include "File.h"
#include "Util.h"

File::File(const string& fileName,bool write):f(NULL),eof(false)
{
#ifdef WIN32
	DWORD dwDesiredAccess=write?GENERIC_WRITE:GENERIC_READ;
	DWORD dwShareMode=write?0:FILE_SHARE_READ;
	DWORD dwCreationDisposition=write?CREATE_ALWAYS:OPEN_EXISTING;
	f=CreateFileW(Util::UTF8Decode(Util::AdjustPath(fileName)).c_str(),dwDesiredAccess,dwShareMode,NULL,dwCreationDisposition,FILE_FLAG_SEQUENTIAL_SCAN,NULL);
	if(f==INVALID_HANDLE_VALUE)
		throw runtime_error("Failed to open file "+fileName);
#else
	string t=write?"w":"r";
	f=fopen(Util::AdjustPath(fileName).c_str(),"b");
	if(f==NULL)
		throw runtime_error("Failed to open file "+fileName);
#endif
}

File::~File()
{
#ifdef WIN32
	CloseHandle(f);
#else
	fclose(f);
#endif
}

size_t File::Read(char* buf,size_t len)
{
#ifdef WIN32
	size_t br;
	if(ReadFile(f,buf,len,reinterpret_cast<LPDWORD>(&br),NULL)==0)
		throw runtime_error("File read failed");
#else
	size_t br=fread(buf,1,len,f);
	if(ferror(f)!=0)
		throw runtime_error("File read failed");
#endif
	if(br<len)
		eof=true;
	return br;
}

string File::ReadSome()
{
	char buf[256];
	size_t br=Read(buf,sizeof(buf));
	return string(buf,br);
}

string File::ReadLine()
{
	int newline=linebuf.find('\n');
	int pos=linebuf.length();
	while(newline==string::npos)
	{
		string t=ReadSome();
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

bool File::Eof()
{
	return (linebuf.empty())&&(eof);
}

void File::Write(const char* buf,size_t len)
{
#ifdef WIN32
	if(WriteFile(f,buf,len,NULL,NULL)==0)
#else
	if(fwrite(buf,len,1,f)==0)
#endif
		throw runtime_error("File write failed");
}

void File::Write(const string& st)
{
	Write(st.c_str(),st.length());
}

void File::WriteLine(const string& st)
{
	Write(st);
	Write("\n");
}

void File::Seek(i64 offset)
{
#ifdef WIN32
	LARGE_INTEGER o;
	o.QuadPart=offset;
	if(SetFilePointerEx(f,o,NULL,FILE_BEGIN)!=0)
#else
	if(fseeko(f,offset,SEEK_SET)!=0)
#endif
		throw runtime_error("File seek failed");
}

