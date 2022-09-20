// -----------------------------------------------------------------------
// gamemachine.h
// -----------------------------------------------------------------------
// Game Machine
// -----------------------------------------------------------------------
// Copyright (c) 2007, Kronoman
// In loving memory of my father.
// Made in Argentina
// -----------------------------------------------------------------------
// Released under The MIT License.
// -----------------------------------------------------------------------
#ifndef GAMEMACHINE_H
#define GAMEMACHINE_H

class GameMachine
{
	public:
		GameMachine(); ///<  IMPORTANT STUFF HAPPENS HERE
		virtual ~GameMachine(); ///< destructor... cleans up the mess
						
		void start(); ///< start the machine (runs the loop, etc...)
				
		// ** this methods are called indirectly, please don't call them unless you know what to do...
		
		// TODO ** you should override/overwrite this two methods **
		virtual void update(); ///< updates the game, will return states if needed... 
		virtual void render(); ///< renders what is happening
		
		void changeState(const unsigned int state); ///< changes state from current to another
		
		// game machine states, add your own if needed
		static const unsigned int STATE_START = 0; ///< initial state... to load stuff, etc
		static const unsigned int STATE_OK = 1; ///< everything OK... roll
		static const unsigned int STATE_QUIT = 32768; ///< quit game, ending loop...		
		
		// states
		unsigned int currentState; ///< current state of the machine
		unsigned int previousState; ///< previous state of the machine

		// BPS DESIRED FOR THE GAME MACHINE (BEATS PER SECOND, usually 30 or 60)
		static const unsigned int GAMEMACHINE_BPS = 30;
		static unsigned int machineCount; ///< to count how many timers we installed...

		// FPS COUNTER
		int fps_counted;  // we manage this internally , but can be used to display FPS!
		
		BITMAP *doubleBuffer; ///< double buffer to draw without flicker, managed internally, DRAW HERE
};

#endif

