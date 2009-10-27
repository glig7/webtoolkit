/*
	This file is a part of madfish-webtoolkit project.
	http://code.google.com/p/madfish-webtoolkit/

	(c) 2009 Alexander "Mad Fish" Chehovsky
	See License.txt for licensing information.
*/

#pragma once
#ifndef _UTIL_H
#define	_UTIL_H

#include <sstream>
#include <string>
#include <vector>

#ifdef __UCLIBC__
namespace std
{
typedef basic_string<wchar_t> wstring;
}
#endif

namespace CoreToolkit
{

class Util
{
public:
	static std::string HTMLEscape(const std::string& st);
	static std::string StringToLower(const std::string& st);
	static std::string URLDecode(const std::string& st);
	static std::string URLEncode(const std::string& st);
	static std::vector<std::string> Extract(const std::string& st);
	static void Substitute(std::string& st,const std::string& what,const std::string& to);
	static std::string MakeHTTPTime(time_t t);
	static time_t ParseHTTPTime(const std::string& httpTime);
	static std::string UTF8Encode(const std::wstring& st);
	static std::wstring UTF8Decode(const std::string& st);
	static void Trim(std::string& st);
	static std::string GenerateRandomString(int len);
	static std::string Timestamp(time_t t);
	static std::string MimeType(const std::string& ext);
	template<class T>
	static std::string NumToString(T t)
	{
		std::ostringstream os;
		os<<t;
		return os.str();
	}
	static std::string ToHumanReadableSize(long long s);
	static std::vector<std::string> DecodeChunks(const std::string& st);
};

}

#endif

