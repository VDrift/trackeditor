#ifndef _TREES_H

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
#include "terrain.h"
#include "camera.h"
#include "model.h"
#include "gamestate.h"

#include "globals.h"

#define NUM_TREE_TEX 2

#define NUM_TREE_LOD 3
#define NUM_TREE_TYPES 5

#define MAX_TREE_ENTS 100000

#define TREES_PER_NODE 3

class TREEENTITY
{
public:
	float x,y,z;
	QUATERNION dir;
	TREEENTITY();
	VERTEX pos() {VERTEX p; p.x = x;p.y=y;p.z=z; return p;}
};

class TREENODE
{
public:
	TREEENTITY tree_ent[TREES_PER_NODE];
	float scale[TREES_PER_NODE];
	VERTEX pos;
	TREENODE * next;
	QUATERNION dir;
	int type;
	int numtrees;
	VERTEX shadowpos[TREES_PER_NODE][4];
};

class TREES
{
private:
	GLuint treetex[NUM_TREE_TEX];
	int foliagenumx;
	int foliagenumy;
	GLuint foliagelist;

	ofstream error_log;

	TREENODE * tree_list;
	void deltree();

	void DrawTree(TREENODE * tree);
	void DrawTreeShadow(TREENODE * tree);
	void DrawFoliage();
	
	JOEMODEL tree_model[NUM_TREE_TYPES][NUM_TREE_LOD];
	GLuint tree_shadow_tex[NUM_TREE_TYPES];
	GLuint composite_shadow_texture;
	bool comploaded;

	bool loaded;
	
public:
	TREES();
	~TREES();
	void Load();
	void Draw();
	void Add(VERTEX pos, float spread, int type, int number);
	void DeleteAll();
	void ReadFromFile(string treemapfile, string foliagemapfile);
	void DrawShadows();
	void BuildCompositeShadow();
	GLuint GetCompositeShadow() {return composite_shadow_texture;}
};

#define _TREES_H
#endif
