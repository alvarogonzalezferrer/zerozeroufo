// -----------------------------------------------------------------------
// msgbox.cpp
// -----------------------------------------------------------------------
// Message box system
// -----------------------------------------------------------------------
// Copyright (c) 2022, Kronoman
// In loving memory of my father.
// -----------------------------------------------------------------------
// Released under The MIT License.
// ----------------------------------------------------------------------- 

#include "msgbox.h"

#include <string>
#include <vector>

void MessageBox::init()
{
	fg = makecol(255,255,255);
	bg = makecol(170,0,0);
	bd = makecol(255,85,85);
	fm = font; // default allegro font
	gap = 3 ; // gap around stuff
}

void MessageBox::showMessage(std::string msg, std::string ok)
{
	// first we tokenize the msg into lines
	// and find width in pixels and height of message box
	int w=0;
	int h=0;
	std::vector<std::string> lines;
	
	int rh = text_height(fm);
	int rw = 0;
	
	
	std::string delim = "\n"; // delimiter of new line
	
	size_t pos = 0;  
    std::string token;
	
	// use find() function to get the position of the delimiters  
    while (( pos = msg.find (delim)) != std::string::npos)  
    {  
		token = msg.substr(0, pos); // store the substring   
		lines.push_back(token); // add this item 
		
		rw = text_length(fm, token.c_str()); // width in pixels
		
		if (rw > w)
			w = rw;
		
		h += rh + gap;
		
		msg.erase(0, pos + delim.length());  // erase() function store the current positon and move to next token. 
    }
	// last chunk
	rw = text_length(fm, msg.c_str());
		
	if (rw > w)
		w = rw;
	
	h += rh + gap;
	
	lines.push_back(msg);
	
	// add for the OK / etc button
	// <gap>
	// BUTTON
	// <gap>
	h += rh + gap*2;
	
	// done with it, now we have w and h 
	
	// render centered
	int x = SCREEN_W / 2;
	int y = SCREEN_H / 2;
	
	int x1 = x-(w/2)-gap;
	int y1 = y-(h/2)-gap;
	int x2 = x+(w/2)+gap;
	int y2 = y+(h/2)+gap;
	
	rectfill(screen,x1,y1,x2,y2, bg);
	rect(screen,x1,y1,x2,y2, bd);
	
	int ys = y1+gap;
	int xs = x;
	for(int i = 0; i<lines.size(); i++)
	{
		textout_centre_ex(screen, fm, lines[i].c_str(), xs, ys, fg, -1);
		ys += rh + gap;
	}
	
	// ok button
	textout_centre_ex(screen, fm, ok.c_str(), xs, ys, fg, bd);
	
	// wait for keyboard
	clear_keybuf();
	clear_keybuf();
	rest(50);
	
	while (!keypressed());
	
	rest(50);
	clear_keybuf();
	clear_keybuf();
}
