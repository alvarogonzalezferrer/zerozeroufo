// -----------------------------------------------------------------------
// b17.h
// -----------------------------------------------------------------------
// B17 bomber enemy
// -----------------------------------------------------------------------
// Copyright (c) 2022, Kronoman
// In loving memory of my father.
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------
#ifndef B17_H
#define B17_H

#include <allegro.h>
#include "enemy.h"
#include "datafile.h"
#include "shoots.h" 
/************************ disabled for better approach 
I INTEGRATED THIS CODE INTO THE SHOOT MAIN CLASS, SO MANY ENEMIES CAN USE SPRITES ON THEIR SHOOTS 

// B17 bomb - special weapon , has a sprite
class BombB17 : public Shoot
{
	public: 
		BombB17(float x, float y,
				float sx, float sy,
				int life,
				int t, 
				float grav, 
				int damage,
				int color,
				int particle_trail,
				BITMAP *sprite);
		
	    ~BombB17();
	
		bool update(Map *m, ParticleManager *pm, UFO *ufo, EnemyList *enemies);

		void render(BITMAP *bmp);

		
		BITMAP *spr;	
};

************************************************/

class B17 : public Enemy
{
	public:
		B17(Datafile *data); // needs preloaded datafile with sprites, sound, etc
		B17(float x, float y, Datafile *data);
		~B17();
	
		bool update(Map *m, UFO *ufo, ParticleManager *pm, ShootsList *shoots ); 

		void render(BITMAP *bmp);
		
		int frame; // sprite frame, goes in 1 increments 0,1,2 because is a airplane!

		int ia; // IA counter
		
		int bomb_run; // bomb run 
		int machine_gun; // machine gun active
	
		static BITMAP *spr[3];	
		static BITMAP *spr_bomb; // bomb sprite
		static SAMPLE *explosion_wav;
		static Datafile *spr_data;
		static int count;
};

#endif