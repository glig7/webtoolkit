/*
	This file is a part of madfish-webtoolkit project.
	http://code.google.com/p/madfish-webtoolkit/

	(c) 2009 Alexander "Mad Fish" Chehovsky
	See License.txt for licensing information.
*/

/**
	\file FileUtils.h
	\brief Declaration of various filesystem-related helper functions.
*/

#pragma once
#ifndef _FILEUTILS_H
#define	_FILEUTILS_H

#include <string>
#include <vector>

namespace CoreToolkit
{

//! Describes a single directory entry, as returned by FileUtils::DirectoryList()
struct DirectoryEntry
{
	//! True for directory, false for file.
	bool isDirectory;
	//! Name of directory entry.
	std::string name;
	//! Size of file. 64bit value.
	long long size;
	//! Time of file's last modification.
	time_t modifyTime;
};

//! Describes the result of path checking.
enum CheckPathResult
{
	PathNotExist, //!< Specified path doesn't exist.
	PathIsFile, //!< Specified path is a file.
	PathIsDirectory //!< Specified path is a directory.
};

/**
	\brief Placeholder for file helper functions.
*/
class FileUtils
{
public:
	//! Get the directory contents.
	static std::vector<DirectoryEntry> DirectoryList(const std::string& path);
	//! Make the path OS-friendly. (convert path separator if needed)
	static std::string AdjustPath(const std::string& path);
	//! Determine what the path is.
	static CheckPathResult CheckPath(const std::string& st);
	//! Get file size.
	static long long GetFileSize(const std::string& st);
	//! Get file's last modification time.
	static time_t GetFileModifyTime(const std::string& st);
	//! Check path for validity, e.g. no /.. sequences in the path.
	static bool PathValid(const std::string& st);
	//! Read the file completely, and return it's contents as string.
	static std::string ReadFile(const std::string& st);
};

}

#endif

