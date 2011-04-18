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

#include <list>

//#include "terrain.h"
#include "utility.h"
#include "quat.h"
#include "camera.h"
#include "model.h"
#include "globals.h"
#include "joepack.h"

#include <map>

#define SELECT_DISTANCE 1.0
#define MAX_SELECTION_DIST 1000.0

#define SELECT_AUTO_DISTANCE 100.0
#define CLOSENESS_AUTO_BIAS 0.0
#define MAX_AUTO_SELECTION_DIST 500.0

class OBJECTMODEL
{
public:
	JOEMODEL jmodel;
	string name;
	OBJECTMODEL * next;
	bool fullbright;
	bool skybox;
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
	JOEPACK jpk;
	bool loadjpk;
	OBJECTMODEL * model_list;
	void delmodel();

	OBJECTNODE * object_list;
	void delobject();

	void DrawObject(OBJECTNODE * object);

	OBJECTMODEL * AddModel(string modelname, string texname, bool mip, bool fullbright, bool skybox, bool highlight);

	map <string, GLuint> texture_db;

	string path;
	
public:
	OBJECTS();
	~OBJECTS();
	void Draw();
	void Add(VERTEX pos, float rotation, string modelname, string texname, bool mip, bool fullbright, bool skybox, bool drv, bool col);
	void DeleteAll();

	bool FindClosestVert(VERTEX orig, VERTEX dir, VERTEX &out);
	bool FindClosestVert(VERTEX orig, VERTEX dir, VERTEX &out, string & obj);
	bool FindClosestVert(VERTEX orig, VERTEX dir, VERTEX &out, OBJECTMODEL * & obj);
	bool AutoFindClosestVert(VERTEX orig1, VERTEX orig2, VERTEX dir, VERTEX &out);

	void LoadObjectsFromFolder(string objectpath);
};

#define OBJECTS_H
#endif
