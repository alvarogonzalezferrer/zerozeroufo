// ---------------------------------------------------------------------------
// parallax.h
// -----------------------------------------------------------------------
// Parallax scroll background
// -----------------------------------------------------------------------
// Copyright (c) 2022, Kronoman
// In loving memory of my father.
// Made in Argentina
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------
#ifndef PARALLAX_H
#define PARALLAX_H

#include <allegro.h>
#include "datafile.h"

class Parallax
{
	public:
		Parallax();
		~Parallax();
		
		int load(int wave); // load background according to wave , returns appropiate terrain type for Map 
		
		void update();
		void render(BITMAP *bmp);
		
		// internal use
		void reset(); // restart scroll 
		void unload(); // releases ram 	


		//data
		Datafile backgrounds_data; // images for background
		
		// scrolling backgrounds
		// static background, middle ground, foreground fast
        BITMAP *background_bg; // static background
		
		BITMAP *background_mg; // middle background
		
		BITMAP *background_fg; // fast foreground
		
		float mg_scroll; // for parallax scrolling
		float fg_scroll; // for parallax scrolling
};

#endif