/***************************************************************************
 *            timer.h
 *
 *  Wed Jun  1 21:31:59 2005
 *  Copyright  2005  Joe Venzon
 *  joe@venzon.net
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#ifndef _TIMER_H

#include <stdio.h>			// Header File For Standard Input/Output
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <cassert>

using namespace std;

#include "font.h"
#include "utility.h"
#include "gamestate.h"
#include "settings.h"
#include "globals.h"

#define STAGING_TIME 4.0

class TIMER
{
private:
	double time;
	double pretime;
	double lastlap;
	double bestlap;
	void SecsToSecsMin(double t, float &secs, int &min);
	GLuint timerbox;

	bool loaded;

public:
	TIMER();
	~TIMER();
	void Load();
	void Reset();
	void Lap();
	float GetTimer();
	void TickTimer(float timefactor, float fps);
	void ResetBest();

	void Draw();
};

#define _TIMER_H
#endif /* _TIMER_H */
