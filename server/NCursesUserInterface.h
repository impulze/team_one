#ifndef READLINEUSERINTERFACE_H_INCLUDED
#define READLINEUSERINTERFACE_H_INCLUDED

#include "UserInterface.h"

#include <curses.h>

#include <memory>
#include <string>
#include <thread>

/**
 * @file server/NCursesUserInterface.h
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 *
 * NCurses implementation for the user command line interface.
 */

namespace userinterface_errors
{
	/**
	 * A generic user interface exception of the NCurses implementation.
	 * Specific errors derive from this class to create
	 * an exception hierarchy.
	 */
	struct NCursesError
		: Failure
	{
		/**
		 * Construct a new NCurses failure with a specific
		 * error message.
		 *
		 * @param message The error message that describes the error.
		 */
		NCursesError(std::string const &message);
	};
}

/**
 * This class implements the basic UserInterface with the ncurses
 * library.
 * The library can be used on almost all POSIX operating systems.
 * The loop basically polls the standard input and checks for user
 * input.
 * Then invokes the UserInterface::process_line() and possibly
 * executes command processors which were registered
 * with UserInterface::register_processor().
 *
 * @startuml{NCursesUserInterface_Class.svg}
 * abstract class UserInterface
 * UserInterface <|-- NCursesUserInterface
 * class NCursesUserInterface << abstract >> {
 * .. Construction ..
 * + NCursesUserInterface()
 * + ~NCursesUserInterface()
 * __
 * + run()
 * + deinitialize();
 * - printfv(format: char const *, ...)
 * __ attributes __
 * deinitialized_: bool
 * screen_: SCREEN *
 * input_window_: WINDOW *
 * current_position_: wstring::size_type
 * printf_mutex_: mutex
 * }
 * @enduml
 */
class NCursesUserInterface
	: public UserInterface
{
public:
	/**
	 * Create an interface with NCurses.
	 * Setup output window and input area.
	 */
	NCursesUserInterface();

	/**
	 * See deinitialize().
	 */
	~NCursesUserInterface();

	void run();

	/**
	 * This function is called by the destructor and if an
	 * invariant class would occur.
	 * It will release all resources used by the interface
	 * and reset the window to its initial state.
	 */
	void deinitialize();

private:
	void printfv(char const *format, ...);

	/**
	 * A state variable which stores the information if the user interface
	 * has been deactivated. In this case printf() no longer prints
	 * on the user interface but rather the standard error/output.
	 */
	bool deinitialized_;
	//! NCurses screen variable for this interface
	SCREEN *screen_;
	//! NCurses window variable for the input box
	WINDOW *input_window_;
	//! The current position in the current line.
	std::wstring::size_type current_position_;
	//! A mutex for mutual exclusion of the printf() function.
	std::mutex printf_mutex_;
};

#endif

