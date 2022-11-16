// -----------------------------------------------------------------------
// menu.h
// -----------------------------------------------------------------------
// Menu system for the game
// DEBUG: Very basic so far 
// -----------------------------------------------------------------------
// Copyright (c) 2022, Kronoman
// In loving memory of my father.
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------
#ifndef MENU_GAME_H
#define MENU_GAME_H

#include <allegro.h>

#include <string>
#include <vector>

class Menu 
{
	public: 
		Menu();
		~Menu();
		
		// menu items should already be loaded in the vector when calling this!
		// also any init like colors, gap, etc
		void init(); // call once before starting the menu rendering
		
		// call each frame
		// will return menu item selected (0..n-1) or -1 if everything all right, -2 if cancelled by used (ESC, etc)
		int update(); 
		
		void render(BITMAP *bmp); // render menu on bitmap
		
	    // stuff
		
		// internal
		int frame; // frame counter 
		int wait; // wait counter
		int selected; // current selected item 		
		int w, h; // dimensions in pixels of current menu (calculated on init)
		
		
		// ** SETUP THE FOLLOWING BEFORE STARTING MENU!! **
		// font
		FONT *fm; // menu font, defaults to Allegro font
		
		// colors 
		// non selected text
		int fg; // text color 
		int bg; // text background
		
		// selected text
		int fg_s; // text color
		int bg_s; // text background 
		
		// position on bitmap to render
		int x,y; // middle position of menu, if -1 will be auto calculated
		int gap; // y gap between items
		
		int border; // do I need a border? -1 no, other number = color
		int border_fg; // around the border outline color
		
		// items of the menu
		// DEBUG FOR NOW THEY SHOULD FIT VERTICALLY ALL INSIDE SCREEN OR ELSE SOME WILL NOT BE SHOWN		
		std::vector<std::string> menuItems;
		
};
#endif