// ---------------------------------------------------------------------------
// main.cpp
// -----------------------------------------------------------------------
// Main
//
// Entry point
// -----------------------------------------------------------------------
// August 2022, Costa Rica
// 
// Zero Zero UFO - alien retro video game
// -----------------------------------------------------------------------
// Copyright (c) 2007-2022, Kronoman
// In loving memory of my father.
// Made in Argentina / Costa Rica
// -----------------------------------------------------------------------
// This game is made based on my old code, I have been coding videogames
// since I was a child, from the 1980s.
//
// Check my website: https://alvarogonzalezferrer.github.io/
//
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------
#include "hardware.h"

#include "mloop.h"

#ifdef __DJGPP__
	// to show a little MS DOS message!
	#include <iostream>
#endif 

// -----------------------------------------------------------------------
// game entry point from operating system...
// -----------------------------------------------------------------------
int main(int argc, char *argv[])
{
	// init hardware
	HardwareManager::init(argc, argv);

	// ---------------------------
	// here starts the game...
	// ---------------------------
	MainGameLoop mgl; // main game loop
	mgl.start();

	// game finished, shut down, go back to text, etc
	HardwareManager::shutdown();

#ifdef __DJGPP__
	// little message for MS DOS
	std::cout << "Thanks for playing! - Krono 2022" << std::endl;
#endif 


	return 0;
}
END_OF_MAIN();

