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
	class ClientAlreadyAdded
		: public std::invalid_argument
	{
		public:
			template<typename T>
			ClientAlreadyAdded(T msg);
	};

	class InvalidMessageType
		: public std::runtime_error
	{
		public:
			MessageType	type;
			template <class T>
			InvalidMessageType(T msg, MessageType type);
	};
};

#include "exceptions.tcc"

#endif
