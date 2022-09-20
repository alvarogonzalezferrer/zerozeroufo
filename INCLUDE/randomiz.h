// -----------------------------------------------------------------------
// randomiz.h
// -----------------------------------------------------------------------
// Random generator for range of numbers in integer or float precission
//
// REMEMBER TO INIT RANDOM NUMBERS WITH  srand(time(0)); BEFORE USING!!
//
// -----------------------------------------------------------------------
// Copyright (c) 2022, Kronoman
// In loving memory of my father.
// Made in Argentina
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------
#ifndef RANDOMIZ_H
#define RANDOMIZ_H

class Randomize
{
	public:
		// NOTE: ONLY TWO DIGIT PRECISION! 0.01 MIN OR -0.01 ETC
		static float random(float min, float max); // will return a random number between min and max with 2 digits of precision i.e 1.22
	
		static int random(int min, int max); // will return a INTEGER random number between min and max 
};

#endif