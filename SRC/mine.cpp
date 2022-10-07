// Mine - enemy

// this is a special enemy, is a floating

#include "mine.h"
#include "logger.h"
#include "enemy.h"
#include "randomiz.h"

// static data for all the objects
BITMAP *Mine::spr[2];
Datafile *Mine::spr_data = NULL;
int Mine::count = 0;


Mine::Mine(Datafile *data) : Enemy(data)
{
	Logger::log("Mine::Mine()");
	
	x = y = sx = sy = 0; // remember to position!
	
	life = 50; 
	
	//if (count == 0) // first helicopter, get sprites and sounds 
	if (spr_data != data)
	{
		spr_data = data;
		
		spr[0] = (BITMAP *)data->getObject("MINE_0_BMP")->dat;
		spr[1] = (BITMAP *)data->getObject("MINE_1_BMP")->dat;

		// TODO - add sounds
	}
	count++;
	
	sprite = spr[0];
	frame = rand()%2;
	ia = 0;

	bbox = new Collide2D(x, y, sprite->w); // as circle
	
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
			sx = -Randomize::random(0.5f, 1.5f);
		else 
			sx = Randomize::random(0.5f, 1.5f);
		
		if (ufo->y < y)
			sy = -Randomize::random(0.5f, 1.5f);
		else 
			sy = Randomize::random(0.5f, 1.5f);
	}
	 
	// dont crash on ground!
	if (y > m->getHeight(x) - sprite->h*3)
	{
		sy = (rand()%15) / -10.0; // dont fly lower!
		y = m->getHeight(x) - sprite->h*3;
	}
	
	if (y < 0)
	{
		y = 0;
		sy = (rand()%15) / 10.0; // dont fly higher
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
		
		// debug ADD sound!
		
		life = -1; // I collided 
	}
	
	if (life < 0)
	{
		// EXPLODE SPARKS 
		int color = makecol(rand()%50+200,0,0);
		int pz = rand()%10+35; // particle ammount
		for (int p=0; p < pz; p++)
			pm->add(new Spark(mx, my, (rand()%40-20)/10.0, (rand()%40-20)/10.0, rand()%10+15, color));
		
		return true; // shoot by player! :O
	}
	
	// leave spark trail IF im alive
	int color = makecol(rand()%50+200,0,0);
	int pz = rand()%3+5; // particle ammount
	for (int p=0; p < pz; p++)
		pm->add(new Spark(mx, my, Randomize::random(-sx*0.8f, -sx*1.2f), Randomize::random(-sy*0.8f, -sy*1.2f), rand()%5+15, color));
	
	
	return false ; // im still alive
}	

void Mine::render(BITMAP *bmp)
{
	draw_sprite(bmp, sprite, (int)x, (int)y);
}
