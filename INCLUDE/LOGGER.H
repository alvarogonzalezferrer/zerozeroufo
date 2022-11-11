// ---------------------------------------------------------------------------
// logger.h
// -----------------------------------------------------------------------
// Logger system
// -----------------------------------------------------------------------
// Copyright (c) 2007, Kronoman
// In loving memory of my father.
// Made in Argentina
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------
#ifndef LOGGER_H_ARCADEFRAMEWORK
#define LOGGER_H_ARCADEFRAMEWORK

class Logger
{
	public:
		static constexpr char *file = "log.txt"; ///< file to log on (append)
		static bool enabled; ///< logger to file enabled? default = false

		static void log(const char *msg, ...); ///< append a message to log file
};

#endif

