// -----------------------------------------------------------------------
// msgbox.h
// -----------------------------------------------------------------------
// Message box system
// -----------------------------------------------------------------------
// Copyright (c) 2022, Kronoman
// In loving memory of my father.
// -----------------------------------------------------------------------
// Released under The MIT License.
// ----------------------------------------------------------------------- 
#ifndef MSGBOX_H
#define MSGBOX_H

#include <allegro.h>

#include <string>

class MessageBox
{
	public:
		
		void init(); // will set default font and colors
		
		// msg = message, ok = ok button text
		void showMessage(std::string msg, std::string ok); // will show a modal message on screen, centered, message can be multiline with \n
		
		// parameters
		FONT *fm; // font for messages
		
		int fg; // text color 
		int bg; // text background box
		int bd; // border around text box
		
		int gap; // gap around texts
		
		
};


#endif