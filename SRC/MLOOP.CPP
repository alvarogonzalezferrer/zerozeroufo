// -----------------------------------------------------------------------
// maingloop.cpp
// -----------------------------------------------------------------------
// Main Game Loop
// Based on my generic game machine
// -----------------------------------------------------------------------
// Copyright (c) 2007-2022, Kronoman
// In loving memory of my father.
// Made in Argentina
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------
#include <allegro.h>
#include "mloop.h"
#include "error.h"
#include "logger.h"


MainGameLoop::MainGameLoop()
{	
    Logger::log("MainGameLoop::MainGameLoop()");
	
	debug=false;
	
	wait = 0;
	camerashake = 0;

	ticks=0;
	
	intro_screens_data.load("screens.dat"); // intro screens
	
	// DEBUG - do this here?
	
	ufo = new UFO(); // new player
	map = new Map(); // new map
	enemyList = new EnemyList(); // new enemies
	pm = new ParticleManager(); // particle manager
	parallax = new Parallax(); // parallax background
	
	wave = 0; // current wave
	
}

MainGameLoop::~MainGameLoop()
{
	Logger::log("MainGameLoop::~MainGameLoop()");

	intro_screens_data.unload();
	
	delete(ufo);
	delete(enemyList);
	delete(map);
	delete(pm);
	delete(parallax);
}


void MainGameLoop::update()
{
	ticks++; // game ticks 
	
	// handle the states...
	switch (currentState)
	{
	case STATE_START: // game starts here , this is state 0 , when you run the exe, this is the entry point
	{
		camerashake = 0;
		// I re create the bitmap as a GAME_W_RES * GAME_H_RES only, to support any resolution
		if (doubleBuffer)
			destroy_bitmap(doubleBuffer); // bitmap
		
		doubleBuffer = create_bitmap(GAME_W_RES, GAME_H_RES);
		
		if (doubleBuffer == NULL)
			Error::error("MainGameLoop -> STATE_START\nERROR: can't get a bitmap");
		
		// locate and load data 
		// THIS SHOULD BE DONE ONLY ONCE PER GAME RUN, NOT ONCE PER GAME RESTART!!
		
		// since this is 8 bpp MS DOS stuff, load palette EGA
		ega_pal = (PALETTE *)intro_screens_data.getObject("EGA_PAL")->dat;
		
		set_palette (*((PALETTE*)ega_pal)); // tengo que hacer esto porque es un puntero, y no una matriz
		
		create_rgb_table(&rgb_table, *((PALETTE*)ega_pal), NULL);
		rgb_map = &rgb_table;
		// end of palette stuff
		
		// load intro 
		intro_bmp = (BITMAP *)intro_screens_data.getObject("INTRO_BMP")->dat;
		
		// FBI retro message
		fbi_bmp = (BITMAP *)intro_screens_data.getObject("FBI_BMP")->dat;
		
		// next wave
		next_wave_bmp = (BITMAP *)intro_screens_data.getObject("NEXT_WAVE_BMP")->dat;
		
		// help screen
		help_bmp = (BITMAP *)intro_screens_data.getObject("HELP_BMP")->dat;
		 
		// game over 
		gameover_bmp = (BITMAP *)intro_screens_data.getObject("GAME_OVER_BMP")->dat;
		
		// you won 
		won_bmp = (BITMAP *)intro_screens_data.getObject("WON_BMP")->dat;

		// kronoman screen 
		kronoman_bmp =  (BITMAP *)intro_screens_data.getObject("KRONOMAN_BMP")->dat;
		kronoman_wav =  (SAMPLE *)intro_screens_data.getObject("KRONOMAN_WAV")->dat;
		  
		/*
		// sounds
		shootSND=(SAMPLE *)data.getObject("SHOOTSND")->dat;
		*/
		
		help_show = 300; // show help screen for 10 seconds
		
		wave = 0;
		
		wait = 60;
		changeState(STATE_FBI); // FBI message intro, retro play! // put STATE_INTRO to avoid fbi message!
	}
	break;
	
	case STATE_FBI:
		if (--wait < 0)
		{
			// play sample for next screen 
			play_sample(kronoman_wav, 255, 128, 1000, 0);
			wait = 90;
			changeState(STATE_KRONOMAN);
		}
	break;
	
	case STATE_KRONOMAN:
		if (--wait < 0)
		{
			wait = 150;
			changeState(STATE_INTRO);
		}
	break;

	// waits for intro screen
	case STATE_INTRO:
		camerashake = 0;
		wave = 0; // first level
		if (--wait < 0)
			changeState(STATE_NEXTLEVEL);
	break;

	// waits game over message
	case STATE_GAMEOVER:
		// TODO highscore , continue, etc 
		camerashake = 0;

		if (--wait < 0)
		{
			// reset score , ufo, etc 
			ufo->next_game_reset();
			
			wait = 150;
			changeState(STATE_FBI);
		}
	break;

	// generates next level
	case STATE_NEXTLEVEL:
	{
		wave ++; // next wave 

		// map generation
		// is a chain to select aproppiate weather, land and background
		
		// in this chain, everything gets generated 
		// first background is loaded 
		// then land generated according to background
		// then weather according to map
		weather.generate(map->generate(parallax->load(wave)));
		
		// end of map generation
		
		cowL.generate(rand() % (CowList::maxCOWS - 5) + 5); // DEBUG CHECK MAX SIZE !! ADJUST TO DIFFICULTY, HARDER LESS COWS!
	
		camerashake = 0;
		
		ufo->next_level_reset();
		ufo_last_life = ufo->life;
		
		pm->finish(); // reset particles
		
		enemyList->reset(); // bye residual enemies
		
		// the shoots of UFO and enemies are handled by themselves, so they get reset without need to call here

		// done, go to level intro
		wait = 150;
		changeState(STATE_INTRONEXTLEVEL);
	}
	break;

	// waits presentation of next level
	case STATE_INTRONEXTLEVEL:
		if (--wait < 0)
		{
			changeState(STATE_OK); // start game!
			ticks=0; // reset game ticks each new level!
		}
	break;

	// gameplay
	case STATE_OK:
	{	
		// main state of game

		// reduce help message
		if (help_show>0)
			help_show--;

		// keyboard
		
		if (key[KEY_F1]) // help
		{
			help_show = 150; // show help screen for 5 seconds
			while(key[KEY_F1]); // wait key release
		}
		
		
		if (key[KEY_K] && key[KEY_R] && key[KEY_O]) // KRO = cheat code to skip level
		{
			changeState(STATE_NEXTLEVEL);
			while (key[KEY_K] && key[KEY_R] && key[KEY_O]); // wait key release!
		}
			
		parallax->update(); // update background
		
		map->update(); // update game map

		ufo->update(doubleBuffer->w, map, pm, enemyList); // update player UFO
		
		// -- camera shake on damage --
		// did ufo receive damage? shake camera  (i check last frame life with new life)
		if (ufo_last_life > ufo->life && ufo_last_life < UFO::MAX_LIFE) // max life prevents shake when overloaded
			camerashake = rand()%5 + (ufo_last_life - ufo->life);
		
		ufo_last_life = ufo->life;
		//--- end camera shake on damage 
		
		cowL.update(map, ufo, pm); // update cows
		
		pm->update(); // update particles
		
		weather.update(); // update weather
		
		int ret_e = enemyList->update(map, ufo, pm); // update enemies
		
		// add enemies each screen, dont add if we traveled enough
		bool finished = ticks > 2000 + wave*200; // each wave the level is a little longer  
		
		// generate new enemies
		if ((ticks % 300 == 0 || ret_e == 0) && !finished)
			enemyList->generate(rand() % 5 + wave, wave, map); //  GENERATE TIER / WAVE APPROPIATE ENEMIES // TODO - ADD BOSSES
		
		// end level when enough travel and no enemies - DEBUG CHECK BOSSES TOO
		if (ret_e == 0 && finished) 
				changeState(STATE_NEXTLEVEL);
			
		// UFO is dying??
		if (ufo->life < 1)
		{
			ufo->life--;
			if (ufo->life < -300) // you have +10 seconds to try to grab some energy
			{
				wait = 180;
				changeState(STATE_GAMEOVER);
			}
		}
	}
	break;

	default:
		// probably a error if we go here... a unhandled state... mmm
		//Error::error("ERROR\nUnhandled state in mloop.cpp (state = %d); %s in line %d\n", currentState, __FILE__, __LINE__ );
	break;
	}


	// for all states
	if (key[KEY_SPACE]) // avoid waiting screens!
	{
		if (wait > 30) // 30 ticks = 1 second real life
		{
			wait = wait / 2;
			help_show = help_show / 2; // speed up help screen too, if available
			clear_keybuf();
		}
	}

	if (key[KEY_F10] || key[KEY_ESC]) // key to exit...DEBUG - confirm!
		changeState(STATE_QUIT);
		
	if (key[KEY_F12]) // debug key
	{
		debug = !debug; // toggle debug
		while(key[KEY_F12]); // wait key release
	}

	if (camerashake > 0)
		camerashake --;
}

void MainGameLoop::render()
{
	switch (currentState)
	{
	case STATE_START:
		// TODO -- add loading message
		textprintf_ex(doubleBuffer, font,0,0, makecol(255,255,255), -1, "Loading, please wait.");
	break;
	
	case STATE_FBI:
		blit(fbi_bmp, doubleBuffer, 0, 0, 0, 0, fbi_bmp->w, fbi_bmp->h);
	break; 
	
	case STATE_KRONOMAN:
		clear_to_color(doubleBuffer, makecol(0, 0, 0));
		blit(kronoman_bmp, doubleBuffer, 0, 0, (doubleBuffer->w - kronoman_bmp->w) / 2 , (doubleBuffer->h - kronoman_bmp->h) / 2, kronoman_bmp->w, kronoman_bmp->h);
	break;

	// shows intro screen
	case STATE_INTRO:
		blit(intro_bmp, doubleBuffer, 0, 0, 0, 0, intro_bmp->w, intro_bmp->h);		
	break;

	// shows game over screen
	case STATE_GAMEOVER:
		blit(gameover_bmp, doubleBuffer, 0, 0, 0, 0, gameover_bmp->w, gameover_bmp->h);
		textprintf_ex(doubleBuffer, font,  0, 0, makecol(255,255,255), makecol(85,85,85) , "FINAL SCORE %03lu000000", ufo->score);
	break;

	// shows intro to next level
	case STATE_INTRONEXTLEVEL:
		
		blit(next_wave_bmp, doubleBuffer, 0, 0, 0, 0, intro_bmp->w, intro_bmp->h);
		
		textprintf_ex(doubleBuffer, font,  doubleBuffer->w/2 -  text_length(font, "WAVE XX")/2, doubleBuffer->h/2 - text_height(font)/2, makecol(255,255,255),makecol(85,85,85) , "WAVE %d", wave);
	break;


	case STATE_OK: // main game state
	{
		parallax->render(doubleBuffer); // render background
		
		map->render(doubleBuffer); // render map
		
		// do I must show help?
		if (help_show > 0)
			masked_blit(help_bmp, doubleBuffer, 0, 0, (doubleBuffer->w - help_bmp->w) / 2, (doubleBuffer->h - help_bmp->h) / 2, help_bmp->w, help_bmp->h);
	
		ufo->render(doubleBuffer); // render player UFO
		
		cowL.render(doubleBuffer); // render cows
		
		enemyList->render(doubleBuffer); // render enemies
		
		weather.render(doubleBuffer); // render weather
		
		pm->render(doubleBuffer,0,0); // render particles
		
		ufo->render_HUD(doubleBuffer); // render HUD
		
		// DEBUG
		if (debug)
			textprintf_ex(doubleBuffer, font, 0, doubleBuffer->h - text_height(font)*2, makecol(255,255,255), -1, "e %d - t %lu - FPS %d", enemyList->enemies_n, ticks, fps_counted);
	}	
	break;

	default:
		// sometimes comes here with states that dont have graphic representation, like loading, etc
		// render nothing
	break;
	}

	// fill screen -- lame way to support many resolutions :P
	if (camerashake > 0) // do camera shake effect, for explosions / damage / etc
	{
		stretch_blit(doubleBuffer, screen, 0, 0, doubleBuffer->w, doubleBuffer->h, rand()%10-5, rand()%10-5, SCREEN_W+rand()%15, SCREEN_H+rand()%15); 
	}
	else
	{
		if (doubleBuffer->w == SCREEN_W && doubleBuffer->h == SCREEN_H)
			blit(doubleBuffer, screen, 0, 0, 0, 0, doubleBuffer->w, doubleBuffer->h);
		else
			stretch_blit(doubleBuffer, screen, 0, 0, doubleBuffer->w, doubleBuffer->h, 0, 0, SCREEN_W, SCREEN_H);
	}
}

