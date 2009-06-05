#ifndef _COMMON_H
#define	_COMMON_H

#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
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

