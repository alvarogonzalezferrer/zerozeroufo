// -----------------------------------------------------------------------
// Base boss
// -----------------------------------------------------------------------
// NOTE THIS IS A PURE VIRTUAL CLASS
// base BOSS class , all bosses should derive from here
//
// -----------------------------------------------------------------------
#ifndef BOSS_H
#define BOSS_H

#include <allegro.h>

#include "map.h"
#include "datafile.h"
#include "particle.h"
#include "ufo.h" 

#include "collide.h"

#include "shoots.h"


// forward declaration
class ShootsList;
class Shoot;
class UFO;

class Boss
{
	public:
		Boss();
		virtual ~Boss();
		
		// I receive the shoot list of enemies! to be able to add my own shoots
		// I receive the UFO to be able to track him
		// returns TRUE when enemy is dead and must be deleted
		virtual bool update(Map *m, UFO *ufo, ParticleManager *pm, ShootsList *shoots ) = 0; 
		
		virtual void render(BITMAP *bmp) = 0; // render in bmp

		// position & direction
		float x,y;
		float sx,sy;
		
		// LIFE LEFT
		int life;
		
		// collide with UFO ? some enemies do not collide against UFO, default TRUE
		bool collideWithUFO;
		
		// we have two collisions
		// one is the hot spot to receive shoots from player 
		// two is a general collision that does not damage the boss
		// can be NULL if not needed
		Collide2D *shoot_bbox; // bounding box for collisions against enemy shoots, bosses have a "hot spot" for shoots 
		Collide2D *bbox; // bounding box for collisions in general, no damage taken in this area
		
		// animation in general 
		
		BITMAP *sprite; // current sprite to draw
		int frame; // current frame 
		int ai_c; // AI counter for state machine
};

#endif