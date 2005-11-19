/***************************************************************************
 *            font.h
 *
 *  Sun Sep 26 13:33:51 2004
 *  Copyright  2004  Joe Venzon
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
 
#ifndef _FONT_H

#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <string>
#include <iostream>
#include <fstream>

#include "utility.h"
#include "settings.h"
#include "globals.h"

using namespace std;

class FONT
{
private:
	GLuint base;
	GLuint texture;

	ofstream error_log;

	int spacing[2][128];

	bool loaded;
	
public:
	FONT();
	~FONT();
	void Load();
	void Print( float px, float py, const char *string, int set, int size, float opacity );
	void Print( float px, float py, const char *string, int set, int size, float r, float g, float b );
	void Print( float px, float py, const char *string, int set, int size, float r, float g, float b, float trans );
};

#define _FONT_H
#endif /* _FONT_H */
