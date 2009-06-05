#include "Common.h"
#include "Util.h"
#include "File.h"

#include <sys/types.h>
#include <sys/stat.h>

#ifdef WIN32
#include <io.h>
#else
#include <dirent.h>
#endif

vector<DirectoryEntry> Util::DirectoryList(const string& path)
{
	string adjPath=AdjustPath(path);
	vector<DirectoryEntry> r;
#ifdef WIN32
	wstring wadjPath=Util::UTF8Decode(adjPath);
	_wfinddata64_t fi;
	intptr_t h=_wfindfirst64((wadjPath+L"\\*.*").c_str(),&fi);
	if(h==-1)
		throw runtime_error("Failed to get directory list");
	for(;;)
	{
		if(fi.name[0]!='.')
		{
			DirectoryEntry e;
			e.name=UTF8Encode(fi.name);
			e.isDirectory=(fi.attrib&_A_SUBDIR)!=0;
			if(e.isDirectory)
				e.size=0;
			else
			{
				struct _stat64 s;
				_wstat64((wadjPath+L"\\"+fi.name).c_str(),&s);
				e.size=s.st_size;
			}
			r.push_back(e);
		}
		if(_wfindnext64(h,&fi)!=0)
			break;
	}
	_findclose(h);
#else
	DIR* dir=opendir(path.c_str());
	if(dir==NULL)
		throw runtime_error("Failed to get directory list");
	dirent64* entry;
	while((entry=readdir64(dir))!=NULL)
	{
		if(entry->d_name[0]!='.')
		{
			DirectoryEntry e;
			e.isDirectory=(entry->d_type==DT_DIR);
			e.name=entry->d_name;
			if(e.isDirectory)
				e.size=0;
			else
			{
				struct stat64 s;
				stat64((adjPath+"/"+e.name).c_str(),&s);
				e.size=s.st_size;
			}
			r.push_back(e);
		}
	}
	closedir(dir);
#endif
	return r;
}

string Util::AdjustPath(const string& path)
{
#ifdef WIN32
	ostringstream r;
	for(size_t i=0;i<path.length();i++)
	{
		switch(path[i])
		{
		case '/':
			r<<"\\";
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

string Util::HTMLEscape(const string& st)
{
	ostringstream r;
	for(size_t i=0;i<st.length();i++)
	{
		switch(st[i])
		{
		case '<':
			r<<"&lt;";
			break;
		case '>':
			r<<"&gt;";
			break;
		case '&':
			r<<"&amp;";
			break;
		default:
			r<<st[i];
		}
	}
	return r.str();
}

unsigned char hex_conv(char t)
{
	if((t>='0')&&(t<='9'))
		return static_cast<unsigned char>(t)-'0';
	else
		return static_cast<unsigned char>(t)-'A'+0xa;
}

string Util::StringToLower(const string& st)
{
	ostringstream r;
	for(size_t i=0;i<st.length();i++)
		r<<static_cast<char>(tolower(st[i]));
	return r.str();
}

CheckPathResult Util::CheckPath(const string& st)
{
	wstring t=UTF8Decode(AdjustPath(st));
#ifdef WIN32
	struct _stat64 s;
	if(_wstat64(UTF8Decode(AdjustPath(st)).c_str(),&s)!=0)
#else
	struct stat64 s;
	if(stat64(AdjustPath(st).c_str(),&s)!=0)
#endif
		return PathNotExist;
	if(s.st_mode&S_IFDIR)
		return PathIsDirectory;
	else
		return PathIsFile;
}

i64 Util::GetFileSize(const string& st)
{
#ifdef WIN32
	struct _stat64 s;
	if(_wstat64(UTF8Decode(AdjustPath(st)).c_str(),&s)!=0)
#else
	struct stat64 s;
	if(stat64(AdjustPath(st).c_str(),&s)!=0)
#endif
		return -1;
	return s.st_size;
}

string Util::URLDecode(const string& st)
{
	ostringstream r;
	const char* ptr=st.c_str();
	const char* end=ptr+st.length();
	while(ptr<end)
	{
		switch(*ptr)
		{
		case '%':
			r<<static_cast<char>((hex_conv(*(ptr+1))<<4)+(hex_conv(*(ptr+2))));
			ptr+=3;
			break;
		default:
			r<<*ptr;
			ptr++;
		}
	}
	return r.str();
}

string Util::URLEncode(const string& st)
{
	static const char lookup_table[]="0123456789ABCDEF";
	ostringstream r;
	unsigned int t;
	for(size_t i=0;i<st.length();i++)
	{
		if(((st[i]>='a')&&(st[i]<='z'))||((st[i]>='A')&&(st[i]<='Z'))||((st[i]>='0')&&(st[i]<='9'))||(st[i]=='-')||(st[i]=='_')||(st[i]=='.')||(st[i]=='~'))
			r<<st[i];
		else
		{
			t=static_cast<unsigned char>(st[i]);
			r<<"%"<<lookup_table[t>>4]<<lookup_table[t&0xf];
		}
	}
	return r.str();
}

bool Util::PathValid(const string& st)
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

vector<string> Util::Extract(const string& st)
{
	vector<string> r;
	unsigned int curPos,valBegin,valEnd;
	for(curPos=0;curPos<st.size();)
	{
		for(valBegin=curPos;valBegin<st.size();valBegin++)
		{
			if(static_cast<unsigned char>(st[valBegin])>' ')
				break;
		}
		if(valBegin!=st.size())
		{
			if((st[valBegin]=='\"')||(st[valBegin]=='\''))
			{
				for(valEnd=valBegin+1;valEnd<st.size();valEnd++)
				{
					if(st[valEnd]==st[valBegin])
						break;
				}
				curPos=valEnd+1;
				valBegin++;
			}
			else
			{
				for(valEnd=valBegin+1;valEnd<st.size();valEnd++)
				{
					if(static_cast<unsigned char>(st[valEnd])<=' ')
						break;
				}
				curPos=valEnd;
			}
			r.push_back(st.substr(valBegin,valEnd-valBegin));
		}
		else
			break;
	}
	return r;
}

string Util::ReadFile(const string& st)
{
	i64 size64=Util::GetFileSize(st);
	if(size64==-1)
		throw runtime_error("File not found");
	if(size64>10*1024*1024)
		throw runtime_error("Don't use Util::ReadFile for huge files");
	int size=static_cast<int>(size64);
	Buffer buf(size);
	File file(st,false,true);
	file.Read(buf.buf,size);
	string r(buf.buf,size);
	return r;
}

void Util::Substitute(string& st,const string& what,const string& to)
{
	size_t i=st.find(what);
	if(i!=string::npos)
		st.replace(i,what.length(),to);
}

const char* months[]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

string Util::MakeHTTPTime(time_t t)
{
	ostringstream r;
	tm* tms=gmtime(&t);
	if(tms->tm_mday<10)
		r<<"0";
	r<<tms->tm_mday<<" ";
	r<<months[tms->tm_mon]<<" ";
	r<<tms->tm_year+1900<<" ";
	if(tms->tm_hour<10)
		r<<"0";
	r<<tms->tm_hour<<":";
	if(tms->tm_min<10)
		r<<"0";
	r<<tms->tm_min<<":";
	if(tms->tm_sec<10)
		r<<"0";
	r<<tms->tm_sec<<" GMT";
	return r.str();
}

time_t Util::ParseHTTPTime(const string& httpTime)
{
	if(httpTime.length()<2)
		return 0;
	string t=httpTime;
	size_t commaPos=t.find(',');
	if(commaPos!=string::npos)
		t=t.substr(commaPos+2);
	tm tms;
	istringstream in(t);
	in>>tms.tm_mday;
	string mon;
	in>>mon;
	for(int i=0;i<12;i++)
	{
		if(mon==months[i])
		{
			tms.tm_mon=i;
			break;
		}
	}
	in>>tms.tm_year;
	tms.tm_year-=1900;
	in>>tms.tm_hour;
	in.ignore();
	in>>tms.tm_min;
	in.ignore();
	in>>tms.tm_sec;
	return mktime(&tms);
}

string Util::UTF8Encode(const wstring& st)
{
	ostringstream r;
	for(size_t i=0;i<st.length();i++)
	{
		unsigned int t=static_cast<unsigned short>(st[i]);
		if(t<0x80)
			r<<static_cast<char>(t);
		else
		{
			if(t<0x800)
				r<<static_cast<char>(((t>>6))|0xC0)<<static_cast<char>((t&0x3F)|0x80);
			else
				r<<static_cast<char>(((t>>12))|0xE0)<<static_cast<char>(((t>>6)&0x3F)|0x80)<<static_cast<char>((t&0x3F)|0x80);
		}
	}
	return r.str();
}

wstring Util::UTF8Decode(const string& st)
{
	wstring r;
	size_t i;
	unsigned int t;
	for(i=0;i<st.size();)
	{
		if(st[i]&0x80)
		{
			if(st[i]&0x20)
			{
				if(i>=(st.size()-2))
					throw runtime_error("UTF8 decoding error");
				t=(static_cast<unsigned int>(st[i]&0x0F)<<12)|(static_cast<unsigned int>(st[i+1]&0x3F)<<6)|(static_cast<unsigned int>(st[i+2]&0x3F));
				r+=static_cast<wchar_t>(t);
				i+=3;
			}
			else
			{
				if(i>=(st.size()-1))
					throw runtime_error("UTF8 decoding error");
				t=(static_cast<unsigned int>(st[i]&0x1F)<<6)|(static_cast<unsigned int>(st[i+1]&0x3F));
				r+=static_cast<wchar_t>(t);
				i+=2;
			}
		}
		else
		{
			r+=static_cast<wchar_t>(st[i]);
			i++;
		}
	}
	return r;
}

