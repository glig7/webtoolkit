#ifndef _FILEUTILS_H
#define	_FILEUTILS_H

struct DirectoryEntry
{
	bool isDirectory;
	string name;
	i64 size;
	time_t modifyTime;
};

enum CheckPathResult
{
	PathNotExist,
	PathIsFile,
	PathIsDirectory
};

class FileUtils
{
public:
	static vector<DirectoryEntry> DirectoryList(const string& path);
	static string AdjustPath(const string& path);
	static CheckPathResult CheckPath(const string& st);
	static i64 GetFileSize(const string& st);
	static time_t GetFileModifyTime(const string& st);
	static bool PathValid(const string& st);
	static string ReadFile(const string& st);
};

#endif

