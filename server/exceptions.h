/**
	file: exceptions.h
	author: Maximilian Lasser [max.lasser@online.de]
	created: Tuesday, 22nd May 2012
**/

#include "Message.h"

namespace Exception
{
	class InvalidMessageType
	{
		public:
			MessageType	type;
			InvalidMessageType(MessageType type):
				type(type) {};
	};
};