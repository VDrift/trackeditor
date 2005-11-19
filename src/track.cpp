/***************************************************************************
 *            track.cc
 *
 *  Sat Nov 19 11:08:52 2005
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
 
#include "track.h"

ROADSTRIP::ROADSTRIP()
{
	patchnodes = NULL;
}

void ROADSTRIP::ClearPatches()
{
	while (patchnodes != NULL)
	{
		BEZIERNODE * oldfirst = patchnodes;
		patchnodes = patchnodes->next;
		delete oldfirst;
	}
}

void ROADSTRIP::Add(BEZIER newpatch)
{
	BEZIERNODE * oldfirst = patchnodes;
	patchnodes = new BEZIERNODE;
	patchnodes->patch.CopyFrom(newpatch);
	patchnodes->next = oldfirst;
}

TRACK::TRACK()
{
	roads = NULL;
}

void TRACK::ClearRoads()
{
	while (roads != NULL)
	{
		ROADSTRIPNODE * oldfirst = roads;
		roads = roads->next;
		delete oldfirst;
	}
}

ROADSTRIP * TRACK::AddNewRoad()
{
	ROADSTRIPNODE * oldfirst = roads;
	roads = new ROADSTRIPNODE;
	roads->next = oldfirst;
	
	return &(roads->road);
}
