/**
	file: exceptions.h
	author: Maximilian Lasser [max.lasser@online.de]
	created: Tuesday, 22nd May 2012
**/

#ifndef EXCEPTIONS_H_INCLUDED
#define EXCEPTIONS_H_INCLUDED

#include "Message.h"

#include <stdexcept>

namespace Exception
{
	struct ClientAlreadyAdded : std::invalid_argument
	{
		template<typename T>
		ClientAlreadyAdded(T msg);
	};
	
	struct ErrnoError : std::runtime_error
	{
		const int			 error;
		const char			*const function;
		template<typename T>
		ErrnoError(T msg, int error, const char *function = 0);
	};

	struct InvalidMessageType : std::runtime_error
	{
		const Message::MessageType type;
		template <class T>
		InvalidMessageType(T msg, Message::MessageType type);
	};
};

#include "exceptions.tcc"

#endif
