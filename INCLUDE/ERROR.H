// ---------------------------------------------------------------------------
// error.h
// -----------------------------------------------------------------------
// Error system
// -----------------------------------------------------------------------
// Copyright (c) 2007, Kronoman
// In loving memory of my father.
// Made in Argentina
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------
#ifndef ERROR_H_ARCADEFRAMEWORK
#define ERROR_H_ARCADEFRAMEWORK

class Error
{
	public:
		static void error(const char *msg, ...); ///< sends a error message and ends the program
};

#endif


