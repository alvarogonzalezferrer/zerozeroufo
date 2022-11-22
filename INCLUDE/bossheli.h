// -----------------------------------------------------------------------
// bossheli.h
// -----------------------------------------------------------------------
// Helicopter #1 boss
// -----------------------------------------------------------------------
// Copyright (c) 2022, Kronoman
// In loving memory of my father.
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------

#ifndef BOSSHELI_H
#define BOSSHELI_H


#include <allegro.h>

#include "boss.h"
#include "datafile.h"

#include "enemies.h" // to spawn paratroopers


class BossHeli : public Boss
{
	public:
		BossHeli();
		~BossHeli();
	
		bool update(Map *m, UFO *ufo, ParticleManager *pm, ShootsList *shoots ); 

		void render(BITMAP *bmp);
		

		BITMAP *spr[4];	
		SAMPLE *shoot_wav;
		SAMPLE *explosion_wav;
		
		Datafile data;
				
		EnemyList *enemyList; // will spawn paratroopers, remember to set this when doing new boss !
};

#endif 