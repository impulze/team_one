#include "UserInterface.h"

/**
 * @file UserInterface.cpp
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 *
 * Implementation file for common abstractions of user interface things.
 */

namespace userinterface_errors
{
	Failure::Failure(std::string const &message)
		: std::runtime_error(message)
	{
	}
}

UserInterface::~UserInterface()
{
}

void UserInterface::printf(std::string const &format, ...)
{
	va_list list;

	va_start(list, format);
	printfv(format, list);
	va_end(list);
}
