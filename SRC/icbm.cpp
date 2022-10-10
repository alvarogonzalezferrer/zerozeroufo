// ICBM - enemy

// this is a special enemy, is a MISSILE

#include "mine.h"
#include "logger.h"
#include "enemy.h"
#include "randomiz.h"

// static data for all the objects
BITMAP *ICBM::spr[2];
SAMPLE *ICBM::explosion_wav;
Datafile *ICBM::spr_data = NULL;
int ICBM::count = 0;


ICBM::ICBM(Datafile *data) : Enemy(data)
{
	Logger::log("ICBM::ICBM()");
	
	x = y = sx = sy = 0; // remember to position!
	
	life = rand()%50+75; 
	
	//if (count == 0) // first helicopter, get sprites and sounds 
	if (spr_data != data)
	{
		spr_data = data;
		
		spr[0] = (BITMAP *)data->getObject("ICBM_0_BMP")->dat;
		spr[1] = (BITMAP *)data->getObject("ICBM_1_BMP")->dat;

		explosion_wav = (SAMPLE *)data->getObject("EXPLOSION_WAV")->dat;
	}
	count++;
	
	sprite = spr[0];
	frame = rand()%2;
	ia = 0;

	bbox = new Collide2D(x, y, sprite->w, sprite->h); // DEBUG change as a circle
	
	collideWithUFO = false; // very important so UFO can BE DAMAGED BY THE MINE
}

ICBM::ICBM(float x, float y, Datafile *data) : ICBM(data)
{
	this->x = x;
	this->y = y;
}

ICBM::~ICBM()
{
	Logger::log("ICBM::~ICBM()");
	delete(bbox);
	count--;
	// DATA IS HANDLED BY ENEMIES.CPP, DO NOT UNLOAD HERE! IS SHARED ACROSS ENEMIES!
}


bool ICBM::update(Map *m, UFO *ufo, ParticleManager *pm, ShootsList *shoots )
{
	// move
	x += sx;
	y += sy;	
		
	// animate
	frame++;
	
	if (frame > 1) // (0..1)
		frame=0;
	
	sprite = spr[frame];
	
	// IA
	// easy state machine, when reaches 0, choose another action
	ia--;
	if (ia < 0)
	{
		ia = 5; 
		
		// movement
		sx = -1.5f; // always go left
		
		// semi-chase the player 
		if (ufo->y < y)
			sy = -Randomize::random(0.5f, 1.5f);
		else 
			sy = Randomize::random(0.5f, 1.5f);
	}
	 
	// dont crash on ground!
	if (y > m->getHeight(x) - sprite->h*3)
	{
		sy = -Randomize::random(0.5f, 1.5f); // dont fly lower!
		y = m->getHeight(x) - sprite->h*3;
	}
	
	if (y < 0)
	{
		y = 0;
		sy = Randomize::random(0.5f, 1.5f); // dont fly higher
	}
	
	// sides
	
	// side right should not clip since i can come from outside screen!
	if (x > m->mapW + sprite->w)
	{
		sx = -2;
		sy = 0;
		ia = 5; 
	}
	
	// if I go out of screen by left, Im gone by good
	if (x < -sprite->w )
		return true; // im gone
		
	// update collision detection
	bbox->x = x;
	bbox->y = y;

	// middle point for sparks
	int mx = x+sprite->w/2;
	int my = y+sprite->h/2;

	
	// collide against UFO ? 
	if (bbox->collide(ufo->bbox))
	{
		ufo->life -= rand()%25+25; // big damage up to 50%
		
		// make UFO bounce randomly
		ufo->sx = UFO::MV_X * sx;
		ufo->sy = UFO::MV_Y * sy;
		
		life = -1; // I collided 
	}
	
	if (life < 0)
	{
		ufo->score++;
		
		// EXPLODE SPARKS 
		int color = makecol(255, rand()%255,0);
		int pz = rand()%10+45; // particle ammount
		for (int p=0; p < pz; p++)
			pm->add(new Spark(mx, my, (rand()%50-25)/10.0, (rand()%50-25)/10.0, rand()%15+15, color));
		
		// double sound, stronger explosion
		play_sample(explosion_wav, 200 + rand()%55, x * 255 / 320, 800+rand()%600, 0);
		
		play_sample(explosion_wav, 200 + rand()%55, x * 255 / 320, 900+rand()%600, 0);
		
		return true; // shoot by player! :O
	}
		
	return false ; // im still alive
}	

void ICBM::render(BITMAP *bmp)
{
	draw_sprite(bmp, sprite, (int)x, (int)y);
}
