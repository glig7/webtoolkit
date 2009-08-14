/*
	This file is a part of madfish-webtoolkit project.
	http://code.google.com/p/madfish-webtoolkit/

	(c) 2009 Alexander "Mad Fish" Chehovsky
	See License.txt for licensing information.
*/

#ifndef _UTIL_H
#define	_UTIL_H

class Util
{
public:
	static string HTMLEscape(const string& st);
	static string StringToLower(const string& st);
	static string URLDecode(const string& st);
	static string URLEncode(const string& st);
	static vector<string> Extract(const string& st);
	static void Substitute(string& st,const string& what,const string& to);
	static string MakeHTTPTime(time_t t);
	static time_t ParseHTTPTime(const string& httpTime);
	static string UTF8Encode(const wstring& st);
	static wstring UTF8Decode(const string& st);
	static void Trim(string& st);
	static string GenerateRandomString(int len);
	static string Timestamp(time_t t);
	static string MimeType(const string& ext);
};

#endif

