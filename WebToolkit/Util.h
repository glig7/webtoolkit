#ifndef _UTIL_H
#define	_UTIL_H

struct DirectoryEntry
{
	bool isDirectory;
	string name;
	i64 size;
};

enum CheckPathResult
{
	PathNotExist,
	PathIsFile,
	PathIsDirectory
};

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

class Util
{
public:
	static vector<DirectoryEntry> DirectoryList(const string& path);
	static string AdjustPath(const string& path);
	static string HTMLEscape(const string& st);
	static string StringToLower(const string& st);
	static CheckPathResult CheckPath(const string& st);
	static i64 GetFileSize(const string& st);
	static string URLDecode(const string& st);
	static string URLEncode(const string& st);
	static bool PathValid(const string& st);
	static vector<string> Extract(const string& st);
	static string ReadFile(const string& st);
	static void Substitute(string& st,const string& what,const string& to);
	static string MakeHTTPTime(time_t t);
	static time_t ParseHTTPTime(const string& httpTime);
	static string UTF8Encode(const wstring& st);
	static wstring UTF8Decode(const string& st);
};

#endif

