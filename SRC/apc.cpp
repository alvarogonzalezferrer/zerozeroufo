// armored personal carrier enemy

#include "apc.h"
#include "logger.h"
#include "enemy.h"

// static data for all the objects
BITMAP *APC::spr[4];
Datafile *APC::enemy_data = NULL;
int APC::count = 0;


APC::APC(Datafile *data) : Enemy(data)
{
	Logger::log("APC::APC()");
	
	x = y = sx = sy = 0; // remember to position - TODO make a proper constructor!!
	
	life = 65;
	
	//if (count == 0) // first spr, get sprites and sounds 
	if (enemy_data != data)
	{
		enemy_data = data;
		
		spr[0] = (BITMAP *)data->getObject("APC_0_BMP")->dat;
		spr[1] = (BITMAP *)data->getObject("APC_1_BMP")->dat;
		spr[2] = (BITMAP *)data->getObject("APC_2_BMP")->dat;
		spr[3] = (BITMAP *)data->getObject("APC_3_BMP")->dat;

		// TODO - add sounds
	}
	count++;
	
	sprite = spr[0];
	frame = rand()%40;
	ia = 0;
	shoot_ia = 0;
	
	bbox = new Collide2D(x, y, sprite->w, sprite->h);
}


APC::~APC()
{
	Logger::log("APC::~APC()");
	
	delete(bbox);
	
	count--;
}


bool APC::update(Map *m, UFO *ufo, ParticleManager *pm, ShootsList *shoots )
{
	// move
	x += sx;
	y += sy; // useless, I set y on map, always
	
	// set on map 
	y = m->getHeight(x + sprite->w / 2) - sprite->h + 2;
	
	// animate
	frame++;
	
	if (frame > 39) // 0...10...20...30 (0,1,2,3)
		frame=0;
	
	sprite = spr[frame/10];
	
	
	// IA
	// easy state machine, when reaches 0, choose another action
	if (shoot_ia > 0)
	{
		shoot_ia--;
		
		// when shoot ia is < 20 , shoot in rapid fire each 4 frames	
		if (shoot_ia % 4 == 0)
		{
			// double shoot
			float shoot_dx = 0;
			
			if (ufo->x > x)
				shoot_dx = 4;
			else
				shoot_dx = -4;
			
			float delta = -2; 
								
			shoots->add(new Shoot(x + sprite->w/2, y,
								shoot_dx, delta, 
								100, 
								3, 
								0.02, 
								4,
								makecol(255,255,85),
								0));
			delta = -3;
			shoots->add(new Shoot(x + sprite->w/2, y,
								shoot_dx, delta, 
								100, 
								3, 
								0.02, 
								4, 
								makecol(255,255,85),
								0));								
		}
	}
	
	
	ia--;
	if (ia<0)
	{
		ia = rand()%80+40; // frames to do the same?
		sx = (rand()%40-20) / 10.0; // spr left or right?
		sy = 0;

		// shoot? 50% chance
		if (rand()%100 < 50)
		{	
			// setup the shoot ia counter
			shoot_ia = 40; 

			// stand still
			ia = 60;
			sx = sy = 0;
		}
	}
	
	// bounce on borders 
	// I go a little out of screen before coming back
	
	// side right should not clip since i can come from outside screen!
	if  (x > m->mapW - sprite->w)
	{
		sx = -2; // go left
		ia = rand()%10+30;
		shoot_ia = 0; // do not track player from outside screen
	}
	
	if (x < 0)
	{
		x = 0;
		sx = 2 + rand()%3; // go right
		ia = rand()%10+10;
		
		if (rand()%10 < 3)
			return true; // disappear for good, ran away from battle
	}
	
	// update bounding box collision detection
	bbox->x = x;
	bbox->y = y;

	// --- end of IA ---
	if (life > 0) 
		return false ; // im still alive

	// IF IM STLL ALIVE, THIS CODE NEVER HAPPENS
	// -- DYING ZONE ANIMATION -- 
	// burn for 2 seconds 
	sx = sy = 0; // dont move
	life--;
	ia = 1000; // disable ia
	shoot_ia = 0;
	
	// add smoke
	int cmx = x + sprite->w/2 + rand()%8 - 4;
	int cmy = y + sprite->h/2 + rand()%8 - 4;

	int f = rand()%85+25; // smoke shade
	int cp = makecol(f,f,f); // smoke	
	pm->add(new Smoke(cmx, cmy, (((rand()%50)-50)/10.0), -(rand()%20)/10.0, rand()%5+5, cp, rand()%3+1, (rand()%100)/100.0));
	// add burn
	f = rand()%85+25; // red shade
	cp = makecol(f,0,0); // red	
	pm->add(new Smoke(cmx, cmy, (((rand()%50)-50)/10.0), -(rand()%20)/10.0, rand()%5+5, cp, rand()%3+1, (rand()%100)/100.0));
	
	// explosion when almost dead
	if (life < -50)
	{
		cp = makecol(255,255,85); // yellow
		int pz = rand()%5+5; // particle ammount
		for (int p=0; p<pz;p++)
			pm->add(new Spark(cmx, cmy, (rand()%60-30)/10.0, (rand()%10+20)/-10.0, rand()%15+10, cp));
	}

	if (life < -60) // takes about 2 seconds to die
	{
		ufo->score++;
		return true;
	}

	return false; // im still alive but dying
}	

void APC::render(BITMAP *bmp)
{ 
	if (sx <= 0) // left or right?
		draw_sprite(bmp, sprite, (int)x, (int)y);
	else
		draw_sprite_h_flip(bmp, sprite, (int)x, (int)y);	
}
