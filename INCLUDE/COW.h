// -----------------------------------------------------------------------
// cow.h
// -----------------------------------------------------------------------
// Cows
//
// The objetive of the game is to abduct cows 
//
// -----------------------------------------------------------------------
// Copyright (c) 2022, Kronoman
// In loving memory of my father.
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------
#ifndef COW_H
#define COW_H

#include <allegro.h>
#include "map.h"
#include "datafile.h"
#include "ufo.h" 
#include "particle.h"

class Cow 
{
	public:
		Cow();
		~Cow();
		
		void update(Map *m, UFO *ufo, ParticleManager *pm);
		void render(BITMAP *bmp);
	
		// x,y and speed x,y
		float x,y;
		float sx,sy;

		int life;
		int respawn;
		
		
		bool insideBeam; // im inside UFO's beam?
		
		int anm; // animation frame
		BITMAP *sprite; // current sprite
	
		static SAMPLE *cow_wav; // cow moo
		static BITMAP *walk[4];

		static Datafile data;
		static int count; // how many active, to load and release data mainly
};

class CowList
{
	public:
		CowList();
		~CowList();

		void generate(int i); // generate i cows
		
		int update(Map *m, UFO *ufo, ParticleManager *pm); // returns how many cows are alive yet...
		void render(BITMAP *bmp);
		
		static const int maxCOWS = 10; // how many cows at most at screen? 

		Cow *cow[maxCOWS];
		int cows_n; // how much cows we have now
};

#endif