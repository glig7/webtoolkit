/*
	This file is a part of madfish-webtoolkit project.
	http://code.google.com/p/madfish-webtoolkit/

	(c) 2009 Alexander "Mad Fish" Chehovsky
	See License.txt for licensing information.
*/

#pragma once
#ifndef _FILEUTILS_H
#define	_FILEUTILS_H

#include <string>
#include <vector>

namespace CoreToolkit
{

struct DirectoryEntry
{
	bool isDirectory;
	std::string name;
	long long size;
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
	static std::vector<DirectoryEntry> DirectoryList(const std::string& path);
	static std::string AdjustPath(const std::string& path);
	static CheckPathResult CheckPath(const std::string& st);
	static long long GetFileSize(const std::string& st);
	static time_t GetFileModifyTime(const std::string& st);
	static bool PathValid(const std::string& st);
	static std::string ReadFile(const std::string& st);
};

}

#endif

