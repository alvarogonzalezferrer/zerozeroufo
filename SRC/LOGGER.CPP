// ---------------------------------------------------------------------------
// logger.cpp
// -----------------------------------------------------------------------
// Logger system
// -----------------------------------------------------------------------
// Copyright (c) 2007, Kronoman
// In loving memory of my father.
// Made in Argentina
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------
#include "logger.h"
#include <stdio.h>
#include <allegro.h>

#include <time.h> // for time stamp

char *Logger::file = "log.txt"; // file to log on (append)
bool Logger::enabled = false; // logger to file enabled? default = false

void Logger::log(const char *msg, ...)
{
	if (!enabled || file == NULL)
		return; // don't log

	// creates the buffer, and populates with the msg to log...
	char buf[4096];
	va_list ap;
	va_start(ap, msg);
	uvszprintf(buf, 4096, msg, ap);
	va_end(ap);

	// get time of day, to time stamp loggin
	time_t t_now = time(NULL); // get current time
	struct tm *time_now_local = localtime (&t_now); // get finally the time in appropiate format
	char tbufstr[256]; // time string buffer
	strftime(tbufstr, 256, "%b %d %Y %H:%M:%S", time_now_local); // This function formats the time data

	//printf("[%s]\t%s\n", tbufstr, buf); // log to console

	// log to file
	FILE *fp = NULL;
	fp = fopen(file, "a");
	if (!fp)
		return;

	fprintf(fp, "[%s]\t%s\n", tbufstr, buf); // log

	fclose(fp);
}


