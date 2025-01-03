#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include <raylib.h>
#include <string>

struct Instrument
{
	Sound sound;
	std::string icon;
	Rectangle row;
	Color row_clr;
};

#endif