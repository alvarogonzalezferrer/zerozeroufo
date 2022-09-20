// ---------------------------------------------------------------------------
// datafile.cpp
// ---------------------------------------------------------------------------
// This has data loading helpers, for loading Allegro datafiles and such.
// ---------------------------------------------------------------------------
// By Kronoman.
// Copyright (c) 2005-2007, Kronoman
// In loving memory of my father.
// ---------------------------------------------------------------------------

#include "datafile.h"
#include "error.h"

#include <allegro.h>

//static int r = 24; // radius for graphic DISABLED NO GRAPHICS FOR MS DOS SPEED REASONS

// ---------------------------------------------------------------------------
// helper drawing routine for feedback to user
// ---------------------------------------------------------------------------
static void datafileLoadCallback(DATAFILE *d)
{
	// disabled for speed reasons
	/*int x, y;
	static int a = 0;
	static int c = 0; // color & angle : 0 .. 255


	c += rand()%16;
	if (c > 255)
		c = 0;
	a += rand()%16;
	if (a > 255)
		a = 0;

	x = fixtoi(fixmul(fixcos(itofix(a)), itofix(r-2)));
	y = fixtoi(fixmul(fixsin(itofix(a)), itofix(r-2)));
	//line(screen, SCREEN_W/2, SCREEN_H/2, (SCREEN_W/2) + x, (SCREEN_H/2) + y, makecol(255,c,0));
	circlefill(screen, (SCREEN_W/2) + x, (SCREEN_H/2) + y, 2, makecol(255,c,0));*/
	
	// new faster routine
	static const char indicator[] = "-\\|/-.oOXOo.";
    static int current = 0;
          
	// Show a different character every time.
	// magic center numbers based on allegro 8x8 basic font
	textprintf_ex(screen, font, SCREEN_W/2 - 50, SCREEN_H/2 - 4, makecol(0, 0, 0),
				   makecol(255, 255, 255), "%c Loading %c",
				   indicator[current], indicator[current]);
	
	// Increase index and check if we need to reset it.
	current++;
	if (!indicator[current])
		current = 0;
}

// ---------------------------------------------------------------------------
// base graph for progress animation
// ---------------------------------------------------------------------------
void BaseAnimationForLoad()
{
	// base graph
	//circlefill(screen, SCREEN_W/2, SCREEN_H/2, r, makecol(140,0,0));

	//textout_centre_ex(screen, font, "Loading", SCREEN_W/2, SCREEN_H/2- text_height(font)/2, makecol(255,255,255), -1);
	datafileLoadCallback(NULL);
}

// ---------------------------------------------------------------------------
// animate the progress animation
// ---------------------------------------------------------------------------
void UpdateAnimationForLoad()
{
	datafileLoadCallback(NULL); // disabled for speed
}

// ---------------------------------------------------------------------------
// loads a datafile while showing a progress animation on screen.
// if a error occurs, WILL RETURN NULL (will NOT fail)
// ---------------------------------------------------------------------------
DATAFILE *loadDatafile(const char *filename)
{
	// this are "last resource paths", the last places to check before giving up.
	// this is good to keep compatibility with many systems, such as Windows, DOS, Linux
	// DEBUG : if we add more install paths	on new releases, add them on this list.

	// END IT WITH THE LAST ITEM = NULL!!
	const char *extra_paths[] =
	    {
	        "C:\\windows\\",
	        "C:\\",
	        "/usr/share/",
	        "/usr/lib/",
	        "/usr/local/share/",
	        NULL
	    };

	// search for file in many places
	char buffer[PATH_MAX]; // we will open this file name

	usprintf(buffer, "%s", filename); // copy to buffer

	if (!exists(filename))
	{
		char str[PATH_MAX], str2[PATH_MAX]; // buffer for path making

		// check in current executable path
		get_executable_name(str, PATH_MAX);
		replace_filename(str2, str, filename, PATH_MAX);

		if (exists(str2))
		{
			usprintf(buffer, "%s", str2);
		}
		else
		{
			get_executable_name(str2, PATH_MAX);
			replace_filename(str, str2, "", PATH_MAX);

			if (find_allegro_resource(str2, filename, get_extension(filename), NULL, NULL, NULL, str, PATH_MAX) == 0)
			{
				usprintf(buffer, "%s", str2);
			}
			else
			{
				// search in last resource paths... :(
				for (int i = 0 ; extra_paths[i] != NULL ; i++)
				{
					usprintf(str, "%s\%s", extra_paths[i], filename);
					fix_filename_slashes(str);
					if ( exists(str) )
					{
						usprintf(buffer, "%s", str);
						break;
					}
				}
			}
		}
	}


	// load it , and animate it
	BaseAnimationForLoad();

	fix_filename_slashes(buffer);
	fix_filename_case(buffer);

	return load_datafile_callback(buffer, datafileLoadCallback);
}


// ---------------------------------------------------------------------------
// searchs for a object inside a datafile while showing progress animation on screen
// will IGNORE uppercase/lowercase distinction between names
// ---------------------------------------------------------------------------
DATAFILE *findDatafileObject(AL_CONST DATAFILE *dat, AL_CONST char *objectname)
{
	char name[512];
	int recurse = FALSE;
	int pos = 0;
	int c;

	BaseAnimationForLoad(); // animation

	while ((c = ugetxc(&objectname)) != 0)
	{
		if ((c == '#') || (c == '/') || (c == OTHER_PATH_SEPARATOR))
		{
			recurse = TRUE;
			break;
		}

		pos += usetc(name + pos, c);

		UpdateAnimationForLoad(); // animation
	}

	usetc(name + pos, 0);


	for (pos = 0; dat[pos].type != DAT_END; pos++)
	{
		if (ustricmp(name, get_datafile_property(dat + pos, DAT_NAME)) == 0)
		{
			if (recurse)
			{
				// we have a nested datafile, check inside it
				if (dat[pos].type == DAT_FILE)
					return findDatafileObject((DATAFILE *)dat[pos].dat, objectname);
				else
					return NULL; // not found :(
			}
			else
				return (DATAFILE*)dat + pos; // found it!
		}

		UpdateAnimationForLoad(); // animation
	}

	// not found :(
	return NULL;
}






// =======================================================================
// =======================================================================
// Datafile class
// =======================================================================
// =======================================================================

Datafile::Datafile()
{
	data = NULL;
	fileName = "";
	size = 0;
}

Datafile::~Datafile()
{
	unload();
}

void Datafile::load(const char *filename)
{
	unload(); // unload first if anything is loaded

	data = loadDatafile(filename);

	if (data == NULL)
		Error::error("ERROR\nCan't load %s!\nError at Datafile::load() ; %s in line %d\n", filename, __FILE__, __LINE__ );

	fileName = filename;

	// cache size
	for (size= 0; data[size].type != DAT_END; size++)
		;
}

void Datafile::unload()
{
	if (data != NULL)
	{
		unload_datafile(data);
		data = NULL;
	}
	fileName = "";
	size = 0;
}


DATAFILE *Datafile::getObject(const char *name)
{
	DATAFILE *ret = findDatafileObject(data,name);

	if (ret == NULL)
		Error::error("ERROR\nCan't load object %s in %s!\nError at Datafile::getObject() ; %s in line %d\n",name, fileName.c_str(), __FILE__, __LINE__ );

	return ret;
}

DATAFILE *Datafile::getObject(const int index)
{
	if (index < 0 || index > getSize()-1)
		Error::error("ERROR\nObject[%d] is out of bounds 0..%d in %s!\nError at Datafile::getObject() ; %s in line %d\n",index, getSize()-1, fileName.c_str(), __FILE__, __LINE__ );

	return (DATAFILE*)data + index;
}


std::string Datafile::getFileName()
{
	return fileName;
}


DATAFILE *Datafile::getDatafile()
{
	return data;
}

int Datafile::getSize()
{
	return size;
}
