// -----------------------------------------------------------------------
// apc.h
// -----------------------------------------------------------------------
// armored personal carrier enemy
// -----------------------------------------------------------------------
// Copyright (c) 2022, Kronoman
// In loving memory of my father.
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------
#ifndef APC_H
#define APC_H

#include <allegro.h>
#include "enemy.h"
#include "datafile.h"

class APC : public Enemy
{
	public:
		APC(Datafile *data); // needs preloaded datafile with sprites, sound, etc
		~APC();
	
		bool update(Map *m, UFO *ufo, ParticleManager *pm, ShootsList *shoots ); 

		void render(BITMAP *bmp);
		
		int frame; // sprite frame, goes in 10 increments 0..10..20..0

		int ia; // IA counter
		
		int shoot_ia; // IA for shooting countdown
		
	
		static BITMAP *spr[4];	
		static Datafile *enemy_data;
		static int count;
};

#endif