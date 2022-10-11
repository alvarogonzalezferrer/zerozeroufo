// -----------------------------------------------------------------------
// paratrooper.h
// -----------------------------------------------------------------------
// is a paratrooper that behaves into a common soldier
// -----------------------------------------------------------------------
// Copyright (c) 2022, Kronoman
// In loving memory of my father.
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------
#ifndef PARATROOPER_H
#define PARATROOPER_H

#include <allegro.h>
#include "enemy.h"
#include "datafile.h"

class Paratrooper : public Enemy
{
	public:
		Paratrooper(Datafile *data); // needs preloaded datafile with sprites, sound, etc
		Paratrooper(float x, float y, Datafile *data);
		~Paratrooper();
	
		bool update(Map *m, UFO *ufo, ParticleManager *pm, ShootsList *shoots ); 

		void render(BITMAP *bmp);
		
		int frame; // sprite frame, goes in 10 increments 0..10..20..0

		int ia; // IA counter
	
		bool airborne; // I still have parachute?
		bool insideBeam; // Im inside UFO beam?
		
		static BITMAP *parachute[5]; // first 3 frames are parachute (0,1,2), 3,4 are parachute broken by UFO fire 
		static BITMAP *walk[3]; // walking when reaches ground
		static SAMPLE *scream;
		static Datafile *p_data;
		static int count;
};

#endif