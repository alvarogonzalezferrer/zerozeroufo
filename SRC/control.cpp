// -----------------------------------------------------------------------
// Controller wrapper
// control.cpp
// -----------------------------------------------------------------------
// This class handles a generic controller.
// Simulates a 8 way gamepad with 6 buttons using keyboard,joystick,mouse
// By Kronoman - In loving memory of my father
// Copyright (c) 2003 - This file released under the MIT license
// 08-NOV-2003
//
// ** NOTE **
// Modified 04-MAR-2004 to add customize control interface.
// This class is arcane, I have a better implementation.
//
// We will use this in this game, in future games, we will use the new implementation.
//
// **** NOTICE ****
// THIS PARTICULAR GAME, KBALL DOES NOT NEED BUTTONS, SO IS DISABLED IN INTERACTIVE CONFIGURATION!
// -----------------------------------------------------------------------

#include "control.h"

int CController::controller_count = 0;

CController::CController()
{
		// keyboard stuff
		this->use_keyboard = TRUE; // want to use keyboard input? (default=true)
		this->set_default_keyboard();

		// joystick stuff
		this->joy_num = 0; // wich joystick to use? 0..num_joysticks (default=0)
		this->use_joystick = FALSE;

		// mouse stuff
		this->use_mouse = TRUE; // use mouse input? (default=true)
		this->mouse_sens = 10; // square of 'dead' until mouse movement is detected; (default 10)

		this->controller_id = CController::controller_count;
		CController::controller_count++;
}

CController::~CController()
{
		CController::controller_count--;
}

// sets the default keyboard configuration
void CController::set_default_keyboard()
{
	// keys to input: 0..3= up,down,left,right | 4..7= reserved | 8..13= buttons | 14= reserved
	key_val[0] = KEY_UP;
	key_val[1] = KEY_DOWN;
	key_val[2] = KEY_LEFT;
	key_val[3] = KEY_RIGHT;

	key_val[8] = KEY_A;
	key_val[9] = KEY_S;
	key_val[10] = KEY_D;
	key_val[11] = KEY_Z;
	key_val[12] = KEY_X;
	key_val[13] = KEY_C;
}

// this function is used to configure the keyboard
void CController::set_keyboard_par(int value, int index)
{
	if ((index < 0) || (index > 14)) return; // error :P

	this->key_val[index] = value;
}

void CController::set_use_keyboard(bool use)
{
	this->use_keyboard = use;
}

void CController::set_mouse_sens(int s)
{
  this->mouse_sens = abs(s);
}

void CController::set_use_mouse(bool use)
{
	this->use_mouse = use;
}

void CController::set_joystick_number(int n)
{
	if (n > num_joysticks-1) return; // error, that joystick is not there!

	this->joy_num = n;
}

void CController::set_use_joystick(bool use)
{
	this->use_joystick = use;
}


// This is the main function
// This actually does the input from hardware
// and returns the bitmask acording to action
int CController::do_input_poll()
{
	int ret = KC_NONE; // return value
	static int old_mouse_z = -666; // mouse_z last call, special flag = -666, means dirty

	// keyboard
	if (this->use_keyboard)
	{
		if (keyboard_needs_poll()) poll_keyboard();

		if (key[key_val[0]]) ret |= KC_UP;
		if (key[key_val[1]]) ret |= KC_DOWN;
		if (key[key_val[2]]) ret |= KC_LEFT;
		if (key[key_val[3]]) ret |= KC_RIGHT;

		if (key[key_val[8]])  ret |= KC_BTN1;
		if (key[key_val[9]])  ret |= KC_BTN2;
		if (key[key_val[10]]) ret |= KC_BTN3;
		if (key[key_val[11]]) ret |= KC_BTN4;
		if (key[key_val[12]]) ret |= KC_BTN5;
		if (key[key_val[13]]) ret |= KC_BTN6;
	}

	// mouse
	if (this->use_mouse)
	{
		int mickeyx = 0;  int mickeyy = 0;

		if (mouse_needs_poll()) poll_mouse();

		get_mouse_mickeys(&mickeyx, &mickeyy);

		if (mickeyx < -mouse_sens) ret |= KC_LEFT;
		if (mickeyx > mouse_sens) ret |= KC_RIGHT;

		if (mickeyy < -mouse_sens) ret |= KC_UP;
		if (mickeyy > mouse_sens) ret |= KC_DOWN;

		if (mouse_b & 1) ret |= KC_BTN1;
		if (mouse_b & 2) ret |= KC_BTN2;
		if (mouse_b & 4) ret |= KC_BTN3;

		// also mouse_z is used, up = btn4, down = btn5, sadly I can't do yet button 6
		if (old_mouse_z != -666)
		{
			if (mouse_z < old_mouse_z) ret |= KC_BTN4;
			if (mouse_z > old_mouse_z) ret |= KC_BTN5;
		}
		old_mouse_z = mouse_z;
	}

	// joystick
	if (use_joystick)
	{
		poll_joystick(); // needed

		// digital joystick input
		if (joy[joy_num].stick[0].axis[0].d1) ret |= KC_LEFT;
		if (joy[joy_num].stick[0].axis[0].d2) ret |= KC_RIGHT;

		if (joy[joy_num].stick[0].axis[1].d1) ret |= KC_UP;
		if (joy[joy_num].stick[0].axis[1].d2) ret |= KC_DOWN;

		if (joy[joy_num].num_buttons > 0)
			if (joy[joy_num].button[0].b) ret |= KC_BTN1;

		if (joy[joy_num].num_buttons > 1)
			if (joy[joy_num].button[1].b) ret |= KC_BTN2;

		if (joy[joy_num].num_buttons > 2)
			if (joy[joy_num].button[2].b) ret |= KC_BTN3;

		if (joy[joy_num].num_buttons > 3)
			if (joy[joy_num].button[3].b) ret |= KC_BTN4;

		if (joy[joy_num].num_buttons > 4)
			if (joy[joy_num].button[4].b) ret |= KC_BTN5;

		if (joy[joy_num].num_buttons > 5)
			if (joy[joy_num].button[5].b) ret |= KC_BTN6;
	}

return ret;
}


int CController::get_keyboard_par(int value, int index)
{
	if ((index < 0) || (index > 14)) return -1; // error :P

	return this->key_val[index];
}


void CController::save_configuration_of_controller(char *cfg_section)
{
char str[256];

	// how can I save a bool? is this the correct way?
	set_config_int(cfg_section, "use_keyboard", (int)this->use_keyboard);
	set_config_int(cfg_section, "use_joystick", (int)this->use_joystick);
	set_config_int(cfg_section, "use_mouse", (int)this->use_mouse);

	// save keys
	for (int i=0; i < 15; i++)
	{
		usprintf(str,"key_val_%d",i);
		set_config_int(cfg_section, str, this->key_val[i]);
	}

	// save joystick number
	set_config_int(cfg_section, "joy_num", this->joy_num);

	set_config_int(cfg_section, "mouse_sens", this->mouse_sens);

}

void CController::load_configuration_of_controller(char *cfg_section)
{
char str[256];

	// is this the correct way?
	this->use_keyboard = (bool)get_config_int(cfg_section, "use_keyboard", (int)this->use_keyboard);
	this->use_joystick = (bool)get_config_int(cfg_section, "use_joystick", (int)this->use_joystick);
	this->use_mouse = (bool)get_config_int(cfg_section, "use_mouse", (int)this->use_mouse);

	// get keys
	for (int i=0; i < 15; i++)
	{
		usprintf(str,"key_val_%d",i);
		this->key_val[i] = get_config_int(cfg_section, str, this->key_val[i]);
	}

	this->joy_num = get_config_int(cfg_section, "joy_num", this->joy_num);

	this->mouse_sens = get_config_int(cfg_section, "mouse_sens", this->mouse_sens);
}

// interactive configuration of controller (really LAME interface with user... improve it! :P)
void CController::interactive_configuration_keyboard(FONT *font, int color)
{
	int y = 100, h = text_height(font);
	
	clear_keybuf();
	rest(10);
	while (keypressed()) readkey();
	
	textout_ex(screen, font, "-- Keyboard configuration--", 0, y, color,-1);
	
	y+=h;
	
	textout_ex(screen, font, "Press key for 'UP'", 0, y+=h, color,-1);
	key_val[0] =  readkey() >> 8;
	
	textout_ex(screen, font, "Press key for 'DOWN' ", 0, y+=h, color,-1);
	key_val[1] =  readkey() >> 8;
	
	textout_ex(screen, font, "Press key for 'LEFT' ", 0, y+=h, color,-1);
	key_val[2] =  readkey() >> 8;
	
	textout_ex(screen, font, "Press key for 'RIGHT' ", 0, y+=h, color,-1);
	key_val[3] =  readkey() >> 8;

// DEBUG -- DEBUG -- THIS PARTICULAR GAME, KBALL DOES NOT NEED BUTTONS, SO THIS IS DISABLED
#ifdef THIS_IS_DISABLED_FOR_THIS_GAME
	
	textout_ex(screen, font, "Press key for 'BUTTON 1' ", 0, y+=h, color,-1);
	key_val[8] =  readkey() >> 8;
	
	textout_ex(screen, font, "Press key for 'BUTTON 2' ", 0, y+=h, color,-1);
	key_val[9] =  readkey() >> 8;

	textout_ex(screen, font, "Press key for 'BUTTON 3' ", 0, y+=h, color,-1);
	key_val[10] =  readkey() >> 8;
	
	textout_ex(screen, font, "Press key for 'BUTTON 4' ", 0, y+=h, color,-1);
	key_val[11] =  readkey() >> 8;
	
	textout_ex(screen, font, "Press key for 'BUTTON 5' ", 0, y+=h, color,-1);
	key_val[12] =  readkey() >> 8;
	
	textout_ex(screen, font, "Press key for 'BUTTON 6' ", 0, y+=h, color,-1);
	key_val[13] =  readkey() >> 8;
	
#endif

	clear_keybuf();
	rest(10);
	
	textout_ex(screen, font, "Done. Press any key...   ", 0, y+=h*2, color,-1);
	readkey();
	// done
}

// interactive configuration of controller (really LAME interface with user... improve it! :P)
void CController::interactive_configuration_joystick(FONT *font, int color)
{
	int y = 100, h = text_height(font);
	
	clear_keybuf();
	while (keypressed()) readkey();
	
	if (!num_joysticks)
	{
		textout_ex(screen, font, "-- Error: Joystick not found! --", 0, y, makecol(255,0,0),-1);
		textout_ex(screen, font, "Press any key...", 0, y+=h, color,-1);
		readkey();
		return;
	}

	textout_ex(screen, font, "-- Joystick configuration--", 0, y, color,-1);
	
	y+=h;
	
	if (num_joysticks > 1)
	{
		textprintf_ex(screen, font, 0, y+=h, color,-1, "You have %d joysticks", num_joysticks);
		textprintf_ex(screen, font, 0, y+=h, color,-1, "Wich one to use? (Press number 1..%d)", num_joysticks);
		
		char tmp = (readkey() & 0xff); // take ASCII code
		
		joy_num = 0; // default
		
		if (tmp == '1') joy_num = 0;
		if (tmp == '2') joy_num = 1;
		if (tmp == '3') joy_num = 2;
		if (tmp == '4') joy_num = 3;
		// although Allegro driver currently supports up to 4 controllers, we let room to grow in future
		if (tmp == '5') joy_num = 4;
		if (tmp == '6') joy_num = 5;
		if (tmp == '7') joy_num = 6;
		if (tmp == '8') joy_num = 7;
		if (tmp == '9') joy_num = 8;

		if (joy_num > num_joysticks) joy_num = 0; // default
	}
	else
	{
		joy_num = 0;	
	}
	
	textprintf_ex(screen, font, 0, y+=h, color, -1,"We will use joystick number %d", joy_num+1);
	
	// calibrate joystick if needed, although not needed, because we use digital input
	AL_CONST char *msg;
	while (joy[joy_num].flags & JOYFLAG_CALIBRATE) 
	{
		msg = calibrate_joystick_name(joy_num);
		
		textout_ex(screen, font, msg, 0, y+=h, color,-1);
		textout_ex(screen, font, "And press any key.", 0, y+=h, color,-1);
		
		if ((readkey()&0xFF) == 27)	return;
		
		if (calibrate_joystick(0) != 0) 
		{
			textout_ex(screen, font, "Error calibrating joystick!", 0, y+=h, makecol(255,0,0),-1);
			textout_ex(screen, font, "Press any key...", 0, y+=h, color,-1);
			readkey();
			return;
		}
	}

	clear_keybuf();
	rest(10);
	  
	textout_ex(screen, font, "Done. Press any key...   ", 0, y+=h*2, color,-1);
	readkey();
	// done
}

void CController::interactive_configuration_mouse(FONT *font, int color)
{
	int y = 100, h = text_height(font);
	
	clear_keybuf();
	while (keypressed()) readkey();
	
	textout_ex(screen, font, "--Mouse configuration--", 0, y, color,-1);
	
	y+=h;
	
	textout_ex(screen, font, "Press a key number from 1..9 to choose sensitiviness", 0, y+=h, color,-1);
	textout_ex(screen, font, "1 = minimum to 9 = maximum", 0, y+=h, color,-1);
	
	textout_ex(screen, font, "Any other key to cancel.", 0, y+=h, color,-1);
	char ret = (readkey() & 0xff); // take ASCII code
	
	if (ret == '1') mouse_sens = 40;
	if (ret == '2') mouse_sens = 30;
	if (ret == '3') mouse_sens = 25;
	if (ret == '4') mouse_sens = 20;
	if (ret == '5') mouse_sens = 15;
	if (ret == '6') mouse_sens = 10;
	if (ret == '7') mouse_sens =  5;
	if (ret == '8') mouse_sens =  1;
	if (ret == '9') mouse_sens =  0;

	textout_ex(screen, font, "Done. Press any key...   ", 0, y+=h, color,-1);
	clear_keybuf();
	rest(10);
	readkey();
	// done
}
