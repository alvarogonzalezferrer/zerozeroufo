// -----------------------------------------------------------------------
// jetpack.h
// -----------------------------------------------------------------------
// jetpack soldier enemy
// -----------------------------------------------------------------------
// Copyright (c) 2022, Kronoman
// In loving memory of my father.
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------
#ifndef JETPACK_H
#define JETPACK_H

#include <allegro.h>
#include "enemy.h"
#include "datafile.h"

class Jetpack : public Enemy
{
	public:
		Jetpack(Datafile *data); // needs preloaded datafile with sprites, sound, etc
		~Jetpack();
	
		bool update(Map *m, UFO *ufo, ParticleManager *pm, ShootsList *shoots ); 

		void render(BITMAP *bmp);
		
		int frame; // sprite frame, goes in 1 increments 0,1,2
		int ia; // IA counter
		bool insideBeam; // Im inside UFO beam?
		
		static BITMAP *spr[3];
		static SAMPLE *scream;
		static SAMPLE *explosion;
		static Datafile *spr_data;
		static int count;
};

#endif