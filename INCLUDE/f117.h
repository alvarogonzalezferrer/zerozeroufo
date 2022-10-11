// -----------------------------------------------------------------------
// F117.h
// -----------------------------------------------------------------------
// F117 enemy, fast jet
// -----------------------------------------------------------------------
// Copyright (c) 2022, Kronoman
// In loving memory of my father.
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------
#ifndef F117_H
#define F117_H

#include <allegro.h>
#include "enemy.h"
#include "datafile.h"

class F117 : public Enemy
{
	public:
		F117(Datafile *data); // needs preloaded datafile with sprites, sound, etc
		~F117();
	
		bool update(Map *m, UFO *ufo, ParticleManager *pm, ShootsList *shoots ); 

		void render(BITMAP *bmp);
		
		int frame; // sprite frame
		
		int ia; // IA counter
			
		static BITMAP *spr[3];	
		static SAMPLE *explosion_wav;
		static Datafile *spr_data;
		static int count;
};

#endif