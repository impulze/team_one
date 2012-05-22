/**
	file: constants.h
	author: Maximilian Lasser [max.lasser@online.de]
	created: Tuesday, 22nd May 2012
**/

#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#if SIZE_MAX < 1000000000
	#define MAX_SOCKET_BUFFER_SIZE SIZE_MAX
#else
	#define MAX_SOCKET_BUFFER_SIZE 1000000000
#endif

#endif