# Zero Zero UFO

By Kronoman

Under the MIT license

Copyright (c) Kronoman, 2022

## Play online 

**EXPERIMENTAL FEATURE**

https://alvarogonzalezferrer.github.io/zerozeroufo/

## About the Game

A open source game that you play as a UFO 

Abduct cows to regain energy, fight the army that will try impede your mission.

## How to Play

A,S,D,W or arrow keys : move the UFO

Z or SPACE - use your beam to abduct cows and solders
X or ENTER - shoot in the direction you're moving

F1 to see help screen
F2 to configure keyboard / mouse / joystick
F10 or ESC to quit the game
SPACE to skip intro screen, etc

The command line takes some switches.
Try -? to see the commands.

## Tools and Libraries Used

Tools I used:

* DJGPP C++ compiler
* Allegro 4.2
* Notepad++
* ASEprite - sprite editor
* Audacity 
* Orangator
* The GIMP
* Irfanview
* DOSBOX
* Volkov Commander
* FAR Manager 

Platform: MS-DOS was main target, Linux, Windows maybe, not tested yet.

## How to Compile

Note, this game was developed for a retro game competition, I only tested the code with MS DOS under DOSBOX so far.
All other platforms remain untested, should compile with some massage.

### MS-DOS / DOSBOX

DJGPP + Allegro 4.2 under DOSBOX host is what I used
You need to run 'make'

### Windows

You need Allegro 4.2 library, I suggest precompiled one!
Run 'make'

https://sourceforge.net/projects/alleg/files/allegro-bin/

And the Mingw32 + MSYS toolchain and compiler

http://www.mingw.org/

### Linux

You need Allegro 4.2 dev package and g++ compiler. Run 'make'


### Game data

Sprites drawn by me with ASE sprite using the standard VGA 13h palette.

Sound by me using Audacity.

The game data is compiled with the GRABBER tool that comes with Allegro 4.2

The sprites for the enemies are handled by EnemyList, kept loaded, only bosses are loaded on-demand to keep RAM usage low.

Level backgrounds are loaded on demand too, to keep RAM usage low.

The terrain is procedurally generated randomly, so no two games are the same.

## Engine game design considerations

* Most classes has everything public, this is to keep the design simple and fast, since Im a single developer working on my own project, so no get/set pairs for everything. 

* On major projects I would use the proper encapsulation, dont take this as my skill level programming, in fact developing for MS DOS in 2022 takes some skills!

* I'm pretty tired of recruiters wasting my time, they don't know anything about programming and they judge without knowing. 

* Don't take my design decisions for MS DOS like you would for any other project, this is a niche where very specific decisions have to be made because of platform and hardware limitations.

* Most hacks or weird designs you will see in the code are because of MS DOS nature, like early games, I use 320x200x8 bpp video mode (13h)

* I almost didnt used linked lists or STL, just arrays, to keep it going fast, dynamic memory allocation in MS DOS is slow.

* I dont use transparent effects, for speed reasons (you have to read and write instead of only writing the pixel).

* Floating point math is very slow on real hardware of the time (386, etc), although it used a lot of float, I should change it to fixed math, I didn't have time to do it, maybe in the next version. On an emulator it works fine.

* I couldn't test it on real hardware either, since I left my 486 9000 km from where I am now, if anyone tries it on real hardware and let me know, welcome.

* **All patches and fixes are welcome. Also the comments, I love receiving comments!**

* Data will be automatically loaded - unloaded when needed to keep RAM usage to a minimum

## REUSED CODE & STUFF

I reused a Arcade Game Framework that I'm making since years ago (2007?).

No third party game engines used, all the engine is written by me.


