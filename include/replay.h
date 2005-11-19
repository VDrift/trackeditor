/***************************************************************************
 *            replay.h
 *
 *  Fri Jun 10 18:57:03 2005
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
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#ifndef _REPLAY_H

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

using namespace std;

#include <vamos/geometry/Three_Vector.h>
#include <vamos/geometry/Three_Matrix.h>

#include "utility.h"
#include "timer.h"
#include "gamestate.h"

#include "globals.h"

#define MAX_PACKETS 83000
//#define MAX_PACKETS 100

#define FUNCTION_CHARS 50

#define MAX_STATES 3600

#define REPLAY_VERSION "VDRIFTREPLAYV05"

#define CHAR_FUNCNUM 0
#define CHAR_STATE 1
#define CHARDATA_SIZE 2

#define FUNCTION_NETCONTROL 255
#define FUNCTION_STATEINFO 254
#define FUNCTION_NOOP 253
#define FUNCTION_PACKET_ARRAY 252

//how often we remember the car's state
#define STATE_FREQUENCY 1.0

#define DBG_REPLAY false

struct REPLAY_PACKET
{
	//char dofunction[FUNCTION_CHARS];
	//int funcnum;
	unsigned char chardata[CHARDATA_SIZE]; //funcnum is 0, state is 1
	float val;
	float time;
	//int state; //0 for one-time, 1 for held, 2 for stop holding
};

struct FUNCTION_MEMORY
{
	string func_name;
	float oldval;
	float newval;
	bool held;
	bool active;
	double lastupdateat;
};

struct FUNCTION_MEMORY_SYNC
{
	float oldval;
	float newval;
	bool held;
	bool active;
	double lastupdateat;
};

class CARSTATE
{
public:
	double time;
	
	Vamos_Geometry::Three_Vector chassispos;
	Vamos_Geometry::Three_Matrix chassisorientation;
	Vamos_Geometry::Three_Vector chassisvel;
	Vamos_Geometry::Three_Vector chassisangvel;
	double suspdisp[4];
	double suspcompvel[4];
	Vamos_Geometry::Three_Vector whlangvel[4];
	int gear;
	double enginespeed;
	double clutchspeed;
	double enginedrag;
	double tirespeed[4];

	int segment;

	FUNCTION_MEMORY_SYNC * funcmem;
	int fnum;
	
	void CopyFrom(CARSTATE other);
	
	void WriteToFile(FILE * fout);
	void WriteVector(Vamos_Geometry::Three_Vector wv, FILE * fout);
	void WriteMatrix(Vamos_Geometry::Three_Matrix wv, FILE * fout);

	void ReadFromFile(FILE * fin);
	void ReadVector(Vamos_Geometry::Three_Vector &wv, FILE * fin);
	void ReadMatrix(Vamos_Geometry::Three_Matrix &wv, FILE * fin);
};

class REPLAY
{
private:
	int num_packets;
	REPLAY_PACKET packet[MAX_PACKETS];
	FUNCTION_MEMORY * funcmem;
	int fnum;
	bool replay;
	FILE * rf;
	double timeindex;
	bool playing;
	int cur_packet;
	void PlayProcessFrame();
	float playend;

	void WriteCarState();
	void RememberCarState();
	CARSTATE state_mem[MAX_STATES];
	int num_states;
	CARSTATE loadstate;
	bool state_to_load;
	int cur_state;

	CARSTATE * GetNextState(double until);
	REPLAY_PACKET * GetNextPacket(double until);
	
	bool ghostcar;
	
	string replaycar;
	int replaypaint;

public:
	void BuildFuncMem(FUNCTION_MEMORY * &fmem, int & fnm);

	REPLAY();
	~REPLAY();
	void Clear();
	void Start();
	void AddRecord(string newdofunction, float newval, int newstate);
	void IncrementFrame();
	void Stop();
	float Recording();

	void PlayStart(string replayname);
	void PlayStart(string replayname, bool ngc);
	void PlayStop();
	float Playing();
	int GetNumFuncs();
	FUNCTION_MEMORY GetFunc(int idx);
	int GetFuncIdx(string funcname);
	void PlayIncrement();
	double GetTime();

	CARSTATE curstate;
	CARSTATE * LoadState();
	//CARSTATE GetState(int idx) {return state_mem[idx];}
	
	void Jump(double newtime);
	
	//extract constants
	int Get_FuncChars() {return FUNCTION_CHARS;}
	int Get_CharState() {return CHAR_STATE;}
	int Get_CharFuncnum() {return CHAR_FUNCNUM;}
	int Get_FuncNetControl() {return FUNCTION_NETCONTROL;}
	int Get_FuncStateInfo() {return FUNCTION_STATEINFO;}
	int Get_FuncNoop() {return FUNCTION_NOOP;}
	int Get_FuncPacketArray() {return FUNCTION_PACKET_ARRAY;}
	
	//file writing routines
	void WriteHeader(FILE * rf, FUNCTION_MEMORY * fmem, int fnm, int carnumber);
	void WritePackets(FILE * rf, REPLAY_PACKET * pak, int pnum, CARSTATE * smem, int snum);
	
	bool GhostCar() {return ghostcar;}
	string ReplayCar() {return replaycar;}
	int ReplayPaint() {return replaypaint;}
};

#define _REPLAY_H
#endif /* _REPLAY_H */
