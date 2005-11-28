/***************************************************************************
 *            utility.h
 *
 *  Sat Mar 26 08:48:31 2005
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
 
//#define GL_GLEXT_PROTOTYPES
 
#ifndef _UTILITY_H

#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <math.h>
#include <string>
#include <fstream>
#include <iostream>

#include "quat.h"
#include "globals.h"

using namespace std;

#define ENDOFFILESTRING "!!!END OF FILE!!!"

#define EPSILON 0.000001
#define CROSS(dest,v1,v2) \
          dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
          dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
          dest[2]=v1[0]*v2[1]-v1[1]*v2[0];
#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])
#define SUB(dest,v1,v2) \
          dest[0]=v1[0]-v2[0]; \
          dest[1]=v1[1]-v2[1]; \
          dest[2]=v1[2]-v2[2]; 

class UTILITY
{
private:
	ofstream error_log;
	GLint nb_multitexture;
	bool initdone;
	void initerror();
	
public:
	UTILITY();
	~UTILITY();
	void Init();
	GLuint TexLoad(string texfile, bool mipmap);
	GLuint TexLoad(string texfile, int format, bool mipmap);
	GLuint TexLoad(string texfile, int format, bool mipmap, int &w, int &h, const bool supresserror, bool &err);
	GLuint TexLoad(string texfile, int format, bool mipmap, int &w, int &h, const bool supresserror, bool &err, int attempt);
	void SelectTU(int TU);
	void Tex2D(int TU, bool enable);
	void TexCoord2d2f(int TU, float u, float v);
	string sGetLine(ifstream &ffrom);
	string sGetParam(ifstream &ffrom);
	int iGetParam(ifstream &ffrom);
	float fGetParam(ifstream &ffrom);
	bool bGetParam(ifstream &ffrom);
	int numTUs();
	void Draw2D(float x1, float y1, float x2, float y2, GLuint texid);
	void Draw2D(float x1, float y1, float x2, float y2, GLuint texid, float rotation);
	string GetEOFString() {return ENDOFFILESTRING;}
	float GetValue(SDL_Surface * surf, int channel, float x, float y, bool interpolate);
	float GetValue(SDL_Surface * surf, int channel, float x, float y, bool interpolate, bool wrap);
	bool FileExists(string filename);
	int IntersectTriangleD(double orig[3], double dir[3],
                   double vert0[3], double vert1[3], double vert2[3],
                   double *t, double *u, double *v);
	int IntersectTriangleF(float orig[3], float dir[3],
                   float vert0[3], float vert1[3], float vert2[3],
                   float *t, float *u, float *v);
	float randf(float min, float max);
	void seedrandom(int seed) {srand(seed);}
};

#define _UTILITY_H
#endif /* _UTILITY_H */
