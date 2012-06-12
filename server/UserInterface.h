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
	/**
	 * Construct a user command line interface object.
	 * Provide a default constructor, because this is just an interface.
	 */
	UserInterface() = default;

	/**
	 * Deconstruct a user command line interface object, freeing all its resources
	 * and probably closing the window if any.
	 */
	virtual ~UserInterface();

	/**
	 * Delete the default copy constructor, making copying a user command line
	 * interface object impossible.
	 */
	UserInterface(UserInterface const &) = delete;

	/**
	 * Delete the default assignment operator, making assigning a user command line
	 * interface object impossible.
	 */
	UserInterface &operator=(UserInterface const &) = delete;

	/**
	 * Run the processing loop and return the entered line.
	 *
	 * @throws std::runtime_error If processing failed in such a way
	 *                            that a restart of the program is required.
	 */
	virtual std::wstring get_line() = 0;
};

#endif
