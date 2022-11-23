// -----------------------------------------------------------------------
// map.cpp
// -----------------------------------------------------------------------
// Map of the game
// -----------------------------------------------------------------------
// Copyright (c) 2007-2022, Kronoman
// In loving memory of my father.
// Made in Argentina
// -----------------------------------------------------------------------
// DEBUG TODO IMPROVE
// ALL COLORS SHOULD BE CACHED AND FIXED TO EGA PALETTE!!
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------

#include "error.h"
#include "map.h"
#include "logger.h"
#include "randomiz.h"

Map::Map()
{
	Logger::log("Map::Map()");
	mapBMP = NULL;
	mapBMP2 = NULL;
	tw = 0;
	tm = 0;
	segment_length = 0;
	segment_slope = 0;
	boss_mode = false;
}

Map::~Map()
{
	Logger::log("Map::~Map()");
	
	if (mapBMP != NULL)
		destroy_bitmap(mapBMP);
	
	if (mapBMP2 != NULL)
		destroy_bitmap(mapBMP2);
	
}

// ----------------------------
// create a single map colum and returns height
// taking into account last slope aand lenght of segment
// ----------------------------
float Map::createColumn(float y)
{
	// parameters, normal map
	float max_slope = 0.6f;
	float min_height = mapH * 0.7;
	float max_height = mapH * 0.98;
	float min_s_length = 8;
	float max_s_length = (int)(mapW/10);
	
	// for boss fight we have a flatter map
	if (boss_mode)
	{
		max_slope = 0.2f;
		min_height = mapH * 0.9;
		min_s_length = 16;
		max_s_length = mapW/5;
	}
	
	if (--segment_length < 1) // next segment
	{
		segment_length = Randomize::random(min_s_length, max_s_length);  // new segment lenght
		segment_slope =  Randomize::random(-max_slope, max_slope);  // slope -- DEBUG -- TODO, ADD A "BOSS" MODE TO MAKE FLAT LAND!
	}

	y += segment_slope;
	
	if (y < min_height) // too high, bounce down
	{
		//y = min_height; // commented to avoid abrupt cut on boss mode
		//segment_length = 0; 
		// force bounce down
		segment_length = Randomize::random(min_s_length, max_s_length);
		segment_slope = Randomize::random(0.1f, max_slope); // slope down
	}
	
	if (y > max_height) // too low , bounce up
	{
		y = max_height;
		
		segment_length = Randomize::random(min_s_length, max_s_length);
		segment_slope = Randomize::random(-max_slope, -0.1f); // slope up
	}
	
	return y;
}

void Map::drawColumn(int x)
{
	for (int i = mapH; i > map[x]; i--)
	{
		int c;
		if ( i > map[x]*1.02 + rand()%15)
		{
			// bottom is darker
			switch (tm)
			{
				case SNOW:
					putpixel(mapBMP, x, i, makecol(0,0,rand()%132+64));
				break;

				case ROCK:
					c = rand()%32+16;
					putpixel(mapBMP, x, i, makecol(c,c,c));
				break;
				
				case SAND:
					putpixel(mapBMP, x, i, makecol(rand()%85+170,85,0));
				break;
				
				case DARKSAND:
					putpixel(mapBMP, x, i, makecol(rand()%85+85,0,0));
				break;
				
				case GRASS:
				default:
					putpixel(mapBMP, x, i, makecol(128,rand()%48+32,0));
				break;
			}
		}
		else
		{
			// top is different color
			switch (tm)
			{
				case SNOW:
					c = rand()%55+180;
					putpixel(mapBMP, x, i, makecol(c,c,c));
				break;

				case ROCK:
					c = rand()%64+32;
					putpixel(mapBMP, x, i, makecol(c,c,c));
				break;

				case SAND:
					putpixel(mapBMP, x, i, makecol(255,rand()%190+65,0));
				break;
				
				case DARKSAND:
					putpixel(mapBMP, x, i, makecol(rand()%85+170,128,0));
				break;

				case GRASS:
				default:
					putpixel(mapBMP, x, i, makecol(0,rand()%104+96,0));
				break;
			}
		}
	}
	
}

void Map::cache()
{
	Logger::log("Map::cache()");
	
	if (mapBMP == NULL)
	{
		mapBMP = create_bitmap(mapW, mapH);
		if (mapBMP == NULL)
			Error::error("Map::cache()\nERROR: can't get a bitmap %d x %d px", mapW, mapH);
	}
	
	if (mapBMP2 == NULL)
	{
		mapBMP2 = create_bitmap(mapW, mapH);
		if (mapBMP2 == NULL)
			Error::error("Map::cache()\nERROR: can't get a bitmap cache %d x %d px", mapW, mapH);
	}

	// select weather
	tw = 2; // no weather default
	if (tm == SNOW)
	{
		if (rand()%100 < 90)
			tw = 0; // snow	
	}
	else
	{
		if (rand()%100 < 45)
			tw = 1; // rain
	}

	// clear map with transparent color
	clear_to_color(mapBMP, bitmap_mask_color(mapBMP));
	clear_to_color(mapBMP2, bitmap_mask_color(mapBMP2));
	
	// floor
	for (int x = 0; x < mapW; x++)
	{
		// draw terrain
		drawColumn(x);
	}

}

int Map::getHeight(int x)
{
	if (x < 0)
		x = 0;

	if (x > mapW-1)
		x = mapW-1;

	return (int)map[x];
}

// ----------------------------
// generate a new map 
// ----------------------------
int Map::generate(int typemap)
{
	Logger::log("Map::generate()");

	float y = mapH * 0.9;
	tm = typemap; // map type

	// reset length & slope
	segment_length = 0;
	segment_slope = 0;

	// do terrain
	for (int x = 0; x < mapW; x++)
	{
		y = createColumn(y);
		map[x] = y;
	}

	// cache my map - I cache the graphic representation of the map
	cache();
	
	// select weather according to map type 
	int ret = 2; // no weather effects, 0 = snow, 1= rain, all the others = none, see weather.h
	switch(tm)
	{
		case SNOW:
			// can snow 
			if (rand()%10 < 5)
				ret = 0; // snow
		break;
		
		case GRASS:
		case ROCK:
			// in rock or grass, can rain
			if (rand()%10 < 4)
				ret = 1; // rain
		break;
		
		case DARKSAND:
		case SAND:
			// will never rain or snow in desert
			ret = 2;
		break;
	}
	
	return ret; // return desired weather effect
}

// ----------------------------
// updates the scroll of the map, moving left 1 px
// ----------------------------
void Map::update()
{
	// move map to left and recreate 1 column
	
	
	// move map height to left
	for (int x = 0; x < mapW - 1; x++)
	{
		map[x] = map[x+1];
	}
	
	// make new column, respects last slope etc
	map[mapW-1] = createColumn(map[mapW-2]);
	
	// move map cache to left
	
	// DEBUG THIS WASTES LOT OF HIGH SPACE DOING THE BLIT, SHOULD COPY ONLY FROM MAX MAP Y COORDINATE TO BOTTOM!! ALL THE REST IS EMPTY -- TODO
	blit(mapBMP, mapBMP2, 1, 0, 0, 0, mapBMP->w, mapBMP->h);
	
	// switch map caches
	BITMAP *bmap3 = mapBMP;
	mapBMP = mapBMP2;
	mapBMP2 = bmap3;
	
	// delete v line moved
	vline(mapBMP, mapBMP->w-1, 0, mapBMP->h, bitmap_mask_color(mapBMP));
	
	// render new v line
	drawColumn(mapW-1);
}


void Map::render(BITMAP *bmp)
{
	masked_blit(mapBMP, bmp, 0, 0, 0, 0, mapBMP->w, mapBMP->h);
}




