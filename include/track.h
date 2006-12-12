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
#include "camera.h"
#include "configfile.h"
#include "globals.h"

#include <sstream>

#include <list>
#include <vector>

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
	void ClearPatches();
	
public:
	BEZIERNODE * patchnodes;
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
	bool Collide(VERTEX origin, VERTEX direction, VERTEX &outtri, bool closest, BEZIER * &collidepatch);
};

class ROADSTRIPNODE
{
public:
	ROADSTRIP road;
	ROADSTRIPNODE * next;
	ROADSTRIPNODE() {next = NULL;}
};

class LAPAREA
{
public:
	unsigned int roadidx;
	unsigned int patchidx;
};

class TRACK
{
private:
	ROADSTRIPNODE * roads;
	int NumRoads();
	std::vector<VERTEX> startloc;
	std::vector<QUATERNION> startquat;
	vector <LAPAREA> lapsequence;
	
public:
	TRACK();
	~TRACK();
	ROADSTRIP * AddNewRoad();
	void VisualizeRoads(bool wireframe, bool fill, ROADSTRIP * selectedroad);
	void ClearRoads();
	void Write(string trackname);
	void Load(string trackname);
	void Delete(ROADSTRIP * striptodel);
	bool Collide(VERTEX origin, VERTEX direction, VERTEX &outtri, bool closest, ROADSTRIP * &collideroad, BEZIER * &collidepatch);
	void SetStart(VERTEX newloc) {startloc.push_back(newloc);}
	void RemoveStart() {if (startloc.size() > 0) startloc.pop_back();}
	VERTEX GetStart(unsigned int i) {return (i>=startloc.size())?startloc[0]:startloc[i];}
	void SetStartOrientation(QUATERNION newquat) {startquat.push_back(newquat);}
	QUATERNION GetStartOrientation(unsigned int i) {return (i>=startquat.size())?startquat[0]:startquat[i];}
	void RemoveStartOrientation() {if (startquat.size() > 0) startquat.pop_back();}
	
	void ClearLapSequence() {lapsequence.clear();}
	void AddLapSequence(BEZIER * patch);
	int NumLapSeqs() {return lapsequence.size();}
	BEZIER * GetLapSeq(int idx);
};

#define _TRACK_H
#endif /* _TRACK_H */
