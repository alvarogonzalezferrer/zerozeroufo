// -----------------------------------------------------------------------
// ufo.h
// -----------------------------------------------------------------------
// UFO
// 
// This is the player's UFO
// -----------------------------------------------------------------------
// Copyright (c) 2022, Kronoman
// In loving memory of my father.
// Made in Argentina
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------
#ifndef UFO_H
#define UFO_H

#include <allegro.h>

#include "datafile.h"
#include "map.h"
#include "collide.h"
#include "particle.h"
#include "shoots.h" 
#include "enemies.h"

#include "control.h" // controller 

// fordward declaration for cross reference dependencies
class EnemyList;
class ShootsList;
class Shoot;
class Enemy;

class UFO 
{
	public:
		UFO();
		~UFO();
		
		void update(int maxW, Map *map, ParticleManager *pm, EnemyList *enemies);
		
		void render(BITMAP *bmp);
		
		void render_HUD(BITMAP *bmp); // render UFO's HUD (life, score, etc)
		
		// resets for next level and game
		void next_level_reset();
		
		void next_game_reset();
	
		// TODO change this to fixed math for 386s
		float x,y; // position
		float sx,sy; // speed

		int life;
		
		// BEAM
		int beam_color[3]; // beam colors for animation (this is a cache)
		
		int beam_size; // beam size from 0 to MAX 
		int beam_power; // beam power that I have left
		static const int MAX_BEAM_POWER=150; // max beam power to recharge, NOT MEASURED IN PIXELS, BUT IN GAME TICKS (30 = 1 SECOND)
 		
		// cache of position of beam (bounding box)
		int beam_x1;
		int beam_x2;
		int beam_y1;
		int beam_y2;
		int beam_xu; // x ufo middle point, attrack cows to this center
		
		int anm; // animation frame
		BITMAP *sprite; // current sprite 
		
		int weapon; // weapon power
		
		ShootsList *shoots; // UFO shoot list, is tested against enemies
		
		int shoot_recharge; // shoot recharge
		
		// data 
		Datafile data;
		
		BITMAP *ufo_bmp[4]; // sprites
		
		FONT *ufo_hud_font; // HUD font
		
		SAMPLE *ufo_laser_wav; // laser shoot
		
		// score
		unsigned long int score; 
		
		Collide2D *bbox; // bouding box for collisions
		
		// --- constants ---
		static const int MAX_LIFE=100;
		
		static constexpr float V_Y=0.3f; // vertical acceleration
		static constexpr float V_X=0.5f; // horizontal acceleration
		static constexpr float F_Y=0.1f; // vertical friction
		static constexpr float F_X=0.2f; // horizontal friction

		// max speed
		static constexpr float MV_Y=3.0f;
		static constexpr float MV_X=4.0f;
		
};



#endif
