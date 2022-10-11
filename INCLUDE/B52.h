// -----------------------------------------------------------------------
// B52.h
// -----------------------------------------------------------------------
// B52 paratrooper airplane enemy
// -----------------------------------------------------------------------
// Copyright (c) 2022, Kronoman
// In loving memory of my father.
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------
#ifndef B52_H
#define B52_H

#include <allegro.h>
#include "enemy.h"
#include "datafile.h"
#include "shoots.h" 
#include "enemies.h"

class B52 : public Enemy
{
	public:
		B52(Datafile *data); // needs preloaded datafile with sprites, sound, etc
		B52(float x, float y, Datafile *data);
		~B52();
	
		bool update(Map *m, UFO *ufo, ParticleManager *pm, ShootsList *shoots ); 

		void render(BITMAP *bmp);
		
		int frame; // sprite frame, goes in 1 increments 0,1,2 because is a airplane!

		int ia; // IA counter
		
		int jumps; // paratrooper jump counter 
		int machine_gun; // machine gun active
	
		static BITMAP *spr[3];	
		static SAMPLE *explosion_wav;
		static Datafile *spr_data;
		static int count;
		static EnemyList *enemyList;
};

#endif