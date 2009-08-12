#ifndef _OSUTILS_H
#define	_OSUTILS_H

class Environment
{
public:
	static bool terminated;
	static void Init();
	static void WaitForTermination();
};

#endif

