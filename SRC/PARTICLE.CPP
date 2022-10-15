// ---------------------------------------------------------------------------
// particle.cpp
// ---------------------------------------------------------------------------
// Particle system and manager
// ---------------------------------------------------------------------------
// By Kronoman. Sharp dressed man.
// Copyright (c) 2005, Kronoman
// In loving memory of my father.
// ---------------------------------------------------------------------------

#include <allegro.h>
#include "particle.h"
//#include "geometry.h"

// ---------------------------------------------------------------------------
// base particle
// ---------------------------------------------------------------------------
BaseParticle::BaseParticle()
{
	x = y = dx = dy = 0;
	life = 0;
	color = 0;
}

BaseParticle::BaseParticle(float x, float y, float dx, float dy, int life, int color)
{
	this->x = x;
	this->y = y;
	this->dx = dx;
	this->dy = dy;
	this->life = life;
	this->color = color;
}

BaseParticle::BaseParticle(float x, float y, float dx, float dy, int life)
{
	this->x = x;
	this->y = y;
	this->dx = dx;
	this->dy = dy;
	this->life = life;
	this->color = makecol(255,255,255);
	
}

BaseParticle::~BaseParticle()
{
	// nothing to be done here.
}

// a dead particle should return TRUE to be removed from the particle list
bool BaseParticle::update()
{
	x += dx;
	y += dy;
	
	if (--life < 1)
		return true;
	
	return false;
}

void BaseParticle::render(BITMAP *bmp, int sx, int sy)
{
	putpixel(bmp, (int)x - sx, (int)y - sy, color);
}


// ---------------------------------------------------------------------------
// blood
// ---------------------------------------------------------------------------
Blood::Blood(float x, float y, float dx, float dy, int life) : BaseParticle(x, y, dx, dy, life)
{
	color =  makecol(170,0,0);
	t = rand()%2;
}

void Blood::render(BITMAP *bmp, int sx, int sy)
{
	if (t == 0)
		line(bmp, (int)x - sx, (int)y - sy, (int)(x+dx-sx), (int)(y+dy-sy), color);
	else
		circlefill(bmp, (int)x - sx, (int)y - sy, t, color);
	
}

bool Blood::update()
{
	dy += (float)(rand()%400)/1000.0f;
	return BaseParticle::update();
}

// ---------------------------------------------------------------------------
// spark
// ---------------------------------------------------------------------------
Spark::Spark(float x, float y, float dx, float dy, int life, int color) : BaseParticle( x,  y,  dx,  dy,  life,  color)
{
}

void Spark::render(BITMAP *bmp, int sx, int sy)
{
	 line(bmp, (int)x - sx, (int)y - sy, (int)(x+dx-sx), (int)(y+dy-sy), color);
}


// ---------------------------------------------------------------------------
// circle
// ---------------------------------------------------------------------------
Circle::Circle(float x, float y, float dx, float dy, int life, int color, int radius) : BaseParticle(x, y, dx, dy, life, color)
{
	this->radius = radius;
}

void Circle::render(BITMAP *bmp, int sx, int sy)
{
	 circlefill(bmp, (int)x - sx, (int)y - sy, radius, color);
}

// ---------------------------------------------------------------------------
// smoke
// ---------------------------------------------------------------------------
Smoke::Smoke(float x, float y, float dx, float dy, int life, int color, float radius, float radiusGrow) : BaseParticle(x, y, dx, dy, life, color)
{
	this->radius = radius;
	this->radiusGrow = radiusGrow;
}

void Smoke::render(BITMAP *bmp, int sx, int sy)
{
	// DEBUG - this is slow!
	// TODO - DEBUG TRANSPARENCY DISABLED FOR MS DOS!
	//set_trans_blender(16,16,16,16);
	//drawing_mode(DRAW_MODE_TRANS, NULL,0,0); 
	
	circlefill(bmp, (int)x - sx, (int)y - sy, (int)radius, color);
	
	//drawing_mode(DRAW_MODE_SOLID, NULL,0,0);
	//set_trans_blender(128,128,128,128);
}

bool Smoke::update()
{
	radius += radiusGrow;
	return BaseParticle::update();
}

// ---------------------------------------------------------------------------
// sprite particle
// ---------------------------------------------------------------------------
SpriteParticle::SpriteParticle(float x, float y, float dx, float dy, int life, BITMAP *sprite) : BaseParticle(x, y, dx, dy, life)
{
	this->sprite = sprite;
}


void SpriteParticle::render(BITMAP *bmp, int sx, int sy)
{
	draw_sprite(bmp, sprite, (int)x - sx, (int)y - sy);
}

// ---------------------------------------------------------------------------
// rotational sprite particle
// ---------------------------------------------------------------------------
RotationalSpriteParticle::RotationalSpriteParticle(float x, float y, float dx, float dy, float angle, float angleSpd, int life, BITMAP *sprite) : BaseParticle(x, y, dx, dy, life, color)
{
	this->angle = angle;
	this->angleSpd = angleSpd;	
	this->sprite = sprite;
}

void RotationalSpriteParticle::render(BITMAP *bmp, int sx, int sy)
{
	rotate_sprite(bmp, sprite, (int)x - sx, (int)y - sy, ftofix(angle));
}

bool RotationalSpriteParticle::update()
{
	angle += angleSpd;
	
	dy += 0.05; // TODO DEBUG ADDED THIS TO SIMULATE GRAVITY IN JIBS
	return BaseParticle::update();
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// particle manager
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------


ParticleManager::ParticleManager()
{
	// nothing to be done here for now
}

ParticleManager::~ParticleManager()
{
	finish();
}


void ParticleManager::finish()
{
	for (std::list <BaseParticle *>::iterator it = particles.begin(); it != particles.end(); it++)
		delete *it;
	
	particles.clear();
}

void ParticleManager::add(BaseParticle *p)
{
	particles.push_back(p);
}

	
void ParticleManager::update()
{
	for (std::list <BaseParticle *>::iterator it = particles.begin(); it != particles.end(); it++)
	{
		if ((*it)->update())
		{
			// debug, is this code OK? maybe we have a memory leak here
			std::list <BaseParticle *>::iterator i2 = it;
			it++;
			delete *i2;
			particles.erase(i2);
		}
	}
}

void ParticleManager::render(BITMAP *bmp, int sx, int sy)
{
	for (std::list <BaseParticle *>::iterator it = particles.begin(); it != particles.end(); it++)
		(*it)->render(bmp,sx,sy);	
}
