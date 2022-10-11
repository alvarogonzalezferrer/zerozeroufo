// Health prize - enemy

// this is a special enemy, is a heatlh prize, will increase health in UFO when colliding with UFO

#include "health_p.h"
#include "logger.h"
#include "enemy.h"

// static data for all the objects
BITMAP *HealthPrize::spr[4];
SAMPLE *HealthPrize::snd = NULL;
Datafile *HealthPrize::spr_data = NULL;
int HealthPrize::count = 0;


HealthPrize::HealthPrize(Datafile *data) : Enemy(data)
{
	Logger::log("HealthPrize::HealthPrize()");
	
	x = y = sx = sy = 0; // remember to position! - TODO make a proper constructor!!
	
	life = 30;
	
	//if (count == 0) // first helicopter, get sprites and sounds 
	if (spr_data != data)
	{
		spr_data = data;
		
		spr[0] = (BITMAP *)data->getObject("HEALTH_PRIZE_0_BMP")->dat;
		spr[1] = (BITMAP *)data->getObject("HEALTH_PRIZE_1_BMP")->dat;
		spr[2] = (BITMAP *)data->getObject("HEALTH_PRIZE_2_BMP")->dat;
		spr[3] = (BITMAP *)data->getObject("HEALTH_PRIZE_3_BMP")->dat;

		snd = (SAMPLE *)data->getObject("HEALTH_PRIZE_WAV")->dat;
	}
	count++;
	
	sprite = spr[0];
	frame = rand()%4;
	ia = 0;

	bbox = new Collide2D(x, y, sprite->w, sprite->h);
	
	collideWithUFO = false; // very important so UFO can pick it up!
}

HealthPrize::HealthPrize(float x, float y, Datafile *data) : HealthPrize(data) 
{
	Logger::log("HealthPrize::HealthPrize(x %f,y %f)", x, y);
	this->x = x;
	this->y = y;
}

HealthPrize::~HealthPrize()
{
	Logger::log("HealthPrize::~HealthPrize()");
	delete(bbox);
	count--;
	// DATA IS HANDLED BY ENEMIES.CPP, DO NOT UNLOAD HERE! IS SHARED ACROSS ENEMIES!
}


bool HealthPrize::update(Map *m, UFO *ufo, ParticleManager *pm, ShootsList *shoots )
{
	// move
	x += sx;
	y += sy;	
		
	// animate
	frame++;
	
	if (frame > 39) // (0..10...20...30)
		frame=0;
	
	sprite = spr[frame/10];
	
	// IA
	// easy state machine, when reaches 0, choose another action
	ia--;
	if (ia < 0)
	{
		ia = 30 + rand()%30; 
		
		// movement
		sx = -(rand()%20+10) / 10.0;
		sy = (rand()%20-10) / 10.0; // up or down?
	}
	 
	// dont crash on ground! unless im dead 
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
		
	// update bounding box collision detection
	bbox->x = x;
	bbox->y = y;
	
	// can be picked up using the UFO's beam
	if (ufo->beam_size > 0)
	{
		// middle x,y
		int cmx = x + sprite->w/2;
		int cmy = y + sprite->h/2;
		
		// is this point inside the beam? 	
		if (cmx > ufo->beam_x1 &&
			cmx < ufo->beam_x2 &&
			cmy > ufo->beam_y1 - 5 &&
			cmy < ufo->beam_y2 )
			{
				ia = 5;
				// fly up towards the UFO
				sy = -(rand()%2+1); // up!

				// fly X to center of UFO
				if (cmx > ufo->beam_xu + 3)
					sx = -(rand()%2+1);
				else if (cmx < ufo->beam_xu - 3)
					sx = rand()%2+1;
				else
					sx = 0; // keep it centered, already reached center, prevents flip seizure!
			}
	}
	
	// collide against UFO ? 
	if (bbox->collide(ufo->bbox))
	{
		ufo->life += rand()%25+25;
		
		play_sample(snd, 255, 128, 1000, 0);
		
		int color = makecol(255,85,85);
		int pz = rand()%10+10; // particle ammount
		for (int p=0; p<pz;p++)
			pm->add(new Spark(x, y, (rand()%40-20)/10.0, (rand()%40-20)/10.0, rand()%10+10, color));

		return true; // I collided 

	}
	
	//if (life < 0)
	//	return true; // shoot by player! :O
	
	return false ; // im still alive
}	

void HealthPrize::render(BITMAP *bmp)
{
	draw_sprite(bmp, sprite, (int)x, (int)y);
}
