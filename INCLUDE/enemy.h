// -----------------------------------------------------------------------
// Base enemy
// -----------------------------------------------------------------------
// NOTE THIS IS A PURE VIRTUAL CLASS
// base enemy class , all enemies should derive from here
//
// NOTE implementation is at enemies.cpp
// -----------------------------------------------------------------------
#ifndef ENEMY_H
#define ENEMY_H

#include <allegro.h>

#include "map.h"
#include "datafile.h"
#include "particle.h"
#include "ufo.h" 

#include "collide.h"

#include "enemy.h"

#include "shoots.h"


// fordward declaration
class ShootsList;
class Shoot;
class UFO;

class Enemy
{
	public:
		Enemy(Datafile *data); // needs preloaded datafile with sprites, sound, etc
		virtual ~Enemy();
		
		// I receive the shoot list of enemies! to be able to add my own shoots
		// returns TRUE when enemy is dead and must be deleted
		virtual bool update(Map *m, UFO *ufo, ParticleManager *pm, ShootsList *shoots ) = 0; 
		
		virtual void render(BITMAP *bmp) = 0; // render in bmp

		// POSITION
		float x,y;
		float sx,sy;
		
		// LIFE LEFT
		int life;
		
		
		Collide2D *bbox; // bouding box for collisions
		
		BITMAP *sprite; // current sprite to draw
};

#endif