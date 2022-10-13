// -----------------------------------------------------------------------
// heli.h
// -----------------------------------------------------------------------
// common helicopter enemy
// -----------------------------------------------------------------------
// Copyright (c) 2022, Kronoman
// In loving memory of my father.
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------
#ifndef HELICOPTER_H
#define HELICOPTER_H

#include <allegro.h>
#include "enemy.h"
#include "datafile.h"

class Helicopter : public Enemy
{
	public:
		Helicopter(Datafile *data); // needs preloaded datafile with sprites, sound, etc
		~Helicopter();
	
		bool update(Map *m, UFO *ufo, ParticleManager *pm, ShootsList *shoots ); 

		void render(BITMAP *bmp);
		
		int frame; // sprite frame, goes in 1 increments 0,1,2 because is a chopper!

		int ia; // IA counter
	
		static BITMAP *heli[3];	
		static BITMAP *heli_mis; // missile
		static SAMPLE *explosion_wav;
		static Datafile *helicopter_data;
		static int count;
};

#endif