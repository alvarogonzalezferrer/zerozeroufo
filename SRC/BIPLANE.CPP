// biplane WW1 style enemy

#include "biplane.h"
#include "logger.h"
#include "enemy.h"
#include "shoots.h"
#include "randomiz.h"


// static data for all the objects
BITMAP *Biplane::spr[3];
SAMPLE *Biplane::explosion_wav = NULL;
Datafile *Biplane::spr_data = NULL;
int Biplane::count = 0;


Biplane::Biplane(Datafile *data) : Enemy(data)
{
	Logger::log("Biplane::Biplane()");
	
	x = y = sx = sy = 0; // DEBUG - SHOULD BE POSITIONED BY A PROPER CONSTRUCTOR!
	
	if (spr_data != data)
	{
		spr_data = data;
		
		spr[0] = (BITMAP *)data->getObject("BIPLANE_0_BMP")->dat;
		spr[1] = (BITMAP *)data->getObject("BIPLANE_1_BMP")->dat;
		spr[2] = (BITMAP *)data->getObject("BIPLANE_2_BMP")->dat;
		
		explosion_wav = (SAMPLE *)data->getObject("EXPLOSION_WAV")->dat;
	}
	count++;
	
	sprite = spr[0];
	frame = rand()%3;
	ia = 0;
	
	life = 25;
		
	bbox = new Collide2D(x, y, sprite->w, sprite->h);
}


Biplane::~Biplane()
{
	// NOTE: do NOT release data in RAM, is handled somewhere else!!
	Logger::log("Biplane::~Biplane()");
	delete(bbox);
	count--;
}


bool Biplane::update(Map *m, UFO *ufo, ParticleManager *pm, ShootsList *shoots )
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
		ia = Randomize::random(30, 75); // frames to do the same?
		sx = -1.5f;  // always go left
		sy = Randomize::random(-1.5f, 1.5f);

		// shoot? 60% chance
		if (rand()%100 < 60)
		{			
			float ssx = (sx <= 0) ? -2.5 : 2.5; // left or right?
			float sx = (sx <= 0) ? x : x + sprite->w;
			
			shoots->add(new Shoot(sx, y + sprite->h / 2,
								  ssx, 0, 
								  200, 
								  1,
								  0.0, 
								  15, 
								  makecol(255,255,85),
								  1));
		}
	}
	
	// bounce on borders 

	// side right should not clip since i can come from outside screen!
	if  (x > m->mapW + sprite->w)
	{
		sx = -1.5f; // go left
		ia = 60;
	}
	
	if (x < -sprite->w) // im gone by left
	{
		//return true; // disappear for good, ran away from battle
		
		// appear by left again :p 
		x = m->mapW + sprite->w + 5;
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
		int pz = rand()%5+5; // particle ammount
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

void Biplane::render(BITMAP *bmp)
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
