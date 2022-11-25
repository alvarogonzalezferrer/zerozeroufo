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
	
	life = 250;
	
	// position outside screen 
	sx = -2;
	sy = 0;
	x = 321;
	y = 100;
	
	// bounding box 
	bbox = new Collide2D(x, y, sprite->w, sprite->h); // non damage area, only collision with UFO
	shoot_bbox = new Collide2D(x, y, sprite->w, sprite->h);
	
	collideWithUFO = true;

	face = true; // face left
	
	openfire = 0;
	
	Logger::log("BossHeli::BossHeli() -> OK");
}

BossHeli::~BossHeli()
{
	Logger::log("BossHeli::~BossHeli()");
	
	// release data 
	delete(bbox);
	delete(shoot_bbox);
	
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
		sx = 0.5;
	}
	
	if (x > 320 - sprite->w)
	{
		sx = -0.5;
	}
	
	if (y < 0)
	{
		sy = 0.5;
	}
	
	if (life > 0) // prevent ground crash only if alive
	{
		if (y > m->getHeight(x) - sprite->h - 5)
		{
			y = m->getHeight(x) - sprite->h - 5;
			sy = -0.5;
		}
	}
	
	x += sx;
	y += sy;
	
	// my middle point is quite used for AI
	int mx = x + sprite->w / 2; 
	int my = y + sprite->h / 2;
	
	// always face the player
	if (ufo->x < mx )
		face = true; // face left
	else 
		face = false;  // face right
	
	// AI
	openfire--;
	if (openfire > 0 && openfire % 4 == 0) // machine gun
	{		
		float ssx = (face) ? -2.5 : 2.5; // left or right?
		
		play_sample(shoot_wav, 255, x * 255 / 320, 900 + rand()%300, 0);
		
		shoots->add(new Shoot(mx, my,
							  ssx, 0, 
							  200, 
							  3,
							  0, 
							  7, 
							  makecol(255,85,85),
							  0));
		
		shoots->add(new Shoot(mx, my,
							  ssx, 1.3, 
							  200, 
							  3,
							  0, 
							  7, 
							  makecol(255,85,85),
							  0));
							  
		shoots->add(new Shoot(mx, my,
							  ssx, -1.3, 
							  200, 
							  3,
							  0, 
							  7, 
							  makecol(255,85,85),
							  0));
	}
	
	
	ai_c--;
	if (ai_c <0)
	{
		ai_c = 30 + rand()%30;
		
		// fly to opposite of screen of player
		if (ufo->x < 160 && x < 160)
			sx = 1.2;
		
		if (ufo->x > 160 && x > 160)
			sx = -1.2;
		
		// try to match in height the UFO 
		if (ufo->y < my)
			sy = -0.7;
		else 
			sy = 0.7;
		
		// open fire? 
		
		// shoot? 60% chance
		if (rand()%100 < 60)
		{
			openfire = 60; // engage machine gun
			ai_c = 90;
		}
		
		// deploy paratroopers or prizes
		if (enemyList)
		{
			// release some prize? 
			if (rand()%100<8)
			{
				if (rand()%2)
					enemyList->addEnemy(new WeaponPrize(mx, my, &enemyList->data));
				else
					enemyList->addEnemy(new HealthPrize(mx, my, &enemyList->data));
			}

			// deploy paratrooper
			if (rand()%100 < 35)
				enemyList->addEnemy(new Paratrooper(mx, my, &enemyList->data));
		}
	}
	
	// update bbox
	bbox->x = x;
	bbox->y = y;
	
	shoot_bbox->x = x;
	shoot_bbox->y = y;
	
	// collision with UFO!
	collide(ufo, pm);
	
	if (life < 0) // animate dead
	{
		sy = rand()%2+2;
		sx = 0;
		
		collideWithUFO = false;
		ai_c = 1000;
		life--;
		
		// add smoke
		int cmx = x + sprite->w/2 + rand()%8 - 4;
		int cmy = y + sprite->h/2 + rand()%8 - 4;

		int f = rand()%85+25; // smoke shade
		int cp = makecol(f,f,f); // smoke	
		pm->add(new Smoke(cmx, cmy, (((rand()%50)-50)/10.0), -(rand()%30)/10.0, rand()%5+7, cp, rand()%4+1, (rand()%100)/100.0));
		
		// add burn
		cp = makecol(170,0,0); // red	
		pm->add(new Smoke(cmx, cmy, (((rand()%50)-50)/10.0), -(rand()%30)/10.0, rand()%5+7, cp, rand()%4+1, (rand()%100)/100.0));

		// sparks when almost dead
		if (life < -50)
		{
			cp = makecol(255,255,85); // yellow
			int pz = rand()%5+5; // particle ammount
			for (int p=0; p<pz;p++)
				pm->add(new Spark(cmx, cmy, (rand()%100-50)/10.0, (rand()%20+20)/-10.0, rand()%15+10, cp));
			
			if (rand()%2)
				play_sample(explosion_wav, 200 + rand()%55, x * 255 / 320, 800+rand()%600, 0);
		}

		if (life < -95)
		{
			ufo->score++;
			play_sample(explosion_wav, 200 + rand()%55, x * 255 / 320, 800+rand()%600, 0);
		}

		
		/*if (y > 200 - sprite->h - 10)
			life -= 10; // speed up explosion when going out of screen*/
		
		if (life < -100)
			return true; // died 
	}
	
	return false; // alive
}

void BossHeli::render(BITMAP *bmp)
{	
	// we always face the enemy UFO
	if (face) // left or right?
		draw_sprite(bmp, sprite, (int)x, (int)y); // left
	else
		draw_sprite_h_flip(bmp, sprite, (int)x, (int)y);
}
