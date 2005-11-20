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

BEZIER * ROADSTRIP::Add(BEZIER newpatch)
{
	BEZIERNODE * lastnode = NULL;
	BEZIERNODE * curnode = patchnodes;
	
	while (curnode != NULL)
	{
		lastnode = curnode;
		curnode = curnode->next;
	}

	//only continue if there is a last node and it really is the last node
	if (lastnode != NULL && lastnode->next == NULL)
	{
		lastnode->next = new BEZIERNODE;
		lastnode->next->patch.CopyFrom(newpatch);
		
		//optional....
		lastnode->patch.Attach(lastnode->next->patch);
		return &(lastnode->next->patch);
	}
	
	if (patchnodes == NULL)
	{
		patchnodes = new BEZIERNODE;
		patchnodes->patch.CopyFrom(newpatch);
		return &(patchnodes->patch);
	}
	
	return NULL;
}

BEZIER * ROADSTRIP::AddNew()
{
	BEZIER newpatch;
	return Add(newpatch);
}

bool ROADSTRIP::ReadFrom(ifstream &openfile)
{
	//optional....
	ClearPatches();
	
	if (!openfile)
		return false;
	
	BEZIER * newpatch;
	BEZIER temppatch;
	
	int num;
	
	//the number of patches for this road
	openfile >> num;
	
	int i;
	
	for (i = 0; i < num; i++)
	{
		//create a new patch and make it read from the file
		if (!temppatch.ReadFrom(openfile))
			return false;
		newpatch = Add(temppatch);
	}
	
	return true;
}

bool ROADSTRIP::WriteTo(ofstream &openfile)
{
	BEZIERNODE * curnode = patchnodes;
	
	openfile << NumPatches() << endl << endl;
		
	while (curnode != NULL)
	{
		curnode->patch.WriteTo(openfile);
		openfile << endl;
		
		curnode = curnode->next;
	}
	
	return true;
}

int ROADSTRIP::NumPatches()
{
	BEZIERNODE * curnode = patchnodes;
	
	int num = 0;
	
	while (curnode != NULL)
	{
		num++;
		
		curnode = curnode->next;
	}
	
	return num;
}

void ROADSTRIP::DeleteLastPatch()
{
	BEZIERNODE * lastnode = NULL;
	BEZIERNODE * prevlastnode = NULL;
	BEZIERNODE * curnode = patchnodes;
	
	while (curnode != NULL)
	{
		prevlastnode = lastnode;
		lastnode = curnode;
		curnode = curnode->next;
	}

	//only continue if there is a last node and it really is the last node
	if (lastnode != NULL && lastnode->next == NULL)
	{
		if (prevlastnode != NULL)
		{
			delete prevlastnode->next;
			prevlastnode->next = NULL;
		}
		else
		{
			delete patchnodes;
			patchnodes = NULL;
		}
	}
}

void ROADSTRIP::Visualize (bool wireframe, bool fill)
{
	BEZIERNODE * curnode = patchnodes;
	
	while (curnode != NULL)
	{
		curnode->patch.Visualize(wireframe, fill);
		
		curnode = curnode->next;
	}
}


//----------------- TRACK --------------


TRACK::TRACK()
{
	roads = NULL;
}

TRACK::~TRACK()
{
	ClearRoads();
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

void TRACK::VisualizeRoads(bool wireframe, bool fill)
{
	ROADSTRIPNODE * curnode = roads;
	
	while (curnode != NULL)
	{
		curnode->road.Visualize(wireframe, fill);
		curnode = curnode->next;
	}
}

void TRACK::Write(string trackname)
{
	ofstream trackfile;
	trackfile.open((settings.GetDataDir() + "/tracks/" + trackname + "/roads.trk").c_str());
	
	trackfile << NumRoads() << endl << endl;
	
	ROADSTRIPNODE * curnode = roads;
	
	while (curnode != NULL && trackfile)
	{
		curnode->road.WriteTo(trackfile);
		curnode = curnode->next;
	}
}

int TRACK::NumRoads()
{
	ROADSTRIPNODE * curnode = roads;
	
	int num = 0;
	
	while (curnode != NULL)
	{
		num++;
		curnode = curnode->next;
	}
	
	return num;
}

void TRACK::Load(string trackname)
{
	ifstream trackfile;
	trackfile.open((settings.GetDataDir() + "/tracks/" + trackname + "/roads.trk").c_str());
	
	int numroads, i; 
	
	if (trackfile)
	{
		trackfile >> numroads;
	}
	else
	{
		cout << "No track named " << trackname << ", creating a new one." << endl;
		return;
	}
	
	ROADSTRIP * newroad;
	
	for (i = 0; i < numroads && trackfile; i++)
	{
		newroad = AddNewRoad();
		newroad->ReadFrom(trackfile);
	}
}
