#ifndef _WEATHER_H

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

#include "globals.h"

//once dewpoint gets this close to the temp, clouds begin to form.
#define CLOUD_FORMATION 20.0f

class WEATHERVARIABLE
{
public:
	float value;
	float next_value;
	float duration;
	double start_time;
	float night_dip;
	
	void Update(double abs_time);
	void Init(float minval, float maxval, float mindur, float maxdur);
	void Init(float minval, float maxval, float mindur, float maxdur, float newnightdip);
	void CalcNew(double abs_time);
	float GetVal(double abs_time);

	float valmax, valmin, durmax, durmin;
};

class WEATHERVERTEX
{
public:
	WEATHERVARIABLE components[3];
	
	void Update(double abs_time);
	void Init(float minval, float maxval, float mindur, float maxdur);
	void CalcNew(double abs_time);
	VERTEX GetVal(double abs_time);
};

class WEATHER
{
private:
	double abs_time;
	
	WEATHERVARIABLE temperature;
	WEATHERVARIABLE dewpoint;

	WEATHERVERTEX globalwind_lowfreq;
	WEATHERVERTEX globalwind_highfreq;

	//this will be added later, once it's important
	//WEATHERVERTEX localwind[9];

public:
	void Init();
	void Update(double cur_time);
	
	int GetCloudCover();
	int GetCloudFuzziness();
	VERTEX GetGlobalWind();
	int GetRainDrops();
	float GetFogDensity();
};

#define _WEATHER_H
#endif
