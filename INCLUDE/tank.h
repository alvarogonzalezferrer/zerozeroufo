// -----------------------------------------------------------------------
// tank.h
// -----------------------------------------------------------------------
// common tank enemy
// -----------------------------------------------------------------------
// Copyright (c) 2022, Kronoman
// In loving memory of my father.
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------
#ifndef TANK_H
#define TANK_H

#include <allegro.h>
#include "enemy.h"
#include "datafile.h"

class Tank : public Enemy
{
	public:
		Tank(Datafile *data); // needs preloaded datafile with sprites, sound, etc
		~Tank();
	
		bool update(Map *m, UFO *ufo, ParticleManager *pm, ShootsList *shoots ); 

		void render(BITMAP *bmp);
		
		int frame; // sprite frame, goes in 10 increments 0..10..20..0

		int ia; // IA counter
	
		static BITMAP *tank[4];	
		static SAMPLE *explosion_wav;
		static Datafile *tank_data;
		static int count;
};

#endif