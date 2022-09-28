// ---------------------------------------------------------------------------
// ufo.cpp
// ---------------------------------------------------------------------------
// Player's UFO
// ---------------------------------------------------------------------------
// By Kronoman. Sharp dressed man.
// Copyright (c) 2022, Kronoman
// In loving memory of my father.
// ---------------------------------------------------------------------------


// august 2022 - Kronoman

#include "ufo.h"
#include "logger.h"
#include "enemies.h"
#include  "shoots.h"

UFO::UFO()
{
	Logger::log("UFO::UFO()");
	
	// load data
	data.load("ufo.dat");
	
	ufo_bmp[0] = (BITMAP *)data.getObject("UFO_0")->dat;
	ufo_bmp[1] = (BITMAP *)data.getObject("UFO_1")->dat;
	ufo_bmp[2] = (BITMAP *)data.getObject("UFO_2")->dat;
	ufo_bmp[3] = (BITMAP *)data.getObject("UFO_3")->dat;
	
	// HUD FONT
	ufo_hud_font = (FONT *)data.getObject("UFO_HUD_FONT")->dat;
	
	// TODO - add sounds , dead, shooting etc 
	
	// setup
	anm = 0;
	sprite = ufo_bmp[anm];
	
	// beam 
	beam_color[0] = makecol(85,255,255);
	beam_color[1] = makecol(255,255,85);
	beam_color[2] = makecol(255,255,255);
	
	
	bbox = new Collide2D(x, y, sprite->w, sprite->h);
	
	shoots = new ShootsList();
	
	weapon = 0;
	
	// reset my stats
	next_game_reset();
}


void UFO::next_game_reset()
{
	// reset for next GAME
	score = 0;
	weapon = 0;
	// DEBUG -- ADD LIFES, CONTINUES, WEAPONS, ETC!
	next_level_reset();
}

void UFO::next_level_reset()
{
	// NEXT WAVE RESET 
	
	// ufo position and speed 
	x = 50;
	y = 50;
	sx = 0;
	sy = 0;
	
	//weapon = 0; // weapon power -- each enemy hit decreases our power so no need for this
	
	life = MAX_LIFE;
	beam_power = MAX_BEAM_POWER;	
	
	beam_size = 0;
	
	beam_x1 = beam_x2 = beam_y1 = beam_y2 = beam_xu = 0; // beam cache bounding box
	
	shoots->reset();
	
	shoot_recharge=0;
}

UFO::~UFO()
{
	Logger::log("UFO::~UFO()");
	
	// release data
	data.unload();
	
	delete(bbox);
	delete(shoots);
}


void UFO::update(int maxW, Map *map, ParticleManager *pm, EnemyList *enemies)
{
	// DEBUG ADD JOYSTICK CONTROL FROM MY ARCADE FRAMEWORK
	// DEBUT TODO KEYS SHOULD BE CONFIGURABLE
	
	// KEYBOARD CONTROL
	if (key[KEY_W] || key[KEY_UP])
		sy -= V_Y;

	if (key[KEY_S] || key[KEY_DOWN])
		sy += V_Y;

	if (key[KEY_D] || key[KEY_RIGHT])
		sx += V_X;

	if (key[KEY_A] || key[KEY_LEFT])
		sx -= V_X;
	
	if ((key[KEY_SPACE] || key[KEY_Z]) && beam_power > 10) // only if I have SOME power left
	{
		beam_size +=rand()%3+2; // grows in size fast!
		beam_power -=2;
	}
	else
	{
		// beam decreases power fast
		beam_size -= (beam_size / 2) + (rand()%3 + 1);
		if (beam_size < -10)
			beam_size = -10; // takes a while to power up!	
	}
	// END KEYBOARD CONTROLS
	
	// TODO - RUDIMENTARY SHOOT
	// TODO ADD DIFFERENT WEAPONS
	if ((key[KEY_ENTER] || key[KEY_X]) && shoot_recharge < 1)
	{
		
		float ssy = sy*2;

		// main shoot left or right?
		float ssx = (sx < 0) ? -4 : 4; // min speed for shoot
		
		ssx += sx; // momentum for shoot

		shoots->add(new Shoot(x + sprite->w/2, y + sprite->h/2,
								ssx, ssy, 
								200, 
								4, 
								0.0, 
								10, 
								makecol(85,255,85),
								0));
		
		// secondary shoots?
		if (weapon > 0)
		{
		// down front
		shoots->add(new Shoot(x + sprite->w/2, y + sprite->h/2,
								4, 2, 
								200, 
								3, 
								0.0, 
								7, 
								makecol(85,255,85),
								0));
			
		}
		
		if (weapon > 1)
		{
		// back
		shoots->add(new Shoot(x + sprite->w/2, y + sprite->h/2,
								-4, 0, 
								200, 
								3, 
								0.0, 
								7, 
								makecol(85,255,85),
								0));
			
		}
		
		if (weapon > 2)
		{
		// up front
		shoots->add(new Shoot(x + sprite->w/2, y + sprite->h/2,
								4, -2, 
								200, 
								3, 
								0.0, 
								7, 
								makecol(85,255,85),
								0));
			
		}
		
		if (weapon > 3)
		{
		// back down
		shoots->add(new Shoot(x + sprite->w/2, y + sprite->h/2,
								-4, 2, 
								200, 
								3, 
								0.0, 
								7, 
								makecol(85,255,85),
								0));
			
		}
		
		if (weapon > 4)
		{
		// back up
		shoots->add(new Shoot(x + sprite->w/2, y + sprite->h/2,
								-4, -2, 
								200, 
								3, 
								0.0, 
								7, 
								makecol(85,255,85),
								0));
			
		}
		
		// limit weapon power
		if (weapon > 5)
			weapon = 5;
		
		shoot_recharge = 10 - weapon; // better weapon shoots faster!
	}
	
	// limit beam to ground
	if (y + sprite->h + beam_size > map->getHeight(x + sprite->w*2) + 5) // 5 px of buffer to avoid glitch with weird terrain
		beam_size = map->getHeight(x + sprite->w*2) - y - sprite->h + 5; // prevent too much power
	
	// while beam is on, you cant fly vertical! // DEBUG, SURE ABOUT THIS? affects gameplay, makes it harder and annoying - TODO , remove
	if (beam_size > 0)
		sy = 0; //sy = (rand()%20-10) / 10.0; // random turbulence?
	
	
	// LIMIT MAX SPEED!
	if (sx > MV_X)
		sx = MV_X;

	if (sy > MV_Y)
		sy = MV_Y;
	
	if (sx < -MV_X)
		sx = -MV_X;
	
	if (sy < -MV_Y)
		sy = -MV_Y;
	
	// MOVE
	
	x += sx;
	y += sy;
	
	
	// LIMIT POSITION TO SCREEN!
	if (x < 0)
		x = 0;
	
	if (y < 0)
		y = 0;
	
	if (x > maxW - sprite->w)
		x = maxW - sprite->w;
	
	// bounce on ground and give damage
	if (y > map->getHeight((int)(x + sprite->w/2)) - sprite->h *0.95)
	{
		y = map->getHeight((int)(x + sprite->w/2)) - sprite->h - rand()%8 ;		

		sy = -2 - rand()%2;
		sx = rand()%4-2; // turbulence
		
		life -= rand()%7+3;
		
		// sparks
		int pz = rand()%8+8; // particle ammount
		for (int p=0; p<pz;p++)
			pm->add(new Spark(x + sprite->w/2, y+sprite->h - 2, (rand()%40-20)/10.0, (rand()%10+20)/-10.0, rand()%5+4, makecol(255,255,rand()%170+85)));
	}

	
	// FRICTION	
	if (sx > 0)
	{
		sx -= F_X;
		if (sx < 0)
			sx = 0; // clip
	}
	else if (sx < 0)
	{
		sx += F_X;
		if (sx > 0)
			sx = 0; // clip
	}
	
	if (sy > 0)
	{
		sy -= F_Y;
		if (sy < 0)
			sy = 0; // clip
	}
	else if (sy < 0)
	{
		sy += F_Y;
		if (sy > 0)
			sy = 0; // clip
	}
	
	// animate	
	// frames goes in 10 steps to regulate speed
	anm++;
	
	if (beam_size > 0)
		anm +=5; // if you have the beam active the UFO makes the lights faster! 
	
	if (anm > 39)
		anm = 0;
	
	sprite = ufo_bmp[(int)(anm/10)]; // animate
	
	// update bounding box collision detection
	bbox->x = x;
	bbox->y = y;
	
	// BEAM RECHARGE
	if (beam_power < MAX_BEAM_POWER)
		beam_power++;
	
	if (beam_power > MAX_BEAM_POWER)
		beam_power=MAX_BEAM_POWER;
	
	// SMOKE AND SPARKS IF IM DAMAGED
	// cant start smoking at random at around 30% damage
	if (life < (MAX_LIFE/4) + rand()%10)
	{
		int cmx = x + sprite->w/2 + rand()%8 - 4;
		int cmy = y + sprite->h/2 + rand()%8 - 4;

		// add some smoke particles
		if (rand()%100 < 70) // smoke or sparks?
		{
			int f = rand()%85+25; // smoke shade
			int cp = makecol(f,f,f); // smoke
			pm->add(new Smoke(cmx, cmy, ((rand()%25)/10.0)*(-sx), -(rand()%20)/10.0, rand()%4+5, cp, rand()%3+1, (rand()%100)/100.0));
		}
		else
		{
			// sparks
			int f = rand()%55+200; // spark shade
			int cp = makecol(f,f,0);
			pm->add(new Spark(cmx, cmy, (rand()%40-20)/10.0, (rand()%40-20)/10.0, rand()%5+8, cp));
		}
	}
	
	// update my shoots
	shoots->update(map, pm, NULL, enemies);
	
	shoot_recharge--;
	if (shoot_recharge <0)
		shoot_recharge=0;
	
	// beam size cache
	if (beam_size > 0)
	{
		int by = y + sprite->h + beam_size;
		if (by > 200)
			by = 200; // tops at bottom
		
		// cache beam size
		beam_x1 = x - 10; 
		beam_x2 = x + sprite->w + 10;
		beam_y1 = y + sprite->h;
		beam_y2 = by;
		beam_xu =  x + sprite->w/2;	
	}
	else
		beam_x1 = beam_x2 = beam_y1 = beam_y2 = beam_xu = 0;
	
	// life 
	// if im overcharged, slowly discharge
	// I have a shield that goes up to 200 % !
	if (life > MAX_LIFE * 2)
		life--;
}

void UFO::render(BITMAP *bmp)
{
	shoots->render(bmp); // render my shoots
	
	// do I have a shield ? 
	if (life > MAX_LIFE)
		circle(bmp, x + sprite->w/2, y + sprite->h/2, (sprite->w/2) + rand()%3, beam_color[rand()%3]);
	
	draw_sprite(bmp, sprite, x, y); // render ufo
	 
	// beam 
	if (beam_size > 0)
		triangle(bmp, beam_xu, beam_y1, beam_x1, beam_y2, beam_x2, beam_y2, beam_color[rand()%3]);	
}

void UFO::render_HUD(BITMAP *bmp)
{
	// render game HUD
	int c = makecol(255,255,255); // white
	int hc = c; // health color 
	
	// change color of life legend %
	if (life < MAX_LIFE*0.8)
		hc = makecol(255,255,85); // yellow
	
	if (life < MAX_LIFE*0.5)
		hc = makecol(255,85,255); // pink
	
	if (life < MAX_LIFE*0.25)
		hc = makecol(255,85,85); // light red
	
	if (life < MAX_LIFE*0.1)
		hc = makecol(170,0,0); // red
	
	
	int fh = text_height(ufo_hud_font);
	textprintf_ex(bmp, ufo_hud_font, 0, 0, c, -1, "Score: %lu000000", score);
	
	textprintf_ex(bmp, ufo_hud_font, 0, fh+2, c, -1, "Power: %d%%", (int)((float)beam_power/(float)MAX_BEAM_POWER*100.0));
	
	if (life > 0)
	{
		if (life <= MAX_LIFE)
			textprintf_ex(bmp, ufo_hud_font, 0, (fh+2)*2, hc, -1, "Armor: %d%%", (int)((float)life/(float)MAX_LIFE*100.0));
		else // shield engaged
			textprintf_ex(bmp, ufo_hud_font, 0, (fh+2)*2, beam_color[rand()%3], -1, "Shield: %d%%", (int)((float)life/(float)MAX_LIFE*100.0));
	}
	else
		textprintf_ex(bmp, ufo_hud_font, 0, (fh+2)*2, hc, -1, "CRITICAL DAMAGE");

}
