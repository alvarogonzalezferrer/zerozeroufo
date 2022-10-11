#include "cow.h"

// -----------------------------------------------------------------------
// cow list handler 
// -----------------------------------------------------------------------

CowList::CowList()
{
	cows_n = 0;
}

CowList::~CowList()
{
	for(int i =0; i < cows_n;i++)
		delete(cow[i]);
}

void CowList::generate(int i)
{
	for(int z=0;z < cows_n;z++) // first release old
		delete(cow[z]);

	if (i < 0 || i > maxCOWS)
		i = maxCOWS;
	
	cows_n = i;

	for(int z=0; z < cows_n;z++)
		cow[z] = new Cow();
}

int CowList::update(Map *m, UFO *ufo, ParticleManager *pm)
{
	int ret=0;
	for(int i =0; i < cows_n;i++)
	{
		cow[i]->update(m, ufo, pm);
		if (cow[i]->life > 0) 
			ret++;
		// else
			// cow is dead 
			// I dont remove dead cows from queue since I keep recycling them across the level
	}
	
	
	return ret; // how many cows are alive yet
}

void CowList::render(BITMAP *bmp)
{
	for(int i =0; i < cows_n;i++)
		cow[i]->render(bmp);
}

// -----------------------------------------------------------------------
// cow
// -----------------------------------------------------------------------

// DEBUG sin esto, tengo errores de "undefined reference" que no se bien por que pasan, en fin, esto lo soluciona
BITMAP *Cow::walk[4];
SAMPLE *Cow::cow_wav = NULL;
Datafile Cow::data;
int Cow::count=0; // i have no cows yet


Cow::Cow()
{
	if (count == 0) // if first cow created, load
	{
		data.load("cow.dat");
		walk[0] = (BITMAP *)data.getObject("COW_W_1")->dat;
		walk[1] = (BITMAP *)data.getObject("COW_W_2")->dat;
		walk[2] = (BITMAP *)data.getObject("COW_W_3")->dat;
		walk[3] = (BITMAP *)data.getObject("COW_W_4")->dat;
		
		cow_wav = (SAMPLE *)data.getObject("COW_WAV")->dat;
		
	}

	sprite = walk[0];

	count++;

	y = 190;
	sy = 0;
	sx = rand()%3+1;
	
	life = 30;
	
	respawn = rand()%450 + 150; // min respawn time in ticks
	
	insideBeam = false;

	anm = rand()%40;

	// start walking off screen
	if (rand()%2 < 1)
	{
		x = Map::mapW + sprite->w * (rand()%5 + 1);
		sx = -sx;
	}
	else
	{
		x = -sprite->w * (rand()%5 + 1) -Map::mapW;
	}
}

Cow::~Cow()
{
	count--;
	if (count == 0)
	{
		data.unload();
	}
}

void Cow::update(Map *m, UFO *ufo, ParticleManager *pm)
{
	if (life <= 0)
	{
		respawn--; // regenerate after a while - DEBUG SHOULD BE BASED ON DIFFICULTY
		if (respawn < 0) // recycle cow
		{
				respawn = rand()%450 + 150; // min respawn time
				life = 30;
				x = Map::mapW + sprite->w * (rand()%5 + 1);
				sx = -(rand()%3+1);
				y = 190;
				sy = 0;
				insideBeam=false;
		}
		return; // dead, we don't move anymore... 
	}
	
	// BEAM TRACTION 
	// are we inside the beam of the UFO ? 
	if (ufo->beam_size > 0)
	{
		// cow middle x,y
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
						// add life --allows overcharge that slowly discharges
						ufo->life += rand()%5+5;
						
						// also If the UFO is dying and picks up a cow, can be saved!
						if (ufo->life < 0)
							ufo->life = rand()%2; // on the brink of dead (0% or 1%)
						
						// add some sparks particles
						int pz = rand()%8+10; // particle ammount
						int cp = makecol(85,255,255); // light cyan
						for (int p=0; p<pz;p++)
							pm->add(new Spark(cmx, cmy, (rand()%50-25)/10.0, (rand()%20+25)/10.0, rand()%8+8, cp));
						
						
						play_sample(cow_wav, 200 + rand()%55, x * 255 / 320, 800+rand()%600, 0);
					}
				
				// Im above the UFO??
				if (cmy < ufo->beam_y1)
				{
					y = ufo->beam_y1 + 6; // hover below UFO level
					x = ufo->beam_xu + rand()%6 - 3;
				}
			}
			else
				insideBeam = false;
	}
	else
		insideBeam = false; // so the cow doesnt fly randomly - fix a bug


	// animate
	if (++anm > 39) // 4 frames 0...39 / 10 = 0,1,2,3 in steps
		anm = 0;

	sprite = walk[(int)(anm/10)]; // change sprite

	// move
	y += sy;
	x += sx; 
	
	// fall to ground or stay on ground
	// does not apply if inside beam
	if (!insideBeam)
	{
		if (y > m->getHeight((int)(x + sprite->w/2)) - sprite->h+2)
		{
			y = m->getHeight((int)(x + sprite->w/2)) - sprite->h+2;
			sy = 0;
		}
		else
			sy += 0.5; // gravity
	}

	// randomly turn around harder to aim!
	if (!insideBeam)
	{
		if (rand()%1000 < 15) // 1,5% chances de cambiar de direccion
		{
				sx = -sx;	
		}

		// jump? maybe... harder to aim!	
		if (sy == 0 && rand()%1000<10) // sy == 0 means "not jumping already"
			sy = -(rand()%4+3);

		// bounce on borders
		if (x < -sprite->w)
			sx = rand()%3+1;	
		
		
		if (x > Map::mapW + sprite->w)
			sx = -(rand()%3+1);
			
	}
}

void Cow::render(BITMAP *bmp)
{
	
	if (life <= 0)
		return; // dont draw

	if (sx < 0) // left or right?
		draw_sprite(bmp, sprite, (int)x, (int)y);
	else
		draw_sprite_h_flip(bmp, sprite, (int)x, (int)y);	
		
}

