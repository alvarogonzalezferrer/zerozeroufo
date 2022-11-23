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
	
	// initialization
	
	ufo = new UFO(); // new player
	map = new Map(); // new map
	enemyList = new EnemyList(); // new enemies
	pm = new ParticleManager(); // particle manager
	parallax = new Parallax(); // parallax background
	
	wave = 0; // current wave
	
	// remove last enemy message
	x_destroy_m = y_destroy_m = 0;
	t_destroy_m = -1;
	
	// menus ; they're setup at state start below	
	menu_main = new Menu(); // main menu of the game	
	menu_ingame = new Menu(); // ingame menu 
	menu_options = new Menu(); // options menu
	
	// message box 
	message_box = new MessageBox();
	
	// active boss = none
	boss = NULL;
	boss_defeated = false;

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
	delete(menu_main);
	delete(menu_ingame);
	delete(menu_options);
	delete(message_box);
	
	if (boss) // remove active boss from RAM
	{
		delete(boss);
	}
}


void MainGameLoop::update()
{
	ticks++; // game ticks 
	
	// handle the states...
	switch (currentState)
	{
	case STATE_START: // game starts here , this is state 0 , when you run the exe, this is the entry point, will run just once per execution!
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
		
		// menu init, must be done here so the colors are ok with the palette
		// main menu
		menu_main->menuItems.push_back("Story mode");
		menu_main->menuItems.push_back("Endurance");
		menu_main->menuItems.push_back("Options");
		menu_main->menuItems.push_back("About");
		menu_main->menuItems.push_back("Quit game");

		menu_main->init();
		
		// ingame menu 		
		menu_ingame->menuItems.push_back("Continue...");
		menu_ingame->menuItems.push_back("Restart game");
		menu_ingame->menuItems.push_back("Options");
		menu_ingame->menuItems.push_back("Quit game");

		menu_ingame->init();
		
		// options menu			
		menu_options->menuItems.push_back("Sound ON");
		menu_options->menuItems.push_back("Music ON");
		menu_options->menuItems.push_back("Keyboard");
		menu_options->menuItems.push_back("Mouse");
		menu_options->menuItems.push_back("Joystick");
		menu_options->menuItems.push_back("Continue...");

		menu_options->init();
		// DEBUG SETUP COLORS AND FONT OF EACH MENU! IT WILL BE DEFAULT FOR NOW
		
		message_box->init();
		
		// -- end menu init 
		
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
			changeState(STATE_MAIN_MENU);
	break;
	
	
	case STATE_MAIN_MENU:
	{
		int rm = menu_main->update();
		
		if (rm == 4) // quit game
			changeState(STATE_QUIT);
			
		if (rm == 0) // start game, story mode
		{
			message_box->showMessage("Story mode\n\nThis game mode is not ready yet!\nWe will play endurance instead\nSurvive as many waves as you can!\nGo for the highest score!", "[ OK! ]");
			changeState(STATE_NEXTLEVEL);
		}
			
		if (rm == 1) // start game, endurance mode
		{
			message_box->showMessage("Endurance mode\n\nSurvive as many waves as you can.\nGo for the highest score!", "[ OK! ]");
			changeState(STATE_NEXTLEVEL);
		}
		
		if (rm == 2)
			changeState(STATE_OPTIONS_MENU);
		
		
		if (rm == 3)
		{
		message_box->showMessage("By Kronoman from Argentina\nCopyright (c) 2022\nDone in exile in Costa Rica\nRetro fun, no DRM bullshit!\nC++ & Allegro\nThanks to testers:\nZir, Joel, Slava, Sebas,\nJuane, Nacho, Pablo-San, Guille\nThanks for playing!", "[ Pura Vida ]");
			
			if (rand()%100 > 95) // easter egg, fuck kirchner & peronism
				message_box->showMessage("Cristina Kirchner Thief of Argentina\nKirchner = corrupts", "[ C.F.K.L.D.L.N.A ]"); // cristina fernandez de kirchner ladrona de la nacion argentina
		}
		
	}
	break;
	
	case STATE_INGAME_MENU: // only comes here when playing the game
	{
		// in game menu
		int rm = menu_ingame->update();
		if (rm == 0 || rm == -2) // -2 is cancel menu
		{
			ticks = old_ticks; // resume ticks
			changeState(STATE_OK); // resune game!
		}
		
		// restart game -- DEBUG, CONFIRM !!
		if (rm == 1)
		{
			wait = 180;
			changeState(STATE_GAMEOVER);
		}
		
		if (rm == 2)
			changeState(STATE_OPTIONS_MENU);
		
		if (rm == 3) // quit game
			changeState(STATE_QUIT);
	}
	break;
	
	case STATE_OPTIONS_MENU:
	{
		// 0 sound
		// 1 music 
		// 2 keyboard
		// 3 mouse 
		// 4 joystick
		// 5 continue 
		
		int rm = menu_options->update();
		if (rm >= 0 && rm <= 4)
			message_box->showMessage("Not done yet!\nWorking on it!\nThanks!", "[ Continue ]");
		
		if (rm == 5 || rm == -2) // -2 is cancel menu
		{
			ticks = old_ticks;
			changeState(previousState);
		}
		
	}
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
		
		// is a wave with stage boss ? 
		if (wave % 3 == 0)
			boss_defeated = false;
		else 
			boss_defeated = true; // no boss

		// safeguard delete previous boss
		if (boss)
		{
			delete(boss);
			boss = NULL;
		}

		// map generation
		// is a chain to select aproppiate weather, land and background
		
		// in this chain, everything gets generated 
		// first background is loaded 
		// then land generated according to background
		// then weather according to map
		weather.generate(map->generate(parallax->load(wave)));
		
		// end of map generation
		
		cowL.generate(rand() % (CowList::maxCOWS - 5) + 5); 
	
		camerashake = 0;
		
		ufo->next_level_reset();
		ufo_last_life = ufo->life;
		
		pm->finish(); // reset particles
		
		enemyList->reset(); // bye residual enemies
		
		// the shoots of UFO and enemies are handled by themselves, so they get reset without need to call here

		// destroy last enemy message
		x_destroy_m = y_destroy_m = 0;
		t_destroy_m = -1;

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
	
	// the level finished, we do a little pause before going for next level
	case STATE_WAIT_FOR_NEXT_LEVEL:
	{
		// update logic, but stop scrolling
		ufo->update(doubleBuffer->w, map, pm, enemyList, boss); // update player UFO
		cowL.update(map, ufo, pm); // update cows
		pm->update(); // update particles
		weather.update(); // update weather
		enemyList->update(map, ufo, pm); // update enemies -- should be none for this time, I do to finish the enemy shoots flying...
		
		if (ufo->life <0)
			ufo->life=1; // prevent dead in case UFO damaged!
		
		camerashake = 0;
		
		if (--wait < 0)
		{
				changeState(STATE_NEXTLEVEL);
		}
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
		
		
		if (key[KEY_ESC]) // ingame menu 
		{
			old_ticks = ticks; // save current ticks to resume game
			changeState(STATE_INGAME_MENU);
			while (key[KEY_ESC]); // wait key release
		}
		
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

		ufo->update(doubleBuffer->w, map, pm, enemyList, boss); // update player UFO
		
		// -- camera shake on damage --
		// did ufo receive damage? shake camera  (i check last frame life with new life)
		if (ufo_last_life > ufo->life && ufo_last_life < UFO::MAX_LIFE) // max life prevents shake when overloaded
			camerashake = rand()%5 + (ufo_last_life - ufo->life);
		
		ufo_last_life = ufo->life;
		//--- end camera shake on damage 
		
		cowL.update(map, ufo, pm); // update cows
		
		pm->update(); // update particles
		
		weather.update(); // update weather
		
		bool ret_b = true; // Im dead boss

		if (boss)
		{
			ret_b = boss->update(map, ufo, pm, enemyList->shootsE); // update boss
			
			if (ret_b) // boss killed!
			{
				delete(boss);
				boss = NULL;
				boss_defeated = true;
			}
		}
		
		int ret_e = enemyList->update(map, ufo, pm); // update enemies
		
		// add enemies each screen, dont add if we traveled enough
		bool finished = ticks > (unsigned  long int)(2000 + wave*200); // each wave the level is a little longer  
		
		// generate new enemies if we re not against the boss level
		if ((ticks % 300 == 0 || ret_e == 0) && !finished && !boss)
			enemyList->generate(rand() % 5 + wave, wave, map); //  GENERATE TIER / WAVE APPROPIATE ENEMIES // TODO - ADD BOSSES
		
		// if we finished and we dont defeated the boss, spawn boss
		// only if appropiate wave 
		if (finished && wave % 3 == 0 && !boss && !boss_defeated)
		{
			// DEBUG -- SPAWN BOSS HERE according to wave!
			
			// #1 helicopter boss
			boss = new BossHeli();
			((BossHeli *)boss)->enemyList = enemyList; // to be able to deploy paratroopers
		}			
		
		// taking too long to kill enemies, show message if boss is not active
		if (ticks > (unsigned  long int)(2400 + wave*200) && !boss)
		{
			if (enemyList->enemies_n > 0)
			{
				t_destroy_m = 30;
				x_destroy_m = enemyList->enemy[enemyList->enemies_n-1]->x;
				y_destroy_m = enemyList->enemy[enemyList->enemies_n-1]->y;
			}
			else
			{
				ret_e = 0; // finish anyways, there arent enemies
			}
		}
		
		// end level when enough travel and no enemies and boss defeated
		if (finished && ret_e == 0 && (boss_defeated || !boss) ) 
		{
				wait = 90;
				changeState(STATE_WAIT_FOR_NEXT_LEVEL);
		}
			
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
		
		if (t_destroy_m > 0)
			t_destroy_m--; // remove slowly "kill" message
	}
	break;

	default:
		// probably a error if we go here... a unhandled state... mmm
		//Error::error("ERROR\nUnhandled state in mloop.cpp (state = %d); %s in line %d\n", currentState, __FILE__, __LINE__ );
	break;
	}


	// -- for all states --
	
	// DEBUG - THIS IS BUGGY
	if (key[KEY_SPACE]) // avoid waiting screens!
	{
		if (wait > 30) // 30 ticks = 1 second real life
		{
			wait = wait / 2;
			help_show = help_show / 2; // speed up help screen too, if available
			//while(key[KEY_SPACE]);
			//clear_keybuf();
		}
	}

	
	if (key[KEY_F10] ) // key to hard quit...DEBUG - confirm!
		changeState(STATE_QUIT);
		
	// --- debug commands ---
	if (key[KEY_F12]) // debug key
	{
		debug = !debug; // toggle debug
		while(key[KEY_F12]); // wait key release
	}
	
	if (key[KEY_F11]) // debug key for map boss
	{
		map->boss_mode = !map->boss_mode; // toggle debug
		while(key[KEY_F11]); // wait key release
	}
	// --- debug end ---
	
	

	if (camerashake > 0)
		camerashake --;
}

// ----------------------------------------------
// rendering pipeline
// ----------------------------------------------
void MainGameLoop::render()
{
	switch (currentState)
	{
	case STATE_START:
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
	
	case STATE_MAIN_MENU:
	{
		// DEBUG ADD STAR ANIMATION OR SOMETHING!
		//clear_to_color(doubleBuffer, makecol(0, 0, 0));
		blit(intro_bmp, doubleBuffer, 0, 0, 0, 0, intro_bmp->w, intro_bmp->h);	
		menu_main->render(doubleBuffer);
	}
	break;
	
	case STATE_INGAME_MENU:
	{
		// DEBUG ADD ANIMATION OR SOMETHING!
		//clear_to_color(doubleBuffer, makecol(0, 0, 0));
		blit(intro_bmp, doubleBuffer, 0, 0, 0, 0, intro_bmp->w, intro_bmp->h);	
		menu_ingame->render(doubleBuffer);
	}
	break;

	case STATE_OPTIONS_MENU:
	{
		menu_options->render(doubleBuffer);
	}
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

	// the wait for next level change
	case STATE_WAIT_FOR_NEXT_LEVEL:
	{
		parallax->render(doubleBuffer); // render background
		map->render(doubleBuffer); // render map
		ufo->render(doubleBuffer); // render player UFO
		cowL.render(doubleBuffer); // render cows
		enemyList->render(doubleBuffer); // render enemies
		weather.render(doubleBuffer); // render weather
		pm->render(doubleBuffer,0,0); // render particles
		//ufo->render_HUD(doubleBuffer); // render HUD
		
		 
		textprintf_ex(doubleBuffer, font,  doubleBuffer->w/2 -  text_length(font, "CONGRATULATIONS!")/2, doubleBuffer->h/2 - text_height(font)/2, makecol(255,255,255),makecol(170,0,0) , "CONGRATULATIONS!");
	}
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
		
		if (boss)
			boss->render(doubleBuffer); // render boss 
		
		weather.render(doubleBuffer); // render weather
		
		pm->render(doubleBuffer,0,0); // render particles
		
		ufo->render_HUD(doubleBuffer); // render HUD
		
		// destroy last enemy message
		if (t_destroy_m > 0)
		{
			 textout_centre_ex(doubleBuffer, font, "KILL!", x_destroy_m - 5, y_destroy_m - 5, makecol(170,0,0), -1);
		}
		
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

	// ---------------------------
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

