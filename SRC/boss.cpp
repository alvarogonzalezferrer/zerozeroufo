#include "boss.h"
#include "logger.h"
#include "randomiz.h"

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

void Boss::renderHUD(BITMAP *bmp)
{
	int c = makecol(255,255,255); // white
	int bg = makecol(0,0,170); // blue
	int yh = bmp->h - text_height(font) - 2;
	
	if (life > 0)
		textprintf_centre_ex(bmp, font, bmp->w/2, yh, c, bg, "BOSS POWER %04d", life);
	else 
		textprintf_centre_ex(bmp, font, bmp->w/2, yh, c, bg, "** BOSS DYING **");
}

// collide with UFO
void Boss::collide(UFO *ufo, ParticleManager *pm)
{
	if (!collideWithUFO || !ufo || life < 1 || !bbox)
		return;  // no collision possible
	
	if (!bbox->collide(ufo->bbox))
		return; // no collision
	
	// middle points 
	int ufomx = ufo->x + ufo->sprite->w/2;
	int ufomy = ufo->y + ufo->sprite->h/2;
	
	int mx = x + sprite->w/2;
	int my = y + sprite->h/2;
	
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
	life -= rand()%3+1;
	
	// add particles
	int cp = makecol(255,255,85); // yellow
	int pz = rand()%12+8; // particle ammount
	for (int p=0; p<pz;p++)
		pm->add(new Spark(ufomx, ufomy, ufo->sx * Randomize::random(0.2f, 2.0f), ufo->sy * Randomize::random(0.2f, 2.0f), rand()%15+10, cp));
}