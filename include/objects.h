#ifndef OBJECTS_H

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

//#include "terrain.h"
#include "utility.h"
#include "quat.h"
#include "camera.h"
#include "model.h"
#include "globals.h"

class OBJECTMODEL
{
public:
	JOEMODEL jmodel;
	string name;
	OBJECTMODEL * next;
};

class OBJECTNODE
{
public:
	VERTEX pos;
	OBJECTNODE * next;
	QUATERNION dir;
	OBJECTMODEL * model;

	OBJECTNODE();
};

class OBJECTS
{
private:
	ofstream error_log;
	OBJECTMODEL * model_list;
	void delmodel();

	OBJECTNODE * object_list;
	void delobject();

	void DrawObject(OBJECTNODE * object);

	OBJECTMODEL * AddModel(string modelname);

	string path;
	
public:
	OBJECTS();
	~OBJECTS();
	void Draw();
	void Add(VERTEX pos, float rotation, string modelname);
	void DeleteAll();

	void LoadObjectsFromFolder(string objectpath);
};

#define OBJECTS_H
#endif
