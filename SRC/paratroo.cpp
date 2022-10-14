// paratrooper enemy

#include "paratroo.h"
#include "logger.h"
#include "enemy.h"
#include "shoots.h"
#include "randomiz.h"


// static data for all the objects
BITMAP *Paratrooper::parachute[5]; // first 3 frames are parachute (0,1,2), 3,4 are parachute broken by UFO fire 
BITMAP *Paratrooper::walk[3]; // walking when reaches ground
SAMPLE *Paratrooper::scream = NULL;
Datafile *Paratrooper::p_data = NULL;
int Paratrooper::count = 0;


Paratrooper::Paratrooper(Datafile *data) : Enemy(data)
{
	Logger::log("Paratrooper::Paratrooper()");
	
	x = y = sx = sy = 0; // remember to position!
	
	//if (count == 0) // first soldat, get sprites and sounds 
	if (p_data != data) // new method, this prevents re loading times, I just check IF the thing is already in RAM or changed (unlike)
	{
		p_data = data;
		
		// walk
		walk[0] = (BITMAP *)data->getObject("SOLDAT_0_BMP")->dat;
		walk[1] = (BITMAP *)data->getObject("SOLDAT_1_BMP")->dat;
		walk[2] = (BITMAP *)data->getObject("SOLDAT_2_BMP")->dat;
		
		// fly
		parachute[0] = (BITMAP *)data->getObject("PARAT_0_BMP")->dat;
		parachute[1] = (BITMAP *)data->getObject("PARAT_1_BMP")->dat;
		parachute[2] = (BITMAP *)data->getObject("PARAT_2_BMP")->dat;
		// fall
		parachute[3] = (BITMAP *)data->getObject("PARAT_3_BMP")->dat;
		parachute[4] = (BITMAP *)data->getObject("PARAT_4_BMP")->dat;
		
		scream = (SAMPLE *)data->getObject("SCREAM_WAV")->dat;
	}
	count++;
	
	sprite = parachute[0];
	frame = rand()%30;
	ia = 0;
	
	life = 12;
	
	airborne = true;
	insideBeam = false;
	
	bbox = new Collide2D(x, y, sprite->w, sprite->h);
	
	collideWithUFO = false; // very important so UFO beam can work!
}

Paratrooper::Paratrooper(float x, float y, Datafile *data) : Paratrooper(data) 
{
	Logger::log("Paratrooper::Paratrooper(x %f,y %f)", x, y);
	this->x = x;
	this->y = y;
}

Paratrooper::~Paratrooper()
{
	// NOTE: do NOT release data in RAM, is handled somewhere else!!
	Logger::log("Paratrooper::~Paratrooper()");
	delete(bbox);
	count--;
}


bool Paratrooper::update(Map *m, UFO *ufo, ParticleManager *pm, ShootsList *shoots )
{
	// move
	x += sx;
	y += sy;
	
	// base for my feet
	int feet =  m->getHeight(x) - sprite->h;

		
	if (airborne)
	{
		ia--;
		if (ia < 0)
		{
			sx = Randomize::random(-1.5f, 1.5f);
			ia = 30+rand()%30;
		}
		
		frame++;
		if (life > 0)
		{
			if (frame > 29)
				frame = 0;
				
			sprite = parachute[frame/10]; // 0,1,2 = falling down
		}
		else // falling without parachute
		{	
			if (frame > 1)
				frame = 0; // faster flap of broken parachute
				
			sprite = parachute[frame+3]; // 3,4 = falling down	without parachute
		}
		
		if (!insideBeam)
		{
			// fall on map
			if (y < feet && life > 0)
				sy = 0.3; // fall down
		
			if (life < 0) // falling dead without parachute
			{
				sy += 0.3;
				
				if (rand()%10 > 5)
					play_sample(scream, 200 + rand()%55, x * 255 / 320, 800+rand()%600, 0); // scream falling...
			}
			
			if (y > feet)
			{
				y = feet;
				sy = 0;
				sx = 0;
				ia = 0;
				airborne = false; // reached ground!
			}
		}
	}
	else // Im on the ground
	{
		frame++;
		if (frame > 29)
				frame = 0;

		sprite = walk[frame/10];

		if (!insideBeam)
		{
			if (y < feet)
				sy = +0.3; // fall down
			
			if (y > feet)
				{
					y = feet;
					sy = 0;
					sx = 0;
					ia = 0;
				}
		}
		
		ia--;
		if (ia<0 && !insideBeam)
		{
			// move
			ia = Randomize::random(30, 90); // frames to do the same?
			sx = Randomize::random(-1.5f, 1.5f); //  walk left or right?
			sy = 0;

			// shoot? 
			if (rand()%100 < 30)
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
									1, 
									0.02, 
									5+rand()%5, 
									makecol(255,255,85),
									1));
			}
		}

		// bounce on borders 

		// side right should not clip since i can come from outside screen!
		if  (x + sprite->w > m->mapW)
		{
			sx = -2; // go left
			ia = 10; 
		}

		if (x < 0)
		{
			x = 0;
			sx = 2; // go right
			ia = 10;
		}
	
		if (life <= 0)
		{
			ufo->score++;
			play_sample(scream, 200 + rand()%55, x * 255 / 320, 800+rand()%600, 0);

			// add some blood particles
			int pz = rand()%5+10; // particle ammount
			int cp = makecol(170,0,0); // red
			for (int p=0; p<pz;p++)
				pm->add(new Spark(x+sprite->w/2, y+sprite->h/2, Randomize::random(-2.0f, 2.0f), Randomize::random(-3.0f, -2.0f), Randomize::random(8, 16), cp));
						
			return true; // dead
		}
	}
	
	// UFO beam ray!!
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
						ufo->life += rand()%5;
						
						// add some blood particles
						int pz = rand()%5+5; // particle ammount
						int cp = makecol(170,0,0); // red
						for (int p=0; p<pz;p++)
							pm->add(new Spark(cmx, cmy, Randomize::random(-2.0f, 2.0f), Randomize::random(2.0f, 3.0f), Randomize::random(8, 16), cp));
						
						play_sample(scream, 200 + rand()%55, x * 255 / 320, 800+rand()%600, 0);
						return true; // bye
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
	
	// set collision on new position 
	bbox->setAsBox(x, y, sprite->w, sprite->h);
	
	return false; // still alive
}

void Paratrooper::render(BITMAP *bmp)
{	
	if (sx <= 0 || airborne) // left or right? -- only if not airborne we flip the sprite (because shadows gets funky otherwise)
		draw_sprite(bmp, sprite, (int)x, (int)y); // left
	else
		draw_sprite_h_flip(bmp, sprite, (int)x, (int)y);	
}
