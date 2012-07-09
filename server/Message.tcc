#ifndef _MESSAGE_TCC_
#define _MESSAGE_TCC_

#include <arpa/inet.h>

template<typename T>
std::vector<char> &Message::append_bytes(std::vector<char> &dest, const T *src, size_t length)
{
	// get size via sizeof if not given by caller
	if (length == 0)
	{ length = sizeof(*src); }

	// append bytewise
	const char *buffer = reinterpret_cast<const char *>(src);
	dest.insert(dest.end(), buffer, buffer + length);

	return dest;
}

template<typename T>
std::vector<char> &Message::append_bytes(std::vector<char> &dest, const T src, size_t length)
{ return append_bytes(dest, &src, length); }

std::string Message::get_name_string(void) const
{ return std::string(name.data()); }

/*
uint64_t Message::htonll(uint64_t hostlonglong)
{ return static_cast<uint64_t>(htonl(hostlonglong)) << 32 | htonl(hostlonglong >> 32); }
*/

bool Message::is_empty() const
{ return this->source == 0; }

/*
uint64_t Message::ntohll(uint64_t netlonglong)
{ return static_cast<uint64_t>(ntohl(netlonglong)) << 32 | ntohl(netlonglong >> 32); }
*/

#endif
