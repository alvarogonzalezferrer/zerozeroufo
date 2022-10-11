// f177 enemy

#include "f117.h"
#include "logger.h"
#include "enemy.h"
#include "shoots.h"
#include "randomiz.h"

// this two so the pilot can eject
#include "enemies.h"
#include "paratroo.h"
// to release prizes
#include "weapon_p.h"
#include "health_p.h"

// static data for all the objects
BITMAP *F117::spr[3];
SAMPLE *F117::explosion_wav = NULL;
Datafile *F117::spr_data = NULL;
int F117::count = 0;

EnemyList *F117::enemyList = NULL; // will spawn a "pilot" when going down, remeber to set this when creating this enemy!!


F117::F117(Datafile *data) : Enemy(data)
{
	Logger::log("F117::F117()");
	
	x = y = sx = sy = 0; // DEBUG - SHOULD BE POSITIONED BY A PROPER CONSTRUCTOR!
	
	if (spr_data != data)
	{
		spr_data = data;
		
		spr[0] = (BITMAP *)data->getObject("F117_0_BMP")->dat;
		spr[1] = (BITMAP *)data->getObject("F117_1_BMP")->dat;
		spr[2] = (BITMAP *)data->getObject("F117_2_BMP")->dat;
		
		explosion_wav = (SAMPLE *)data->getObject("EXPLOSION_WAV")->dat;
	}
	count++;
	
	sprite = spr[0];
	frame = rand()%3;
	ia = 0;
	
	life = 40;
	
	sx = -1.5f;  // always go left, then will bounce on right
	
	jumped = false; // pilot did not jump yet
		
	bbox = new Collide2D(x, y, sprite->w, sprite->h);
}


F117::~F117()
{
	// NOTE: do NOT release data in RAM, is handled somewhere else!!
	Logger::log("F117::~F117()");
	delete(bbox);
	count--;
}


bool F117::update(Map *m, UFO *ufo, ParticleManager *pm, ShootsList *shoots )
{
	// move
	x += sx;
	y += sy;
	
	// animate
	frame++;
	if (frame > 2) // 0,1,2 spr frame
		frame=0;
	
	sprite = spr[frame];
	
	
	// IA
	// easy state machine, when reaches 0, choose another action
	ia--;
	if (ia<0)
	{
		// move
		ia = 30; // frames to do the same?
		
		sy = Randomize::random(-1.2f, 1.2f);

		// shoot? 80% chance
		if (rand()%100 < 80)
		{			
			float ssx = (sx <= 0) ? -2.5 : 2.5; // left or right?
			float sx = x + sprite->w / 2;
			float sy = y + sprite->h / 2;
			
			shoots->add(new Shoot(sx, sy,
								  ssx, 0, 
								  200, 
								  2,
								  0.0, 
								  5, 
								  makecol(255,255,85),
								  0));
			
			shoots->add(new Shoot(sx, sy,
								  ssx, 1.3, 
								  200, 
								  2,
								  0.0, 
								  3, 
								  makecol(255,255,85),
								  0));
		}
	}
	
	// bounce on borders 

	// side right should not clip since i can come from outside screen!
	if  (x > m->mapW + sprite->w)
	{
		sx = -1.5f; // go left
		ia = 30;
	}
	
	if (x < -sprite->w*2) // im gone by left
	{
		// go right
		sx = 1.5f;
		ia = 30;
		
		// change altitude
		y = rand()%150;
		
		if (rand()%10 < 3)
			return true; // abandon battle
	}
	
	// dont crash on ground! unless im dead 
	if (y > m->getHeight(x) - sprite->h*3 && life >0)
	{
		sy = -1.5f;
		ia = 30;
		y  = m->getHeight(x) - sprite->h*3;
	}
	
	if (y < 0)
	{
		y  = 0;
		sy = 1.5f; // dont fly higher
		ia = 45;
	}

	// update bounding box collision detection
	bbox->x = x;
	bbox->y = y;
	
	// ---- last line still alive ----
	if (life > 0) 
		return false ; // im still alive
	
	// ----- dying sequence ------
	// go down in flames
	ia = 1000; // disable IA next step
	life--; // keep going below 0
	sy = abs(sy) + .3; // go down fast
	
	// eject 
	if (!jumped)
	{
		if (enemyList)
		{
			// release some prize? 
			if (rand()%10<6)
			{
				if (rand()%2)
					enemyList->addEnemy(new WeaponPrize(x, y, spr_data));
				else
					enemyList->addEnemy(new HealthPrize(x, y, spr_data));
			}

			// pilot eject
			enemyList->addEnemy(new Paratrooper(x, y, spr_data));
		}
		jumped = true; 
	}
	
	// dont crash below ground
	if (y > m->getHeight(x) - sprite->h+10)
	{
		y = m->getHeight(x) - sprite->h+10;
		sy = 0;
		life = -100; // die on crash
	}
	
	// smoke and burn
	int cmx = x + sprite->w/2 + rand()%8 - 4;
	int cmy = y + sprite->h/2 + rand()%8 - 4;

	// add some smoke particles	
	int f = rand()%85+25; // smoke shade
	int cp = (rand()%3 < 1) ? makecol(f,f,f) : makecol(f,0,0); // smoke or burn 
	pm->add(new Smoke(cmx, cmy, ((rand()%25)/10.0)*(-sx), -(rand()%20)/10.0, rand()%4+5, cp, rand()%3+1, (rand()%100)/100.0));
	
	// explosion when almost dead
	if (life < -50)
	{
		cp = makecol(255,255,85); // yellow
		int pz = rand()%5+10; // particle ammount
		for (int p=0; p<pz;p++)
			pm->add(new Spark(cmx, cmy, (rand()%60-30)/10.0, (rand()%10+20)/-10.0, rand()%15+10, cp));
	}
	
	if (life > -60)
		return false; // im still burning
	
	// dead for good
	ufo->score++;
	play_sample(explosion_wav, 200 + rand()%55, x * 255 / 320, 800+rand()%600, 0);
	return true; // im dead
}	

void F117::render(BITMAP *bmp)
{
	if (life > 0)
	{
		if (sx <= 0) // left or right?
			draw_sprite(bmp, sprite, (int)x, (int)y); // left
		else
			draw_sprite_h_flip(bmp, sprite, (int)x, (int)y);
	}
	else
		 rotate_sprite(bmp, sprite, (int)x, (int)y, itofix(rand()%255)); // tumble falling
}
