// -----------------------------------------------------------------------
// soldat.h
// -----------------------------------------------------------------------
// common foot soldier enemy
// -----------------------------------------------------------------------
// Copyright (c) 2022, Kronoman
// In loving memory of my father.
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------
#ifndef SOLDAT_H
#define SOLDAT_H

#include <allegro.h>
#include "enemy.h"
#include "datafile.h"

class Soldat : public Enemy
{
	public:
		Soldat(Datafile *data); // needs preloaded datafile with sprites, sound, etc
		~Soldat();
	
		bool update(Map *m, UFO *ufo, ParticleManager *pm, ShootsList *shoots ); 

		void render(BITMAP *bmp);
		
		int frame; // sprite frame, goes in 10 increments 0..10..20..0

		int ia; // IA counter
	
		bool first_frame; // I need this for initial setup (is 1st frame that Im alive?)
		bool insideBeam; // Im inside UFO beam?
		
		static BITMAP *walk[3];
		static SAMPLE *scream;
		static Datafile *soldat_data;
		static int count;
};

#endif