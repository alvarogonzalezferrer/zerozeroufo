#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <algorithm>    // std::max

#include "randomiz.h"

float Randomize::random(float min, float max)
{
	float a=min;
	float b=max;
	min = std::min(a, b);
	max = std::max(a, b);
	return ((float)(rand() % std::abs((int)(max*100.0-min*100.0)) ) + min*100.0)/100.0;
}

int Randomize::random(int min, int max)
{
	/*** slow , old code 
	int a = std::max(min,max); // sort
	int b = std::min(min,max);
	int range = a - b;
	
	int s = rand()%range + min;
	return s;
	***/
	
	int a = min;
	int b = max;
	min = std::min(a, b);
	max = std::max(a, b);
	
	return rand() % std::abs(max-min) + min;
}