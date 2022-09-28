// -----------------------------------------------------------------------
// enemies.h
// -----------------------------------------------------------------------
// Enemies manager
// -----------------------------------------------------------------------
// Copyright (c) 2022, Kronoman
// In loving memory of my father.
// Made in Argentina
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------
#ifndef ENEMIES_H
#define ENEMIES_H

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
class Enemy;
class UFO;

// -----------------------------------------------------------------------
// Enemy list manager 
// -----------------------------------------------------------------------
class EnemyList
{
	public:
		EnemyList();
		~EnemyList();

		void generate(int n, int t, Map *m); // generate i enemies, of max tier t
		
		bool addEnemy(Enemy *e); // add enemy e to active enemy list, will DELETE it IF list is full, no need to manage this, returns TRUE if added, FALSE if list is full 
		
		// I need to receive UFO to test against my own shoots and test against theirs, ETC
		int update(Map *m, UFO *ufo, ParticleManager *pm); // returns how many enemies are alive yet... DEBUG SHOULD RECEIVE PARTICLE AND UFO SHOOT LIST TOO!
		
		// render enemies AND SHOOTS
		void render(BITMAP *bmp);

		void reset(); // remove all enemies, i.e level change, WILL RESET SHOOTS!

		static const int maxENEMIES = 8; // how many enemies at most at screen? not too much or screen gets too spammed

		Enemy *enemy[maxENEMIES];  // enemy container
		
		ShootsList *shootsE; // enemy shoot list, is tested against UFO
		
		int enemies_n; // how many enemies we have now
		
		
		// sprites and sound data for enemies set
		// WARNING, BOSSES LOAD THEIR OWN DATA SET THAT IS UNLOADED AFTER BOSS DEFEAT, TO KEEP RAM USAGE LOW
		Datafile data;
};

#endif