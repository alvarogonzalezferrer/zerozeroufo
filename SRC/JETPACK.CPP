// jetpack enemy

#include "jetpack.h"
#include "logger.h"
#include "enemy.h"
#include "randomiz.h"

// static data for all the objects
BITMAP *Jetpack::spr[3];
Datafile *Jetpack::spr_data = NULL;
int Jetpack::count = 0;


Jetpack::Jetpack(Datafile *data) : Enemy(data)
{
	Logger::log("Jetpack::Jetpack()");
	
	x = y = sx = sy = 0; // remember to position - TODO make a proper constructor!!
	
	life = 20;
	
	//if (count == 0) // first instance, get sprites and sounds 
	if (spr_data != data)
	{
		spr_data = data;
		
		spr[0] = (BITMAP *)data->getObject("JETPACK_0_BMP")->dat;
		spr[1] = (BITMAP *)data->getObject("JETPACK_1_BMP")->dat;
		spr[2] = (BITMAP *)data->getObject("JETPACK_2_BMP")->dat;
		// TODO - add sounds
	}
	count++;
	
	sprite = spr[0];
	frame = rand()%3;
	ia = 0;
	insideBeam = false;
		
	bbox = new Collide2D(x, y, sprite->w, sprite->h);
	
	collideWithUFO = false; // very important so UFO beam can work!
}


Jetpack::~Jetpack()
{
	Logger::log("Jetpack::~Jetpack()");
	delete(bbox);
	count--;
}


bool Jetpack::update(Map *m, UFO *ufo, ParticleManager *pm, ShootsList *shoots )
{
	// move
	x += sx;
	y += sy; // THIS ENEMY FLIES!
	
		
	// animate
	frame++;
	
	if (frame > 2) // (0,1,2), fast animation for fire
		frame=0;
	
	sprite = spr[frame];
	
	// IA
	// easy state machine, when reaches 0, choose another action
	ia--;
	if (ia<0 && !insideBeam)
	{
		ia = rand()%90+30; // frames to do the same?
		sx = (rand()%30-15) / 10.0; // spr left or right?
		sy = (rand()%30-15) / 10.0; // spr up or down?
		
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
			// shoot straigh
			float ssy = 0;
			
			// left or right?
			float ssx = (sx <= 0) ? -3 : 3;
			
			shoots->add(new Shoot(x + sprite->w/2, y + sprite->h /2,
									ssx, ssy, 
									rand()%100+200, 
									2, 
									0.01, 
									rand()%5+5, 
									makecol(255,255,85),
									1));	
		}
	}
	
	// Take damage from ufo beam ray!!
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
				// fly up towards the UFO
				sy = -(rand()%2+1); // up!

				// fly X to center of UFO
				if (cmx > ufo->beam_xu + 3)
					sx = -(rand()%2+1);
				else if (cmx < ufo->beam_xu - 3)
					sx = rand()%2+1;
				else
					sx = 0; // keep it centered, already reached center, prevents flip seizure!
					
				insideBeam = true;
				
				// I hit the UFO ?? near 5 px y = hit
				if (abs(cmx - ufo->beam_xu) < 5 &&
					abs(cmy - ufo->beam_y1) < 5 )
					{
						// im in the UFO, bye
						life = 0;
						insideBeam = false;
						// add score
						ufo->score++;
						ufo->life += rand()%3;
						
						// add some blood particles
						int pz = rand()%5+5; // particle ammount
						int cp = makecol(170,0,0); // red
						for (int p=0; p<pz;p++)
							pm->add(new Spark(cmx, cmy, Randomize::random(-2.0f, 2.0f), Randomize::random(2.0f, 3.0f), Randomize::random(8, 16), cp));
						
						return true; // no crash animation 
					}
				
				// Im above the UFO??
				if (cmy < ufo->beam_y1)
				{
					y = ufo->beam_y1 + 6; // hover below UFO level
					x = ufo->beam_xu + Randomize::random(-3, 3);
				}
			}
			else
				insideBeam = false;
	}
	else
		insideBeam = false; // no beam to be inside
	
	// bounce on borders 
	
	// dont crash on ground! unless im dead 
	if (y > m->getHeight(x) - sprite->h - 5 && life >0)
	{
		sy = (rand()%15) / -10.0;; // dont fly lower!
		y = m->getHeight(x) - sprite->h - 5;
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
	if (life < -30)
	{
		cp = makecol(255,255,85); // yellow
		int pz = rand()%5+5; // particle ammount
		for (int p=0; p<pz;p++)
			pm->add(new Spark(cmx, cmy, (rand()%60-30)/10.0, (rand()%10+20)/-10.0, rand()%15+10, cp));
	}

	
	if (life < -40) // takes 2 seconds to die
	{
		ufo->score++;
		return true;
	}
		
	return false; // im still alive, but dying
}	

void Jetpack::render(BITMAP *bmp)
{
	if (sx <= 0) // left or right?
		draw_sprite(bmp, sprite, (int)x, (int)y);
	else
		draw_sprite_h_flip(bmp, sprite, (int)x, (int)y);
	
	// DEBUG
	//textprintf_ex(bmp, font, x, y, makecol(255,255,255), -1, "%d", life);
}
