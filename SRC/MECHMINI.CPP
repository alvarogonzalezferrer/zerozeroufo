// mech flying enemy

#include "mechmini.h"
#include "logger.h"
#include "enemy.h"
#include "randomiz.h"

// static data for all the objects
BITMAP *MechMini::spr[4];
Datafile *MechMini::spr_data = NULL;
int MechMini::count = 0;


MechMini::MechMini(Datafile *data) : Enemy(data)
{
	Logger::log("MechMini::MechMini()");
	
	x = y = sx = sy = 0; // remember to position - TODO make a proper constructor!!
	
	life = 70;
	
	//if (count == 0) // first instance, get sprites and sounds 
	if (spr_data != data)
	{
		spr_data = data;
		
		// 0..2 FLYING
		spr[0] = (BITMAP *)data->getObject("MECH_0_BMP")->dat;
		spr[1] = (BITMAP *)data->getObject("MECH_1_BMP")->dat;
		spr[2] = (BITMAP *)data->getObject("MECH_2_BMP")->dat;
		
		spr[3] = (BITMAP *)data->getObject("MECH_3_BMP")->dat; // SHOOTING
		
		// TODO - add sounds
	}
	count++;
	
	sprite = spr[0];
	frame = rand()%30;
	ia = 0;
	shoot_c = 0;
		
	bbox = new Collide2D(x, y, sprite->w, sprite->h);
}


MechMini::~MechMini()
{
	Logger::log("MechMini::~MechMini()");
	delete(bbox);
	count--;
}


bool MechMini::update(Map *m, UFO *ufo, ParticleManager *pm, ShootsList *shoots )
{
	// move
	x += sx;
	y += sy; // THIS ENEMY FLIES!
	
		
	// animate
	frame++;
	
	if (frame > 29) // (0,10,20), FLYING animation
		frame=0;
	
	sprite = spr[frame/10];
	
	// IA
	// easy state machine, when reaches 0, choose another action
	ia--;
	if (ia<0)
	{
		ia = Randomize::random(30, 120); // frames to do the same?
		sx = Randomize::random(-1.5f, 1.5f); //  left or right?
		sy = Randomize::random(-1.5f, 1.5f); //  up or down?
		
		// chase enemy?
		if (rand()%100 < 40)
		{
			if (ufo->x > x)
				sx = abs(sx);
			else
				sx = -abs(sx);

			/*if (ufo->y > y)
				sy = abs(sy);
			else
				sy = -abs(sy);*/
		}
		
		// shoot? 75% chance
		if (rand()%100 < 75)
		{	
			shoot_c = 20;
			ia = 20;
		}
	}
	
	if (shoot_c > 0)
	{
		shoot_c--;
		
		if (shoot_c % 5 == 0)
		{
			// shoot
			
			// left or right?
			float ssx = (sx <= 0) ? -3.5 : 3.5;
			float sxd = (sx <= 0) ? x : x + sprite->w; // shoot from gun cannon
			
			shoots->add(new Shoot(  sxd, y + 15,
									ssx, 0, 
									200, 
									3, 
									0.01, 
									5, 
									makecol(255,255,85),
									0));
									
			sprite = spr[3]; // shoot frame
			
			// DEBUG ADD SOUND!
		}
		else
			sprite = spr[frame/10];
	}
	
	// bounce on borders 
	
	// dont crash on ground! unless im dead 
	if (y > m->getHeight(x) - sprite->h - 5 && life >0)
	{
		sy = -1.5; // dont fly lower!
		y = m->getHeight(x) - sprite->h - 5;
	}
	
	if (y < 0)
	{
		y = 0;
		sy = 1.5; // dont fly higher
	}
	
	// sides
	
	// side right should not clip since I can come from outside screen!
	if (x > m->mapW - sprite->w)
	{
		sx = -3;
		sy = 0;
		ia = 5; 
	}
	
	if (x < -sprite->w) // im gone by left
	{	
		// appear by left again :p 
		x = m->mapW + sprite->w + 5;
	}
		
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
	
	// end of still alive
	if (life > 0) 
		return false ; // im still alive
	
	
	// ----- dying sequence -----
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

void MechMini::render(BITMAP *bmp)
{
	if (sx <= 0) // left or right?
		draw_sprite(bmp, sprite, (int)x, (int)y);
	else
		draw_sprite_h_flip(bmp, sprite, (int)x, (int)y);
	
	// DEBUG
	//textprintf_ex(bmp, font, x, y, makecol(255,255,255), -1, "%d", life);
}
