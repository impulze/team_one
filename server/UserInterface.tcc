#ifndef USERINTERFACE_TCC_INCLUDED
#define USERINTERFACE_TCC_INCLUDED

#include "UserInterface.h"

namespace
{
	template <class T>
	T printf_forward(T &&t);

	template <class T>
	struct printf_forwarder
	{
		static T forward(T &&t)
		{
			return t;
		}
	};

	template <>
	struct printf_forwarder<std::string>
	{
		static char const *forward(std::string &&string)
		{
			return string.c_str();
		}
	};
}

template <class... T>
void UserInterface::printf(std::string const &format, T &&... args)
{
	return printfv(format.c_str(), printf_forward(args)...);
}

namespace
{
	template <class T>
	T printf_forward(T &&t)
	{
		return printf_forwarder<T>::forward(t);
	}
}

#endif
