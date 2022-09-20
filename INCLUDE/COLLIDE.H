// -----------------------------------------------------------------------
// collide.h
// -----------------------------------------------------------------------
// Collision 2D manager
// -----------------------------------------------------------------------
// Copyright (c) 2022, Kronoman
// In loving memory of my father.
// Made in Argentina
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------
#ifndef COLLIDE2D_H
#define COLLIDE2D_H

// checks different 2D collisions
class Collide2D
{
	public:
		Collide2D();
		
		Collide2D(int x, int y, int w, int h); // as box
		
		Collide2D(int x, int y, int r); // as circle
		
		Collide2D(int x, int y); // as point
		
		void setWithCoordinates(int x1, int y1, int x2, int y2); // instead of W,H I can use coordinates
		
		void setAsCircle(int x, int y, int r); // set as if I was a circle 
		
		void setAsBox(int x, int y, int w, int h);
		
		void setAsPoint(int x, int y);
		
		// general collision of my type against another type
		bool collide(Collide2D *box); // can be any pixel circle etc
		
		// box against box 
		bool collideWithBox(Collide2D *box); // collide against another box, return true if collides

		// circle against circle 
		bool collideWithCircle(Collide2D *box); // collide against another circle, return true if collide, will take w as radius
		
		// point in box
		bool pointInsideBox(int x, int y); // is the point x,y inside my box? 
		
		// point in circle 
		bool pointInsideCircle(int x, int y); // is the point x,y inside my radius? 
		
		int x,y;
		int w,h; // w will act as radius if Im using circles 
	
		int t; // type:0 point, 1 square, 2 circle
};


#endif