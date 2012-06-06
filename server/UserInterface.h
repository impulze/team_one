#ifndef USERINTERFACE_H_INCLUDED
#define USERINTERFACE_H_INCLUDED

#include <string>

/**
 * @file UserInterface.h
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 *
 * Abstractions for the user command line interface.
 * One should be able to wait for input and retrieve what was typed
 * in.
 */

class UserInterface
{
public:
	std::string get_input() = 0;
};

#endif
