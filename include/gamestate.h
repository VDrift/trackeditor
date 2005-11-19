/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#ifndef _GAMESTATE_H

#include <iostream>
#include "settings.h"
#include "globals.h"

using namespace std;

#define MAX_PLAYERS 2

enum STATES
{
	STATE_INIT=0,
	STATE_LOGO=1,
	STATE_INITIALMENU=2,
	STATE_PLAYING=3,
	STATE_STAGING=4,
	
	STATE_EXIT=100
};

enum MODES
{
	MODE_FREEDRIVE=0,
	MODE_TIMETRIAL=1,
	MODE_NETMULTIFREE=2
};

class PLAYERINFO
{
public:		
	string carname;
	int carpaint;
};

class GAMESTATE
{
private:
	STATES state;
	MODES mode;
	PLAYERINFO players[MAX_PLAYERS];
	string trackname;

	string treedetail;
	string terraindetail;
	
public:
	GAMESTATE();
	STATES GetGameState();
	void SetGameState(STATES newstate);
	void SetGameMode(MODES newmode) {mode = newmode; settings.SetGameMode(newmode); }
	MODES GetGameMode() {return mode;}
	
	void SetCarName(int p, string cn) {players[p].carname = cn;}
	void SetCarPaint(int p, int cp) {players[p].carpaint = cp;}
	void SetTrackName(string tn) {trackname = tn; settings.SetTrack(tn); }
	
	string GetTrackName() {return trackname;}
	string GetCarName(int p) {return players[p].carname;}
	int GetCarPaint(int p) {return players[p].carpaint;}
	
	string GetTreeDetail() {return treedetail;}
	void SetTreeDetail(string newd) {treedetail = newd; settings.SetTreeDetail(newd); }
	string GetTerrainDetail() {return terraindetail;}
	void SetTerrainDetail(string newd) {terraindetail = newd; settings.SetTerrainDetail(newd); }
};

#define _GAMESTATE_H
#endif /* _GAMESTATE_H */
