#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <algorithm>    // std::max

#include "randomiz.h"

#include "logger.h"

float Randomize::random(float min, float max)
{
	// DEBUG - optimize this!
	float a=min;
	float b=max;
	
	min = std::min(a, b);
	max = std::max(a, b);
	
	int range = (int)std::abs((max - min) * 100.0);
	if (range == 0) 
	{
		Logger::log("Division by zero avoided at Randomize::random(float, %f, %f)", min, max);
		return min;
	}
		
	return ((float)(rand() % range) + min*100.0)/100.0;
	// old single line : return ((float)(rand() % std::abs((int)(max*100.0-min*100.0)) ) + min*100.0)/100.0;
}

int Randomize::random(int min, int max)
{
	int a = std::max(min,max); // sort
	int b = std::min(min,max);
	int range = std::abs(a - b);
	
	if (range == 0) 
	{
		Logger::log("Division by zero avoided at Randomize::random(int, %d, %d)", min, max);
		return min;
	}
		
	int s = rand()%range + min;
	return s;
}