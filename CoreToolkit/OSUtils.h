/*
	This file is a part of madfish-webtoolkit project.
	http://code.google.com/p/madfish-webtoolkit/

	(c) 2009 Alexander "Mad Fish" Chehovsky
	See License.txt for licensing information.
*/

#pragma once
#ifndef _OSUTILS_H
#define	_OSUTILS_H

namespace CoreToolkit
{

class Environment
{
public:
	static bool terminated;
	static void Init();
	static bool CheckForTermination();
	static void WaitForTermination();
};

}

#endif

