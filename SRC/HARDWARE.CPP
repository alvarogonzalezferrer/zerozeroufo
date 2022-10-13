// ---------------------------------------------------------------------------
// hardwaremanager.cpp
// -----------------------------------------------------------------------
// Hardware manager
//
// Takes care of init and shutdown of hardware system.
// -----------------------------------------------------------------------
// Copyright (c) 2007-2022, Kronoman
// In loving memory of my father.
// Made in Argentina
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// WARNING WARNING WARNING
// 
// This file has been chopped to suit MS DOS 320x200x8 bpp screen
// If you need other resolutions and hardware, please use my Arcade Game Framework
//
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------

#include <allegro.h>

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hardware.h"
#include "error.h"
#include "logger.h"

// default configuration -- setup
char *HardwareManager::configFilename = "game.cfg";

int HardwareManager::screenW = 320;
int HardwareManager::screenH = 200;
int HardwareManager::videoMode = GFX_AUTODETECT_FULLSCREEN; // screen desired graphic mode
int HardwareManager::colorDepth = 8;  // default color depth, -1 = autodetect from desktop default

bool HardwareManager::wantSound = true;
bool HardwareManager::wantMouse = false;
bool HardwareManager::wantJoystick = false;


// -----------------------------------------------------------------------
// HELPER : This will return true if the search_command is on the list of commands, false if not found
// -----------------------------------------------------------------------
bool HardwareManager::check_command( int argc, char *argv[], const char *search_command )
{
        for ( int i = 1; i < argc; i++ )
                if ( stricmp( argv[ i ], search_command ) == 0 )
                        return true; // found!

        return false;
}

// -----------------------------------------------------------------------
// INITS HARDWARE
// -----------------------------------------------------------------------
void HardwareManager::init(int argc, char *argv[])
{
        if (allegro_init() !=0) // init allegro
        {
                // Allegro failed!!
                // I can't use standard error routines... :(
                printf("ERROR:\nALLEGRO FAILED!\nThe world is doomed :P\n");
                exit(-1); // abort the program
        }

        // load configuration from config file
        if (configFilename != NULL)
        {
                push_config_state();
                set_config_file(configFilename);

                screenW = get_config_int("video", "screenW", screenW);
                screenH = get_config_int("video", "screenH", screenH);
                videoMode = get_config_int("video", "videoMode", videoMode);
                colorDepth = get_config_int("video", "colorDepth", colorDepth);

                wantSound = get_config_int("sound", "wantSound", wantSound);

                wantMouse = get_config_int("input", "wantMouse", wantMouse);
                wantJoystick = get_config_int("input", "wantJoystick", wantJoystick);

                pop_config_state();
        }

        // take care of command line parameters
        if (argc > 0 && argv != NULL)
        {
                /* DISABLED FOR THIS GAME

                if (check_command( argc, argv, "-w") || check_command( argc, argv, "-win") || check_command( argc, argv, "-windowed"))
                        videoMode = GFX_AUTODETECT_WINDOWED;

                if (check_command( argc, argv, "-bpp8"))
                        colorDepth = 8;

                if (check_command( argc, argv, "-bpp15"))
                        colorDepth = 15;

                if (check_command( argc, argv, "-bpp16"))
                        colorDepth = 16;

                if (check_command( argc, argv, "-bpp24"))
                        colorDepth = 24;

                if (check_command( argc, argv, "-bpp32"))
                        colorDepth = 32;
                */
                if (check_command( argc, argv, "-nosound"))
                        wantSound = false;

                if (check_command( argc, argv, "-nojoy") || check_command( argc, argv, "-nojoystick") )
                        wantJoystick = false;

                if (check_command( argc, argv, "-nomouse"))
                        wantMouse = false;

                // some extra resolutions -- DEBUG -- ADD TO HELP TOO IF ENABLED
                if ( check_command( argc, argv, "-800x600" ) )
                {
                        screenW = 800;
                        screenH = 600;
                }

                if ( check_command( argc, argv, "-1024x768" ) )
                {
                        screenW = 1024;
                        screenH = 768;
                }

                if ( check_command( argc, argv, "-640x480" ) )
                {
                        screenW = 640;
                        screenH = 480;
                }


                if ( check_command( argc, argv, "-320x200" ) )
                {
                        screenW = 320;
                        screenH = 200;
                }

                if ( check_command( argc, argv, "-log" ) )
                        Logger::enabled = true;

                // help message - DEBUG remember to add new commands
                if (check_command( argc, argv, "-help") || check_command( argc, argv, "--help") || check_command( argc, argv, "/?") || check_command( argc, argv, "/help"))
                        Error::error("HELP\nCommand line parameters\n\n-320x200 or -640x480 or 800x600 or -1024x768 resolutions, 8 bit color\n-nojoy no joystick\n-nosound no sound\n-log enable log file");

        }

        srand(time(0)); // init random numbers

        // -- video start
        // set up hardware
        /* DISABLED FOR THIS GAME
		int desk_bpp;
        desk_bpp = desktop_color_depth(); // using the same color depth as the host will make the game run faster

        

        if (desk_bpp != 8 && desk_bpp != 0 && colorDepth < 8)
                colorDepth = desk_bpp; // use color bpp of desktop

        if (colorDepth < 8) 
                colorDepth = 16; // desired color depth  */

		// FOR THIS MS DOS GAME, WE FORCE 8 BIT COLOR DEPTH -- WARNING DEBUG TODO
		// I NEED THE PALETTE EFFECTS
		colorDepth=8;

        set_color_depth(colorDepth);
        if (set_gfx_mode(videoMode, screenW, screenH, 0, 0))
        {
				// FAILED, DEFAULT TO 320X200x8 bpp -- MS DOS HACK for VGA -- DEBUG
				screenW = 320;
				screenH = 200;

                set_color_depth(8);
                if (set_gfx_mode(videoMode, screenW, screenH, 0, 0))
                {
                        set_color_depth(32);
                        if (set_gfx_mode(videoMode, screenW, screenH, 0, 0))
                        {
                                set_color_depth(24);
                                if (set_gfx_mode(videoMode, screenW, screenH, 0, 0))
                                {
                                        set_color_depth(16);
                                        if (set_gfx_mode(videoMode, screenW, screenH, 0, 0))
                                                Error::error("HardwareManager::init()\nI can't set the graphics mode (%d x %d @ %d bpp)\nI also tried with 8 bpp, 16 bpp, 32 bpp and 24 bpp", screenW, screenH, colorDepth);
                                }
                        }
                }
        }
        // -- end video

        if (install_timer() != 0)
                Error::error("HardwareManager::init()\nCan't install timer driver");

        if (install_keyboard() != 0)
                Error::error("HardwareManager::init()\nCan't install keyboard driver");
        
        if (wantMouse)
			if (install_mouse() < 0)
				Logger::log("Warning! Can't detect mouse.");
        
        if (wantJoystick)
                if (install_joystick(JOY_TYPE_AUTODETECT) != 0)
                        Logger::log("Warning! Can't detect joystick.");

        if (wantSound)
        {
                reserve_voices( 8, 0 );
                //set_volume_per_voice( 2 ); // warning - this may cause distortion
                if (install_sound( DIGI_AUTODETECT, MIDI_NONE, NULL ) != 0)
                        Logger::log("Warning! Can't detect sound system.");
                else
                        set_volume( 255, -1 );
        }

        // default blender mode
        set_trans_blender(128,128,128,128);

}

// -----------------------------------------------------------------------
// SHUTDOWNS HARDWARE
// -----------------------------------------------------------------------
void HardwareManager::shutdown()
{
        // save configuration

        push_config_state();
        set_config_file(configFilename);

        set_config_int("video", "screenW", screenW);
        set_config_int("video", "screenH", screenH);
        set_config_int("video", "videoMode", videoMode);
        set_config_int("video", "colorDepth", colorDepth);

        set_config_int("sound", "wantSound", wantSound);

        set_config_int("input", "wantMouse", wantMouse);
        set_config_int("input", "wantJoystick", wantJoystick);

        pop_config_state();

        // shutdown hardware
        // DEBUG - ALLEGRO TAKES CARE WHEN EXITING, I THINK
        // WE SHOULD GO TO TEXT MODE MAYBE?
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
}

