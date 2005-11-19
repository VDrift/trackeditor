#ifndef _BACKDROP_H

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

#include "quat.h"
#include "camera.h"
#include "weather.h"
#include "utility.h"
#include "settings.h"

#include "globals.h"

//this should be the same as the #define in terrain.h
//#define EARTH_RADIUS 50000.0
#define EARTH_RADIUS 637800.0

class BACKDROP
{
private:
	GLuint sun_texture;
	GLuint sun_texture_ref;
	GLuint sky_texture;
	GLuint rain_texture;
	GLuint cloud_texture;
	GLuint moon_texture;
	GLuint stars_texture;
	SDL_Surface * sky_tex_img;
	ofstream error_log;
	bool volumetricfog;
	float fogdensity;
	float daytime_cache;
	float windx,windz;
	int cloudthresh;

	void DrawDrop(VERTEX pos, float rndperm, float yscale, float zheight);
	bool initdone;

	string TEXTURE_PATH;

public:
	BACKDROP();
	~BACKDROP();
	void Init();
	void Draw(VERTEX tersize);
	void DrawSky(double daytime, float transparency);
	void DrawSkyGradient(double daytime, float transparency, GLdouble *temp_matrix);
	void DrawSun(double daytime, float transparency, GLdouble *temp_matrix);
	void DrawMoon(double daytime, float transparency, GLdouble *temp_matrix);
	void DrawClouds(double daytime, float transparency, GLdouble *temp_matrix);
	void DrawStars(double daytime, float transparency, GLdouble *temp_matrix);
	void SetFogStrength(float density);
	void RefreshFog();
	void GetHorizonColor(float * color3);
	void GetSkyColor(float * color3);
	void SetCloudCover(int newcloudthresh);
	void SetWindoffset(float windoffset_x, float windoffset_z);
	void DrawRain(float day_time);

	float sunpos_lng;
	float sunpos_lat;

	float moonpos_lng;
	float moonpos_lat;
	
	void DeInit();
};

#define _BACKDROP_H
#endif
