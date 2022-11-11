// ---------------------------------------------------------------------------
// maingloop.h
// -----------------------------------------------------------------------
// Main Game Loop
// Based on my generic game machine
// -----------------------------------------------------------------------
// Copyright (c) 2007-2022, Kronoman
// In loving memory of my father.
// Made in Argentina
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------
#ifndef MAINGLOOP_H
#define MAINGLOOP_H

#include "game.h"
#include "map.h"
#include "weather.h"
#include "ufo.h"
#include "cow.h"
#include "particle.h"
#include "datafile.h"
#include "enemies.h"
#include "parallax.h"

// internal game resolution (not video screen)
#define GAME_W_RES 320
#define GAME_H_RES 200

class MainGameLoop : public GameMachine
{
	public:
		MainGameLoop();
		~MainGameLoop();
        
		void update(); // updates the game, will return states if needed...
		void render(); // renders what is happening

		static const unsigned int STATE_NEXTLEVEL = 2; // generates next level 
		static const unsigned int STATE_INTRONEXTLEVEL = 3; // next level intro screen
		static const unsigned int STATE_INTRO = 4;  // intro screen
		static const unsigned int STATE_GAMEOVER = 5;  // gameover screen
		static const unsigned int STATE_FBI = 6; // fbi splash screen
		static const unsigned int STATE_KRONOMAN = 7; // kronoman splash
		static const unsigned int STATE_WAIT_FOR_NEXT_LEVEL = 8; // when you finish the level , there is a little pause before changing wave		
		
		// menus, main and in game 
		static const unsigned int STATE_MAIN_MENU = 9;
		static const unsigned int STATE_INGAME_MENU = 10;
		
		int wait; // delay for various screen pauses...
		int camerashake; // delay for camera shake effects...

		// this would be the 'world'
		Map *map; // map
		
		UFO *ufo; // player
		
		CowList cowL; // cow list 
		
		EnemyList *enemyList; // enemy list
		
		Weather weather; // weather of the map
		
		ParticleManager *pm; // particle system
		
		Parallax *parallax; // parallax background
		
		unsigned long int ticks; // how many game ticks (30 in a second) , useful to count next level, resets to 0 on each new level

		int wave; // current wave?
		
		int ufo_last_life; // to check if UFO received damage and shake camera
		
		// destroy message
		// this message shows up when there is 1 enemy left and player takes too long to kill it
		int x_destroy_m;
		int y_destroy_m;
		int t_destroy_m; // timer, -1 dont show

		// data stuff
		Datafile intro_screens_data; // screens for intro / next level / etc
		
		PALETTE *ega_pal; // EGA palette
		RGB_MAP rgb_table; // RGB lookup table for colors

		BITMAP *intro_bmp; // intro image
		BITMAP *next_wave_bmp; // next wave screen
		
		// some screens
		BITMAP *won_bmp; // you won!
		BITMAP *fbi_bmp; // fbi message
		BITMAP *gameover_bmp; // game over
		BITMAP *help_bmp; // help screen
		
		BITMAP *kronoman_bmp; // kronoman logo
		SAMPLE *kronoman_wav; // kronoman sound
		
		int help_show; // frames of help info?
		
		// debug info?
		bool debug;
		
};

#endif

