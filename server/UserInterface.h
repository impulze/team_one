#ifndef USERINTERFACE_H_INCLUDED
#define USERINTERFACE_H_INCLUDED

#include <functional>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

/**
 * @file server/UserInterface.h
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 *
 * Abstractions for the user command line interface.
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

	/**
	 * Represent an error which happens if a command was passed with
	 * wrong amount of arguments or other constraints.
	 */
	struct InvalidCommandError
		: Failure
	{
		/**
		 * Construct a new invalid command error with a specific
		 * error message.
		 *
		 * @param message The error message that describes the error.
		 */
		InvalidCommandError(std::string const &message);
	};

	/**
	 * Represent an error which happens if a command was passed that didn't
	 * execute correct.
	 */
	struct CommandFailedError
		: Failure
	{
		/**
		 * Construct a new command failed error with a specific
		 * error message.
		 *
		 * @param message The error message that describes the error.
		 */
		CommandFailedError(std::string const &message);
	};
}

/**
 * This abstract user interface should be implemented to receive
 * user input and process lines and then finally execute commands, which
 * can be registered with register_processor().
 *
 * One should be able to wait for input and retrieve what was typed
 * in.
 */
class UserInterface
{
public:
	//! Represents a list of arguments typed in by the user.
	typedef std::vector<std::wstring> command_arguments_t;
	//! Represents the function executed for a command entered by the user.
	typedef std::function<void(command_arguments_t const &)> command_processor_t;
	//! Represent a list of commands and their executed functions.
	typedef std::unordered_multimap<std::wstring, command_processor_t> command_processors_t;

	/**
	 * Construct a user command line interface object.
	 */
	UserInterface();

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
	 *
	 * The function never returns.
	 *
	 * @throws userinterace_errors::Failure If processing failed in such a way
	 *                                      that a restart of the program is required.
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
	 *
	 * @return An iterator to the inserted command processor.
	 */
	command_processors_t::iterator register_processor(std::wstring const &command,
	                                                  command_processor_t const &function);

	/**
	 * Unregister a previously registered processor.
	 *
	 * @param iterator The iterator previously returned by register_processor.
	 */
	void unregister_processor(command_processors_t::iterator);

	/**
	 * Print text via the user interface.
	 * This member function is thread safe. It locks a mutex, so do not
	 * call it in performance critical areas.
	 *
	 * @param format The format as used by printf(3).
	 * @param args A list of parameters.
	 */
	template <class... T>
	void printf(std::string const &format, T &&... args);

	/**
	 * This sets an internal state (see quit_) that implementations can
	 * use to check if the processing loop shall quit.
	 */
	void quit();

protected:
	/**
	 * Process a line (command input) after the interface successfully
	 * obtained a line of input.
	 * This will eventually call the registered command processor if
	 * the command matches.
	 *
	 * May execute several or none of the registered processors
	 * (see register_processor()).
	 */
	void process_line();

	//! A storage for the current line, which can be used by the implementation
	std::wstring current_line_;

private:
	/**
	 * Print text via the user interface.
	 * This member function is thread safe. It locks a mutex, so do not
	 * call it in performance critical areas.
	 *
	 * @param format The format as used by printf(3).
	 * @param ... Variable arguments in plain old data.
	 */
	virtual void printfv(char const *format, ...) = 0;

	/**
	 * Parse arguments for a command.
	 *
	 * The arguments are split at whitespace
	 * except the whitespace is escaped with the
	 * character '\'.
	 *
	 * @param string The argument string, probably with escaped
	 *               whitespace characters.
	 * @return The arguments for this command.
	 */
	static command_arguments_t parse_arguments(std::wstring const &string);

protected:
	/**
	 * This flag is set by a call to quit() to inform the implementation that the
	 * user interface shall end processing.
	 */
	bool quit_requested_;

private:
	//! A list of commands and their processors.
	command_processors_t command_processors_;
	/**
	 * Since several threads can execute quit() this mutex is required
	 * to allow mutual exclusion between those threads
	 */
	std::mutex quit_mutex_;
};

/**
 * Provide a singleton template which is useful here, because only
 * one user interface shall be active at a time.
 */
template <class Implementation>
class UserInterfaceSingleton
	: public UserInterface
{
public:
	/**
	 * Obtain a reference to the one and only
	 * instance of this singleton.
	 */
	static Implementation &get_instance();

	//! Store a unique ptr to the implementation details.
	static std::unique_ptr<Implementation> instance_;
};

#include "UserInterface.tcc"

#endif
