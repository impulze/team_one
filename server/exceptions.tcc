/**
	file: exceptions.tcc
	author: Maximilian Lasser [max.lasser@online.de]
	created: Thursday, 24th May 2012
**/

#ifndef _EXCEPTIONS_TCC_
#define _EXCEPTIONS_TCC_

#include "errno.h"

template<typename T>
Exception::ClientAlreadyAdded::ClientAlreadyAdded(T msg):
	std::invalid_argument(msg)
{}

template<typename T>
Exception::ErrnoError::ErrnoError(T msg, int error, const char *function):
	std::runtime_error(msg), error(error), function(function)
{}

template<typename T>
Exception::ErrnoError::ErrnoError(T msg, const char *function):
	ErrnoError(msg, errno, function)
{}

template<typename T>
Exception::InvalidMessageType::InvalidMessageType(T msg, Message::MessageType type, int socket):
	std::runtime_error(msg), socket(socket), type(type)
{}

#endif
