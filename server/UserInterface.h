#ifndef USERINTERFACE_H_INCLUDED
#define USERINTERFACE_H_INCLUDED

#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

/**
 * @file UserInterface.h
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 *
 * Abstractions for the user command line interface.
 * One should be able to wait for input and retrieve what was typed
 * in.
 */

namespace userinterface_errors
{
	/**
	 * A generic user interface exception.
	 * Specific errors derive from this class to create
	 * an exception hierarchy.
	 */
	struct Failure
		: std::runtime_error
	{
		/**
		 * Construct a new user interface failure with a specific
		 * error message.
		 *
		 * @param message The error message that describes the error.
		 */
		Failure(std::string const &message);
	};
}

class UserInterface
{
public:
	typedef std::vector<std::wstring> command_arguments_t;
	typedef std::function<void(command_arguments_t const &)> command_processor_t;

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
	 * Run the processing loop as in obtain user input and call the registered
	 * processors.
	 * The function returns if a line was entered and the specified callback
	 * was called.
	 *
	 * @throws std::runtime_error If processing failed in such a way
	 *                            that a restart of the program is required.
	 */
	virtual void run() = 0;

	/**
	 * Register a processor for a specific command entered.
	 * If the string is entered by the user the processor is called with the
	 * parameters the user passed.
	 *
	 * @param command The name of the command.
	 * @param function The processor that is executed for this command.
	 *
	 * @throws InvalidCommandError Thrown if the command includes whitespace.
	 */
	virtual void register_processor(std::wstring const &command,
	                                command_processor_t const &function) = 0;

	/**
	 * Print text via the user interface.
	 * This member function is thread safe. It locks a mutex, so do not
	 * call it in performance critical areas.
	 *
	 * @param format The format as used by printf(3).
	 * @param args A list of parameters. The parameters need to be plain old data.
	 */
	void printf(std::string const &format, ...);

	/**
	 * Print text via the user interface.
	 * This member function is thread safe. It locks a mutex, so do not
	 * call it in performance critical areas.
	 *
	 * @param format The format as used by printf(3).
	 * @param list A va_list created by the va_start macro.
	 */
	virtual void printfv(std::string const &format, va_list list) = 0;
};

#endif
