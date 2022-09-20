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

		static const unsigned int STATE_NEXTLEVEL = 2;
		static const unsigned int STATE_INTRONEXTLEVEL = 3;
		static const unsigned int STATE_INTRO = 4; 
		static const unsigned int STATE_GAMEOVER = 5; 
		static const unsigned int STATE_FBI = 6; 

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
		
		
		int help_show; // frames of help info?
		
		// debug info?
		bool debug;
		
};

#endif

