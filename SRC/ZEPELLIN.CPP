// zepellin enemy

#include "zepellin.h"
#include "logger.h"
#include "enemy.h"

// static data for all the objects
BITMAP *Zepellin::spr[2];
Datafile *Zepellin::spr_data = NULL;
int Zepellin::count = 0;


Zepellin::Zepellin(Datafile *data) : Enemy(data)
{
	Logger::log("Zepellin::Zepellin()");
	
	x = y = sx = sy = 0; // remember to position - TODO make a proper constructor!!
	
	life = 60;
	
	//if (count == 0) // first instance, get sprites and sounds 
	if (spr_data != data)
	{
		spr_data = data;
		
		spr[0] = (BITMAP *)data->getObject("ZEPELLIN_0_BMP")->dat;
		spr[1] = (BITMAP *)data->getObject("ZEPELLIN_1_BMP")->dat;
		
		// TODO - add sounds
	}
	count++;
	
	sprite = spr[0];
	frame = rand()%2;
	ia = 0;
		
	bbox = new Collide2D(x, y, sprite->w, sprite->h);
}


Zepellin::~Zepellin()
{
	Logger::log("Zepellin::~Zepellin()");
	delete(bbox);
	count--;
}


bool Zepellin::update(Map *m, UFO *ufo, ParticleManager *pm, ShootsList *shoots )
{
	// move
	x += sx;
	y += sy; // THIS ENEMY FLIES!
	
		
	// animate
	frame++;
	
	if (frame > 1) // (0,1), fast animation
		frame=0;
	
	sprite = spr[frame];
	
	// IA
	// easy state machine, when reaches 0, choose another action
	ia--;
	if (ia<0)
	{
		ia = rand()%90+30; // frames to do the same?
		sx = -0.3; // goes always left and slow
		sy = (rand()%10-5) / 10.0; // spr up or down?
		
		// chase enemy?
		if (rand()%100 < 80)
		{
			if (ufo->x > x)
				sx = abs(sx);
			else
				sx = -abs(sx);

			if (ufo->y > y)
				sy = abs(sy);
			else
				sy = -abs(sy);
		}
		
		// shoot? 60% chance
		if (rand()%100 < 60)
		{	
			// shoot straigh and down
			
			// left or right?
			float ssx = (sx <= 0) ? -4 : 4;
			
			// shoot coming from cache
			int cmx = x + sprite->w/2;
			int cmy = y + sprite->h/2;
			
			// straight 
			shoots->add(new Shoot(cmx,cmy ,
								ssx, 0, 
								rand()%100+200, 
								4, 
								0.01, 
								rand()%10+10, 
								makecol(255,255,85),
								0));
			
			// down
			shoots->add(new Shoot(cmx,cmy,
								ssx, rand()%2+1, 
								rand()%100+200, 
								4, 
								0.01, 
								rand()%10+10, 
								makecol(255,255,85),
								0));
		}
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
	
	// side right should not clip since I can come from outside screen!
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
	
	if (life < 25)
	{
		// spark a little
		// smoke and burn
		int cmx = x + sprite->w/2 + rand()%10 - 5;
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
	sy = 1; // go down slow
	sx = -1; // go left slow
	
	// dont crash below ground
	if (y > m->getHeight(x) - sprite->h)
	{
		y = m->getHeight(x) - sprite->h;
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
	if (life < -30)
	{
		cp = makecol(255,255,85); // yellow
		int pz = rand()%5+5; // particle ammount
		for (int p=0; p<pz;p++)
			pm->add(new Spark(cmx, cmy, (rand()%60-30)/10.0, (rand()%60-30)/10.0, rand()%15+10, cp));
	}

	
	if (life < -60) // takes 2 seconds to die
	{
		ufo->score++;
		return true;
	}
		
	return false; // im still alive, but dying
}	

void Zepellin::render(BITMAP *bmp)
{
	if (sx <= 0) // left or right?
		draw_sprite(bmp, sprite, (int)x, (int)y);
	else
		draw_sprite_h_flip(bmp, sprite, (int)x, (int)y);
	
	// DEBUG
	//textprintf_ex(bmp, font, x, y, makecol(255,255,255), -1, "%d", life);
}
