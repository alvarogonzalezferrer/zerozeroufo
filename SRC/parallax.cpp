// parallax scrolling background 

#include "parallax.h"
#include "map.h" // to know map types to return!

Parallax::Parallax()
{
	reset();
}

Parallax::~Parallax()
{
	unload(); 
}

// will load a background and return the appropiate terrain map type
// for example Map::SNOW, etc 
int Parallax::load(int wave)
{
	unload();
	
	int bg = wave%6; // select according to wave
	
	int ret = 0; // default terrain
	
	switch (bg)
	{
		case 0:
			backgrounds_data.load("citybg.dat"); 
			ret = Map::ROCK;
		break;
		case 1:
			backgrounds_data.load("sunsetbg.dat");
			ret = Map::GRASS;
		break;
		case 2:
			backgrounds_data.load("beachbg.dat");
			ret = Map::SAND;
		break;
		case 3: 
			backgrounds_data.load("nightbg.dat");
			ret = Map::SNOW;
		break;
		case 4:
			backgrounds_data.load("islandbg.dat");
			ret = Map::GRASS;
		break;
		case 5:
			backgrounds_data.load("desertbg.dat");
			ret = Map::DARKSAND;
		break;
		
		default:
			backgrounds_data.load("sunsetbg.dat"); // DEBUG SELECT ACORDING TO WAVE
			ret = Map::GRASS;
		break;
	}
	
	
	
	// load backgrounds
	
	// they are always called like this in each data file
	background_bg = (BITMAP *)backgrounds_data.getObject("BG_BG")->dat;
	background_mg = (BITMAP *)backgrounds_data.getObject("BG_MG")->dat;
	background_fg = (BITMAP *)backgrounds_data.getObject("BG_FG")->dat;

	return ret;
}

void Parallax::reset()
{
	background_bg=background_mg=background_fg=NULL;
	mg_scroll=fg_scroll=0;
}

void Parallax::unload()
{
	backgrounds_data.unload(); // is safe to call even if it is unloaded already
	reset();
}



void Parallax::update()
{
	// scroll background
	mg_scroll -= 0.05; // scroll background 
	fg_scroll -= 0.2;  // scroll background
	
	if (mg_scroll < -background_mg->w)
		mg_scroll = 0;

	if (fg_scroll < -background_fg->w)
		fg_scroll = 0;
	
}

void Parallax::render(BITMAP *bmp)
{
			// backgroud fixed
		blit(background_bg, bmp, 0, 0, 0, 0, background_bg->w, background_bg->h);
		
		// parallax scroll , I use two copies moving side by side 
		
		// bg background (slow)
		masked_blit(background_mg, bmp, 0, 0, (int)mg_scroll, bmp->h - background_mg->h, background_mg->w, background_mg->h);
		masked_blit(background_mg, bmp, 0, 0, (int)mg_scroll + background_mg->w, bmp->h - background_mg->h, background_mg->w, background_mg->h);
		
		// fg background (fast)
		masked_blit(background_fg, bmp, 0, 0, (int)fg_scroll, bmp->h - background_fg->h, background_fg->w, background_fg->h);
		masked_blit(background_fg, bmp, 0, 0, (int)fg_scroll + background_fg->w, bmp->h - background_fg->h, background_fg->w, background_fg->h);

	
}