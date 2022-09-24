// -----------------------------------------------------------------------
// B17 enemy
// -----------------------------------------------------------------------
// can do a bomb run or use their machine guns up, front or back, quite dangerous enemy!
// always goes from right to left, doesnt chase, flies leveled
// -----------------------------------------------------------------------
#include "b17.h"
#include "logger.h"
#include "enemy.h"
/******************************* disabled for better approach **************************
// -----------------------------------------------------------------------
// B 17 - BOMB weapon
// -----------------------------------------------------------------------
BombB17::BombB17(float x, float y,
				 float sx, float sy,
				 int life,
				 int t, 
				 float grav, 
				 int damage,
				 int color,
				 int particle_trail,
				 BITMAP *sprite) : Shoot(x,y,sx,sy,life,t,grav,damage,color,particle_trail)
{
	spr = sprite;
}

BombB17::~BombB17()
{
	// nothing to do
}


bool BombB17::update(Map *m, ParticleManager *pm, UFO *ufo, EnemyList *enemies)
{
	life --;
	
	x += sx;
	y += sy;
	
	sy += grav; // apply gravity
	
	bbox->setAsBox(x ,y, spr->w, spr->h);
	
	if (y > m->getHeight(x)+t+2)
	{
		// hit the ground, add sparks!
		int pz = rand()%5+5;

		for (int p=0; p<pz;p++)
			pm->add(new Spark(x, y, (rand()%40-20)/10.0, -(rand()%40)/10.0, rand()%5+5, color));

		return true; // hit the ground, go away
	}
	
	if (x < -10 || x > m->mapW + 10 )
		return true; // remove me
	
	if (y > m->mapH + 10)
			return true;
		
	if (y < -100) // out of map up
		return true;

	// detect collision ?
	if (ufo) 
	{
		// check against UFO
		if (bbox->collide(ufo->bbox))
		{
			ufo->life -= damage;
			
			// downgrade weapon
			if (ufo->weapon > 0)
				ufo->weapon--;
			
			// add sparks!
			// DEBUG should be according to damage
			// DEBUG PLAY SOUND!
			// DEBUG TODO: shake camera! (how?)
			int pz = rand()%5+5+(damage/10); // particle ammount
			
			for (int p=0; p<pz;p++)
				pm->add(new Spark(x, y, (rand()%40-20)/10.0, (rand()%40-20)/10.0, rand()%8+8, color));

			return true; // I collided 
		}
	}
	
	if (life < 0)
		return true; // im gone
	
	return false; // im alive
}

void BombB17::render(BITMAP *bmp)
{
	if (sx <= 0) // left or right?
		draw_sprite(bmp, spr, (int)x, (int)y);
	else
		draw_sprite_h_flip(bmp, spr, (int)x, (int)y);
}

class BombB17;
************************************************************************************/

// -----------------------------------------------------------------------
// B 17 - ENEMY
// -----------------------------------------------------------------------
// static data for all the objects
BITMAP *B17::spr[3];
BITMAP *B17::spr_bomb = NULL;
Datafile *B17::spr_data = NULL;
int B17::count = 0;



B17::B17(Datafile *data) : Enemy(data)
{
	Logger::log("B17::B17()");
	
	x = y = sx = sy = 0; // remember to position! - TODO make a proper constructor!!
	
	life = 50;
	
	//if (count == 0) // first , get sprites and sounds 
	if (spr_data != data)
	{
		spr_data = data;
		
		spr[0] = (BITMAP *)data->getObject("B17_0_BMP")->dat;
		spr[1] = (BITMAP *)data->getObject("B17_1_BMP")->dat;
		spr[2] = (BITMAP *)data->getObject("B17_2_BMP")->dat;
		
		// bomb 
		spr_bomb = (BITMAP *)data->getObject("B17_BOMB_BMP")->dat;
		
		// TODO - add sounds
	}
	count++;
	
	sprite = spr[0];
	frame = rand()%3;
	ia = 0;
	bomb_run = false;
	machine_gun = false;
	
	bbox = new Collide2D(x, y, sprite->w, sprite->h);
}


B17::~B17()
{
	Logger::log("B17::~B17()");
	delete(bbox);
	count--;
	// DATA IS HANDLED BY ENEMIES.CPP, DO NOT UNLOAD HERE! IS SHARED ACROSS ENEMIES!
}


bool B17::update(Map *m, UFO *ufo, ParticleManager *pm, ShootsList *shoots )
{
	// move
	x += sx;
	y += sy; // THIS ENEMY FLY!
	
		
	// animate
	frame++;
	
	if (frame > 2) // (0,1,2), fast animation 
		frame=0;
	
	sprite = spr[frame];
	
	// IA
	// easy state machine, when reaches 0, choose another action
	ia--;
	if (ia < 0)
	{
		ia = rand()%15+30; // frames to do the same?
		sx = -0.3; // always left very slow
		
		sy = (rand()%30-15) / 10.0; // up or down?
	
	
		// try to go above the UFO to bomb it 
		if (ufo->y > y + sprite->h*2 && rand()%100 < 70)
			sy = -0.5;
		else
		{
			if (rand()%100 < 60)
				sy = 0; // level flight
		}
	
		machine_gun = 0;
		bomb_run = 0;
		
		if (rand()%100 < 60)
		{
			machine_gun = rand()%15+15;
			ia += rand()%15;
		}
		
		if (rand()%100 < 50 || ufo->y > y + sprite->h * 2)
		{
			bomb_run = rand()%15+10;
			sy = -abs(sy); // always fly up or level
			ia += rand()%15;
		}
	}
	
	if (--machine_gun > 0 && ia % 6 == 0)
	{
		shoots->add(new Shoot(x, y + sprite->h / 2,
							-3, 0, 
							100, 
							2, 
							0.01, 
							3, 
							makecol(255,255,85),
							0));
							
		// turret
		shoots->add(new Shoot(x + sprite->w / 2, y + sprite->h / 2,
							-3, -2, 
							100, 
							2, 
							0.02, 
							3, 
							makecol(255,255,85),
							0));

		// tail gunner
		shoots->add(new Shoot(x + sprite->w / 2, y + sprite->h / 2,
							3, 0, 
							100, 
							2, 
							0.01, 
							3, 
							makecol(255,255,85),
							0));
	}
	//else // can operate machine gun OR bombs, not both at the same time
	if (--bomb_run > 0 && ia % 6 == 0)
	{
		shoots->add(new Shoot(x + sprite->w / 2, y + sprite->h - 5 + rand()%5,
								0, 0.3, 
								300, 
								spr_bomb, 
								0.05, 
								5, 
								makecol(255,255,85),
								0));
	}
	
	// bounce on borders 
	
	// dont crash on ground! unless im dead 
	if (y > m->getHeight(x) - sprite->h*3 && life >0)
	{
		sy = (rand()%15) / -10.0;; // dont fly lower!
		y = m->getHeight(x) - sprite->h*3;
	}
	
	if (y < 0)
	{
		y = 0;
		sy = (rand()%15) / 10.0;; // dont fly higher
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
	
	// -- end alive --
	if (life > 0) 
		return false ; // im still alive
	// if im still alive I end doing AI here 
	// if not, I do a dying sequence below
	
	// IM DEAD , do the fall animation
	ia = 1000; // disable IA next step
	life--; // keep going below 0
	sy = abs(sy) + .3; // go down fast
	machine_gun = 0;
	bomb_run = 0;

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
		return true;
	}
		
	return false; // im still alive, but dying
}	

void B17::render(BITMAP *bmp)
{
	if (sx <= 0) // left or right?
		draw_sprite(bmp, sprite, (int)x, (int)y);
	else
		draw_sprite_h_flip(bmp, sprite, (int)x, (int)y);
}
