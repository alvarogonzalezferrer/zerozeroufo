// -----------------------------------------------------------------------
// map.h
// -----------------------------------------------------------------------
// Map of the game
// -----------------------------------------------------------------------
// For this game I have a infinite rolling map, so is a trick to use little RAM
// -----------------------------------------------------------------------
// Copyright (c) 2007-2022, Kronoman
// In loving memory of my father.
// Made in Argentina
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------
#ifndef MAP_H
#define MAP_H

#include <allegro.h>

// map itself
class Map
{
	public:
		Map();
		~Map();
	
		int generate(int typemap); // generate a map, returns appropiate weather type
	
		void render(BITMAP *bmp); // render on bitmap
		
		void update(); // moves the map left and creates a new piece of map, to keep the thing scrolling
	
		int getHeight(int x); // get height at position x
	
		void cache(); // cache bitmap with map heights

		void drawColumn(int x); // helper function draws a single column of map on bitmaps

		float createColumn(float y); // helper creates map column following previous slope

		// map size 
		static const int mapW = 320;
		static const int mapH = 200;
	
		float map[mapW]; // height of the map

		int tm; // type of map, grass, snow, rocks

		int tw; // type of weather, 0 = snow, 1 = rain, 2 = none

		// map segments
		int segment_length; // length
		float segment_slope; // slope


		static const int SNOW = 0; // snow
		static const int GRASS = 1; // grass
		static const int ROCK = 2; // rocks
		static const int SAND = 3; // sand
		static const int DARKSAND = 4; // night sand
	
		BITMAP *mapBMP; // current cached representation of the map
		
		BITMAP *mapBMP2; // double buffer to be able to move the map
};

#endif
