#include "Common.h"
#include "Util.h"

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
		case '+':
			r<<' ';
			ptr++;
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

string Util::Timestamp(time_t t)
{
	ostringstream r;
	tm* tms=localtime(&t);
	r<<tms->tm_year+1900<<"/";
	if(tms->tm_mon<10)
		r<<"0";
	r<<tms->tm_mon<<"/";
	if(tms->tm_mday<10)
		r<<"0";
	r<<tms->tm_mday<<" ";
	if(tms->tm_hour<10)
		r<<"0";
	r<<tms->tm_hour<<":";
	if(tms->tm_min<10)
		r<<"0";
	r<<tms->tm_min<<":";
	if(tms->tm_sec<10)
		r<<"0";
	r<<tms->tm_sec;
	return r.str();
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

void Util::Trim(string& st)
{
	while((!st.empty())&&((st[0]==' ')||(st[0]=='\t')||(st[0]=='\n')||(st[0]=='\r')))
		st.erase(0,1);
	while((!st.empty())&&((st[st.size()-1]==' ')||(st[st.size()-1]=='\t')||(st[st.size()-1]=='\n')||(st[st.size()-1]=='\r')))
		st.erase(st.size()-1,1);
}

string Util::GenerateRandomString(int len)
{
	string r;
	for(int i=0;i<len;i++)
		r+='a'+(rand()%('z'-'a'+1));
	return r;
}

string Util::MimeType(const string& ext)
{
	string r;
	if(ext=="txt"||ext=="cpp"||ext=="h")
		r="text/plain";
	if(ext=="htm"||ext=="html")
		r="text/html";
	if(ext=="xml")
		r="text/xml";
	if(ext=="css")
		r="text/css";
	if(ext=="js")
		r="application/javascript";
	if(ext=="gif")
		r="image/gif";
	if(ext=="jpg")
		r="image/jpeg";
	if(ext=="png")
		r="image/png";
	return r;
}

