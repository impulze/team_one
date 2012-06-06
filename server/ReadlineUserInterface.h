#ifndef READLINEUSERINTERFACE_H_INCLUDED
#define READLINEUSERINTERFACE_H_INCLUDED

#include "UserInterface.h"

/**
 * @file ReadlineUserInterface.h
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 *
 * Readline implementation for the user command line interface.
 */

class ReadlineUserInterface
	: public UserInterface
{
public:
	std::string get_input();
};

#endif
