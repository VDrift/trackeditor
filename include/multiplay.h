/***************************************************************************
 *            multiplay.h
 *
 *  Sun Sep 11 10:16:10 2005
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
 
#ifndef _MULTIPLAY_H

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

#include "utility.h"
#include "timer.h"
#include "gamestate.h"
#include "replay.h"
#include "net.h"
#include "messageq.h"
//#include "vamosworld.h"

#include "globals.h"

#define CONTROL_HANDSHAKE 0
#define CONTROL_WORLDINFO 1
#define CONTROL_STATE 2

#define GENERIC_TIMEOUT 3000

#define STATE_TIMEOUT_COUNT 2

#define PACKET_TIMEOUT_COUNT 4

#define MP_DEBUG false
#define MP_DBGDEEP false

#define MP_DISABLEADDRECORD false
#define MP_DISABLETICK false
#define MP_DISABLEGET false
#define MP_DISABLESEND false
#define MP_DISABLEFUNCUPDATE false

#define MP_RECORD false
#define MP_REMOTE_RECORD false

#define PACKET_ARRAY_SIZE 2500

#define PACKET_ARRAY_FREQUENCY 0.1

#define NETSTAT_UPDATE_FREQUENCY 1.0

#define CLIENT_DISCONNECT_TIMEOUT 10.0

class MULTIPLAY
{
private:
	bool Connected() {return net.Connected();}
	bool ExchangeWorldInfo();
	int remote_players;
	int remote_playernum;

	FUNCTION_MEMORY * funcmems[MAX_PLAYERS];
	int fnums[MAX_PLAYERS];

	int AddToData(void * dest, void * source, int len, int start);
	int GetFromData(void * source, void * dest, int len, int start);

	double timeindex[MAX_PLAYERS];
	CARSTATE curstates[MAX_PLAYERS]; //only curstate zero is used (for the local car)
	CARSTATE loadstates[MAX_PLAYERS]; //only loadstates 1 to n are used (for the remove cars)
	//... should combine curstates and loadstates.

	//bool loadstatevalid[MAX_PLAYERS];
	bool loadstatenow[MAX_PLAYERS];

	REPLAY_PACKET * packetarrays[MAX_PLAYERS];
	REPLAY_PACKET * GetPacketArray(int idx) {return packetarrays[idx];}
	//bool packetarrayvalid[MAX_PLAYERS];
	bool PacketArrayValid(int idx) {return (numpackets[idx] > 0);}
	double GetPacketArrayTime(int idx);
	//double packetarraytime[MAX_PLAYERS];
	int numpackets[MAX_PLAYERS];
	double nooptime[MAX_PLAYERS];
	bool noopvalid[MAX_PLAYERS];
	bool tickthisframe[MAX_PLAYERS];
	bool nooptick[MAX_PLAYERS];
	int curpackets[MAX_PLAYERS];

	void QueuePacket();
	void SendPacketArray();
	void ReceivePacketArray();
	void UpdateFuncmem(int idx);

	//deprecated
	void ProcessPacket(PBUFFER * p);

	void SendState();

	void ReadState(Uint8 * s, int slot);

	int WriteVector(Vamos_Geometry::Three_Vector wv, Uint8 * dest, int start);
	int WriteMatrix(Vamos_Geometry::Three_Matrix wv, Uint8 * dest, int start);
	int ReadVector(Vamos_Geometry::Three_Vector &wv, Uint8 * source, int start);
	int ReadMatrix(Vamos_Geometry::Three_Matrix &wv, Uint8 * source, int start);

	void UpdateStats();

	int tx, rx;

	int port;

	//debug output
	CARSTATE dbgstate[MAX_STATES];
	int dbgnumstates;
	REPLAY_PACKET dbgpacket[MAX_PACKETS];
	int dbgnumpackets;

public:
	MULTIPLAY();
	~MULTIPLAY();
	bool Connect(string host);
	void Disconnect();
	bool Host();
	void Update(double inc);
	void Send(double inc);
	int NumConnected();
	int MyNum() {return remote_playernum;}
	bool Server();
	void AddRecord(string newdofunction, float newval, int newstate);
	CARSTATE * GetCurState(int idx) {return &(curstates[idx]);}
	double GetTime(int idx) {return timeindex[idx];}
	bool in_menu;
	
	bool StateToLoad(int idx) {return loadstatenow[idx];}
	void ClearStateToLoad(int idx) {loadstatenow[idx] = false;}
	CARSTATE * GetLoadState(int idx) {return &(loadstates[idx]);}
	
	void ReceiveState();
	
	bool TickCar(int idx) {return (tickthisframe[idx] && !MP_DISABLETICK);}
	bool NOOPTick(int idx) {return nooptick[idx];}
	
	int NumFuncs(int idx) {return fnums[idx];}
	FUNCTION_MEMORY * GetFuncMem(int i) {return funcmems[i];}
	
	int GetTxRate();
	int GetRxRate();
	float GetLatency(int player);
};

#define _MULTIPLAY_H
#endif /* _MULTIPLAY_H */
