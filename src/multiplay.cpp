/***************************************************************************
 *            multiplay.cpp
 *
 *  Sun Sep 11 10:15:34 2005
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
 
#include "multiplay.h"

MULTIPLAY::MULTIPLAY()
{
	remote_players = 0;
	remote_playernum = 0;
	
	int i;
	for (i = 0; i < MAX_PLAYERS; i++)
	{
		funcmems[i] = NULL;
		fnums[i] = 0;
	}
	
	for (i = 0; i < MAX_PLAYERS; i++)
	{
		timeindex[i] = 0.0;
		loadstates[i].time = 0.0;
		loadstatenow[i] = false;
		numpackets[i] = 0;
		
		packetarrays[i] = NULL;
		//packetarraytime[i] = 0.0;
		
		nooptime[i] = 0;
		noopvalid[i] = false;
		tickthisframe[i] = false;
		
		nooptick[i] = false;
	}
	
	tx = 0;
	rx = 0;

	port = settings.GetServerPort();
	
	dbgnumstates = 0;
	dbgnumpackets = 0;
}

bool MULTIPLAY::Server()
{
	return net.Server();
}

bool MULTIPLAY::Connect(string host)
{
	Disconnect();
	
	replay.BuildFuncMem(funcmems[0], fnums[0]);
	
	bool ret = net.Connect(host, port);
	if (!Server() && ret)
	{
		remote_players = 0;
		remote_players++;
		
		ret = ExchangeWorldInfo();
		int i;
		for (i = 0; i < MAX_PLAYERS; i++)
		{
			timeindex[i] = 0.0;
			loadstates[i].time = 0.0;
			loadstatenow[i] = false;
			numpackets[i] = 0;
			
			//packetarraytime[i] = 0.0;
			
			nooptime[i] = 0;
			noopvalid[i] = false;
			tickthisframe[i] = false;
			nooptick[i] = false;
		}
		
		dbgnumstates = 0;
		dbgnumpackets = 0;
		
		mq1.Clear();
		mq1.AddMessage("Successfully connected to server");
	}
	return ret;
}

bool MULTIPLAY::Host()
{
	Disconnect();
	
	replay.BuildFuncMem(funcmems[0], fnums[0]);
	
	remote_playernum = 0;
	return net.Connect("Server", port);
}

extern float FrameTime();

void MULTIPLAY::Update(double inc)
{	
	if (MP_DBGDEEP)
		cout << "multiplay update" << endl;
	
	//create packet arrays if necessary
	int i;
	for (i = 0; i < NumConnected() + 1; i++)
	{
		if (packetarrays[i] == NULL)
		{
			packetarrays[i] = new REPLAY_PACKET [PACKET_ARRAY_SIZE];
		}
	}
	
	if (MP_DBGDEEP)
		cout << "packet mem allocated" << endl;
	
	bool oldc = Connected();
	net.Update();
	if (!oldc && Connected())
	{
		//wasn't connected before, connected now.
		if (Server())
			remote_players++;
		
		ExchangeWorldInfo();
		
		int i;
		for (i = 0; i < MAX_PLAYERS; i++)
		{
			timeindex[i] = 0.0;
			loadstates[i].time = 0.0;
			loadstatenow[i] = false;
			numpackets[i] = 0;
			
			//packetarraytime[i] = 0.0;
			
			nooptime[i] = 0;
			noopvalid[i] = false;
			tickthisframe[i] = false;
			nooptick[i] = false;
		}
		
		dbgnumstates = 0;
		dbgnumpackets = 0;
		
		mq1.Clear();
		mq1.AddMessage("A client successfully connected");
	}
	
	if (MP_DBGDEEP)
		cout << "net updated" << endl;
	
	//read incoming data
	/*if (Connected() && net.NumBufferedPackets() > 0)
	{
		int i;
		for (i = 0; i < net.GetMaxBuffers(); i++)
		{
			if (net.GetBuffer(i)->Valid())
				ProcessPacket(net.GetBuffer(i));
		}
	}*/
	if (Connected())
	{
		if (!MP_DISABLEGET)
		{		
			ReceiveState();
		
			if (MP_DBGDEEP)
				cout << "state receive" << endl;
			
			ReceivePacketArray();
			
			if (MP_DBGDEEP)
				cout << "packet array receive" << endl;
		}
		
		if (!MP_DISABLEFUNCUPDATE)
		{
			double tval = 0;
			string ticktype = "packet array";
			nooptick[1] = false;
			if (PacketArrayValid(1))
				tval = GetPacketArrayTime(1);
			if (noopvalid[1] && nooptime[1] > tval)
			{
				tval = nooptime[1];
				ticktype = "noop";
				nooptick[1] = true;
			}
			if ((noopvalid[1] || PacketArrayValid(1)) && timeindex[1] < tval + PACKET_ARRAY_FREQUENCY - FrameTime()/2.0)
			{
				tickthisframe[1] = true;
				
				/*int nextpacket = curpackets[1];
				if (nextpacket >= numpackets[1])
					nextpacket = numpackets[1] - 1;
				if (ticktype == "packet array")
					cout << "ticking " << ticktype << ": " << GetFuncMem(1)[GetPacketArray(1)[nextpacket].chardata[CHAR_FUNCNUM]].func_name << " " << curpackets[1] << "/" << numpackets[1] << " packets for " << tval << " at " << timeindex[1] << endl;
				else
					cout << "ticking " << ticktype << " for " << tval << " at " << timeindex[1] << endl;*/
				
				timeindex[1] += inc;
				
				//process packet data into the function memory (which is then given to DoOp by vamosworld)
				//if (PacketArrayValid(1))// && !(ticktype == "noop"))
				int i;
				for (i = 0; i < fnums[1]; i++)
				{
					if (!GetFuncMem(1)[i].held && GetFuncMem(1)[i].active)
						GetFuncMem(1)[i].active = false;
				}
				
				UpdateFuncmem(1);
			}
			else
			{
				tickthisframe[1] = false;
				//cout << "not ticking at " << timeindex[1] << endl;
			}
		}
		
		if (MP_DBGDEEP)
			cout << "ticked" << endl;
		
		//check to see if we need to increment		
		/*for (i = 0; i < NumConnected(); i++)
		{
			if (loadstatevalid[i+1] && timeindex[i+1] < loadstates[i+1].time + STATE_FREQUENCY + FrameTime()/2.0)
			{
				timeindex[i+1] += inc;
			}
		}*/
		//if (packetarrayvalid[1] && timeindex[1] < loadstates[1].time + STATE_FREQUENCY - FrameTime()/2.0)
	}

	//cout << "Latency: " << GetLatency(1) << " (" << timeindex[0] << "-" << timeindex[1] << ")" << endl;
	
	//disconnect if the latency is super high
	if (GetLatency(1) > CLIENT_DISCONNECT_TIMEOUT)
	{
		Disconnect();
	}
	
	//update statistics
	UpdateStats();
	
	if (MP_DBGDEEP)
			cout << "multiplay update done" << endl;
}

void MULTIPLAY::Disconnect()
{
	bool wasconnected = Connected();
	
	net.Disconnect();
	remote_players = 0;
	remote_playernum = 0;
	
	int i;
	for (i = 0; i < MAX_PLAYERS; i++)
		timeindex[i] = 0.0;
	
	if (MP_RECORD && dbgnumpackets > 0)
	{
		int slot = 0;
		
		if (MP_REMOTE_RECORD)
			slot = 1;
		
		FILE * dbgout = fopen("replays/mpdebug.vdr", "wb");
		if (dbgout)
		{
			replay.WriteHeader(dbgout, GetFuncMem(slot), NumFuncs(slot), slot);
			replay.WritePackets(dbgout, dbgpacket, dbgnumpackets, dbgstate, dbgnumstates);
			fclose(dbgout);
		}
		
		dbgnumpackets = 0;
		dbgnumstates = 0;
	}
	
	if (wasconnected)
		mq1.AddMessage("Disconnected");
}

extern bool LoadWorld();
extern void SelectCar(string cfile, bool trim);

MULTIPLAY::~MULTIPLAY()
{
	int i;
	for (i = 0; i < MAX_PLAYERS; i++)
	{
		if (funcmems[i] != NULL)
			delete [] funcmems[i];
		
		funcmems[i] = NULL;
		
		if (packetarrays[i] != NULL)
			delete [] packetarrays[i];
		
		packetarrays[i] = NULL;
	}
}

bool MULTIPLAY::ExchangeWorldInfo()
{
	char tc[32767];
	sprintf(tc, "%c%c %s\n%i\n%s\n%i\n%i", (char) (replay.Get_FuncNetControl()), (char) CONTROL_WORLDINFO, state.GetCarName(0).c_str(), state.GetCarPaint(0), state.GetTrackName().c_str(), NumConnected(), NumConnected());
	
	//encode string length into the packet
	tc[2] = (char) ((Uint8) strlen(tc));
	
	//encode function memories, starting at strlen(tc)
	int tclen = strlen(tc);
	int opos = tclen;
	
	//tc[opos] = (char)((Uint8) fnums[0]);
	opos = AddToData(tc, &(fnums[0]), sizeof(int), opos);
	int f;
	for (f = 0; f < fnums[0]; f++)
	{
		char fout[FUNCTION_CHARS];
		strcpy(fout, GetFuncMem(0)[f].func_name.c_str());
		//fwrite(fout,1,FUNCTION_CHARS, rf);
		opos = AddToData(tc, fout, FUNCTION_CHARS, opos);
	}
	
	net.Send(tc, opos);
	
	int ret = net.RecvBlock(tc, 32767, GENERIC_TIMEOUT);
	if (ret > 0)
	{
		int pos = 0;
		int err = 0;
		
		if (tc[pos] == (char) (replay.Get_FuncNetControl()))
		{
			pos++;
			if (tc[pos] == (char) (CONTROL_WORLDINFO))
			{
				pos++;
				
				//decode string section length
				int slen = tc[2];
				pos++;
				
				string car;
				string carpaint;
				string track;
				string numplayers;
				string myplayernum;
				char sc[2];
				sc[1] = '\0';
				
				while (pos < slen && tc[pos] != '\n')
				{
					sc[0] = tc[pos];
					car.append(sc);
					
					pos++;
				}
				
				pos++;
				
				while (pos < slen && tc[pos] != '\n')
				{
					sc[0] = tc[pos];
					carpaint.append(sc);
					
					pos++;
				}
				
				pos++;
				
				while (pos < slen && tc[pos] != '\n')
				{
					sc[0] = tc[pos];
					track.append(sc);
					
					pos++;
				}
				
				pos++;
				
				while (pos < slen && tc[pos] != '\n')
				{
					sc[0] = tc[pos];
					numplayers.append(sc);
					
					pos++;
				}
				
				pos++;
				
				while (pos < slen && tc[pos] != '\n')
				{
					sc[0] = tc[pos];
					myplayernum.append(sc);
					
					pos++;
				}
				
				int icarpaint = atoi(carpaint.c_str());
				int inumplayers = atoi(numplayers.c_str());
				int imyplayernum = atoi(myplayernum.c_str());
				
				if (MP_DEBUG)
				{
					/*tc[ret] = '\0';
					cout << "raw worldinfo:  " << tc << endl;*/
					cout << "worldinfo size:  " << ret << endl;
					cout << "got worldinfo:  " << car << "," << icarpaint << "," << track << "," << inumplayers << "," << imyplayernum << endl;
				}
				
				//decode function memory
				//first clear it out
				int newpnum = NumConnected();
				if (funcmems[newpnum] != NULL)
				{
					delete [] funcmems[newpnum];
					funcmems[newpnum] = NULL;
				}
				pos = slen;
				pos = GetFromData(tc, &(fnums[newpnum]), sizeof(int), pos);
				funcmems[newpnum] = new FUNCTION_MEMORY [fnums[newpnum]];
				for (f = 0; f < fnums[newpnum]; f++)
				{
					char fin[FUNCTION_CHARS];
					pos = GetFromData(tc, fin, FUNCTION_CHARS, pos);
					GetFuncMem(newpnum)[f].func_name = fin;
					GetFuncMem(newpnum)[f].oldval = 0.0;
					GetFuncMem(newpnum)[f].held = false;
					GetFuncMem(newpnum)[f].active = false;
				}
				
				if (NET_DEBUG)
				{
					cout << "got functions:  " << fnums[newpnum];
					cout << " (" << fnums[0] << " local)" << endl;
					/*cout << "function list:  ";
					for (f = 0; f < fnums[newpnum]; f++)
					{
						cout << GetFuncMem(newpnum)[f].func_name << ",";
					}
					cout << endl;*/
				}
				
				if (!Server())
				{
					state.SetTrackName(track);
					state.SetCarName(1, car);
					state.SetCarPaint(1, icarpaint);
					remote_players = inumplayers;
					remote_playernum = imyplayernum;
					
					LoadWorld();
				}
				else
				{
					state.SetCarName(1, car);
					state.SetCarPaint(1, icarpaint);
					
					SelectCar(state.GetCarName(0), true);
					/*try
					{
						int numcars = NumConnected();
						Vamos_Body::Gl_Car* car = 0;
						car = new Vamos_Body::Gl_Car (Vamos_Geometry::Three_Vector (11.0, 0.0, 0.6));
						car->read ("data/", state.GetCarName(numcars));
						car->SetPaint(state.GetCarPaint(numcars));
						car->chassis ().translate (Vamos_Geometry::Three_Vector (10.0, 0.0, 
							-car->chassis ().lowest_contact_position () + 0.5));
						car->start_engine ();
						car->set_controller(2);
						world.add_car (car);
					}
					catch (Vamos_Geometry::XML_Exception& error)
					{
					  std::cerr << error.message () << std::endl;
					  std::exit (EXIT_FAILURE);
					}*/
				}
			}
			else
				err = 2;
		}
		else
			err = 1;
		
		if (err)
		{
			//retry?
		
			//nah, just disconnect.
			net.Disconnect();
			if (MP_DEBUG)
			{
				cout << "multiplay:  Update:  error parsing world info:  error " << err << endl;
			}
			return false;
		}
	}
	else
	{
		//retry?
		
		//nah, just disconnect.
		net.Disconnect();
		if (MP_DEBUG)
		{
			cout << "multiplay:  Update:  didn't receive world info:  error " << ret << endl;
		}
		
		return false;
	}
	
	return true;
}

int MULTIPLAY::NumConnected()
{
	return remote_players;
}

//add source (size len) to dest, starting at start, and return the new length
int MULTIPLAY::AddToData(void * dest, void * source, int len, int start)
{
	int i;
	for (i = 0; i < len; i++)
	{
		((char *) dest)[i+start] = ((char *) source)[i];
	}
	
	return start + len;
}

//get dest (size len) from source, starting at start, and return len+start
int MULTIPLAY::GetFromData(void * source, void * dest, int len, int start)
{
	int i;
	for (i = 0; i < len; i++)
	{
		((char *) dest)[i] = ((char *) source)[i+start];
	}
	
	return start + len;
}

void MULTIPLAY::AddRecord(string newdofunction, float newval, int newstate)
{
	if (MP_DISABLEADDRECORD)
		return;
	
	//if (replay && num_packets < MAX_PACKETS)
	if (NumConnected() > 0 && GetFuncMem(0) != NULL)
	{
		//packet
		
		int funcidx = 0;
		int i;
		for (i = 0; i < fnums[0]; i++)
		{
			if (GetFuncMem(0)[i].func_name == newdofunction)
				funcidx = i;
		}
		
		//put the values into their functional slot.  this allows higher priority
		// commands to be considered over lower priority ones (as with the vamosworld
		// control processing code (effectively)
		if (newstate != 2)
		{
			GetFuncMem(0)[funcidx].lastupdateat = timeindex[0];
			GetFuncMem(0)[funcidx].newval = newval;
			if (newstate == 1)
				GetFuncMem(0)[funcidx].held = true;
			else
				GetFuncMem(0)[funcidx].held = false;
			GetFuncMem(0)[funcidx].active = true;
			//if (newdofunction == "gas")
				//cout << newdofunction << ", lastupdate set at " << timeindex << endl;
		}
	}
	else
	{
		if (NET_DEBUG)
		{
			cout << "net:  AddRecord:  wrong state for addrecord or funcmems not created" << endl;
		}
	}
}

void MULTIPLAY::ProcessPacket(PBUFFER * p)
{
	int pos = 0;
	
	Uint8 ptype;
	
	pos = GetFromData(p->data, &ptype, sizeof(Uint8), pos);
	
	if (NET_DEBUG)
		cout << "net:  ProcessPacket:  got packet, size " << p->len << endl;		
	
	if (ptype == replay.Get_FuncStateInfo())
	{
		if (NET_DEBUG)
			cout << "net:  ProcessPacket:  packet type is State Info" << endl;
		
		double ptime;
		pos = GetFromData(p->data, &ptime, sizeof(double), pos);
		
		if (NET_DEBUG)
			cout << "net:  ProcessPacket:  packet time is " << ptime << endl;
	}
	else if (ptype == replay.Get_FuncNetControl())
	{
		if (NET_DEBUG)
			cout << "net:  ProcessPacket:  packet type is unexpected control sequence" << endl;
	}
	else
	{
		if (NET_DEBUG)
			cout << "net:  ProcessPacket:  packet type is unknown" << endl;
	}
	
	p->Clear();
}

void MULTIPLAY::SendState()
{
	//send a state if necessary
	
	float remain;
	int mult;
	mult = (int) ((timeindex[0]+FrameTime()/2.0) / STATE_FREQUENCY);
	remain = (timeindex[0] + FrameTime()/2.0) - (float)(mult * STATE_FREQUENCY);
	
	if (remain < FrameTime())
	{
		if (NET_DEBUG)
		{
			cout << "net:  SendState:  sending state at time " << GetCurState(0)->time << endl;
			cout << "used buffers: " << net.NumBufferedPackets() << endl;
		}
		
		//it's time to record a state
		curstates[0].fnum = fnums[0]; //set the number of funcs
		if (curstates[0].funcmem != NULL) //clear old func data
		{
			delete [] curstates[0].funcmem;
			curstates[0].funcmem = NULL;
		}
		curstates[0].funcmem = new FUNCTION_MEMORY_SYNC [fnums[0]]; //allocate func data
		int i;
		for (i = 0; i < fnums[0]; i++) //record current func data
		{
			curstates[0].funcmem[i].oldval = GetFuncMem(0)[i].oldval;
			curstates[0].funcmem[i].newval = GetFuncMem(0)[i].newval;
			curstates[0].funcmem[i].held = GetFuncMem(0)[i].held;
			curstates[0].funcmem[i].active = GetFuncMem(0)[i].active;
			curstates[0].funcmem[i].lastupdateat = GetFuncMem(0)[i].lastupdateat;
		}
		
		
		//now, send a packet
		Uint8 tp[MAX_PACKET_SIZE];
		int tplen = 0;
		Uint8 tempchar;
		tempchar = replay.Get_FuncStateInfo();
		tplen = AddToData(tp, &tempchar, sizeof(Uint8), tplen);
		tplen = AddToData(tp, &GetCurState(0)->time, sizeof(double), tplen);
		//fwrite(&time, sizeof(double), 1, fout);
		tplen = WriteVector(GetCurState(0)->chassispos, tp, tplen);
		tplen = WriteMatrix(GetCurState(0)->chassisorientation, tp, tplen);
		tplen = WriteVector(GetCurState(0)->chassisvel, tp, tplen);
		tplen = WriteVector(GetCurState(0)->chassisangvel, tp, tplen);
		
		for (i = 0; i < 4; i++)
		{
			tplen = AddToData(tp, &GetCurState(0)->suspdisp[i], sizeof(double), tplen);
			tplen = AddToData(tp, &GetCurState(0)->suspcompvel[i], sizeof(double), tplen);
			tplen = WriteVector(GetCurState(0)->whlangvel[i], tp, tplen);
			tplen = AddToData(tp, &GetCurState(0)->tirespeed[i], sizeof(double), tplen);
		}
		
		tplen = AddToData(tp, &GetCurState(0)->gear, sizeof(int), tplen);
		tplen = AddToData(tp, &GetCurState(0)->enginespeed, sizeof(double), tplen);
		tplen = AddToData(tp, &GetCurState(0)->clutchspeed, sizeof(double), tplen);
		tplen = AddToData(tp, &GetCurState(0)->enginedrag, sizeof(double), tplen);
		
		tplen = AddToData(tp, &GetCurState(0)->segment, sizeof(int), tplen);
		
		//write a function state block
		tplen = AddToData(tp, &(fnums[0]), sizeof(int), tplen);
		
		for (i = 0; i < fnums[0]; i++)
		{
			//fwrite(&(funcmem[i]), sizeof(struct FUNCTION_MEMORY_SYNC), 1, fout);
			tplen = AddToData(tp, &(GetCurState(0)->funcmem[i]), sizeof(struct FUNCTION_MEMORY_SYNC), tplen);
		}
		
		net.Send(tp, tplen);
		
		if (MP_RECORD && !MP_REMOTE_RECORD)
		{
			dbgstate[dbgnumstates].CopyFrom(curstates[0]);
			dbgnumstates++;
		}
	}
	//else cout << timeindex[0] << "," << mult << "," << remain << endl;
}

int MULTIPLAY::WriteVector(Vamos_Geometry::Three_Vector wv, Uint8 * dest, int start)
{
	//fwrite(&wv[0], sizeof(double), 1, fout);
	
	int len = start;
	
	len = AddToData(dest, &wv[0], sizeof(double), len);
	len = AddToData(dest, &wv[1], sizeof(double), len);
	len = AddToData(dest, &wv[2], sizeof(double), len);
	
	return len;
}

int MULTIPLAY::WriteMatrix(Vamos_Geometry::Three_Matrix wv, Uint8 * dest, int start)
{
	int len = start;
	
	//fwrite(&wv[0][0], sizeof(double), 1, fout);
	//fwrite(&wv[0][1], sizeof(double), 1, fout);
	//...
	len = AddToData(dest, &wv[0][0], sizeof(double), len);
	len = AddToData(dest, &wv[0][1], sizeof(double), len);
	len = AddToData(dest, &wv[0][2], sizeof(double), len);
	
	len = AddToData(dest, &wv[1][0], sizeof(double), len);
	len = AddToData(dest, &wv[1][1], sizeof(double), len);
	len = AddToData(dest, &wv[1][2], sizeof(double), len);
	
	len = AddToData(dest, &wv[2][0], sizeof(double), len);
	len = AddToData(dest, &wv[2][1], sizeof(double), len);
	len = AddToData(dest, &wv[2][2], sizeof(double), len);
	
	return len;
}

void MULTIPLAY::ReadState(Uint8 * s, int slot)
{
	//read a state

	//allocate function sync memory
	loadstates[slot].fnum = fnums[slot]; //set the number of funcs
	if (loadstates[slot].funcmem != NULL) //clear old func data
	{
		delete [] loadstates[slot].funcmem;
		loadstates[slot].funcmem = NULL;
	}
	
	//now, read the state 
	int len = 0;
	Uint8 tempchar;
	len = GetFromData(s, &tempchar, sizeof(Uint8), len);
	if (tempchar != replay.Get_FuncStateInfo())
	{
		if (NET_DEBUG)
			cout << "net:  ReadState:  packet is not a state" << endl;
		return;
	}
	
	len = GetFromData(s, &loadstates[slot].time, sizeof(double), len);
	//tplen = AddToData(tp, &GetCurState(0)->time, sizeof(double), tplen);
	//fwrite(&time, sizeof(double), 1, fout);
	len = ReadVector(loadstates[slot].chassispos, s, len);
	len = ReadMatrix(loadstates[slot].chassisorientation, s, len);
	len = ReadVector(loadstates[slot].chassisvel, s, len);
	len = ReadVector(loadstates[slot].chassisangvel, s, len);

	int i;	
	for (i = 0; i < 4; i++)
	{
		len = GetFromData(s, &loadstates[slot].suspdisp[i], sizeof(double), len);
		len = GetFromData(s, &loadstates[slot].suspcompvel[i], sizeof(double), len);
		len = ReadVector(loadstates[slot].whlangvel[i], s, len);
		len = GetFromData(s, &loadstates[slot].tirespeed[i], sizeof(double), len);
	}
	
	len = GetFromData(s, &loadstates[slot].gear, sizeof(int), len);
	len = GetFromData(s, &loadstates[slot].enginespeed, sizeof(double), len);
	len = GetFromData(s, &loadstates[slot].clutchspeed, sizeof(double), len);
	len = GetFromData(s, &loadstates[slot].enginedrag, sizeof(double), len);
	
	len = GetFromData(s, &loadstates[slot].segment, sizeof(int), len);
	
	//read the function state block
	len = GetFromData(s, &(fnums[slot]), sizeof(int), len);
	loadstates[slot].funcmem = new FUNCTION_MEMORY_SYNC [fnums[slot]]; //allocate func data
	//len = AddToData(tp, &(fnums[0]), sizeof(int), tplen);
	if (MP_DEBUG)
		cout << "net:  ReadState:  " << fnums[slot] << " functions, active: ";
	
	for (i = 0; i < fnums[slot]; i++)
	{
		//fwrite(&(funcmem[i]), sizeof(struct FUNCTION_MEMORY_SYNC), 1, fout);
		//tplen = AddToData(tp, &(GetFuncMem(0)[i]), sizeof(struct FUNCTION_MEMORY_SYNC), tplen);
		len = GetFromData(s, &loadstates[slot].funcmem[i], sizeof(struct FUNCTION_MEMORY_SYNC), len);
			
		if (MP_DEBUG && loadstates[slot].funcmem[i].active)
			cout << GetFuncMem(slot)[i].func_name << "(" << i << ") ";
	}
	
	if (MP_DEBUG)
		cout << endl;
}

int MULTIPLAY::ReadVector(Vamos_Geometry::Three_Vector &wv, Uint8 * source, int start)
{
	//fwrite(&wv[0], sizeof(double), 1, fout);
	
	int len = start;
	
	len = GetFromData(source, &wv[0], sizeof(double), len);
	len = GetFromData(source, &wv[1], sizeof(double), len);
	len = GetFromData(source, &wv[2], sizeof(double), len);
	
	return len;
}

int MULTIPLAY::ReadMatrix(Vamos_Geometry::Three_Matrix &wv, Uint8 * source, int start)
{
	int len = start;
	
	//fwrite(&wv[0][0], sizeof(double), 1, fout);
	//fwrite(&wv[0][1], sizeof(double), 1, fout);
	//...
	len = GetFromData(source, &wv[0][0], sizeof(double), len);
	len = GetFromData(source, &wv[0][1], sizeof(double), len);
	len = GetFromData(source, &wv[0][2], sizeof(double), len);
	
	len = GetFromData(source, &wv[1][0], sizeof(double), len);
	len = GetFromData(source, &wv[1][1], sizeof(double), len);
	len = GetFromData(source, &wv[1][2], sizeof(double), len);
	
	len = GetFromData(source, &wv[2][0], sizeof(double), len);
	len = GetFromData(source, &wv[2][1], sizeof(double), len);
	len = GetFromData(source, &wv[2][2], sizeof(double), len);
	return len;
}

void MULTIPLAY::Send(double inc)
{
	if (MP_DBGDEEP)
			cout << "multiplay send start" << endl;
	
	//send outgoing data
	if (Connected() && !MP_DISABLESEND)
	{
		if (!in_menu)
		{
			SendState(); // only sends if necessary
			
			QueuePacket();
			SendPacketArray(); // only sends if necessary
		}
	}
	
	if (!in_menu)
	{
		timeindex[0] += inc;
	}
	
	if (MP_DBGDEEP)
			cout << "multiplay send done" << endl;
}

void MULTIPLAY::ReceiveState()
{
	//check to see if it's time for a state packet
	int i;
	float remain;
	int mult;
	mult = (int) ((timeindex[1]+FrameTime()/2.0) / STATE_FREQUENCY);
	remain = (timeindex[1] + FrameTime()/2.0) - (float)(mult * STATE_FREQUENCY);
	
	//cout << timeindex[1] << "," << mult << "," << remain << endl;
	
	bool found = false;
	
	if (remain < FrameTime())
	{
		//cout << "get state" << endl;
		//search packets for correct time
		int count = 0;
		for (i = 0; i < net.GetMaxBuffers(); i++)
		{
			if (net.GetBuffer(i)->Valid())
			{
				//ProcessPacket(net.GetBuffer(i));
				Uint8 ptype;
				GetFromData(net.GetBuffer(i)->data, &ptype, sizeof(Uint8), 0);
				double ptime;
				GetFromData(net.GetBuffer(i)->data, &ptime, sizeof(double), 1);
				if (ptype == replay.Get_FuncStateInfo())
				{
					//clean out old state packets
					if (ptime < timeindex[1] - STATE_FREQUENCY)
					{
						net.GetBuffer(i)->Clear();
						
						if (NET_DEBUG)
							cout << "net:  Update:  cleaned out old state packet with time " << ptime << endl;
					}
					
					if (ptime > timeindex[1] - FrameTime()/2.0 && ptime < timeindex[1] + FrameTime()/2.0)
					{
						//exected time
						net.GetBuffer(i)->Clear();
						
						if (NET_DEBUG)
							cout << "net:  Update:  received state with time " << ptime << endl;
						
						loadstates[1].time = ptime;
						//loadstatevalid[1] = true;
						
						ReadState(net.GetBuffer(i)->data, 1);
						loadstatenow[1] = true;

						//update funcmem from state
						for (i = 0; i < fnums[1]; i++)
						{
							GetFuncMem(1)[i].oldval = loadstates[1].funcmem[i].oldval;
							GetFuncMem(1)[i].newval = loadstates[1].funcmem[i].newval;
							GetFuncMem(1)[i].held = loadstates[1].funcmem[i].held;
							GetFuncMem(1)[i].active = loadstates[1].funcmem[i].active;
							GetFuncMem(1)[i].lastupdateat = loadstates[1].funcmem[i].lastupdateat;
						}
						
						if (MP_RECORD && MP_REMOTE_RECORD)
						{
							dbgstate[dbgnumstates].CopyFrom(loadstates[1]);
							dbgnumstates++;
						}
						
						found = true;
					}
					else
					{
						//cout << "packet " << i << ": " << ptime << "," << timeindex[1] << endl;
						
						//increment timeout counter
						count++;
					}
				}
				//else cout << "i: " << i << ", p: " << ptype << ", t: " << ptime << endl;
			}
		}
		
		/*//if the state packet is late and we have new packets waiting for us,
		// give up waiting for the old one
		if (!found && count >= STATE_TIMEOUT_COUNT)
		{
			if (NET_DEBUG)
			{
				cout << "net:  Update:  timeout waiting for state at time " << timeindex[1] << endl;
			}
			
			timeindex[1] += STATE_FREQUENCY;
		}
		else if (!found)
		{
			if (NET_DEBUG)
			{
				cout << "waiting for timeout: " << count << " states in buffer" << endl;
			}
		}*/
		
		//if we have new packets waiting for us,
		// give up waiting for the old one
		// (moved back into the if (remain < frametime) part because packets can now be timed out)
		int scount = 0;
		double highesttime = 0;
		for (i = 0; i < net.GetMaxBuffers(); i++)
		{
			if (net.GetBuffer(i)->Valid())
			{
				//ProcessPacket(net.GetBuffer(i));
				Uint8 ptype;
				GetFromData(net.GetBuffer(i)->data, &ptype, sizeof(Uint8), 0);
				double ptime;
				GetFromData(net.GetBuffer(i)->data, &ptime, sizeof(double), 1);
				if (ptype == replay.Get_FuncStateInfo())
				{
					scount++;
					
					if (ptime > highesttime)
						highesttime = ptime;
				}
			}
		}
		
		if (!found && scount >= STATE_TIMEOUT_COUNT)
		{
			double newt;
			
			newt = (double)((mult+1) * STATE_FREQUENCY);
			//newt = highesttime;
			
			if (NET_DEBUG)
			{
				cout << "net:  Update:  timeout waiting for state at time " << timeindex[1] << " skipping to " << newt << endl;
			}
			
			//timeindex[1] += STATE_FREQUENCY;
			timeindex[1] = newt;
		}
	}
}

void MULTIPLAY::QueuePacket()
{
	int i;
	for (i = 0; i < fnums[0]; i++)
	{
		//only add a packet if it's non-held or if it's held but the value is different from the stored one
		if (GetFuncMem(0)[i].active && (!GetFuncMem(0)[i].held || (GetFuncMem(0)[i].held && GetFuncMem(0)[i].oldval != GetFuncMem(0)[i].newval)))
		{
			GetPacketArray(0)[numpackets[0]].time = timeindex[0];
			GetPacketArray(0)[numpackets[0]].val = GetFuncMem(0)[i].newval;
			GetPacketArray(0)[numpackets[0]].chardata[CHAR_FUNCNUM] = i;
			//strcpy(packet[num_packets].dofunction, funcmem[i].func_name.c_str());
			int newstate = 0;
			if (GetFuncMem(0)[i].held)
				newstate = 1;
			GetPacketArray(0)[numpackets[0]].chardata[CHAR_STATE] = newstate;
			
			numpackets[0]++;
			
			//cout << "added " << funcmem[i].func_name << " packet, " << timeindex << ": " << newstate << " - " << funcmem[i].newval << "/" << funcmem[i].oldval << endl;
		}
		
		//add a "stop" packet to indicate the (held) function is no longer being
		// called each frame.  only do this if there wasn't an update this frame
		if (GetFuncMem(0)[i].lastupdateat != timeindex[0] && GetFuncMem(0)[i].held && GetFuncMem(0)[i].active)
		{
			GetFuncMem(0)[i].active = false;
			//cout << funcmem[i].lastupdateat << ":" << timeindex << endl;
			//AddRecord(funcmem[i].func_name, funcmem[i].oldval, 2);
			
			GetPacketArray(0)[numpackets[0]].time = timeindex[0];
			GetPacketArray(0)[numpackets[0]].val = GetFuncMem(0)[i].newval;
			GetPacketArray(0)[numpackets[0]].chardata[CHAR_FUNCNUM] = i;
			//strcpy(packet[num_packets].dofunction, funcmem[i].func_name.c_str());
			GetPacketArray(0)[numpackets[0]].chardata[CHAR_STATE] = 2;
			
			numpackets[0]++;
		}
		
		//for non-held keys, set the active to false after one frame.
		if (GetFuncMem(0)[i].active && !GetFuncMem(0)[i].held)
			GetFuncMem(0)[i].active = false;
		
		//propagate the value in memory
		GetFuncMem(0)[i].oldval = GetFuncMem(0)[i].newval;
		
		//if (funcmem[i].func_name == "gas")
			//cout << "gas " << funcmem[i].lastupdateat << ":" << timeindex << " - " << funcmem[i].held << " " << funcmem[i].active << endl;
	}
}

void MULTIPLAY::SendPacketArray()
{
	float remain;
	int mult;
	mult = (int) ((timeindex[0]+FrameTime()/2.0) / PACKET_ARRAY_FREQUENCY);
	remain = (timeindex[0] + FrameTime()/2.0) - (float)(mult * PACKET_ARRAY_FREQUENCY);
	
	if (remain < FrameTime() && timeindex[0] > FrameTime()/2.0)
	{
		if (numpackets[0] > 0)
		{
			if (NET_DEBUG)
				cout << "net:  SendPacketArray:  sending " << numpackets[0] << " packets for time " << GetPacketArrayTime(0) << " at time " << timeindex[0] << endl;
			
			Uint8 tp[MAX_PACKET_SIZE];
			int tplen = 0;
			
			Uint8 tempchar;
			tempchar = replay.Get_FuncPacketArray();
			
			tplen = AddToData(tp, &tempchar, sizeof(Uint8), tplen);
			//tplen = AddToData(tp, &ti, sizeof(double), tplen);
			tplen = AddToData(tp, &numpackets[0], sizeof(int), tplen);
			tplen = AddToData(tp, GetPacketArray(0), sizeof(REPLAY_PACKET)*numpackets[0], tplen);
			net.Send(tp, tplen);
			
			//record the packets that were sent to our local debug record
			if (MP_RECORD && !MP_REMOTE_RECORD)
			{
				int i;
				for (i = 0; i < numpackets[0]; i++)
				{
					dbgpacket[dbgnumpackets] = GetPacketArray(0)[i];
					dbgnumpackets++;
				}
			}
			
			numpackets[0] = 0;
		}
		else
		{	
			//send NOOP
			
			double ntime = ((mult-1) * PACKET_ARRAY_FREQUENCY);
			
			if (NET_DEBUG)
				cout << "net:  SendPacketArray:  sending NOOP for time " << ntime << " at time " << timeindex[0] << endl;
			
			Uint8 tempchar;
			tempchar = replay.Get_FuncNoop();
			Uint8 tp[MAX_PACKET_SIZE];
			int tplen = 0;
			
			tplen = AddToData(tp, &tempchar, sizeof(Uint8), tplen);
			tplen = AddToData(tp, &ntime, sizeof(double), tplen);
			
			net.Send(tp, tplen);
			
			numpackets[0] = 0;
		}
		
		//packetarraytime[0] += PACKET_ARRAY_FREQUENCY;
	}
}

void MULTIPLAY::ReceivePacketArray()
{
	//check to see if it's time for a packet array
	int i;
	float remain;
	int mult;
	mult = (int) ((timeindex[1]+FrameTime()/2.0) / PACKET_ARRAY_FREQUENCY);
	remain = (timeindex[1] + FrameTime()/2.0) - (float)(mult * PACKET_ARRAY_FREQUENCY);
	
	//cout << timeindex[1] << "," << mult << "," << remain << endl;
	
	if (remain < FrameTime())
	{
		//cout << "get state" << endl;
		//search packets for correct time
		int count = 0;
		bool found = false;
		for (i = 0; i < net.GetMaxBuffers(); i++)
		{
			if (net.GetBuffer(i)->Valid())
			{
				//ProcessPacket(net.GetBuffer(i));
				Uint8 ptype;
				int pos = 0;
				pos = GetFromData(net.GetBuffer(i)->data, &ptype, sizeof(Uint8), pos);
								
				if (ptype == replay.Get_FuncPacketArray())
				{
					//check array time
					int np;
					pos = GetFromData(net.GetBuffer(i)->data, &np, sizeof(int), pos);
					REPLAY_PACKET rp;
					pos = GetFromData(net.GetBuffer(i)->data, &rp, sizeof(REPLAY_PACKET), pos);
					
					double t = rp.time;
					int mult;
					mult = (int) ((t+FrameTime()/2.0) / PACKET_ARRAY_FREQUENCY);
					double ptime = (double)(mult * PACKET_ARRAY_FREQUENCY);
					
					//clean out old packet arrays
					if (ptime < timeindex[1] - PACKET_ARRAY_FREQUENCY)
					{
						net.GetBuffer(i)->Clear();
						
						if (NET_DEBUG)
							cout << "net:  Update:  cleaned out old packet array with time " << ptime << endl;
					}
					
					if (ptime > timeindex[1] - FrameTime()/2.0 && ptime < timeindex[1] + FrameTime()/2.0)
					{
						pos = 0;
						pos = GetFromData(net.GetBuffer(i)->data, &ptype, sizeof(Uint8), pos);
						pos = GetFromData(net.GetBuffer(i)->data, &numpackets[1], sizeof(int), pos);
						pos = GetFromData(net.GetBuffer(i)->data, GetPacketArray(1), sizeof(REPLAY_PACKET)*numpackets[1], pos);
						
						//expected time
						net.GetBuffer(i)->Clear();
						
						if (NET_DEBUG)
							cout << "net:  Update:  received packet array size " << numpackets[1] << " for time " << ptime << endl;
							
						found = true;
						
						if (MP_RECORD && MP_REMOTE_RECORD)
						{
							int i;
							for (i = 0; i < numpackets[1]; i++)
							{
								dbgpacket[dbgnumpackets] = GetPacketArray(1)[i];
								dbgnumpackets++;
							}
						}
						
						curpackets[1] = 0;
					}
					else
					{
						//cout << "packet " << i << ": " << ptime << "," << timeindex[1] << endl;
						
						//increment timeout counter
						count++;
					}
				}
				//else cout << "i: " << i << ", p: " << ptype << ", t: " << ptime << endl;
				else if (ptype == replay.Get_FuncNoop())
				{
					double ptime;
					pos = GetFromData(net.GetBuffer(i)->data, &ptime, sizeof(double), pos);
					
					//clean out old packet arrays
					if (ptime < timeindex[1] - PACKET_ARRAY_FREQUENCY)
					{
						net.GetBuffer(i)->Clear();
						
						if (NET_DEBUG)
							cout << "net:  Update:  cleaned out old NOOP with time " << ptime << endl;
					}
					
					if (ptime > timeindex[1] - FrameTime()/2.0 && ptime < timeindex[1] + FrameTime()/2.0)
					{
						//exected time
						net.GetBuffer(i)->Clear();
						
						if (NET_DEBUG)
							cout << "net:  Update:  received NOOP at time " << ptime << endl;
						
						noopvalid[1] = true;
						nooptime[1] = timeindex[1];
						
						curpackets[1] = 0;
						numpackets[1] = 0;
						
						/*loadstates[1].time = ptime;
						//loadstatevalid[1] = true;
						
						ReadState(net.GetBuffer(i)->data, 1);
						loadstatenow[1] = true;*/
						
						found = true;
					}
					else
					{
						//cout << "packet " << i << ": " << ptime << "," << timeindex[1] << endl;
						
						//increment timeout counter
						count++;
					}
				}
			}
		}
		
		//if the state packet is late and we have new packets waiting for us,
		// give up waiting for the old one
		if (!found && count >= PACKET_TIMEOUT_COUNT)
		{
			if (NET_DEBUG)
			{
				cout << "net:  Update:  timeout waiting for packet array at time " << timeindex[1] << endl;
			}
			
			timeindex[1] += PACKET_ARRAY_FREQUENCY;
		}
	}
}

double MULTIPLAY::GetPacketArrayTime(int idx)
{
	if (numpackets[idx] > 0) 
	{
		double t = GetPacketArray(idx)[0].time;
		int mult;
		double to;
		mult = (int) ((t+FrameTime()/2.0) / PACKET_ARRAY_FREQUENCY);
		to = (double)(mult * PACKET_ARRAY_FREQUENCY);
		return to;
	}
	else 
	{
		if (MP_DEBUG)
			cout << "Asked for packet array time of empty packet array." << endl;
		return 0;
	}
	
	//return packetarraytime[idx];
}

void MULTIPLAY::UpdateFuncmem(int idx)
{
	REPLAY_PACKET * nextpacket = NULL;
	if (curpackets[idx] < numpackets[idx] && GetPacketArray(idx)[curpackets[idx]].time < timeindex[idx] + FrameTime()/2.0)
		nextpacket = &(GetPacketArray(idx)[curpackets[idx]]); //GetNextPacket(timeindex + FrameTime()/2.0);
	while (nextpacket != NULL)
	{
		int fid = nextpacket->chardata[CHAR_FUNCNUM];
		//cout << "fidx: " << fid << "," << fnums[idx] << endl;
//		FUNCTION_MEMORY * tmem = GetFuncMem(idx);
		//cout << "ptc: " << tmem << endl;
		//cout << "name: " << tmem[fid].func_name << endl;
		string curfunc = GetFuncMem(idx)[fid].func_name;
		
		//cout << "UpdateFuncMem: " << nextpacket->time << ": " << curfunc << endl;
		
		//cout << "assigned." << endl;
		int funcidx = 0;
		int i;
		for (i = 0; i < fnums[idx]; i++)
		{
			if (GetFuncMem(idx)[i].func_name == curfunc)
				funcidx = i;
		}
		
		GetFuncMem(idx)[funcidx].newval = nextpacket->val;
		if (nextpacket->chardata[CHAR_STATE] == 0)
			GetFuncMem(idx)[funcidx].held = false;
		if (nextpacket->chardata[CHAR_STATE] == 1)
			GetFuncMem(idx)[funcidx].held = true;
		
		if (nextpacket->chardata[CHAR_STATE] == 2)
			GetFuncMem(idx)[funcidx].active = false;
		else
			GetFuncMem(idx)[funcidx].active = true;
		
		nextpacket = NULL;
		curpackets[idx]++;
		if (curpackets[idx] < numpackets[idx] && GetPacketArray(idx)[curpackets[idx]].time < timeindex[idx] + FrameTime()/2.0)
			nextpacket = &(GetPacketArray(idx)[curpackets[idx]]);
	}
}

int MULTIPLAY::GetTxRate()
{
	return (int)((float)tx/NETSTAT_UPDATE_FREQUENCY);
}

int MULTIPLAY::GetRxRate()
{
	return (int)((float)rx/NETSTAT_UPDATE_FREQUENCY);
}

float MULTIPLAY::GetLatency(int player)
{
	if (NumConnected() > player-1 && player >= 0)
	{
		return timeindex[0] - timeindex[player];
	}
	else
		return 0;
}

void MULTIPLAY::UpdateStats()
{
	float remain;
	int mult;
	mult = (int) ((timeindex[0]+FrameTime()/2.0) / NETSTAT_UPDATE_FREQUENCY);
	remain = (timeindex[0] + FrameTime()/2.0) - (float)(mult * NETSTAT_UPDATE_FREQUENCY);
	if (remain < FrameTime())
	{
		tx = net.GetTxBytes();
		rx = net.GetRxBytes();
		net.ClearStats();
	}
}
