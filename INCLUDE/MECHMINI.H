// -----------------------------------------------------------------------
// mechmini.h
// -----------------------------------------------------------------------
// Mini mech flying enemy
// -----------------------------------------------------------------------
// Copyright (c) 2022, Kronoman
// In loving memory of my father.
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------
#ifndef MECHMINI_H
#define MECHMINI_H

#include <allegro.h>
#include "enemy.h"
#include "datafile.h"

class MechMini : public Enemy
{
	public:
		MechMini(Datafile *data); // needs preloaded datafile with sprites, sound, etc
		~MechMini();
	
		bool update(Map *m, UFO *ufo, ParticleManager *pm, ShootsList *shoots ); 

		void render(BITMAP *bmp);
		
		int frame; // sprite frame, goes in 1 increments 0,1,2
		int ia; // IA counter
		int shoot_c; // shoot counter
	
		static BITMAP *spr[4];	
		static SAMPLE *explosion_wav;
		static Datafile *spr_data;
		static int count;
};

#endif