#include "enemies.h"
#include "logger.h"
#include "randomiz.h"

// enemies types
#include "soldat.h"
#include "tank.h"
#include "heli.h"
#include "jetpack.h"
#include "zepellin.h"
#include "B17.h" 
#include "mtruck.h"
#include "biplane.h"
#include "mechmini.h"
#include "apc.h"
#include "mine.h"
#include "icbm.h"
#include "f117.h"
#include "b52.h"

// prizes - power ups - theyre a special case of enemy
#include "weapon_p.h"
#include "health_p.h"

// how many enemy classes we have? -- debug shitty way to do it
#define ENEMY_CLASSES_COUNT 16

// -----------------------------------------------------------------------
// base enemy class 
// -----------------------------------------------------------------------
// PURE VIRTUAL CLASS, CANT BE USED ON THE GAME
// IS JUST A BASE FOR THE ENEMIES
// -----------------------------------------------------------------------
Enemy::Enemy(Datafile *data)
{
	Logger::log("Enemy::Enemy(data)");
	
	sx=sy=x=y=life=0; // default
	
	// collide with UFO default 
	collideWithUFO = true;
	
	// un init default
	bbox = NULL;
	sprite = NULL;
}


// you don't need to call the base destructor, a base destructor is always called for you by the derived destructor
Enemy::~Enemy()
{
	Logger::log("Enemy::~Enemy()");
	
}

// -----------------------------------------------------------------------
// ENEMY LIST MANAGER 
// -----------------------------------------------------------------------
EnemyList::EnemyList()
{
	Logger::log("EnemyList::EnemyList()");
	
	// setup enemies
	
	for (int i=0; i<maxENEMIES; i++)
		enemy[i] = NULL;
	
	shootsE = new ShootsList(); // shoot manager 
	
	enemies_n = 0;
	
	// this data is used by ALL enemy derived classes
	// so I dont keep loading and unloading data from slow disks
	data.load("enemies.dat");	

}



EnemyList::~EnemyList()
{
	Logger::log("EnemyList::~EnemyList()");
	// release resources
	reset();
	
	delete(shootsE);
	
	data.unload(); // unload datafile
}

void EnemyList::reset()
{
	Logger::log("EnemyList::reset()");
	
	for (int i=0; i<enemies_n; i++)
	{
		delete(enemy[i]);
		enemy[i] = NULL;
	}
	
	enemies_n = 0;
	
	shootsE->reset();
}

// add enemy e to active enemy list, 
// will DELETE it IF list is full, no need to manage this,
// returns TRUE if added, FALSE if list is full 
bool EnemyList::addEnemy(Enemy *e)
{
	if (e == NULL)
		return false; // prevent NULL pointers 
	
	if (enemies_n+1 > maxENEMIES)
	{
		delete(e);
		return false; // im full
	}
	
	enemies_n++;
	
	// ADD TO LIST
	enemy[enemies_n-1] = e;
	
	return true;
}

// generate n enemies, of max tier t, on map m
// will be added to the map from the right
// generates common enemies, NOT BOSSES
//
// to select the enemies, I assign random chance to each like this:
// Step 1: accumulate all the possible chances 
// Step 2: pick a random value in proportion to total chance 
// Step 3: loop through the resources to see in which part it random value falls under.
// Step 4: Generate the enemy and add to list
void EnemyList::generate(int n, int t, Map *m)
{
	Logger::log("EnemyList::generate() : start");
	
	// DEBUG -- ADD NEW CLASSES HERE!
	int chance[ENEMY_CLASSES_COUNT]; // chance of each enemy class
	
	int total_chance = 0;
	
	// CHANCES CHANGE ACCORDING TO WAVE
	// DEBUG - THIS CHANCES SHOULD BE LOADED FROM A TXT FILE!
	// DEBUG - THIS SHOULD BE LOADED ONCE, NOT EVERY TIME WE GENERATE ENEMIES!! - MOVE AT LEAST TO ANOTHER FUNCTION!
	
	// tier 0-1 - easy levels
	chance[0] = 80; // soldat
	chance[1] = 20; // tank
	chance[2] = 0; // heli
	chance[3] = 0; // jetpack
	chance[4] = 0; // zepellin
	chance[5] = 0; // B17
	chance[6] = 0; // missile truck
	
	chance[7] = 25; // weapon power up
	chance[8] = 1; // health power up
	
	// more enemies
	chance[9] = 20; // biplane, easy enemy airborne
	chance[10] = 0; // mini flying mech
	chance[11] = 0; // APC
	chance[12] = 0; // Mine
	chance[13] = 0; // ICBM
	chance[14] = 0; // F177	
	chance[15] = 2; // B52	
	
	// increase according to tier
	if (t > 1)
	{
		chance[0] = 70; // soldat
		chance[1] = 30; // tank
		chance[2] = 0; // heli
		chance[3] = 40; // jetpack
		chance[4] = 0; // zepellin
		chance[5] = 0; // B17
		chance[6] = 0; // missile truck
		
		chance[7] = 20; // weapon power up
		chance[8] = 10; // health power up	

		chance[9] = 30; // biplane
		chance[10] = 0; // mini flying mech
		chance[11] = 0; // APC
		chance[12] = 5; // Mine
		chance[13] = 1; // ICBM
		chance[14] = 0; // F177
		chance[15] = 10; // B52
		
	}

	// increase according to tier
	if (t > 2)
	{
		chance[0] = 70; // soldat
		chance[1] = 30; // tank
		chance[2] = 30; // heli
		chance[3] = 30; // jetpack
		chance[4] = 5; // zepellin
		chance[5] = 0; // B17
		chance[6] = 0; // missile truck
		
		chance[7] = 20; // weapon power up
		chance[8] = 10; // health power up	

		chance[9] = 10; // biplane
		chance[10] = 0; // mini flying mech
		chance[11] = 0; // APC
		chance[12] = 7; // Mine
		chance[13] = 10; // ICBM
		chance[14] = 2; // F177
		chance[15] = 10; // B52
	}
	
	// increase according to tier
	if (t > 3)
	{
		chance[0] = 60; // soldat
		chance[1] = 20; // tank
		chance[2] = 20; // heli
		chance[3] = 20; // jetpack
		chance[4] = 30; // zepellin
		chance[5] = 0; // B17
		chance[6] = 0; // missile truck
		
		chance[7] = 20; // weapon power up
		chance[8] = 10; // health power up	

		chance[9] = 5; // biplane
		chance[10] = 0; // mini flying mech
		chance[11] = 5; // APC
		chance[12] = 10; // Mine
		chance[13] = 10; // ICBM
		chance[14] = 5; // F177
		chance[15] = 10; // B52
	}
	
	// increase according to tier
	if (t > 4)
	{
		chance[0] = 50; // soldat
		chance[1] = 20; // tank
		chance[2] = 20; // heli
		chance[3] = 20; // jetpack
		chance[4] = 20; // zepellin
		chance[5] = 20; // B17
		chance[6] = 0; // missile truck
		
		chance[7] = 20; // weapon power up
		chance[8] = 10; // health power up	

		chance[9] = 1; // biplane
		chance[10] = 5; // mini flying mech
		chance[11] = 15; // APC
		chance[12] = 15; // Mine
		chance[13] = 18; // ICBM
		chance[14] = 10; // F177
		chance[15] = 10; // B52
	}
	
	// increase according to tier
	if (t > 5)
	{
		chance[0] = 35; // soldat
		chance[1] = 10; // tank
		chance[2] = 20; // heli
		chance[3] = 20; // jetpack
		chance[4] = 20; // zepellin
		chance[5] = 20; // B17
		chance[6] = 30; // missile truck
		
		chance[7] = 25; // weapon power up
		chance[8] = 20; // health power up	

		chance[9] = 2; // biplane
		chance[10] = 10; // mini flying mech
		chance[11] = 20; // APC
		chance[12] = 25; // Mine
		chance[13] = 25; // ICBM
		chance[14] = 30; // F177
		chance[15] = 8; // B52
	}
	
	// increase according to tier
	if (t > 6)
	{
		chance[0] = 10; // soldat
		chance[1] = 5; // tank
		chance[2] = 20; // heli
		chance[3] = 20; // jetpack
		chance[4] = 20; // zepellin
		chance[5] = 20; // B17
		chance[6] = 20; // missile truck
		
		chance[7] = 27; // weapon power up
		chance[8] = 35; // health power up	

		chance[9] = 5; // biplane
		chance[10] = 20; // mini flying mech
		chance[11] = 25; // APC
		chance[12] = 20; // Mine
		chance[13] = 20; // ICBM
		chance[14] = 35; // F177
		chance[15] = 5; // B52
	}
		
	// total chance ? 
	for (int i = 0; i < ENEMY_CLASSES_COUNT; i++)
		total_chance += chance[i];
	// DEBUG - UP TO HERE SHOULD BE MOVED TO ANOTHER FUNCTION CALLED ONCE PER WAVE!
	
	// select according to chance probability
	for (int z = 0; z < n; z++)
	{			
		int rt = rand() % total_chance; // do the selection
		
		int type_e = 0; // type selected
		
		int previous_chance = 0; // previous chance range
		for (int i = 0; i < ENEMY_CLASSES_COUNT; i++)
		{
			if (rt >= previous_chance && rt < previous_chance + chance[i])
			{
				type_e = i;
				
				// bug correction when chance 0
				if (chance[i] == 0) // should never appear in this level 
					type_e = 0 ; // soldat instead
				
				break; // found it
			}
			previous_chance += chance[i]; // increase test range for next iteration
		}
		
		Enemy *s = NULL;
		
		Logger::log("EnemyList::type_e(%d)", type_e);
		
		switch(type_e)
		{
			case 0: // default enemy
				s = new Soldat(&data);
			break;

			case 1: 
				s = new Tank(&data);
			break;			
			
			case 2: 
				s = new Helicopter(&data);
			break;
			
			case 3:
				s = new Jetpack(&data);
			break;
			
			case 4: 
				s = new Zepellin(&data);
				((Zepellin *)s)->enemyList = this; // needs to know this to spawn "crew" when crashing
			break;
			
			case 5: 
				s = new B17(&data);
			break;
			
			case 6:
				s = new MissileTruck(&data);
			break;
			
			case 7: 
				s = new WeaponPrize(&data);
			break;
			
			case 8:
				s = new HealthPrize(&data);
			break;
			
			case 9:
				s = new Biplane(&data);
			break;
			
			case 10:
				s = new MechMini(&data);
			break;
			
			case 11:
				s = new APC(&data);
			break;
			
			case 12:
				s = new Mine(&data);
			break;
			
			case 13:
				s = new ICBM(&data);
			break;
			
			case 14:
				s = new F117(&data);
				((F117 *)s)->enemyList = this; // F117 needs to know this to spawn "pilot" when crashing
			break;
			
			case 15:
				s = new B52(&data);
				((B52 *)s)->enemyList = this; // needs to know this to drop paratroopers
			break;
			
			default:
				// ERROR!
				Logger::log("EnemyList::generate() - ERROR default type reached! :O");
				return; // abort
			break;
		}
			
		// this positioning sucks! but works... outside screen at right, any heigth
		s->x = rand() % m->mapW + m->mapW;
		s->y = rand() % m->mapH;
		
		if (!addEnemy(s)) // add to list
		{
			Logger::log("EnemyList::generate() - Enemy list full!");
			return;  // abort generation if Im already full of enemies
		}
	}
	
	Logger::log("EnemyList::generate() - OK, return");
}


int EnemyList::update(Map *m, UFO *ufo, ParticleManager *pm)
{
	for (int i=0; i<enemies_n; i++)
	{
			if (enemy[i]->update(m, ufo, pm, shootsE))
			{
				// Im dead 
				// move last here, and reduce list size
				delete(enemy[i]);
				enemy[i] = enemy[enemies_n-1];
				enemies_n--;
			}
			else
			{
				// Im still alive
				// check collision against UFO only if enabled and alive (not burning, falling,etc)
				if ( enemy[i]->collideWithUFO &&
					 enemy[i]->life > 0 &&	
					 enemy[i]->bbox->collide(ufo->bbox))
				{
					// middle points 
					int ufomx = ufo->x + ufo->sprite->w/2;
					int ufomy = ufo->y + ufo->sprite->h/2;
					
					int mx = enemy[i]->x + enemy[i]->sprite->w/2;
					int my = enemy[i]->y + enemy[i]->sprite->h/2;
					// bounce UFO
					if (ufomx < mx)
							ufo->sx = -UFO::MV_X;
					else
							ufo->sx = UFO::MV_X;

					if (ufomy < my)
							ufo->sy = -UFO::MV_Y;
					else
							ufo->sy = UFO::MV_Y;
						
					// PLAY HIT SOUND
					play_sample(ufo->ufo_hit_wav, 255, ufo->x * 255 / 320, 900 + rand()%300, 0);

					
					// damage ufo and myself) 
					ufo->life -= rand()%3+1;
					enemy[i]->life -= rand()%3+1;
					
					// add particles
					int cp = makecol(255,255, rand()%100 + 85); // yellow
					int pz = rand()%12+8; // particle ammount
					for (int p=0; p<pz;p++)
						pm->add(new Spark(ufomx, ufomy, ufo->sx * Randomize::random(0.2f, 2.0f), ufo->sy * Randomize::random(0.2f, 2.0f), rand()%15+10, cp));
				}
			}
	}
	
	// test enemies shoots against player
	shootsE->update(m, pm, ufo, NULL);
	
	return enemies_n; // how many are alive yet, used by game loop master to decide next wave,  etc
}

void EnemyList::render(BITMAP *bmp)
{
	for (int i=0; i<enemies_n; i++)
		enemy[i]->render(bmp);
	
	// render shoots!
	shootsE->render(bmp);
}