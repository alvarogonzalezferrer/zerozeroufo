// -----------------------------------------------------------------------
// biplane.h
// -----------------------------------------------------------------------
// old school biplane enemy, WW1 style
// -----------------------------------------------------------------------
// Copyright (c) 2022, Kronoman
// In loving memory of my father.
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------
#ifndef BIPLANE_H
#define BIPLANE_H

#include <allegro.h>
#include "enemy.h"
#include "datafile.h"

class Biplane : public Enemy
{
	public:
		Biplane(Datafile *data); // needs preloaded datafile with sprites, sound, etc
		~Biplane();
	
		bool update(Map *m, UFO *ufo, ParticleManager *pm, ShootsList *shoots ); 

		void render(BITMAP *bmp);
		
		int frame; // sprite frame
		
		int ia; // IA counter
			
		static BITMAP *spr[3];	
		static Datafile *spr_data;
		static int count;
};

#endif