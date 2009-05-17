#include "Common.h"
#include "File.h"
#include "Util.h"

File::File(const string& fileName,bool write,bool binary):f(NULL)
{
#ifdef WIN32
	wstring t=write?L"w":L"r";
	t+=binary?L"b":L"t";
	f=_wfopen(Util::UTF8Decode(Util::AdjustPath(fileName)).c_str(),t.c_str());
#else
	string t=write?"w":"r";
	t+=binary?"b":"t";
	f=fopen(Util::AdjustPath(fileName).c_str(),t.c_str());
#endif
	if(f==NULL)
		throw runtime_error("Failed to open file");
}

File::~File()
{
	fclose(f);
}

size_t File::Read(char* buf,size_t len)
{
	size_t br=fread(buf,1,len,f);
	if(ferror(f)!=0)
		throw runtime_error("File read failed");
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
	return r;
}

bool File::Eof()
{
	return (linebuf.empty())&&(feof(f)!=0);
}

void File::Write(const char* buf,size_t len)
{
	if(fwrite(buf,len,1,f)==0)
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

void File::Seek(int offset)
{
	if(fseek(f,offset,SEEK_SET)!=0)
		throw runtime_error("File seek failed");
}

