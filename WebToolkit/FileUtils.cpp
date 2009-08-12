#include "Common.h"
#include "FileUtils.h"
#include "Util.h"
#include "File.h"

#include <sys/types.h>
#include <sys/stat.h>

#ifdef WIN32
#include <windows.h>
#else
#include <dirent.h>
#endif

class Buffer
{
public:
	char* buf;
	Buffer(int len)
	{
		buf=new char[len];
	}
	~Buffer()
	{
		delete[] buf;
	}
};

#ifdef WIN32
time_t WindowsFileTimeToUnixTime(FILETIME* filetime)
{
	i64 i=(*(reinterpret_cast<i64*>(filetime)));
	i-=116444736000000000;
	i/=10000000;
	return static_cast<time_t>(i);
}
#endif

vector<DirectoryEntry> FileUtils::DirectoryList(const string& path)
{
	string adjPath=AdjustPath(path);
	vector<DirectoryEntry> r;
#ifdef WIN32
	wstring wadjPath=Util::UTF8Decode(adjPath);
	WIN32_FIND_DATAW findData;
	HANDLE hFind=FindFirstFileW((wadjPath+L"\\*.*").c_str(),&findData);
	if(hFind==INVALID_HANDLE_VALUE)
		throw runtime_error("Failed to get directory list");
	for(;;)
	{
		if(findData.cFileName[0]!=L'.')
		{
			DirectoryEntry e;
			e.name=Util::UTF8Encode(findData.cFileName);
			e.isDirectory=(findData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0;
			if(e.isDirectory)
			{
				e.size=0;
				e.modifyTime=0;
			}
			else
			{
				LARGE_INTEGER i;
				i.LowPart=findData.nFileSizeLow;
				i.HighPart=findData.nFileSizeHigh;
				e.size=i.QuadPart;
				e.modifyTime=WindowsFileTimeToUnixTime(&findData.ftLastWriteTime);
			}
			r.push_back(e);
		}
		if(FindNextFileW(hFind,&findData)==0)
			break;
	}
	FindClose(hFind);
#else
	DIR* dir=opendir(adjPath.c_str());
	if(dir==NULL)
		throw runtime_error("Failed to get directory list");
	dirent* entry;
	while((entry=readdir(dir))!=NULL)
	{
		if(entry->d_name[0]!='.')
		{
			DirectoryEntry e;
			e.name=entry->d_name;
			struct stat s;
			stat((adjPath+"/"+e.name).c_str(),&s);
			e.isDirectory=((s.st_mode&S_IFDIR)!=0);
			if(e.isDirectory)
			{
				e.size=0;
				e.modifyTime=0;
			}
			else
			{
				e.size=s.st_size;
				e.modifyTime=s.st_mtime;
			}
			r.push_back(e);
		}
	}
	closedir(dir);
#endif
	return r;
}

string FileUtils::AdjustPath(const string& path)
{
#ifdef WIN32
	ostringstream r;
	for(size_t i=0;i<path.length();i++)
	{
		switch(path[i])
		{
		case '/':
			r<<'\\';
			break;
		default:
			r<<path[i];
		}
	}
	return r.str();
#else
	return path;
#endif
}

CheckPathResult FileUtils::CheckPath(const string& st)
{
#ifdef WIN32
	DWORD fileAttr=GetFileAttributesW(Util::UTF8Decode(AdjustPath(st)).c_str());
	if(fileAttr==INVALID_FILE_ATTRIBUTES)
		return PathNotExist;
	if((fileAttr&FILE_ATTRIBUTE_DIRECTORY)!=0)
		return PathIsDirectory;
	else
		return PathIsFile;
#else
	struct stat s;
	if(stat(AdjustPath(st).c_str(),&s)!=0)
		return PathNotExist;
	if((s.st_mode&S_IFDIR)!=0)
		return PathIsDirectory;
	else
		return PathIsFile;
#endif
}

i64 FileUtils::GetFileSize(const string& st)
{
#ifdef WIN32
	WIN32_FIND_DATAW findData;
	HANDLE hFind=FindFirstFileW(Util::UTF8Decode(AdjustPath(st)).c_str(),&findData);
	if(hFind==INVALID_HANDLE_VALUE)
		return -1;
	FindClose(hFind);
	LARGE_INTEGER i;
	i.LowPart=findData.nFileSizeLow;
	i.HighPart=findData.nFileSizeHigh;
	return i.QuadPart;
#else
	struct stat s;
	if(stat(AdjustPath(st).c_str(),&s)!=0)
		return -1;
	return s.st_size;
#endif
}

time_t FileUtils::GetFileModifyTime(const string& st)
{
#ifdef WIN32
	WIN32_FIND_DATAW findData;
	HANDLE hFind=FindFirstFileW(Util::UTF8Decode(AdjustPath(st)).c_str(),&findData);
	if(hFind==INVALID_HANDLE_VALUE)
		return -1;
	FindClose(hFind);
	return WindowsFileTimeToUnixTime(&findData.ftLastWriteTime);
#else
	struct stat s;
	if(stat(AdjustPath(st).c_str(),&s)!=0)
		return -1;
	return s.st_mtime;
#endif
}

bool FileUtils::PathValid(const string& st)
{
	if(st.empty()||(st[0]!='/'))
		return false;
	for(size_t i=0;i<st.length();i++)
	{
		if(st[i]=='\\')
			return false;
	}
	size_t p=st.find("/..");
	if(p!=string::npos)
		return false;
	return true;
}

string FileUtils::ReadFile(const string& st)
{
	i64 size64=GetFileSize(st);
	if(size64==-1)
		throw runtime_error("File not found");
	if(size64>10*1024*1024)
		throw runtime_error("Don't use Util::ReadFile for huge files");
	int size=static_cast<int>(size64);
	Buffer buf(size);
	File file(st,false);
	file.Read(buf.buf,size);
	string r(buf.buf,size);
	return r;
}
