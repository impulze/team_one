#ifndef READLINEUSERINTERFACE_H_INCLUDED
#define READLINEUSERINTERFACE_H_INCLUDED

#include "UserInterface.h"

#include <curses.h>

#include <memory>
#include <string>

/**
 * @file NCursesUserInterface.h
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 *
 * NCurses implementation for the user command line interface.
 */

class NCursesUserInterface
	: public UserInterface
{
private:
	/**
	 * Create an interface with NCurses.
	 * Setup output window and input area.
	 */
	NCursesUserInterface();

public:
	/**
	 * Release all resources used by the interface and reset
	 * the window to its initial state.
	 */
	~NCursesUserInterface();

	/**
	 * Obtain a reference to the one and only
	 * instance of this singleton.
	 */
	static NCursesUserInterface &get_instance();

	std::wstring get_line();

	template <class... T>
	void printf(std::string const &format, T &&... args);

private:
	static std::unique_ptr<NCursesUserInterface> instance_;
	SCREEN *screen_;
	WINDOW *input_window_;
	std::wstring current_line_;
	std::wstring::size_type current_position_;
};

#include "NCursesUserInterface.tcc"

#endif
