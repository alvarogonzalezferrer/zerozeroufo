// -----------------------------------------------------------------------
// B52 paratrooper airplane enemy
// -----------------------------------------------------------------------
// can do a paratrooper run or use their machine guns up, front or back, quite dangerous enemy!
// always goes from right to left, doesnt chase, flies leveled
// -----------------------------------------------------------------------
#include "b52.h"
#include "logger.h"
#include "enemy.h"

// to drop paratrooper
#include "enemies.h"
#include "paratroo.h"


// static data for all the objects
BITMAP *B52::spr[3];
SAMPLE *B52::explosion_wav = NULL;
Datafile *B52::spr_data = NULL;
int B52::count = 0;

EnemyList *B52::enemyList = NULL;

B52::B52(Datafile *data) : Enemy(data)
{
	Logger::log("B52::B52()");
	
	x = y = sx = sy = 0; // remember to position!
	
	life = 60;
	
	//if (count == 0) // first , get sprites and sounds 
	if (spr_data != data)
	{
		spr_data = data;
		
		spr[0] = (BITMAP *)data->getObject("B52_0_BMP")->dat;
		spr[1] = (BITMAP *)data->getObject("B52_1_BMP")->dat;
		spr[2] = (BITMAP *)data->getObject("B52_2_BMP")->dat;
		
		explosion_wav = (SAMPLE *)data->getObject("EXPLOSION_WAV")->dat;
	}
	count++;
	
	sprite = spr[0];
	frame = rand()%3;
	ia = 0;
	jumps = 0;
	machine_gun = 0;
	
	bbox = new Collide2D(x, y, sprite->w, sprite->h);
}

B52::B52(float x, float y, Datafile *data) : B52(data)
{
	this->x = x;
	this->y = y;
}


B52::~B52()
{
	Logger::log("B52::~B52()");
	delete(bbox);
	count--;
	// DATA IS HANDLED BY ENEMIES.CPP, DO NOT UNLOAD HERE! IS SHARED ACROSS ENEMIES!
}


bool B52::update(Map *m, UFO *ufo, ParticleManager *pm, ShootsList *shoots )
{
	// move
	x += sx;
	y += sy; // THIS ENEMY FLY!
	
		
	// animate
	frame++;
	
	if (frame > 2) // (0,1,2), fast animation 
		frame=0;
	
	sprite = spr[frame];
	
	// IA
	// easy state machine, when reaches 0, choose another action
	ia--;
	if (ia < 0)
	{
		ia = rand()%15+30; // frames to do the same?
		sx = -0.25; // always left very slow
		
		sy = (rand()%30-15) / 10.0; // up or down?

		// try to go above the UFO to bomb it 
		if (ufo->y > y + sprite->h)
			sy = -0.5;
		else
		{
			if (rand()%100 < 60)
				sy = 0; // level flight
		}
	
		machine_gun = 0;
		jumps = 0;
		
		if (rand()%100 < 60)
		{
			machine_gun = rand()%15+15;
			ia = machine_gun + ia;
		}
		
		if (rand()%100 < 80)
		{
			jumps = rand()%15+30;
			sy = -abs(sy); // always fly up or level
			ia = jumps + ia;
		}
	}
	
	if (--machine_gun > 0 && ia % 6 == 0)
	{
		shoots->add(new Shoot(x, y + sprite->h / 2,
							-3, 0, 
							100, 
							2, 
							0.01, 
							3, 
							makecol(255,255,85),
							0));
							
		// turret
		shoots->add(new Shoot(x + sprite->w / 2, y + sprite->h / 2,
							-3, -2, 
							100, 
							2, 
							0.02, 
							3, 
							makecol(255,255,85),
							0));

		// tail gunner
		shoots->add(new Shoot(x + sprite->w / 2, y + sprite->h / 2,
							3, 0, 
							100, 
							2, 
							0.01, 
							3, 
							makecol(255,255,85),
							0));
	}
	
	if (--jumps > 0 && ia % 6 == 0)
	{
		// drop paratrooper
		if (enemyList)
			enemyList->addEnemy(new Paratrooper(x + sprite->w/2 + rand()%5, y + sprite->h/2 + rand()%5, spr_data));
		else
			ia = 0; // do something else, we cant drop paratroopers, someone set enemyList NULL (should never come here)
	}
	
	// borders 
	
	// dont crash on ground! unless im dead 
	if (y > m->getHeight(x) - sprite->h*2 && life >0)
	{
		sy = (rand()%15) / -10.0;; // dont fly lower!
		y = m->getHeight(x) - sprite->h*2;
	}
	
	if (y < 0)
	{
		y = 0;
		sy = (rand()%15) / 10.0;; // dont fly higher
	}
	
	// sides
	
	// side right should not clip since i can come from outside screen!
	if (x + sprite->w - 5 > m->mapW)
	{
		jumps=0; // dont drop paratroopers outside screen
		sx = -4;
		sy = 0;
		ia = 5; 
	}
	
	// if I go out of screen by left, Im gone by good
	if (x < -sprite->w )
		return true; // im gone
		
	// update bounding box collision detection
	bbox->x = x;
	bbox->y = y;
	
	// -- end alive --
	if (life > 0) 
		return false ; // im still alive
	// if im still alive I end doing AI here 
	// if not, I do a dying sequence below
	
	// IM DEAD , do the fall animation
	ia = 1000; // disable IA next step
	life--; // keep going below 0
	sy = abs(sy) + .3; // go down fast
	machine_gun = 0;
	jumps = 0;
	
	// if im still high in the air, drop paratroopers when falling down 
	if (y < 100)
	{
		if (enemyList && life % 10 == 0)
			enemyList->addEnemy(new Paratrooper(x + sprite->w/2 + rand()%5, y + sprite->h/2 + rand()%5, spr_data));
	}
	
	// dont crash below ground
	if (y > m->getHeight(x) - sprite->h+10)
	{
		y = m->getHeight(x) - sprite->h+10;
		sy = 0;
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
		int pz = rand()%5+5; // particle ammount
		for (int p=0; p<pz;p++)
			pm->add(new Spark(cmx, cmy, (rand()%60-30)/10.0, (rand()%10+20)/-10.0, rand()%15+10, cp));
	}

	
	if (life < -60) // takes 2 seconds to die
	{
		ufo->score++;
		play_sample(explosion_wav, 200 + rand()%55, x * 255 / 320, 800+rand()%600, 0);
		return true;
	}
		
	return false; // im still alive, but dying
}	

void B52::render(BITMAP *bmp)
{
	if (sx <= 0) // left or right?
		draw_sprite(bmp, sprite, (int)x, (int)y);
	else
		draw_sprite_h_flip(bmp, sprite, (int)x, (int)y);
}
