#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <algorithm>    // std::max

#include "randomiz.h"

float Randomize::random(float min, float max)
{
	// NO SAFETY CHECK, MIN SHOULD BE MIN AND MAX SHOULD BE MAX!
	
	return ((float)(rand() % (int)(max*100.0-min*100.0) ) + min*100.0)/100.0;
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
	
	// NO SAFETY CHECK, MIN SHOULD BE MIN AND MAX SHOULD BE MAX!
	
	return rand() % (max-min) + min;
}