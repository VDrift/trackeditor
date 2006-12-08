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

bool ROADSTRIP::DeleteLastPatch()
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
			return true;
		}
		else
		{
			delete patchnodes;
			patchnodes = NULL;
			return true;
		}
	}
	
	return false;
}

BEZIER * ROADSTRIP::GetLastPatch()
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
			return &(prevlastnode->next->patch);
		}
		else
		{
			return &(patchnodes->patch);
		}
	}
	
	return NULL;
}

void ROADSTRIP::Visualize (bool wireframe, bool fill, VERTEX color)
{
	//int tot = 0;
	
	BEZIERNODE * curnode = patchnodes;
	
	/*while (curnode != NULL)
	{
		if (tot < 50) curnode->patch.Visualize(wireframe, fill, color);
		curnode = curnode->next;
		tot ++;
	}
	
	curnode = patchnodes;*/
	
	int count = 0;
	int drawn = 0;
	
	while (curnode != NULL)
	{
		//if (tot - count < 50)
		bool drawme = true;
		//VERTEX pos = curnode->patch.center+cam.position;
		VERTEX & pos = curnode->patch.center;
		for (int i=0;i<6;i++) 
		{
			float rd=cam.frustum[i][0]*pos.x+
			   cam.frustum[i][1]*pos.y+
			   cam.frustum[i][2]*pos.z+
			   cam.frustum[i][3];
			if (rd<=-curnode->patch.radius)
			{
				drawme = false;
			}
		}
		
		if (drawme)
		{
			curnode->patch.Visualize(wireframe, fill, color);
			drawn++;
		}
		
		curnode = curnode->next;
		count++;
	}
	
	//cout << drawn << "/" << count << endl;
}

bool ROADSTRIP::Collide(VERTEX origin, VERTEX direction, VERTEX &outtri, bool closest, BEZIER * &collidepatch)
{
	BEZIERNODE * curnode = patchnodes;
	
	bool collide;
	bool hadcollision = false;
	BEZIER * colpatch = NULL;
	VERTEX tvert;
	
	while (curnode != NULL)
	{
		//curnode->patch.Visualize(wireframe, fill, color);
		collide = curnode->patch.Collide(origin, direction, tvert);
		if (collide && !closest)
		{
			outtri = tvert;
			collidepatch = &(curnode->patch);
			return true;
		}
		else if (collide && closest)
		{
			if ((hadcollision && (origin - tvert).len() < (origin - outtri).len()) || !hadcollision)
			{
				outtri = tvert;
				colpatch = &(curnode->patch);
			}
			
			hadcollision = true;
		}
		
		curnode = curnode->next;
	}
	
	if (closest && hadcollision)
	{
		collidepatch = colpatch;
		return true;
	}
	
	return false;
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
	/*ROADSTRIPNODE * oldfirst = roads;
	roads = new ROADSTRIPNODE;
	roads->next = oldfirst;
	
	return &(roads->road);*/
	
	ROADSTRIPNODE * lastnode = NULL;
	ROADSTRIPNODE * curnode = roads;
	
	while (curnode != NULL)
	{
		lastnode = curnode;
		curnode = curnode->next;
	}

	//only continue if there is a last node and it really is the last node
	if (lastnode != NULL && lastnode->next == NULL)
	{
		lastnode->next = new ROADSTRIPNODE;
		//lastnode->next->patch.CopyFrom(newpatch);
		
		//optional....
		//lastnode->patch.Attach(lastnode->next->patch);
		return &(lastnode->next->road);
	}
	
	if (roads == NULL)
	{
		roads = new ROADSTRIPNODE;
		return &(roads->road);
	}
	
	return NULL;
}

void TRACK::VisualizeRoads(bool wireframe, bool fill, ROADSTRIP * selectedroad)
{
	ROADSTRIPNODE * curnode = roads;
	
	VERTEX color;
	color.zero();
	color.z = 1;
	
	//int count = 0;
	
	//utility.seedrandom(1234);
	
	while (curnode != NULL)
	{
		/*//randomize color!
		color.x = utility.randf(0.0,1.0);
		color.y = utility.randf(0.0,1.0);
		color.z = utility.randf(0.0,1.0);
		
		if (color.len() < 0.1)
		{
			color.y = 1.0;
		}
		
		float maxval = 0.0;
		if (color.x > maxval)
			maxval = color.x;
		if (color.y > maxval)
			maxval = color.y;
		if (color.z > maxval)
			maxval = color.z;
		
		color.x = (1.0/maxval)*color.x;
		color.y = (1.0/maxval)*color.y;
		color.z = (1.0/maxval)*color.z;*/
		
		color.zero();
		if (&(curnode->road) == selectedroad)
			color.y = 1;
		else
		{
			color.y = 0.5;
			color.z = 0.5;
		}
		
		//if (count < 50)
		curnode->road.Visualize(wireframe, fill, color);
		curnode = curnode->next;
		
		//count++;
	}
}

void TRACK::Write(string trackname)
{
	ofstream trackfile;
	trackfile.open((settings.GetDataDir() + "/tracks/" + trackname + "/roads.trk").c_str());
	
	//trackfile << sl.x << " " << sl.y << " " << sl.z << endl << endl;
	CONFIGFILE trackconfig;
	trackconfig.Load((settings.GetDataDir() + "/tracks/" + trackname + "/track.txt").c_str());

	unsigned int index = 0;
	std::vector<VERTEX>::iterator sl_it;
	for (sl_it = startloc.begin(); sl_it != startloc.end(); sl_it++)
	{
		std::stringstream s;
		s<<"start position "<<index;
		trackconfig.SetParam(s.str(), (*sl_it).v3());
		index++;
	}
	QUATERNION so = GetStartOrientation();
	VERTEX sov;
	sov.x = so.x;
	sov.y = so.y;
	sov.z = so.z;
	trackconfig.SetParam("start orientation-xyz", sov.v3());
	trackconfig.SetParam("start orientation-w", so.w);
	trackconfig.SetParam("cull faces", "yes");
	int count = 0;
	int lapsize = lapsequence.size();
	trackconfig.SetParam("lap sequences", lapsize);
	for (vector <LAPAREA>::iterator i = lapsequence.begin(); i != lapsequence.end(); i++,count++)
	{
		stringstream lapname;
		lapname << "lap sequence " << count;
		VERTEX laps;
		laps.Set(i->roadidx, i->patchidx, 0);
		trackconfig.SetParam(lapname.str(), laps.v3());
	}
	trackconfig.SetParam("non-treaded friction coefficient", (float)1.0);
	trackconfig.SetParam("treaded friction coefficient", (float)0.9);
	trackconfig.Write();
	
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
	ClearLapSequence();
	
	ifstream trackfile;
	trackfile.open((settings.GetDataDir() + "/tracks/" + trackname + "/roads.trk").c_str());
	
	CONFIGFILE trackconfig;
	trackconfig.Load((settings.GetDataDir() + "/tracks/" + trackname + "/track.txt").c_str());
	float tvert[3];
	VERTEX sl;
        int index = 0;
        bool end_of_position = false;
        startloc.clear();
        trackconfig.SuppressError(true); //suppress error message
        while (!end_of_position)
        {
                std::stringstream s;
                s<<"start position "<<index;
                if (trackconfig.GetParam(s.str(), tvert))
                {
                        sl.Set(tvert);
                        SetStart(sl);
                        index++;
                }
                else end_of_position = true;
        }
        trackconfig.SuppressError(false); //turn error message back on

        if (index == 0) //still using the old format
        {
		if (trackconfig.GetParam("start position", tvert))
		{
                	sl.Set(tvert);
                	SetStart(sl);
		}
        }

	VERTEX sov;
	trackconfig.GetParam("start orientation-xyz", tvert);
	sov.Set(tvert);
	QUATERNION so;
	so.x = sov.x;
	so.y = sov.y;
	so.z = sov.z;
	trackconfig.GetParam("start orientation-w", so.w);
	SetStartOrientation(so);
	
	int lapmarkers=0;
	trackconfig.GetParam("lap sequences", lapmarkers);
	for (int l = 0; l < lapmarkers; l++)
	{
		float lapraw[3]={0.,0.,0.};
		stringstream lapname;
		lapname << "lap sequence " << l;
		trackconfig.GetParam(lapname.str(), lapraw);
		LAPAREA newarea;
		newarea.roadidx = (int) lapraw[0];
		newarea.patchidx = (int) lapraw[1];
		lapsequence.push_back(newarea);
	}
	
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

void TRACK::Delete(ROADSTRIP * striptodel)
{
	bool deleted = false;
	
	ROADSTRIPNODE * lastnode = NULL;
	ROADSTRIPNODE * curnode = roads;
	
	while (curnode != NULL)
	{
		/*
		NOTE THAT THE BACKSPACE BUTTON NEEDS TO CALL THIS (AND CLEAR 
		 ACTIVESTRIP TO NULL) TO ENSURE THERE ARE NO EMPTY ROADS!
		*/
		
		if (striptodel == &(curnode->road))
		{
			curnode = curnode->next;

			if (lastnode == NULL)
			{
				roads = curnode;
			}
			else
			{
				lastnode->next = curnode;
			}
			
			delete striptodel;
			
			deleted = true;
		}
		else
		{
			lastnode = curnode;
			curnode = curnode->next;
		}
	}
	
	//fail safe stuff
	if (!deleted)
		delete striptodel;
	
	striptodel = NULL;
}

bool TRACK::Collide(VERTEX origin, VERTEX direction, VERTEX &outtri, bool closest, ROADSTRIP * &collideroad, BEZIER * &collidepatch)
{
	ROADSTRIPNODE * curnode = roads;
	
	bool collide;
	bool hadcollision = false;
	VERTEX tvert;
	
	while (curnode != NULL)
	{
		//curnode->patch.Visualize(wireframe, fill, color);
		collide = curnode->road.Collide(origin, direction, tvert, closest, collidepatch);
		if (collide && !closest)
		{
			outtri = tvert;
			collideroad = &(curnode->road);
			return true;
		}
		else if (collide && closest)
		{
			if ((hadcollision && (origin - tvert).len() < (origin - outtri).len()) || !hadcollision)
			{
				outtri = tvert;
				collideroad = &(curnode->road);
			}
			
			hadcollision = true;
		}
		
		curnode = curnode->next;
	}
	
	if (closest && hadcollision)
	{
		return true;
	}
	
	return false;
}

void TRACK::AddLapSequence(BEZIER * patch)
{
	bool found = false;
	ROADSTRIPNODE * curnode = roads;
	
	int ridx = 0;
	
	LAPAREA lap;
	
	while (curnode != NULL)
	{
		BEZIERNODE * curp = curnode->road.patchnodes;
		int pidx = 0;
		
		while (curp != NULL)
		{
			if (patch == &(curp->patch))
			{
				found = true;
				lap.roadidx = ridx;
				lap.patchidx = pidx;
			}
			
			pidx++;
			curp = curp->next;
		}
		
		ridx++;
		curnode = curnode->next;
	}
	
	if (found)
	{
		lapsequence.push_back(lap);
	}
	else
	{
		cerr << "Error finding bezier for lap sequence" << endl;
	}
}

BEZIER * TRACK::GetLapSeq(int idx)
{
	//return &(lapsequence[idx]);
	int ridx = lapsequence[idx].roadidx;
	int pidx = lapsequence[idx].patchidx;
	
	ROADSTRIPNODE * curnode = roads;
	
	int ri = 0;
	
	while (curnode != NULL)
	{
		if (ri == ridx)
		{
			BEZIERNODE * curp = curnode->road.patchnodes;
			int pi = 0;
			
			while (curp != NULL)
			{
				if (pidx == pi)
				{
					return &(curp->patch);
				}
				
				pi++;
				curp = curp->next;
			}
		}
		
		ri++;
		curnode = curnode->next;
	}
	
	cerr << "Error finding bezier for lap sequence in GetLapSeq(" << idx << ")" << endl;
	return NULL;
}
