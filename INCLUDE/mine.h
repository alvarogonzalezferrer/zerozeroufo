// -----------------------------------------------------------------------
// mine.h
// -----------------------------------------------------------------------
// Mine floater 
// -----------------------------------------------------------------------
// Copyright (c) 2022, Kronoman
// In loving memory of my father.
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------
#ifndef MINE_H
#define MINE_H

#include <allegro.h>
#include "enemy.h"
#include "datafile.h"

class Mine : public Enemy
{
	public:
		Mine(Datafile *data); // needs preloaded datafile with sprites, sound, etc
		Mine(float x, float y, Datafile *data);
		~Mine();
	
		bool update(Map *m, UFO *ufo, ParticleManager *pm, ShootsList *shoots ); 

		void render(BITMAP *bmp);
		
		int frame; // sprite frame, goes in 1 increments 0,1,2

		int ia; // IA counter
	
		static BITMAP *spr[2];	
		static Datafile *spr_data;
		static int count;
};

#endif