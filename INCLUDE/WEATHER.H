// -----------------------------------------------------------------------
// weather.h
// -----------------------------------------------------------------------
// Weather effects - hacked in 10 minutes
// -----------------------------------------------------------------------
// Copyright (c) 2007-2022, Kronoman
// In loving memory of my father.
// Made in Argentina
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------
#ifndef WEATHER_H
#define WEATHER_H

#include <allegro.h>

class WPart
{
	public:
		WPart();
		
		void render(BITMAP *bmp);

		float x,y,dx,dy; // position direction
		int c,r; // color ,radius
		int t; // type, 0=snow, 1=rain
};

class Weather
{
	public:
		Weather();
		~Weather();

		void render(BITMAP *bmp);
		void update();

		void generate(int t); // generate weather of type t, 0=snow, 1=rain

		static const unsigned int maxP = 60; // max particles

		int n; // how much particles ? 0..maxP - 1

		int w; // wind

		WPart part[maxP];
};

#endif

