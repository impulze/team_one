#ifndef USERINTERFACE_TCC_INCLUDED
#define USERINTERFACE_TCC_INCLUDED

#include "UserInterface.h"

template <class... T>
void UserInterface::printf(std::string const &format, T &&... args)
{
	return printfv(format.c_str(), std::forward<T>(args)...);
}

#endif
