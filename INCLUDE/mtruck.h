// -----------------------------------------------------------------------
// mtruck.h
// -----------------------------------------------------------------------
// missile truck enemy
// -----------------------------------------------------------------------
// Copyright (c) 2022, Kronoman
// In loving memory of my father.
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------
#ifndef MTRUCK_H
#define MTRUCK_H

#include <allegro.h>
#include "enemy.h"
#include "datafile.h"

class MissileTruck : public Enemy
{
	public:
		MissileTruck(Datafile *data); // needs preloaded datafile with sprites, sound, etc
		~MissileTruck();
	
		bool update(Map *m, UFO *ufo, ParticleManager *pm, ShootsList *shoots ); 

		void render(BITMAP *bmp);
		
		int frame; // sprite frame, goes in 10 increments 0..10..20..0

		int ia; // IA counter
		
		// for the "laser" sight effect
		int shoot_ia; // IA for shooting countdown
		
		int aim_x; // last x aim 
		int aim_y; // last y aim
	
		static BITMAP *mtruck[4];	
		static Datafile *enemy_data;
		static SAMPLE *explosion_wav;
		static int count;
};

#endif