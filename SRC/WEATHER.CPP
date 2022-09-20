#include "weather.h"

WPart::WPart()
{
	x=y=dx=dy=0;
	c=r=0;
	t=0;
}

void WPart::render(BITMAP *bmp)
{
	if (t > 1) 
		return;


	// 2022 - pixel art ; I found the old rain and snow effects too annoying for low resolutions	
	if (t == 0)
		putpixel(bmp, (int)x, (int)y, c); // snow
	else
		line(bmp, (int)x, (int)y, (int)(x + dx), (int)(y + dy), c);  // rain
	
	/*if (t == 0)
		circlefill(bmp, (int)x, (int)y, r, c); // snow
	else // rain
	{
		// DEBUG TRANSPARENT MODE DISABLED FOR 8 bits color!
		drawing_mode(DRAW_MODE_TRANS, NULL,0,0); 
		line(bmp, (int)x, (int)y, (int)(x + dx*5), (int)(y + dy*5), c);
		drawing_mode(DRAW_MODE_SOLID, NULL,0,0);
	}*/
}



Weather::Weather()
{
}

Weather::~Weather()
{
}

void Weather::render(BITMAP *bmp)
{
	for (int i = 0; i < n; i++)
		part[i].render(bmp);
}

void Weather::update()
{
	for (int i = 0; i < n; i++)
	{
		part[i].x += part[i].dx;
		part[i].y += part[i].dy;

		if (part[i].x < 0)
			part[i].x += 320;

		if (part[i].x > 320)
			part[i].x -= 320;

		if (part[i].y > 200)
		{
			part[i].y = -rand()%15;
			part[i].x = rand()%320;
			part[i].dx = (rand()%150 / 100.0f) * w;
		}
	}
}



// type
// 0=snow
// 1=rain
// 3 or up, none
void Weather::generate(int t)
{
	n = 0;
	if (t > 1) 
		return;

	n = rand()%(maxP/2) + (maxP/2);
	w = rand()%3-1;
	for (int i = 0; i < n; i++)
	{
		part[i].x = rand()%320;
		part[i].y = rand()%200;

		part[i].dx = (rand()%150 / 100.0f) * w;

		part[i].dy = rand()%400 / 100.0f + 2;

		part[i].r = rand()%3+1;
		part[i].t = t;

		if (t == 0)
			part[i].c = (rand()%2) ? makecol(255,255,255) : makecol(170,170,170); // snow
		else
			part[i].c = (rand()%2) ? makecol(85,85,255) :  makecol(0,0,170) ; // rain
	}
}

