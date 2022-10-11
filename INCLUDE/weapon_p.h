// -----------------------------------------------------------------------
// weapon_p.h
// -----------------------------------------------------------------------
// Weapon power up prize 
// this is a special enemy, is a weapon prize, will increase weapon tier in UFO when colliding with UFO
// -----------------------------------------------------------------------
// Copyright (c) 2022, Kronoman
// In loving memory of my father.
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------
#ifndef WEAPON_P_H
#define WEAPON_P_H

#include <allegro.h>
#include "enemy.h"
#include "datafile.h"

class WeaponPrize : public Enemy
{
	public:
		WeaponPrize(Datafile *data); // needs preloaded datafile with sprites, sound, etc
		WeaponPrize(float x, float y, Datafile *data);
		~WeaponPrize();
	
		bool update(Map *m, UFO *ufo, ParticleManager *pm, ShootsList *shoots ); 

		void render(BITMAP *bmp);
		
		int frame; // sprite frame, goes in 1 increments 0,1,2 because is a chopper!

		int ia; // IA counter
	
		static BITMAP *spr[4];
		static SAMPLE *snd;
		static Datafile *spr_data;
		static int count;
};

#endif