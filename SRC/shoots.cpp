#include "shoots.h"

// -----------------------------------------------------------------------
// base shoot
// -----------------------------------------------------------------------

Shoot::Shoot()
{
	x=y=sx=sy=0;
	life=0;
	t=0;
	grav=0;
	damage=0;
	color=0;
	particle_trail=0;
	bbox = new Collide2D(x, y, sx, sy);
	sprite = NULL;

}

// geometry shoot
Shoot::Shoot( float x, float y,
			  float sx, float sy,
			  int life,
			  int t, 
			  float grav, 
			  int damage,
			  int color,
			  int particle_trail)			  
{
	this->x = x;
	this->y = y;
	this->sx = sx;
	this->sy = sy;
	this->life = life;
	this->t = t; // shoot type 0=pixel 1=line 2 and plus circle radius
	this->grav = grav; // gravity
	this->damage = damage;
	this->color = color;
	this->particle_trail = particle_trail;
	sprite = NULL;
	bbox = new Collide2D(x, y, sx, sy);
}

// sprite shoot
Shoot::Shoot(float x, float y,
			float sx, float sy,
			int life,
			BITMAP *spr, 
			float grav, 
			int damage,
			int color,
			int particle_trail)
{
	this->x = x;
	this->y = y;
	this->sx = sx;
	this->sy = sy;
	this->life = life;
	this->t = -1; // sprite shoot
	this->grav = grav; // gravity
	this->damage = damage;
	this->color = color;
	this->particle_trail = particle_trail;
	
	sprite = spr;
	bbox = new Collide2D(x, y, sprite->w, sprite->h);			  
}

Shoot::~Shoot()
{
	if (bbox)
		delete(bbox);
}

// update shoots
bool Shoot::update(Map *m, ParticleManager *pm, UFO *ufo, EnemyList *enemies, Boss *boss)
{
	// update movement 
	life --;
	
	x += sx;
	y += sy;
	
	sy += grav; // apply gravity
	
	// test against out of screen
	// if (y < 0)   // I dont care if it goes up screen, it may come down in some shoots
	//
		
	if (y + bbox->h > m->getHeight(x)+t+2)
	{
		// hit the ground, add sparks!
		int pz = rand()%5+5;

		for (int p=0; p<pz;p++)
			pm->add(new Spark(x, y, (rand()%40-20)/10.0, -(rand()%40)/10.0, rand()%5+5, color));

		return true; // hit the ground, go away
	}
	
	if (x < -bbox->w || x > m->mapW + bbox->w )
		return true; // remove me
	
	if (y > m->mapH + 10)
			return true;
		
	if (y < -100) // out of map up
		return true;
	
	// update bounding box 
	if (sprite)
	{
		bbox->setAsBox(x ,y, sprite->w, sprite->h);
	}
	else
	{
		switch (t)
		{
			case 0: // is a pixel
			case 1: // is a line, i also try it as a pixel
				bbox->setAsPoint(x,y);			
			break; 
			
			default: // is a circle of radius t-1 (2 and up)
				bbox->setAsCircle(x,y,t-1);			
			break;
		}
	}
	
	// detect collision against UFO?
	if (ufo) 
	{
		// check against UFO
		if (bbox->collide(ufo->bbox))
		{
			ufo->life -= damage; // DEBUG TODO : ADD DIFFICULTY MULTIPLIER!
			
			// PLAY HIT SOUND
			play_sample(ufo->ufo_hit_wav, 200 + rand()%55, ufo->x * 255 / 320, 900 + rand()%500, 0);
			
			// downgrade weapon , only if does not have shield, Zir suggestion
			if (ufo->life < UFO::MAX_LIFE) 
				if (ufo->weapon > 0)
					ufo->weapon--;
			
			// add sparks!
			int pz = rand()%5+5+(damage/10); // particle ammount
			
			for (int p=0; p<pz;p++)
				pm->add(new Spark(x, y, (rand()%40-20)/10.0, (rand()%40-20)/10.0, rand()%10+5, color));

			return true; // I collided 
		}
	}
	
	// against enemies?
	if (enemies)
	{
		// check against ALL enemies 
		for (int i=0; i<enemies->enemies_n; i++)
		{
			if (bbox->collide(enemies->enemy[i]->bbox))
			{
				enemies->enemy[i]->life -= damage; // damage enemy
				
				// DEBUG - PLAY HIT SOUND?
				
				// add "damage" sparks
				int pz = rand()%5 + 5 + (damage/5); // particle ammount
			
				for (int p=0; p<pz;p++)
					pm->add(new Spark(x, y, (rand()%50-25)/10.0, (rand()%50-25)/10.0, rand()%10+5, color));

				return true; // I collided 
			}
		}
	}
	
	// against boss ? 
	if (boss)
	{
		if (boss->shoot_bbox) // damage area?
		{
			// check first against shoot box of boss
			if (bbox->collide(boss->shoot_bbox))
			{
				boss->life -= damage; 
				
				// DEBUG - PLAY HIT SOUND? boss should have one!
				// add "damage" sparks
				int pz = rand()%5 + 5 + (damage/5); // particle ammount
			
				for (int p=0; p<pz;p++)
					pm->add(new Spark(x, y, (rand()%50-25)/10.0, (rand()%50-25)/10.0, rand()%10+5, color));

				return true; // I collided 
			}
		}
		
		// check against non-damage area, if there is one
		if (boss->bbox)
		{
			// check first against shoot box of boss
			if (bbox->collide(boss->bbox))
			{
				//boss->life -= damage; this does not damage the boss
				
				// DEBUG - show message that this is not a damage area!
				
				// add "damage" sparks
				int pz = 5; // particle ammount
			
				for (int p=0; p<pz;p++)
					pm->add(new Spark(x, y, (rand()%50-25)/10.0, (rand()%50-25)/10.0, rand()%10+5, makecol(85,85,85)));

				return true; // I collided 
			}
		}
	}
	
	// add particles? 
	if (particle_trail > 0)
	{
		// position of particles ?
		//int px = x;
		//int py = y;
		
		// DEBUG FOR SPRITES, SHOULD COME OF THE TAIL OR CENTER? 
		
		// TODO add diferent particle effects
		switch(particle_trail)
		{
			case 0:
				// none
			break; 
			
			case 1: // spark on same color going out (like a trail) 
				pm->add(new Spark(x + bbox->w/2, y + bbox->h/2, -sx, -sy, rand()%3+3, color));
			break;
			
			case 2: // smoke on same color going out (like a trail) 
				//Smoke(float x, float y, float dx, float dy, int life, int color, float radius, float radiusGrow);
				pm->add(new Smoke(x + bbox->w/2, y + bbox->h/2, -sx, -sy, rand()%3+3, color, rand()%2+1, (rand()%30)/100.0));
			break;
			
			case 3: // dark smoke
			{   // {} block needed because of internal vars
				int f = rand()%85+25;
				int cp = makecol(f,f,f);
				pm->add(new Smoke(x+ bbox->w/2-sx, y+ bbox->h/2-sy, -sx, -sy, rand()%5+5, cp, rand()%3+2, (rand()%30)/100.0));
			
				break;
			}
			
			default:
			break;
		}
	}
	
	if (life < 0)
		return true; // im gone
	
	return false; // im alive
}

void Shoot::render(BITMAP *bmp)
{
	// is sprite or geometry?
	if (sprite)
	{
		if (sx <= 0) // left or right?
			draw_sprite(bmp, sprite, (int)x, (int)y);
		else
			draw_sprite_h_flip(bmp, sprite, (int)x, (int)y);
	
		return;
	}
	
	// geometry
	switch(t)
	{
		case 0:
			 putpixel(bmp, (int)x, (int)y, color);
		break;
		
		case 1:
			line(bmp,(int)x,(int)y,(int)(x+sx*2),(int)(y+sy*2), color);
		break;
		
		default:
			circlefill(bmp, (int)x, (int)y, t-1, color);
		break;
	}
}

// -----------------------------------------------------------------------
// shoot list handler 
// -----------------------------------------------------------------------
ShootsList::ShootsList()
{
	shoots_n = 0;
	// reset list 
	for (int i=0; i < maxSHOOTS; i++)
		shoots[i] = NULL;
}

ShootsList::~ShootsList()
{
	reset(); // bye
}

void ShootsList::reset()
{
	for (int i=0; i < shoots_n; i++)
		if (shoots[i])
		{
			delete(shoots[i]);
			shoots[i] = NULL;
		}
	
	shoots_n = 0;
}


void ShootsList::update(Map *m, ParticleManager *pm, UFO *ufo)
{
	update(m, pm, ufo, NULL, NULL); // enemis call this
}

void ShootsList::update(Map *m, ParticleManager *pm, EnemyList *enemies, Boss *boss)
{
	update(m, pm, NULL, enemies, boss); // UFO call this
}

// moves the shoots and test against enemies or UFO (any can be NULL to avoid friendly fire)
void ShootsList::update(Map *m, ParticleManager *pm, UFO *ufo, EnemyList *enemies, Boss *boss)
{
	for (int i=0; i < shoots_n; i++)
		if (shoots[i]->update(m, pm, ufo, enemies, boss))
		{
			// shoot is dead , remove from last one
			delete(shoots[i]);
			shoots[i] = shoots[shoots_n-1];
			shoots_n--;
		}
}

// add a shoot, will be auto deleted when life <= 0 or hit something
void ShootsList::add(Shoot *shoot)
{
	// will replace last shoot if queue is full!
	shoots_n++;
	
	if (shoots_n > maxSHOOTS)
	{
		// TODO - DEBUG - I DIDNT TESTED THIS , MAY PRODUCE A MEMORY LEAK!
		delete(shoots[maxSHOOTS-1]); // prevents memory leak deleting previously occupied shoot
		shoots_n = maxSHOOTS;
	}

	shoots[shoots_n-1] = shoot;
}

void ShootsList::render(BITMAP *bmp)
{
	for (int i=0; i < shoots_n; i++)
		shoots[i]->render(bmp);
}

