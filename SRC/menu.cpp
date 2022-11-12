// -----------------------------------------------------------------------
// menu.cpp
// -----------------------------------------------------------------------
// Menu system for the game
// -----------------------------------------------------------------------
// Copyright (c) 2022, Kronoman
// In loving memory of my father.
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------

#include "menu.h"

Menu::Menu()
{
	// init
	fm = NULL; // default allegro font
	
	fg = bg = fg_s = bg_s = -1; // colors default
	x = y = gap = -1;
	
	w = -1;
	h = 0;
	
	border = -1; 
}

Menu::~Menu()
{
	// end
}

// INIT
// menu items should already be loaded in the vector when calling this!
// also any init like colors, gap, etc
void Menu::init()
{
	// initialize position of items, etc
	selected = wait = frame = 0;
	
	if (!fm)
	{
		fm = font; // default allegro font
		gap = -1; // set default gap
	}
	
	if (gap < 0)
	{
		gap = text_height(fm)+3;
	}
	
	// default colors, VGA palette safe
	if (fg < 0)
	{
		fg = makecol(255,255,255);
		bg = -1;
		
		fg_s = makecol(255,255,85);
		bg_s = makecol(170,0,0);
		
		border = makecol(32,32,32);
	}
	
	// calculate dimensiones of the menu
	w = 0; // calculate
	h = 0;
	int rh = text_height(fm);
	for(int i = 0; i<menuItems.size(); i++)
	{
		int rw = text_length(fm, menuItems[i].c_str());
		if (rw > w)
			w = rw;
		
		h += rh + gap;
	}
	
	// DEBUG CALCULATE ACCORDING TO MAX MENU HEIGHT
	if (x < 0 || y < 0)
	{
		x = 160;
		y = 100; 
	}
}

int Menu::update()
{
	// update logic
	if (wait < 1)
	{
		if (key[KEY_W] || key[KEY_UP] || key[KEY_A] || key[KEY_LEFT])
		{
			selected--;
			wait = 3; // slow down 
		}

		if (key[KEY_S] || key[KEY_DOWN] || key[KEY_D] || key[KEY_RIGHT])
		{
			selected++;
			wait = 3; // slow down 
		}
	}
	else
	{
		wait--;
	}

	if (selected < 0)
		selected = menuItems.size()-1;
	
	if (selected > menuItems.size()-1)
		selected = 0;

	if (key[KEY_SPACE] || key[KEY_Z] || key[KEY_ENTER] || key[KEY_X])
		return selected;
	
	if (key[KEY_ESC])
		return -2 ; // cancel
	
	return -1; // all OK
}


void Menu::render(BITMAP *bmp)
{
	// render on bitmap
	int c_f = fg;
	int c_b = bg;
	
	// real y to start drawing
	int ry = y - h/2;
	
	// step y
	int rys = text_height(fm) + gap;
	
	// border ? 
	if (border>=0)
		rectfill(bmp, x-w/2-gap, y-h/2-gap, x+w/2+gap, y-h/2+gap, border);
	
	for(int i = 0; i<menuItems.size(); i++)
	{
		if (i == selected)
		{
			c_f = fg_s;
			c_b = bg_s;
		}
		else
		{
			c_f = fg;
			c_b = bg;
		}
		
		 textout_centre_ex(bmp, fm, menuItems[i].c_str(), x, ry, c_f, c_b);
		 
		 ry += rys;
	}
}
