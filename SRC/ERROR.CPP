// ---------------------------------------------------------------------------
// error.cpp
// -----------------------------------------------------------------------
// Error system
// -----------------------------------------------------------------------
// Copyright (c) 2007, Kronoman
// In loving memory of my father.
// Made in Argentina
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------
#include "error.h"
#include "logger.h"

#include <allegro.h> // I use some stuff from here too, mainly, unicode strings ;)
#include <stdarg.h> // for the variable argument list
#include <stdlib.h> // for use of malloc 

void Error::error(const char *msg, ...)
{
	char *buf;

	// exits the graphics mode - this may crash if allegro is not init...
	set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);

	// creates the buffer, and populates with the msg to show...
	buf = (char *)malloc(4096);

	if (buf == NULL)
	{
		allegro_message("raise_error(): There is a error, and I'm out of virtual memory to show the error message. :^(\n");
	}
	else
	{
		// parse the variable parameters
		va_list ap;
		va_start(ap, msg);
		uvszprintf(buf, 4096, msg, ap);
		va_end(ap);

		Logger::log(buf); // LOG the error (if enabled)

		allegro_message("%s\n", buf); // show message

		free(buf);
	}

	exit( -1); // abort the program
}

