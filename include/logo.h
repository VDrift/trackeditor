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
 
#ifndef _LOGO_H

#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <math.h>
#include <fstream>
#include <iostream>
#include <string>

#include "utility.h"
#include "settings.h"
#include "globals.h"

using namespace std;

#define MAX_LOGOS 10
#define SECS_PER_LOGO 3.0f
#define LOGO_FADE_SECS 0.5f
#define FADE_OUT true
#define LOGO_DIM 512 //width and height of logo image

class LOGO
{
private:
	int num_logos;
	string logo_fn[MAX_LOGOS];
	GLuint logo_tex[MAX_LOGOS];
	float ltime;
	void Cleanup();
	
public:
	LOGO();
	void AddLogo(string filename);
	bool IncrementTime(float fps);
	void Draw();

	void run();
};

#define _LOGO_H
#endif /* _LOGO_H */
