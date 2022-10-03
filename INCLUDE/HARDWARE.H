// ---------------------------------------------------------------------------
// hardwaremanager.h
// -----------------------------------------------------------------------
// Hardware manager
//
// Takes care of init and shutdown of hardware system.
// -----------------------------------------------------------------------
// Copyright (c) 2007, Kronoman
// In loving memory of my father.
// Made in Argentina
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------

#ifndef HARDWAREMANAGER_H
#define HARDWAREMANAGER_H

#include <allegro.h>

class HardwareManager
{
	public:
		// configuration desired
		static char *configFilename;
		
		static int screenW;
		static int screenH;
		static int videoMode;
		static int colorDepth;
		
		static bool wantSound;
		static bool wantMouse;
		static bool wantJoystick;
		
		static void init(int argc, char *argv[]); ///< inits hardware
		static void shutdown(); ///< shutdowns hardware
		
	private:
		static bool check_command(int argc, char *argv[], const char *search_command);
};

#endif
