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

	HardwareManager::shutdown();

	return 0;
}
END_OF_MAIN();

