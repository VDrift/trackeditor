/***************************************************************************
 *            particle.cpp
 *
 *  Sun Apr  3 11:02:01 2005
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
 
#include "particles.h"

PARTICLE::PARTICLE()
{
	drawlist = 0;
	int i;
	for (i = 0; i < PARTICLE_TEXTURES; i++)
		texture[i] = 0;
	slotguess = 0;
	
	VERTEX direction;
	direction.y = 1.0;
	
	params[0].transparency = 1.0;
	params[0].longevity = 2.0;
	params[0].speed = 0.5;
	params[0].direction = direction;
	params[0].size = 1.0;
	
	params[1].transparency = 1.0;
	params[1].longevity = 2.0;
	params[1].speed = 0.5;
	params[1].direction = direction;
	params[1].size = 1.0;
	
	srand(1337);
	
	curtime = 0;
	curtex = 0;
	
	loaded = false;
}

extern bool verbose_output;
PARTICLE::~PARTICLE()
{
	if (verbose_output)
		cout << "particle deinit" << endl;
	
	if (loaded)
	{
		glDeleteTextures( PARTICLE_TEXTURES, texture );
		glDeleteLists(drawlist, 1);
		drawlist = 0;
	}
}

void PARTICLE::Load()
{
	int i;
	
	if (loaded)
	{
		for (i = 0; i < PARTICLE_TEXTURES; i++)
		{
			glDeleteTextures( PARTICLE_TEXTURES, &(texture[i]) );
		}
		glDeleteLists(drawlist, 1);
		drawlist = 0;
	}
	
	for (i = 0; i < MAX_PARTICLES; i++)
	{
		particle[i].active = false;
	}
	
	//load textures
	char fname[64];
	for (i = 0; i < PARTICLE_TEXTURES; i++)
	{
		sprintf(fname, "smoke/particle%i.png", i);
		if (!texture[i])
			texture[i] = utility.TexLoad(fname, GL_RGBA, false);
	}
	
	//create display list
	if (!drawlist)
	{
		drawlist = glGenLists(1);
		glNewList (drawlist, GL_COMPILE);
		
		for (i = 0; i <= NUM_ROTATIONS; i++)
		{
			float thisrot = 90.0 / (float) NUM_ROTATIONS;
			thisrot *= i;
			
			glPushMatrix();
			glRotatef(thisrot,0,1,0);
			//glRotatef(p.rotation,0,0,1);
			init_draw();
			glPopMatrix();
			
			//cout << thisrot << " ";
			
			/*glPushMatrix();
			glRotatef(90,1,0,0);
			glRotatef(p.rotation,0,0,1);
			glCallList(drawlist);
			glPopMatrix();*/
		}
		
		glEndList ();
	}
	
	curtime = 0;
	
	loaded = true;
}

void PARTICLE::init_draw()
{
	float x1 = .5;
	float x2 = -.5;
	float y1 = .5;
	float y2 = -.5;
	
	pheight = y1 - y2;
	if (pheight < 0)
		pheight = -pheight;
	
	glBegin(GL_QUADS);
		glTexCoord2d(0,0);
		glVertex3f(x1, y1, 0);
	
		glTexCoord2d(0,1);
		glVertex3f(x1, y2, 0);
	
		glTexCoord2d(1,1);
		glVertex3f(x2, y2, 0);
	
		glTexCoord2d(1,0);
		glVertex3f(x2, y1, 0);
	glEnd();
}

void PARTICLE::SetParams(float transmin, float transmax, float longmin, float longmax,
					float speedmin, float speedmax, VERTEX direction, float sizemin,
					float sizemax)
{
	params[0].transparency = transmin;
	params[0].longevity = longmin;
	params[0].speed = speedmin;
	params[0].direction = direction;
	params[0].size = sizemin;
	
	params[1].transparency = transmax;
	params[1].longevity = longmax;
	params[1].speed = speedmax;
	params[1].direction = direction;
	params[1].size = sizemax;
}

void PARTICLE::AddParticle(VERTEX pos)
{
	int idx = slotguess;
	int i;
	for (i = 0; i < MAX_PARTICLES; i++)
	{
		if (!particle[idx].active)
		{
			particle[idx].active = true;
			particle[idx].start_position = pos;
			VERTEX noise;
			float noisemag = 0.1;
			noise.x = randf(-noisemag, noisemag);
			noise.y = randf(-noisemag, noisemag);
			noise.z = randf(-noisemag, noisemag);
			particle[idx].direction = (params[0].direction + noise + wind).normalize();
			particle[idx].timestamp = curtime;
			
			particle[idx].speed = randf(params[0].speed, params[1].speed);
			particle[idx].transparency = randf(params[0].transparency, params[1].transparency);
			particle[idx].longevity = randf(params[0].longevity, params[1].longevity);
			particle[idx].size = randf(params[0].size, params[1].size);
			
			particle[idx].rotation = randf(0,360);
			particle[idx].texture = curtex;
			curtex++;
			if (curtex >= PARTICLE_TEXTURES)
				curtex -= PARTICLE_TEXTURES;
			
			slotguess = idx+1;
			if (slotguess >= MAX_PARTICLES)
				slotguess = 0;
			
			i = MAX_PARTICLES; //break
		}
		else
		{
			idx++;
			if (idx >= MAX_PARTICLES)
				idx -= MAX_PARTICLES;
		}
	}
}

float PARTICLE::randf(float min, float max)
{
	if (min > max)
	{
		float temp = min;
		min = max;
		max = temp;
	}
	
	float r = (float) rand() / (float) RAND_MAX;
//float r = 0;
	r *= max-min;
	r += min;
	return r;
}

void PARTICLE::Update(float timefactor, float fps)
{
	curtime += timefactor/fps;
}

void PARTICLE::Draw()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(0);
	int i;
	for (i = 0; i < MAX_PARTICLES; i++)
	{	
		if (particle[i].active)
		{
			if (curtime - particle[i].timestamp > particle[i].longevity)
				particle[i].active = false;
			else
				DrawParticle(particle[i]);
		}
	}
	glDepthMask(1);
	glPopAttrib();
}

void PARTICLE::DrawParticle(struct SINGLE_PARTICLE & p)
{
	VERTEX drawpos;
	drawpos = p.start_position;
	float age = curtime - p.timestamp;
	VERTEX offset = p.direction.ScaleR(age*p.speed);
	drawpos = drawpos + offset;
	
	glPushMatrix();
	glTranslatef(drawpos.x, drawpos.y, drawpos.z);
	
	float trans = 0.5;
	float sizescale = 1.0;
	if (p.longevity > 0)
	{
		trans = p.transparency*pow((double)(1.0f-age/p.longevity),4.0)*1.0f/((float)NUM_ROTATIONS+1.0f);
		float transmax = 0.5/(NUM_ROTATIONS+1.0f);
		if (trans > transmax)
			trans = transmax;
		if (trans < 0.0)
			trans = 0.0;
		
		sizescale = 15.0*(age/p.longevity)+1.0;
	}
	glColor4f(1,1,1,trans);
	glBindTexture(GL_TEXTURE_2D, texture[p.texture]);
	
	glScalef(p.size*sizescale, p.size*sizescale, p.size*sizescale);
	
	//glRotatef(p.rotation,0,0,1);
	
	glCallList(drawlist);
	
	glPopMatrix();
}

void PARTICLE::ProbAddParticle(VERTEX pos, float probability)
{
	float roll = randf(0,1);
	if (probability > roll)
	{
		AddParticle(pos);
	}
}

float PARTICLE::ParticleHeight()
{
	return pheight;
}

void PARTICLE::ClipParticles(VERTEX * rect)
{
	//return;
	
	float height = 0;
	int j;
	for (j = 0; j < 4; j++)
	{
		height += rect[j].y;
	}
	height /= 4.0f;
	
	height += 0.2f;
	
	int i;
	for (i = 0; i < MAX_PARTICLES; i++)
	{	
		if (particle[i].active)
		{
			VERTEX drawpos;
			drawpos = particle[i].start_position;
			float age = curtime - particle[i].timestamp;
			VERTEX offset = particle[i].direction.ScaleR(age*particle[i].speed);
			drawpos = drawpos + offset;
			
			float dimension = 0.5f;
			VERTEX dprect[4];
			dprect[0] = drawpos;
			dprect[0].x -= dimension;
			dprect[0].z -= dimension;
			dprect[1] = drawpos;
			dprect[1].x -= dimension;
			dprect[1].z += dimension;
			dprect[2] = drawpos;
			dprect[2].x += dimension;
			dprect[2].z += dimension;
			dprect[3] = drawpos;
			dprect[3].x += dimension;
			dprect[3].z -= dimension;
			
			//if (inrect(rect, drawpos))// && drawpos.y > height)
			if (overlap(rect, dprect) && drawpos.y > height)
			{
				if (drawpos.y - height <= 0.2)
				{
					float age = (curtime - particle[i].timestamp);
					particle[i].start_position = drawpos;
					particle[i].start_position.y = height;
					particle[i].longevity = particle[i].longevity - age;
					particle[i].timestamp = curtime;
					particle[i].transparency = particle[i].transparency*(1.0f-age/particle[i].longevity)*1.0f/((float)NUM_ROTATIONS+1.0f);
				}
				else
					particle[i].active = false;
			}
		}
	}
}

bool PARTICLE::inrect (VERTEX * rect, VERTEX p)
{
	int i;
	
	int positive = 0;
	
	float x0, x1, x2, y0, y1, y2;
	
	for (i = 0; i < 4; i++)
	{
		x2 = p.x;
		y2 = p.z;
		
		x0 = rect[i].x;
		y0 = rect[i].z;
		
		int nexti = i+1;
		if (nexti >= 4)
			nexti = 0;
		x1 = rect[nexti].x;
		y1 = rect[nexti].z;
		
		if ((.5)*(x1*y2 - y1*x2 -x0*y2 + y0*x2 + x0*y1 - y0*x1) < 0)
			positive ++;
	}
	
	if (positive == 4)
		return true;
	
	//cout << positive << endl;
	
	return false;
}

bool PARTICLE::inrange(float x1, float x2, float val)
{
	if (x1 >= x2 && val >= x2 && val <= x1)
		return true;
	if (x2 > x1 && val <= x2 && val >= x1)
		return true;
	
	return false;
}

bool PARTICLE::overlap (VERTEX * rect1, VERTEX * rect2)
{
	return (inrect(rect1, rect2[0]) || inrect(rect1, rect2[1]) 
			|| inrect(rect1, rect2[2]) || inrect(rect1, rect2[3]));
}

void PARTICLE::Clear()
{
	int i;
	for (i = 0; i < MAX_PARTICLES; i++)
	{
		particle[i].active = false;
	}
}
