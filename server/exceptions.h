/**	@file exceptions.h

	@author Maximilian Lasser <max.lasser@online.de>
	@author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
	@date Tuesday, 22nd May 2012
**/

#ifndef EXCEPTIONS_H_INCLUDED
#define EXCEPTIONS_H_INCLUDED

#include "Message.h"

#include <stdexcept>

/**
	@brief Contains several useful custom exception classes.
**/
namespace Exception
{
	/**
		A class that may be instantiated only once already has been instantiated, but another
		instantiation was requested.
	**/
	struct AlreadyInstantiated : std::logic_error
	{
		/**
			Standard constructor.

			@param msg an error message
		**/
		template<typename T>
		AlreadyInstantiated(T msg);
	};

	/**
		A Client has already been added any may not be added twice, but exactly that was requested.
	**/
	struct ClientAlreadyAdded : std::invalid_argument
	{
		/**
			Standard constructor.

			@param msg an error message
		**/
		template<typename T>
		ClientAlreadyAdded(T msg);
	};
	
	/**
		A generic error class that encapsulates a low-level error indicated by a syscall returning
		-1 and setting the errno variable.
		It contains information about the function that failed and the errno that has been set.
	**/
	struct ErrnoError : std::runtime_error
	{
		const int			 error; ///< the error's errno
		const char			*const function; ///< the name of the function that set the errno

		/**
			Standard constructor.

			@param msg an error message
			@param error the errno
			@param function a pointer to the name of the function that failed; defaults to NULL
		**/
		template<typename T>
		ErrnoError(T msg, int error, const char *function = NULL);
		/**
			Simplified constructor. Automatically fetches the errno from the global variable.

			@note This is just a simplified version of the standard constructor.
			@see ErrnoError::ErrnoError(T, int, const char*)
		**/
		template<typename T>
		ErrnoError(T msg, const char *function = NULL);
	};

	/**
		A Message has an invalid type.
	**/
	struct InvalidMessageType : std::runtime_error
	{
		const int					socket; //< socket the Message has been received from
		const Message::MessageType	type; //< the invalid type

		/**
			Standard constructor.

			@param msg an error message
			@param type the invalid MessageType
			@param socket the socket the Message has been received from
		**/
		template <typename T>
		InvalidMessageType(T msg, Message::MessageType type, int socket);
	};

	/**
		An existing instance of a class was requested, although the class has not been instantiated
		yet.
	**/
	struct NotYetInstantiated : std::logic_error
	{
		/**
			Standard constructor.

			@param msg an error message
		**/
		template<typename T>
		NotYetInstantiated(T msg);
	};

	/**
		An action on a socket failed.
	**/
	struct SocketFailure : std::runtime_error
	{
		const int	socket; ///< the socket where something went wrong
		/**
			Standard constructor.

			@param msg an error message
			@param socket the socket where something went wrong
		**/
		template<typename T>
		SocketFailure(T msg, int socket);
	};

	/**
	 * A Socket disconnected while reading.
	**/
	struct SocketDisconnected
		: SocketFailure
	{
		/**
		 * Standard constructor.
		 *
		 * @param msg an error message
		 * @param socket the socket where the disconnect happened
		 */
		template <class T>
		SocketDisconnected(T msg, int socket);
	};
};

#include "exceptions.tcc"

#endif
