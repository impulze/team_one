#ifndef _EXCEPTIONS_TCC_
#define _EXCEPTIONS_TCC_

/**
 * @file exceptions.tcc
 * @author Maximilian Lasser <max.lasser@online.de>
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 */

#include "errno.h"

template<typename T>
Exception::AlreadyInstantiated::AlreadyInstantiated(T msg):
	std::logic_error(msg)
{}

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

template<typename T>
Exception::NotYetInstantiated::NotYetInstantiated(T msg):
	std::logic_error(msg)
{}

template<typename T>
Exception::SocketFailure::SocketFailure(T msg, int socket):
	std::runtime_error(msg), socket(socket)
{}

template<class T>
Exception::SocketDisconnected::SocketDisconnected(T msg, int socket)
	: SocketFailure(msg, socket)
{
}

#endif
