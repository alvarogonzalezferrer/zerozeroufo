// Mine - enemy

// this is a special enemy, is a floating

#include "mine.h"
#include "logger.h"
#include "enemy.h"

// static data for all the objects
BITMAP *Mine::spr[4];
Datafile *Mine::spr_data = NULL;
int Mine::count = 0;


Mine::Mine(Datafile *data) : Enemy(data)
{
	Logger::log("Mine::Mine()");
	
	x = y = sx = sy = 0; // remember to position!
	
	life = 20;
	
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
	
	sprite = spr[frame/10];
	
	// IA
	// easy state machine, when reaches 0, choose another action
	ia--;
	if (ia < 0)
	{
		ia = 30 + rand()%30; 
		
		// movement
		sx = -(rand()%20+10) / 10.0; // ALWAYS LEFT
		sy = (rand()%20-10) / 10.0; // up or down?
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
		sx = -3;
		sy = 0;
		ia = 5; 
	}
	
	// if I go out of screen by left, Im gone by good
	if (x < -sprite->w )
		return true; // im gone
		
	// update collision detection
	bbox->x = x;
	bbox->y = y;
	
	// collide against UFO ? 
	if (bbox->collide(ufo->bbox))
	{
		ufo->life -= rand()%15+10; // big damage up to 25%
		
		// debug ADD sound!
		
		int color = makecol(rand()%50+200,0,0);
		int pz = rand()%20+20; // particle ammount
		for (int p=0; p < pz; p++)
			pm->add(new Spark(x, y, (rand()%40-20)/10.0, (rand()%40-20)/10.0, rand()%10+15, color));

		return true; // I collided 
	}
	
	//if (life < 0)
	//	return true; // shoot by player! :O
	
	return false ; // im still alive
}	

void Mine::render(BITMAP *bmp)
{
	draw_sprite(bmp, sprite, (int)x, (int)y);
}
