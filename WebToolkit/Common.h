#ifndef _COMMON_H
#define	_COMMON_H

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdexcept>
#include <iostream>
#include <string>
#include <sstream>
#include <set>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

#include <time.h>
#include <stdlib.h>

#ifdef __UCLIBC__
typedef basic_string<wchar_t> wstring;
#endif

typedef long long i64;
typedef unsigned long long ui64;

#endif

