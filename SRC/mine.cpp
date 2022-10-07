// Mine - enemy

// this is a special enemy, is a floating

#include "mine.h"
#include "logger.h"
#include "enemy.h"
#include "randomiz.h"

// static data for all the objects
BITMAP *Mine::spr[2];
SAMPLE *Mine::explosion_wav;
Datafile *Mine::spr_data = NULL;
int Mine::count = 0;


Mine::Mine(Datafile *data) : Enemy(data)
{
	Logger::log("Mine::Mine()");
	
	x = y = sx = sy = 0; // remember to position!
	
	life = rand()%50+75; 
	
	//if (count == 0) // first helicopter, get sprites and sounds 
	if (spr_data != data)
	{
		spr_data = data;
		
		spr[0] = (BITMAP *)data->getObject("MINE_0_BMP")->dat;
		spr[1] = (BITMAP *)data->getObject("MINE_1_BMP")->dat;

		explosion_wav = (SAMPLE *)data->getObject("EXPLOSION_WAV")->dat;
	}
	count++;
	
	sprite = spr[0];
	frame = rand()%2;
	ia = 0;

	bbox = new Collide2D(x, y, sprite->w, sprite->h); // DEBUG change as a circle
	
	collideWithUFO = false; // very important so UFO can BE DAMAGED BY THE MINE
}

Mine::Mine(float x, float y, Datafile *data) : Mine(data)
{
	this->x = x;
	this->y = y;
}

Mine::~Mine()
{
	Logger::log("Mine::~Mine()");
	delete(bbox);
	count--;
	// DATA IS HANDLED BY ENEMIES.CPP, DO NOT UNLOAD HERE! IS SHARED ACROSS ENEMIES!
}


bool Mine::update(Map *m, UFO *ufo, ParticleManager *pm, ShootsList *shoots )
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
		// semi-chase the player 
		if (ufo->x < x)
			sx = -Randomize::random(0.5f, 2.0f);
		else 
			sx = Randomize::random(0.5f, 2.0f);
		
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
		ufo->life -= rand()%15+10; // big damage up to 25%
		
		// make UFO bounce randomly
		ufo->sx = UFO::MV_X * sx;
		ufo->sy = UFO::MV_Y * sy;
		
		life = -1; // I collided 
	}
	
	if (life < 0)
	{
		ufo->score++;
		
		// EXPLODE SPARKS 
		int color = makecol(rand()%50+200,0,0);
		int pz = rand()%10+35; // particle ammount
		for (int p=0; p < pz; p++)
			pm->add(new Spark(mx, my, (rand()%40-20)/10.0, (rand()%40-20)/10.0, rand()%10+15, color));
		
		play_sample(explosion_wav, 200 + rand()%55, x * 255 / 320, 800+rand()%600, 0);
		
		return true; // shoot by player! :O
	}
		
	return false ; // im still alive
}	

void Mine::render(BITMAP *bmp)
{
	draw_sprite(bmp, sprite, (int)x, (int)y);
}
