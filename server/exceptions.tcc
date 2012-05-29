/**
	file: exceptions.tcc
	author: Maximilian Lasser [max.lasser@online.de]
	created: Thursday, 24th May 2012
**/

#ifndef _EXCEPTIONS_TCC_
#define _EXCEPTIONS_TCC_

template<typename T>
Exception::ClientAlreadyAdded::ClientAlreadyAdded(T msg):
	std::invalid_argument(msg)
{}

template<typename T>
Exception::InvalidMessageType::InvalidMessageType(T msg, MessageType type):
	std::runtime_error(msg), type(type)
{}

#endif
