#include "boss.h"
#include "logger.h"


// -----------------------------------------------------------------------
// base boss class 
// -----------------------------------------------------------------------
// PURE VIRTUAL CLASS, CANT BE USED ON THE GAME
// IS JUST A BASE FOR THE BOSSES
// -----------------------------------------------------------------------
Boss::Boss()
{
	Logger::log("Boss::Boss()");
	
	sx=sy=x=y=life=0; // default
	
	// collide with UFO default 
	collideWithUFO = false;
	
	// un init default
	bbox = NULL;
	shoot_bbox = NULL;
	sprite = NULL;
	
	frame = 0;
	ai_c = 0;
}


// you don't need to call the base destructor, a base destructor is always called for you by the derived destructor
Boss::~Boss()
{
	Logger::log("Boss::~Boss()");
}