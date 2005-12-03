/***************************************************************************
 *            track.h
 *
 *  Sat Nov 19 10:45:47 2005
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
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#ifndef _TRACK_H

#include "bezier.h"

class BEZIERNODE
{
public:
	BEZIER patch;
	BEZIERNODE * next;
	BEZIERNODE() {next = NULL;}
};

class ROADSTRIP
{
private:
	BEZIERNODE * patchnodes;
	void ClearPatches();
	
public:
	ROADSTRIP();
	~ROADSTRIP() {ClearPatches();}
	BEZIER * Add(BEZIER newpatch);
	BEZIER * AddNew();
	bool ReadFrom(ifstream &openfile);
	bool WriteTo(ofstream &openfile);
	bool DeleteLastPatch();
	void Visualize (bool wireframe, bool fill, VERTEX color);
	BEZIER * GetLastPatch();
	int NumPatches();
	bool Collide(VERTEX origin, VERTEX direction, VERTEX &outtri, bool closest);
};

class ROADSTRIPNODE
{
public:
	ROADSTRIP road;
	ROADSTRIPNODE * next;
	ROADSTRIPNODE() {next = NULL;}
};

class TRACK
{
private:
	ROADSTRIPNODE * roads;
	int NumRoads();
	
public:
	TRACK();
	~TRACK();
	ROADSTRIP * AddNewRoad();
	void VisualizeRoads(bool wireframe, bool fill, ROADSTRIP * selectedroad);
	void ClearRoads();
	void Write(string trackname);
	void Load(string trackname);
	void Delete(ROADSTRIP * striptodel);
	bool Collide(VERTEX origin, VERTEX direction, VERTEX &outtri, bool closest, ROADSTRIP * &collideroad);
};

#define _TRACK_H
#endif /* _TRACK_H */
