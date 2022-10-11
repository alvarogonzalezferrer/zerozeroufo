// -----------------------------------------------------------------------
// health_h.h
// -----------------------------------------------------------------------
// Health power up prize 
// this is a special "enemy", will add health when colliding with player
// -----------------------------------------------------------------------
// Copyright (c) 2022, Kronoman
// In loving memory of my father.
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------
#ifndef HEALTH_P_H
#define HEALTH_P_H

#include <allegro.h>
#include "enemy.h"
#include "datafile.h"

class HealthPrize : public Enemy
{
	public:
		HealthPrize(Datafile *data); // needs preloaded datafile with sprites, sound, etc
		HealthPrize(float x, float y, Datafile *data);
		~HealthPrize();
	
		bool update(Map *m, UFO *ufo, ParticleManager *pm, ShootsList *shoots ); 

		void render(BITMAP *bmp);
		
		int frame; // sprite frame, goes in 1 increments 0,1,2 

		int ia; // IA counter
	
		static BITMAP *spr[4];	
		static SAMPLE *snd;
		static Datafile *spr_data;
		static int count;
};

#endif