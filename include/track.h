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
};

class ROADSTRIP
{
private:
	BEZIERNODE * patchnodes;
	void ClearPatches();
	
public:
	ROADSTRIP();
	~ROADSTRIP() {ClearPatches();}
	void Add(BEZIER newpatch);
};

class ROADSTRIPNODE
{
public:
	ROADSTRIP road;
	ROADSTRIPNODE * next;
};

class TRACK
{
private:
	ROADSTRIPNODE * roads;
	void ClearRoads();
	
public:
	TRACK();
	~TRACK();
	ROADSTRIP * AddNewRoad();
};

#define _TRACK_H
#endif /* _TRACK_H */
