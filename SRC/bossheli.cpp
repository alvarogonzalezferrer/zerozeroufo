// helicopter boss 

#include "bossheli.h"

#include "shoots.h"
#include "logger.h"
#include "randomiz.h"
#include "map.h"

// this two so I can drop paratroopers
#include "enemies.h"
#include "paratroo.h"

// to release prizes
#include "weapon_p.h"
#include "health_p.h"

BossHeli::BossHeli()
{
	Logger::log("BossHeli::BossHeli()");
	
	// position outside screen 
	sx = -2;
	sy = 0;
	x = 360;
	y = 100;
	
	// load my data
	data.load("bossheli.dat"); 
	spr[0] = (BITMAP *)data.getObject("BOSSHELI0")->dat;
	spr[1] = (BITMAP *)data.getObject("BOSSHELI1")->dat;
	spr[2] = (BITMAP *)data.getObject("BOSSHELI2")->dat;
	spr[3] = (BITMAP *)data.getObject("BOSSHELI3")->dat;
	
	shoot_wav = (SAMPLE *)data.getObject("SHOOT_WAV")->dat;
	explosion_wav = (SAMPLE *)data.getObject("EXPLOSION_WAV")->dat;
	
}

BossHeli::~BossHeli()
{
	// release data 
	data.unload(); // unload datafile
}