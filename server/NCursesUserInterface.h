#ifndef READLINEUSERINTERFACE_H_INCLUDED
#define READLINEUSERINTERFACE_H_INCLUDED

#include "UserInterface.h"

/**
 * @file NCursesUserInterface.h
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 *
 * NCurses implementation for the user command line interface.
 */

class NCursesUserInterface
	: public UserInterface
{
public:
	std::string get_input();
};

#endif
