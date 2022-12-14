// -----------------------------------------------------------------------
// icbm.h
// -----------------------------------------------------------------------
// ICBM missile 
// -----------------------------------------------------------------------
// Copyright (c) 2022, Kronoman
// In loving memory of my father.
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------
#ifndef ICBM_H
#define ICBM_H

#include <allegro.h>
#include "enemy.h"
#include "datafile.h"

class ICBM : public Enemy
{
	public:
		ICBM(Datafile *data); // needs preloaded datafile with sprites, sound, etc
		ICBM(float x, float y, Datafile *data);
		~ICBM();
	
		bool update(Map *m, UFO *ufo, ParticleManager *pm, ShootsList *shoots ); 

		void render(BITMAP *bmp);
		
		int frame; // sprite frame, goes in 1 increments 0,1,2

		int ia; // IA counter
	
		static BITMAP *spr[2];	
		static Datafile *spr_data;
		static int count;
		static SAMPLE *explosion_wav;
};

#endif