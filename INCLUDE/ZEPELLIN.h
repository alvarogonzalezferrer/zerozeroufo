// -----------------------------------------------------------------------
// zepellin.h
// -----------------------------------------------------------------------
// zepellin enemy
// -----------------------------------------------------------------------
// Copyright (c) 2022, Kronoman
// In loving memory of my father.
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------
#ifndef ZEPELLIN_H
#define ZEPELLIN_H

#include <allegro.h>
#include "enemy.h"
#include "datafile.h"

class Zepellin : public Enemy
{
	public:
		Zepellin(Datafile *data); // needs preloaded datafile with sprites, sound, etc
		~Zepellin();
	
		bool update(Map *m, UFO *ufo, ParticleManager *pm, ShootsList *shoots ); 

		void render(BITMAP *bmp);
		
		int frame; // sprite frame, goes in 1 increments 0,1
		int ia; // IA counter
	
		static BITMAP *spr[2];	
		static Datafile *spr_data;
		static SAMPLE *explosion_wav;
		static int count;
};

#endif