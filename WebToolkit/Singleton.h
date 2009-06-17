#ifndef _SINGLETON_H
#define _SINGLETON_H

template<class T>
class Singleton
{
protected:
	static T* instance;
public:
	Singleton()
	{
		instance=static_cast<T*>(this);
	}
	static T& Instance()
	{
		return *instance;
	}
};

#endif
