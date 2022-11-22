// helicopter boss 

#include "bossheli.h"

#include "shoots.h"
#include "logger.h"
#include "randomiz.h"
#include "map.h"

// this two so I can drop paratroopers
#include "enemies.h"
#include "paratroo.h"

// to release prizes
#include "weapon_p.h"
#include "health_p.h"

BossHeli::BossHeli()
{
	Logger::log("BossHeli::BossHeli()");
	
	// load my data
	data.load("bossheli.dat"); 
	spr[0] = (BITMAP *)data.getObject("BOSSHELI0")->dat;
	spr[1] = (BITMAP *)data.getObject("BOSSHELI1")->dat;
	spr[2] = (BITMAP *)data.getObject("BOSSHELI2")->dat;
	spr[3] = (BITMAP *)data.getObject("BOSSHELI3")->dat;
	
	shoot_wav = (SAMPLE *)data.getObject("SHOOT_WAV")->dat;
	explosion_wav = (SAMPLE *)data.getObject("EXPLOSION_WAV")->dat;
	
	
	// init
	sprite = spr[0];
	frame = 0;
	ai_c = 0;
	
	life = 1000;
	
	// position outside screen 
	sx = -2;
	sy = 0;
	x = 360;
	y = 100;
	
	// bounding box 
	bbox = NULL;
	shoot_bbox = NULL;
	
	collideWithUFO = true;

	face = false; // face left
}

BossHeli::~BossHeli()
{
	Logger::log("BossHeli::~BossHeli()");
	// release data 
	data.unload(); // unload datafile
}


bool BossHeli::update(Map *m, UFO *ufo, ParticleManager *pm, ShootsList *shoots )
{
	frame++; // animate
	if (frame > 3)
		frame = 0;
	
	sprite = spr[frame];
	
	// check boundaries 
	if (x < 0)
	{
		x = 0;
		sx = 0.2;
	}
	
	if (x > 320+sprite->w)
	{
		x = 320+sprite->w;
		sx = -0.2;
	}
	
	if (y < 0)
	{
		y = 0;
		sy = 0.2;
	}
	
	if (y > m->getHeight(x) - sprite->h - 5)
	{
		y = m->getHeight(x) - sprite->h - 5;
		sy = -0.2;
	}
	
	x += sx;
	y += sy;
	
	// AI
	ai_c--
	if (ai_c <0)
	{
		// do something DEBUG
		
		ai_c = 30;
	}
	
	return false; 
}

void BossHeli::render(BITMAP *bmp)
{
	if (life > 0)
	{
		// we always face the enemy UFO
		if (face) // left or right?
			draw_sprite(bmp, sprite, (int)x, (int)y); // left
		else
			draw_sprite_h_flip(bmp, sprite, (int)x, (int)y);
	}
	else
		 rotate_sprite(bmp, sprite, (int)x, (int)y, itofix(rand()%255)); // tumble falling
}
