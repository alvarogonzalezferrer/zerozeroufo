// ---------------------------------------------------------------------------
// particle.h
// ---------------------------------------------------------------------------
// Particle system and manager
// ---------------------------------------------------------------------------
// By Kronoman. Sharp dressed man.
// Copyright (c) 2005, Kronoman
// In loving memory of my father.
// ---------------------------------------------------------------------------
#ifndef PARTICLE_H
#define PARTICLE_H

#include <allegro.h>

#include <list>

// ---------------------------------------------------------------------------
// base particle
// ---------------------------------------------------------------------------
class BaseParticle
{
	public:
		BaseParticle();
		BaseParticle(float x, float y, float dx, float dy, int life, int color);
		BaseParticle(float x, float y, float dx, float dy, int life);
		virtual ~BaseParticle();
		
		virtual bool update(); ///< returns TRUE when particle is dead and must be deleted
		virtual void render(BITMAP *bmp, int sx, int sy); ///< render in bmp with scroll sx,sy
		
		float x;
		float y;
		float dx;
		float dy;
		int life;
		int color;
};

// ---------------------------------------------------------------------------
// spark
// ---------------------------------------------------------------------------
class Spark : public BaseParticle
{
	public:
		Spark(float x, float y, float dx, float dy, int life, int color);
		void render(BITMAP *bmp, int sx, int sy);
};

// ---------------------------------------------------------------------------
// blood
// ---------------------------------------------------------------------------
class Blood : public BaseParticle
{
	public:
		Blood(float x, float y, float dx, float dy, int life);
		void render(BITMAP *bmp, int sx, int sy);
		bool update();

	int t; // line or spark
};


// ---------------------------------------------------------------------------
// circle
// ---------------------------------------------------------------------------
class Circle : public BaseParticle
{
	public:
		Circle(float x, float y, float dx, float dy, int life, int color, int radius);
		void render(BITMAP *bmp, int sx, int sy);

	
		int radius;
};

// ---------------------------------------------------------------------------
// smoke (is like a circle, but grows in radius, and is transparent
// ---------------------------------------------------------------------------
class Smoke : public BaseParticle
{
	public:
		Smoke(float x, float y, float dx, float dy, int life, int color, float radius, float radiusGrow);
		void render(BITMAP *bmp, int sx, int sy);
		bool update();
	
		float radius;
		float radiusGrow;
};


// ---------------------------------------------------------------------------
// sprite particle
// ---------------------------------------------------------------------------
class SpriteParticle : public BaseParticle
{
	public:
		SpriteParticle(float x, float y, float dx, float dy, int life, BITMAP *sprite);
		void render(BITMAP *bmp, int sx, int sy);
	
	// data
		BITMAP *sprite;
};

// ---------------------------------------------------------------------------
// rotational sprite particle
// ---------------------------------------------------------------------------
class RotationalSpriteParticle : public BaseParticle
{
	public:
		RotationalSpriteParticle(float x, float y, float dx, float dy, float angle, float angleSpd, int life, BITMAP *sprite);
		void render(BITMAP *bmp, int sx, int sy);
		bool update();
	
	// data
		BITMAP *sprite;
		float angle; //0...255 f
		float angleSpd;
};

// ---------------------------------------------------------------------------
// particle manager
// ---------------------------------------------------------------------------
class ParticleManager
{
	public:
		ParticleManager();
		~ParticleManager();
		
		void finish(); ///< unloads all particles, releasing RAM

		void add(BaseParticle *p); ///< add particle
		
		void update();
		void render(BITMAP *bmp, int sx, int sy);
	
		// data
		std::list <BaseParticle *> particles; ///< particle container
};


#endif
