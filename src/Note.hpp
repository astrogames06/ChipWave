#ifndef NOTE_H
#define NOTE_H

#include <raylib.h>

struct Note
{
	Sound sound;
	Rectangle rec;
	Color color;
	bool played = false;
};

#endif