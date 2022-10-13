// helicopter enemy

#include "heli.h"
#include "logger.h"
#include "enemy.h"

// static data for all the objects
BITMAP *Helicopter::heli[3];
BITMAP *Helicopter::heli_mis = NULL;
Datafile *Helicopter::helicopter_data = NULL;
SAMPLE *Helicopter::explosion_wav = NULL;
int Helicopter::count = 0;


Helicopter::Helicopter(Datafile *data) : Enemy(data)
{
	Logger::log("Helicopter::Helicopter()");
	
	x = y = sx = sy = 0; // remember to position! - TODO make a proper constructor!!
	
	life = 40;
	
	//if (count == 0) // first helicopter, get sprites and sounds 
	if (helicopter_data != data)
	{
		helicopter_data = data;
		
		heli[0] = (BITMAP *)data->getObject("HELI_0_BMP")->dat;
		heli[1] = (BITMAP *)data->getObject("HELI_1_BMP")->dat;
		heli[2] = (BITMAP *)data->getObject("HELI_2_BMP")->dat;
		
		heli_mis = (BITMAP *)data->getObject("HELI_MIS_BMP")->dat; // missile
		
		explosion_wav = (SAMPLE *)data->getObject("EXPLOSION_WAV")->dat;
	}
	count++;
	
	sprite = heli[0];
	frame = rand()%3;
	ia = 0;
	
	bbox = new Collide2D(x, y, sprite->w, sprite->h);
}


Helicopter::~Helicopter()
{
	Logger::log("Helicopter::~Helicopter()");
	delete(bbox);
	count--;
}


bool Helicopter::update(Map *m, UFO *ufo, ParticleManager *pm, ShootsList *shoots )
{
	// move
	x += sx;
	y += sy; // THIS ENEMY FLY!
	
		
	// animate
	frame++;
	
	if (frame > 2) // (0,1,2), chopper is fast animation
		frame=0;
	
	sprite = heli[frame];
	
	// IA
	// easy state machine, when reaches 0, choose another action
	ia--;
	if (ia<0)
	{
		ia = rand()%60+30; // frames to do the same?
		sx = (rand()%30-15) / 10.0; // heli left or right?
		sy = (rand()%30-15) / 10.0; // heli up or down?
		
		// chase enemy?
		if (rand()%100 < 70)
		{
			if (ufo->x > x)
				sx = abs(sx);
			else
				sx = -abs(sx);
		}
		
		
		// shoot? 60% chance
		if (rand()%100 < 60)
		{
			
			// shoot straigh
			float ssy = 0;
			
			// left or right?
			float ssx = (sx <= 0) ? -3 : 3;
			
			float sx = x + sprite->w/2;
			
			Shoot *sn = new Shoot(sx, y + sprite->h - 5,
								ssx, ssy, 
								rand()%100+200, 
								heli_mis, 
								0.0, 
								rand()%15+10, 
								makecol(255,255,85),
								3);
			
			shoots->add(sn);	
		}

	}
	
	// bounce on borders 
	
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
	if (x > m->mapW - sprite->w)
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
	
	if (life < 10)
	{
		// spark a little
		// smoke and burn
		int cmx = x + sprite->w/2 + rand()%8 - 4;
		int cmy = y + sprite->h/2 + rand()%8 - 4;

		int f = rand()%55+200; // spark shade
		int cp = makecol(f,f,0);
		pm->add(new Spark(cmx, cmy, (rand()%40-20)/10.0, (rand()%40-20)/10.0, rand()%5+5, cp));
	}
	
	if (life > 0) 
		return false ; // im still alive
	// if im still alive I end doing AI here 
	// if not, I do a dying sequence below
	
	// IM DEAD , do the fall animation
	ia = 1000; // disable IA next step
	life--; // keep going below 0
	sy = abs(sy) + .3; // go down fast
	
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

void Helicopter::render(BITMAP *bmp)
{
	if (sx <= 0) // left or right?
		draw_sprite(bmp, sprite, (int)x, (int)y);
	else
		draw_sprite_h_flip(bmp, sprite, (int)x, (int)y);
}
