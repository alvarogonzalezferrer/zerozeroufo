// -----------------------------------------------------------------------
// Controller wrapper
// control.cpp
// -----------------------------------------------------------------------
// This class handles a generic controller.
// Simulates a 8 way gamepad with 6 buttons using keyboard,joystick,mouse
// By Kronoman - In loving memory of my father
// Copyright (c) 2003 - Released under the MIT license
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
// ** NOTICE ** I CHANGED SOME DATA TO PUBLIC DUE TO KBALL REQUIREMENTS!
// -----------------------------------------------------------------------

#ifndef KCONTROL_H
#define KCONTROL_H

#include <allegro.h>


// This are the return values of the controller
// they are returned as a bit mask
// So, all joystick input is DIGITAL
#define KC_NONE  0
#define KC_UP    1
#define KC_DOWN  2
#define KC_LEFT  4
#define KC_RIGHT 8

// buttons (there is room left, for adding more axis in future)
#define KC_BTN1  256
#define KC_BTN2  512
#define KC_BTN3  1024
#define KC_BTN4  2048
#define KC_BTN5  4096
#define KC_BTN6  8192

class CController
{
	public:
		CController();
		~CController();

		// Set functions
		// -------------

		// keyboard
		void set_default_keyboard(); // we have a 'default' configuration for keyboard
		void set_keyboard_par(int value, int index); // this sets one value for key_val[] array
		void set_use_keyboard(bool use); // set if you want to use keyboard input or not
		void interactive_configuration_keyboard(FONT *font, int color); // to interactive configure keyboard

		// mouse
		void set_mouse_sens(int s); // set mouse sensitiviness
		void set_use_mouse(bool use); // use the mouse?
		void interactive_configuration_mouse(FONT *font, int color); // interactive configure mouse

		// joystick
		void set_joystick_number(int n); // wich joystick you want to input?
		void set_use_joystick(bool use); // use the joystick?
		void interactive_configuration_joystick(FONT *font, int color); // interactive configure joystick

		// Get functions
		// -------------

		// this is the one that does the actual input from user
		int do_input_poll();

		int get_keyboard_par(int value, int index); // get keyboard key, -1 on error (bad index passed)
		bool get_use_keyboard() { return this->use_keyboard; }

		bool get_use_mouse() { return this->use_mouse; }
		int get_mouse_sens() { return this->mouse_sens; }

		bool get_use_joystick() { return this->use_joystick; }
		int get_joystick_number() { return this->joy_num; }

		int get_controller_id() { return this->controller_id; } // unique controller ID (assigned by creation)

		// Config saving stuff
		// NOTICE: you have to _previously_ call to allegro's set_config_file
		void save_configuration_of_controller(char *cfg_section); // save current configuration in a file (cfg_section = section)
		void load_configuration_of_controller(char *cfg_section); // load  configuration from a config file (cfg_section = section)

		// Static members (available from the class itself)
		// --------------

		static int get_controller_count() { return CController::controller_count; }

		// ** NOTICE ** I CHANGED SOME DATA TO PUBLIC DUE TO KBALL REQUIREMENTS!
		bool use_keyboard; // want to use keyboard input? (default=true)
		bool use_joystick; // want to use joystick? (default=false)
		bool use_mouse; // use mouse input? (default=true)
		
	private:
		// keyboard stuff
		int key_val[15]; // keys to input: 0..3= up,down,left,right | 4..7= reserved | 8..13= buttons | 14= reserved
		

		// joystick stuff
		int joy_num; // wich joystick to use? 0..num_joysticks (default=0)
		

		// mouse stuff
		int mouse_sens; // square of 'dead' until mouse movement is detected; (default 10, smaller is more sens)

		// ID of controller
		int controller_id; // controller ID: automated (useful for saving configuration, you can have different sections, well, use it for something... )

		// static members, available to all
		static int controller_count;
};

#endif
