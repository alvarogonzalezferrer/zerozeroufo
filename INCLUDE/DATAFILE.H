// ---------------------------------------------------------------------------
// datafile.h
// ---------------------------------------------------------------------------
// This has data loading helpers, for loading Allegro datafiles and such.
// Has also a class for holding a datafile, with automatic error handling and all.
// ---------------------------------------------------------------------------
// By Kronoman. Highway Star.
// Copyright (c) 2005-2007, Kronoman
// In loving memory of my father.
// ---------------------------------------------------------------------------

#ifndef DATAFILE_H_FRAMEWORK
#define DATAFILE_H_FRAMEWORK

#include <allegro.h>

#include <string>

// ---------------------------------------------------------------------------
// data container class
// ---------------------------------------------------------------------------
class Datafile
{
	public:
		Datafile();
		
		~Datafile();
		
		void load(const char *filename); ///< load a datafile 
		
		void unload(); ///< unload current datafile, is safe to call even if the file was already unloaded
		
		DATAFILE *getObject(const char *name); ///< get object by name
		
		DATAFILE *getObject(const int index); ///< get object by index 0..size-1
		
		std::string getFileName(); ///< loaded file name
		
		DATAFILE *getDatafile(); ///< get internal datafile (dangerous to modify outside!)
		
		int getSize(); ///< get number of loaded objects
		
	private:
		DATAFILE *data;
		int size; // number of loaded objects
		std::string fileName;
};

// ---------------------------------------------------------------------------
// data loading functions
// ---------------------------------------------------------------------------
DATAFILE *loadDatafile(const char *filename); ///< loads a datafile while showing a progress animation on screen.
DATAFILE *findDatafileObject(AL_CONST DATAFILE *dat, AL_CONST char *objectname); ///< searchs for a object inside a datafile, shows animation, ignores upper/lower case diferences in the object name

// ---------------------------------------------------------------------------
// misc, animations for loading progress...
// ---------------------------------------------------------------------------
void BaseAnimationForLoad(); ///< this draws the basic base graph for a progress animation
void UpdateAnimationForLoad(); ///< call this to update the animation on screen of loading (call once for each update)

#endif
