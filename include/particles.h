/***************************************************************************
 *            particle.h
 *
 *  Sun Apr  3 10:56:03 2005
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
 
#ifndef _PARTICLE_H

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

#include "quat.h"
#include "utility.h"
#include "globals.h"

#define MAX_PARTICLES 100
#define PARTICLE_TEXTURES 6
#define NUM_ROTATIONS 5

struct SINGLE_PARTICLE
{
	float transparency;
	float longevity;
	VERTEX start_position;
	float speed;
	VERTEX direction;
	float rotation;
	float size;
	int texture;
	
	float timestamp;
	bool active;
};

class PARTICLE
{
public:
	PARTICLE();
	~PARTICLE();
	void Load();
	void Update(float timefactor, float fps);
	void Draw();
	void SetParams(float transmin, float transmax, float longmin, float longmax,
					float speedmin, float speedmax, VERTEX direction,
					float sizemin, float sizemax);
	void AddParticle(VERTEX pos);
	void ProbAddParticle(VERTEX pos, float probability);
	float ParticleHeight();
	void ClipParticles(VERTEX * rect); //stop particles from drifting up into the car's
										//undercarriage
	void Clear();

	void SetWind(VERTEX newwind) {wind = newwind;}
	
private:
	int slotguess;
	GLuint drawlist;
	GLuint texture[PARTICLE_TEXTURES];

	VERTEX wind;

	double curtime;

	float pheight;

	bool inrange(float x1, float x2, float val);
	bool inrect (VERTEX * rect, VERTEX p);
	bool overlap (VERTEX * rect1, VERTEX * rect2);

	struct SINGLE_PARTICLE particle[MAX_PARTICLES];
	struct SINGLE_PARTICLE params[2];
	
	int curtex;
	
	float randf(float min, float max);
	void DrawParticle(struct SINGLE_PARTICLE & p);
	void init_draw();
	
	bool loaded;
};

#define _PARTICLE_H
#endif /* _PARTICLE_H */
