/***************************************************************************
 *            net.h
 *
 *  Sun Sep 11 08:28:02 2005
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
 
#ifndef _NET_H

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

#include "replay.h"

#include "globals.h"

#include <SDL/SDL_net.h>

using namespace std;

#define NET_DEBUG false

//#define MAX_PACKET_SIZE 65535
//#define PACKET_BUFFER_LEN 20

#define MAX_PACKET_SIZE 8192
#define PACKET_BUFFER_LEN 160

#define UDP_SUCCESS 1
#define UDP_FAILURE -1
#define UDP_TIMEOUT 0

#define HANDSHAKESTR "vdrift-handshake-v01"
#define HANDSHAKEREPLYSTR "vdrift-handshake-v01-reply"
#define HANDSHAKETIMEOUT 3000

#define CONTROL_HANDSHAKE 0

//packet buffer class
class PBUFFER
{
private:
	bool valid;
	
public:
	Uint8 * data;
	int len;
	bool Valid() {return valid;}
	void Put(UDPpacket * newp);
	void Clear() {valid = false;}
	PBUFFER() {valid = false; data = NULL;}
	~PBUFFER() {if (data != NULL) delete [] data; data=NULL;}
};

class NET
{
private:
	IPaddress remote_ip;
	UDPsocket remote_socket;
	UDPpacket *pout, *pin;

	bool server;
	bool connected;

	PBUFFER buffer[PACKET_BUFFER_LEN];

	//size is in bytes
	void WriteToPacket(UDPpacket * p, Uint8 control, void * data, int size);
	bool IsEqualToPacket(UDPpacket * p, Uint8 control, void * data, int size);
	int UDPSend(UDPpacket * p, int channel);
	int UDPRecv(UDPpacket * p, int channel, int timeout);
	bool ClientHandshake();
	bool ServerHandshake();

	int BufferPacket(UDPpacket * newp);

	unsigned int datacounter_tx;
	unsigned int datacounter_rx;

public:
	NET();
	~NET();
	void Init();
	bool Connect(string host, int port);
	void Update();
	bool Server() {return server;}
	bool Connected();
	void Disconnect();
	void Send(void * data, int size);
	int RecvBlock(void * dest, int destsize, int timeout);
	int NumBufferedPackets();
	PBUFFER * GetBuffer(int idx) {return &(buffer[idx]);}
	int GetMaxBuffers() {return PACKET_BUFFER_LEN;}
	int GetTxBytes() {return datacounter_tx;}
	int GetRxBytes() {return datacounter_rx;}
	int ClearStats() {datacounter_tx = datacounter_rx = 0; return 1;}
};

#define _NET_H
#endif /* _NET_H */
