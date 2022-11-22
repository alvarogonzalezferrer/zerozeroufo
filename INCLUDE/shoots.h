// -----------------------------------------------------------------------
// shoots.h
// -----------------------------------------------------------------------
// Shoots manager 
// Can be used for enemis or UFO
// -----------------------------------------------------------------------
// Copyright (c) 2022, Kronoman
// In loving memory of my father.
// Made in Argentina
// -----------------------------------------------------------------------
// useful notes: https://www.eventhelix.com/design-patterns/header-file-include/
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------
#ifndef SHOOTS_H
#define SHOOTS_H

#include <allegro.h>
#include "map.h"
#include "datafile.h"
#include "particle.h"
#include "collide.h"
#include "enemies.h"
#include "ufo.h" 
#include "boss.h"

// forward declaration 
class UFO;
class EnemyList;
class Map;

// -----------------------------------------------------------------------
// base shoot class , all new weapons should derive from here
//
// also handles the basic shoot types, so I dont have to create many classes (I know, bad design but Im in a hurry)
//
// -----------------------------------------------------------------------
class Shoot
{
	public:
		Shoot();
		
		// geometry constructor
		Shoot(float x, float y,
			  float sx, float sy,
			  int life,
			  int t, 
			  float grav, 
			  int damage,
			  int color,
			  int particle_trail);
		
		// for bitmap sprite shoots use this constructor
		// color will affect particle trail only
		Shoot(float x, float y,
			  float sx, float sy,
			  int life,
			  BITMAP *spr, 
			  float grav, 
			  int damage,
			  int color,
			  int particle_trail);
		
		virtual ~Shoot();
		
		// moves the shoots and test against enemies or UFO (any can be NULL to avoid friendly fire) 
		// also test against ground and screen limits
		// returns TRUE when shoot is dead and must be deleted
		virtual bool update(Map *m, ParticleManager *pm, UFO *ufo, EnemyList *enemies);
		
		virtual void render(BITMAP *bmp); // render in bmp

		// POSITION
		float x,y;
		float sx,sy;
		
		// life left
		int life ; 
		
		// FOR GEOMETRY SHOOTS, IF BITMAP IS USED, THIS IS OVERRIDED!! 
		int t; // shoot type 0=pixel 1=line , 2 and plus circle radius t-1 (i.e a 2 is a 1 px circle)
		
		float grav; // gravity
		
		// damage done to enemies or player
		int damage; 
		
		// color with makecol 
		int color;
		
		// particle trail? 0=none, 1=sparks, 2=circles, 3=smoke 
		int particle_trail;
		
		// BITMAP sprite , this OVERRIDES SHOOT TYPE
		BITMAP *sprite;
		
		Collide2D *bbox; // bouding box for collisions
				
};



// -----------------------------------------------------------------------
// Shoots list manager 
// Can be used for UFO or for enemies
// -----------------------------------------------------------------------
class ShootsList
{
	public:
		ShootsList();
		~ShootsList();

		void reset(); // clear all shoots, removing them from RAM too!
		
		// overloaded 
		void update(Map *m, ParticleManager *pm, UFO *ufo); // for enemies against UFO
		
		void update(Map *m, ParticleManager *pm, EnemyList *enemies, Boss *boss); // for UFO against enemies and boss 
		
		void update(Map *m, ParticleManager *pm, UFO *ufo, EnemyList *enemies, Boss *boss); // moves the shoots and test against enemies or UFO or boss (any can be NULL to avoid friendly fire)
		
		void add(Shoot *shoot); // add a shoot, will be auto deleted when life <= 0 or hit something
		
		void render(BITMAP *bmp);

		static const int maxSHOOTS = 500; // how many shoots at most at screen? 

		Shoot *shoots[maxSHOOTS];  // enemy container
		int shoots_n; // how much shoots we have now active
};

#endif