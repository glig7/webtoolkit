/*
	This file is a part of madfish-webtoolkit project.
	http://code.google.com/p/madfish-webtoolkit/

	(c) 2009 Alexander "Mad Fish" Chehovsky
	See License.txt for licensing information.
*/

#ifndef _COMMON_H
#define	_COMMON_H

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <set>
#include <fstream>
#include <vector>
#include <algorithm>
#include <queue>
#include <map>

using namespace std;

#include <time.h>
#include <stdlib.h>
#include <string.h>

#ifdef __UCLIBC__
typedef basic_string<wchar_t> wstring;
#endif

typedef long long i64;
typedef unsigned long long ui64;

#ifdef WIN32
#define atoll _atoi64
#endif

#endif

