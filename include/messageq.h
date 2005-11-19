#ifndef _MESSAGEQ_H

#include <stdio.h>			// Header File For Standard Input/Output
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <string>
#include <iostream>
#include <fstream>

#include "font.h"

using namespace std;

#define MAX_Q_DEPTH 20

//note:  q => queue

class MESSAGEQ
{
//public:
private:
	int qdepth;
	float qpersist;
	string q[MAX_Q_DEPTH];
	bool valid[MAX_Q_DEPTH];
	float qlen[MAX_Q_DEPTH];
	float posx;
	float posy;
	int size;
	int fset;
	bool printtime;
	bool buildup;
	
public:
	MESSAGEQ();
	~MESSAGEQ();
	void Clear();
	void SetDepth(int newqdepth);
	void SetPersist(float newqpersist);
	void SetTimePrint(bool newprinttime);
	void SetBuildUp(bool newbuildup);
	void SetPos(float newx, float newy, int newsize, int newfset);
	void AddMessage(string newq);
	void Draw(float timefactor, float fps, FONT & font);
};

#define _MESSAGEQ_H
#endif
