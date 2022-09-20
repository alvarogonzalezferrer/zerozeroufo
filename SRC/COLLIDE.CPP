// collision manager
#include "collide.h"
#include <algorithm>

Collide2D::Collide2D()
{
	x=y=w=h=0; // no collision
	t=0;
}

Collide2D::Collide2D(int x, int y, int w, int h)
{
	setAsBox(x, y, w, h);
}

Collide2D::Collide2D(int x, int y, int r)
{
	setAsCircle(x,y,r);
}

Collide2D::Collide2D(int x, int y)
{
	setAsPoint(x,y);
}

void Collide2D::setWithCoordinates(int x1, int y1, int x2, int y2)
{
	// does not matter order of coordinates, will get good
	this->x = std::min(x1,x2);
	this->y = std::min(y1,y2);
	this->w = std::abs(x2-x1);
	this->h = std::abs(y2-y1);
	t=1;
	
}

void Collide2D::setAsCircle(int x, int y, int r)
{
	this->x = x;
	this->y = y;
	this->w = r;
	this->h = r;
	t=2;
}

void Collide2D::setAsBox(int x, int y, int w, int h)
{
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
	t=1; // box
}

void Collide2D::setAsPoint(int x, int y)
{
	this->x = x;
	this->y = y;
	t=0;
}

// THIS IS THE MOST IMPORTANT METHOD
// CAN DO ANY COLLISION DETECTION NEEDED!!
bool Collide2D::collide(Collide2D *box)
{
	// Im a point, a circle or box? 
	if (t == 0) // im a point / pixel
	{
		// is the other a point, a circle or a box?
		if (box->t ==0) // pixels both
			return (x == box->x && y == box->y); // pixel to pixel collision
		
		if (box->t == 1) // pixel against square
			return box->pointInsideBox(x,y);
		
		
		if (box->t == 2) // pixel against circle
			return box->pointInsideCircle(x,y);
			
		return false; // cant determine collision
	}		

	if (t == 1) // im a square
	{
		// is the other a point, a circle or a box?
		if (box->t ==0) // pixel
			return pointInsideBox(box->x, box->y);
		
		if (box->t == 1) // square against square
			return collideWithBox(box);
		
		
		if (box->t == 2) // square against circle
			return collideWithCircle(box);
			
		return false; // cant determine collision
	}
	
	if (t == 2) // im a circle
	{
		// is the other a point, a circle or a box?
		if (box->t ==0) // pixel
			return pointInsideCircle(box->x, box->y);
		
		if (box->t == 1) // circle against square
			return collideWithBox(box);
		
		
		if (box->t == 2) // circle against circle
			return collideWithCircle(box);
			
		return false; // cant determine collision
	}
	

	return false;
}

bool Collide2D::collideWithBox(Collide2D *box)
{
	// https://developer.mozilla.org/es/docs/Games/Techniques/2D_collision_detection
	
	return ( x     < box->x + box->w &&
			 x + w > box->x          &&
			 y     < box->y + box->h &&
			 y + h > box->y);
}

bool Collide2D::collideWithCircle(Collide2D *box)
{
	// Collision between two circles is very quick to test. 
	// Just check if the distance between the circle centre points is less than the sum of the two radii.
	
	int  dx = abs(x - box->x);
	int  dy = abs(y - box->y);
	
	return (dx < 2*w && dy < 2*h);
}

bool Collide2D::pointInsideBox(int x, int y)
{
	return (x >= this->x         &&
			y >= this->y         &&
			x <= this->x+this->w &&
			y <= this->y+this->h );
}

bool Collide2D::pointInsideCircle(int x, int y)
{
	// https://stackoverflow.com/questions/481144/equation-for-testing-if-a-point-is-inside-a-circle
	
	int dx = abs(x-this->x);
	int dy = abs(y-this->y);
	return ( dx*dx + dy*dy <= w*w );
	
	
}