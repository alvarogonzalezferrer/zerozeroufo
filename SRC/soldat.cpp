// common foot soldier enemy



#include "soldat.h"
#include "logger.h"
#include "enemy.h"
#include "shoots.h"
#include "randomiz.h"


// static data for all the objects
BITMAP *Soldat::walk[3];
SAMPLE *Soldat::scream = NULL;
Datafile *Soldat::soldat_data = NULL;
int Soldat::count = 0;


Soldat::Soldat(Datafile *data) : Enemy(data)
{
	Logger::log("Soldat::Soldat()");
	
	x = y = sx = sy = 0; // remember to position - TODO make a proper constructor!!
	
	//if (count == 0) // first soldat, get sprites and sounds 
	if (soldat_data != data) // new method, this prevents re loading times, I just check IF the thing is already in RAM or changed (unlike)
	{
		soldat_data = data;
		
		walk[0] = (BITMAP *)data->getObject("SOLDAT_0_BMP")->dat;
		walk[1] = (BITMAP *)data->getObject("SOLDAT_1_BMP")->dat;
		walk[2] = (BITMAP *)data->getObject("SOLDAT_2_BMP")->dat;
		
		scream = (SAMPLE *)data->getObject("SCREAM_WAV")->dat;
	}
	count++;
	
	sprite = walk[0];
	frame = rand()%30;
	ia = 0;
	
	life = 10;
	
	first_frame = true;
	insideBeam = false;
	
	bbox = new Collide2D(x, y, sprite->w, sprite->h);
	
	collideWithUFO = false; // very important so UFO beam can work!
}


Soldat::~Soldat()
{
	// NOTE: do NOT release data in RAM, is handled somewhere else!!
	
	Logger::log("Soldat::~Soldat()");
	delete(bbox);
	count--;
}


bool Soldat::update(Map *m, UFO *ufo, ParticleManager *pm, ShootsList *shoots )
{
	// move
	x += sx;
	y += sy;
	
	// base for my feet
	int feet =  m->getHeight(x) - sprite->h + 2;
	
	// set on map on first frame that Im alive
	if (first_frame)
	{
		y = feet;
		first_frame = false; // DEBUG TODO - this is a shitty way to replace a proper constructor!
	}
	
	if (!insideBeam)
	{
		// fall on map (for the beam)
		if (y < feet)
		{
			sy += 0.3; // fall down -- DEBUG TODO: SHOULD TAKE FALL DAMAGE?
		}
		
		if (y > feet)
		{
			y = feet;
			sy = 0;
		}
	}
	
	// animate
	frame++;
	
	if (frame > 29) // 0...10...20... (0,1,2)
		frame=0;
	
	sprite = walk[frame/10];
	
	
	// IA
	// easy state machine, when reaches 0, choose another action
	ia--;
	if (ia<0 && !insideBeam)
	{
		// move
		ia = Randomize::random(30, 90); // frames to do the same?
		sx = Randomize::random(-1.5f, 1.5f); //  walk left or right?
		sy = 0;

		// shoot? 40% chance
		if (rand()%100 < 40)
		{
	
			// shoot up
			float ssy = Randomize::random(-3, -1);
			
			// left or right?
			float ssx = (sx <= 0) ? -1 : 1;
			
			ssx = ssx * Randomize::random(1, 2); // speed up
			
			float sx = (sx <= 0) ? x : x + sprite->w;
			
			shoots->add(new Shoot(sx,y,
								ssx, ssy, 
								200, 
								0, 
								0.02, 
								3, 
								makecol(255,255,85),
								1));
		}
	}
	
	// bounce on borders 

	// side right should not clip since i can come from outside screen!
	if  (x > m->mapW)
	{
		sx = -2; // go left
		ia = Randomize::random(15, 25); 
	}
	
	if (x < 0)
	{
		x = 0;
		sx = 3 + rand()%3; // go right
		ia = Randomize::random(20, 30); 
		
		if (rand()%10 < 5)
			return true; // disappear for good, ran away from battle
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
						// add score -- debug is counted twice since it falls in life < 0 below too! ok...
						ufo->score++;
						
						// add some blood particles
						int pz = rand()%5+5; // particle ammount
						int cp = makecol(170,0,0); // red
						for (int p=0; p<pz;p++)
							pm->add(new Spark(cmx, cmy, Randomize::random(-2.0f, 2.0f), Randomize::random(2.0f, 3.0f), Randomize::random(8, 16), cp));
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
	
	// update bounding box collision detection
	bbox->x = x;
	bbox->y = y;

		
	if (life > 0) 
		return false ; // im still alive
	else
	{
		ufo->score++;
		
		
		play_sample(scream, 200 + rand()%55, x * 255 / 320, 800+rand()%600, 0);
		
		return true; // im dead
	}
}	

void Soldat::render(BITMAP *bmp)
{
	if (sx <= 0) // left or right?
		draw_sprite(bmp, sprite, (int)x, (int)y); // left
	else
		draw_sprite_h_flip(bmp, sprite, (int)x, (int)y);
}
