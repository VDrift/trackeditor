/***************************************************************************
 *            replay.cc
 *
 *  Fri Jun 10 18:58:04 2005
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
 
#include "replay.h"

REPLAY::REPLAY()
{
	num_packets = 0;
	replay = false;
	timeindex = 0.0;
	funcmem = NULL;
	playing = false;
	cur_packet = 0;
	//loadstate = -1;
	state_to_load = 0;
	num_states = 0;
	cur_state = 0;
	
	ghostcar = false;
	
	int i;
	for (i = 0; i < MAX_STATES; i++)
	{
		if (state_mem[i].funcmem != NULL)
		{
			state_mem[i].funcmem = NULL;
		}
	}
	
	if (curstate.funcmem != NULL)
		curstate.funcmem = NULL;
}

REPLAY::~REPLAY()
{
	if (funcmem != NULL)
		delete [] funcmem;
}

void REPLAY::Clear()
{
	int i;
	for (i = 0; i < num_states; i++)
	{
		if (state_mem[i].funcmem != NULL)
		{
			delete [] state_mem[i].funcmem;
			state_mem[i].funcmem = NULL;
		}
	}
	
	num_packets = 0;
	timeindex = 0;
	num_states = 0;
}

void REPLAY::AddRecord(string newdofunction, float newval, int newstate)
{
	if (replay && num_packets < MAX_PACKETS)
	{
		//packet
		
		//if (newval != 0.0)
			//cout << newdofunction << "," << newval << endl;
		
		int funcidx = 0;
		int i;
		for (i = 0; i < fnum; i++)
		{
			if (funcmem[i].func_name == newdofunction)
				funcidx = i;
		}
		
		//if (newdofunction == "gas")
			//cout << timeindex << ": " << newstate << " - " << newdofunction << ", " << newval << endl;
		
		/*if ((newstate == 1 && !funcmem[funcidx].active) || newstate != 1 || (newstate == 1 && funcmem[funcidx].active && funcmem[funcidx].oldval != newval))
		{
			packet[num_packets].time = timeindex;
			packet[num_packets].val = newval;
			strcpy(packet[num_packets].dofunction, newdofunction.c_str());
			packet[num_packets].state = newstate;
			
			num_packets++;
			
			//if (newdofunction == "gas")
				cout << "added " << newdofunction << " packet, " << timeindex << ": " << newstate << " - " << newdofunction << ", " << newval << "/" << funcmem[funcidx].oldval << endl;
		}*/
		
		//put the values into their functional slot.  this allows higher priority
		// commands to be considered over lower priority ones (as with the vamosworld
		// control processing code (effectively)
		if (newstate != 2)
		{
			funcmem[funcidx].lastupdateat = timeindex;
			funcmem[funcidx].newval = newval;
			if (newstate == 1)
				funcmem[funcidx].held = true;
			else
				funcmem[funcidx].held = false;
			funcmem[funcidx].active = true;
			//if (newdofunction == "gas")
				//cout << newdofunction << ", lastupdate set at " << timeindex << endl;
		}
	}
}

extern float FrameTime();

void REPLAY::IncrementFrame()
{
	//cout << frame[num_frames].num_records << endl;
	
	if (funcmem == NULL)
		return;
	
	if (playing)
		PlayIncrement();
	
	if (!replay)
		return;
	
	if (num_packets >= MAX_PACKETS)
	{
		//write out a block of data to the file
		//fwrite(
		//cout << "Replay buffer full" << endl;
		Stop();
		Clear();
		return;
	}
	
	//cout << "frame increment" << endl;
	
	int i;
	for (i = 0; i < fnum; i++)
	{
		//if (funcmem[i].func_name == "gas")
			//cout << "gas " << funcmem[i].lastupdateat << ":" << timeindex << " - " << funcmem[i].held << " " << funcmem[i].active << endl;

		//only add a packet if it's non-held or if it's held but the value is different from the stored one
		if (funcmem[i].active && (!funcmem[i].held || (funcmem[i].held && funcmem[i].oldval != funcmem[i].newval)))
		{
			packet[num_packets].time = timeindex;
			packet[num_packets].val = funcmem[i].newval;
			packet[num_packets].chardata[CHAR_FUNCNUM] = i;
			//strcpy(packet[num_packets].dofunction, funcmem[i].func_name.c_str());
			int newstate = 0;
			if (funcmem[i].held)
				newstate = 1;
			packet[num_packets].chardata[CHAR_STATE] = newstate;
			
			num_packets++;
			
			//cout << "added " << funcmem[i].func_name << " packet, " << timeindex << ": " << newstate << " - " << funcmem[i].newval << "/" << funcmem[i].oldval << endl;
		}
		
		//add a "stop" packet to indicate the (held) function is no longer being
		// called each frame.  only do this if there wasn't an update this frame
		if (funcmem[i].lastupdateat != timeindex && funcmem[i].held && funcmem[i].active)
		{
			funcmem[i].active = false;
			//cout << funcmem[i].lastupdateat << ":" << timeindex << endl;
			//AddRecord(funcmem[i].func_name, funcmem[i].oldval, 2);
			
			packet[num_packets].time = timeindex;
			packet[num_packets].val = funcmem[i].newval;
			packet[num_packets].chardata[CHAR_FUNCNUM] = i;
			//strcpy(packet[num_packets].dofunction, funcmem[i].func_name.c_str());
			packet[num_packets].chardata[CHAR_STATE] = 2;
			
			num_packets++;
		}
		
		//for non-held keys, set the active to false after one frame.
		if (funcmem[i].active && !funcmem[i].held)
			funcmem[i].active = false;
		
		//propagate the value in memory
		funcmem[i].oldval = funcmem[i].newval;
		
		//if (funcmem[i].func_name == "gas")
			//cout << "gas " << funcmem[i].lastupdateat << ":" << timeindex << " - " << funcmem[i].held << " " << funcmem[i].active << endl;
	}
	
	//record a state if necessary
	float remain;
	int mult;
	mult = (int) (timeindex / STATE_FREQUENCY);
	remain = timeindex - mult * STATE_FREQUENCY;
	if (remain < FrameTime())
	{
		curstate.fnum = fnum;
		if (curstate.funcmem != NULL)
		{
			delete [] curstate.funcmem;
			curstate.funcmem = NULL;
		}
		curstate.funcmem = new FUNCTION_MEMORY_SYNC [fnum];
		int i;
		for (i = 0; i < fnum; i++)
		{
			curstate.funcmem[i].oldval = funcmem[i].oldval;
			curstate.funcmem[i].newval = funcmem[i].newval;
			curstate.funcmem[i].held = funcmem[i].held;
			curstate.funcmem[i].active = funcmem[i].active;
			curstate.funcmem[i].lastupdateat = funcmem[i].lastupdateat;
		}
		RememberCarState();
		if (DBG_REPLAY)
			cout << "Recording state at " << timeindex << endl;
	}
	
	timeindex += FrameTime();
	
	//cout << "frame finished" << endl;
	
	//cout << num_frames << endl;
}

extern void ResetWorld(bool fullreset);

void REPLAY::Start()
{
	if (state.GetGameState() != STATE_PLAYING && state.GetGameState() != STATE_STAGING)
		return;
	
	if (state.GetGameMode() == MODE_TIMETRIAL)
		state.SetGameState(STATE_STAGING);
	
	if (replay)
		Stop();
	if (playing)
		PlayStop();
	
	timer.Reset();
	
	//rebuild function memory
	BuildFuncMem(funcmem, fnum);
	
	Clear();
	replay = true;
	num_packets = 0;
	
	ResetWorld(true);
	//RememberCarState();
	
	//rf = fopen("replays/temp.vdr", "wb");
}

void REPLAY::Stop()
{
	if (!replay)
		return;
	
	replay = false;
	
	//cout << timeindex << endl;
	
	string filename = settings.GetSettingsDir() + "/replays/1.vdr";
	rf = fopen(filename.c_str(), "wb");
	
	WriteHeader(rf, funcmem, fnum, 0);
	
	//fwrite(packet, sizeof(REPLAY_PACKET), num_packets, rf);
	WritePackets(rf, packet, num_packets, state_mem, num_states);
	
	fclose(rf);
	
	if (funcmem != NULL)
		delete [] funcmem;
	funcmem = NULL;
	
	//fclose(rf);
}

void REPLAY::WriteHeader(FILE * rf, FUNCTION_MEMORY * fmem, int fnm, int carnumber)
{
	//write the file format version
	string ver = REPLAY_VERSION;
	fwrite(ver.c_str(), 1, ver.length(), rf);
	
	//write the current car
	int ln = state.GetCarName(carnumber).length();
	fwrite(&ln, 1, sizeof(int), rf);
	string cn = state.GetCarName(carnumber);
	fwrite(cn.c_str(), 1, cn.length(), rf);
	
	//write the current car paint
	int cp = state.GetCarPaint(carnumber);
	fwrite(&cp, sizeof(int), 1, rf);
	
	//write the current track
	ln = state.GetTrackName().length();
	fwrite(&ln, 1, sizeof(int), rf);
	fwrite(state.GetTrackName().c_str(), 1, state.GetTrackName().length(), rf);
	
	fwrite(&fnm, 1, sizeof(int), rf);
	
	int i;
	for (i = 0; i < fnm; i++)
	{
		char fout[FUNCTION_CHARS];
		strcpy(fout, fmem[i].func_name.c_str());
		fwrite(fout,1,FUNCTION_CHARS, rf);
	}
}

void REPLAY::WritePackets(FILE * rf, REPLAY_PACKET * pak, int pnum, CARSTATE * smem, int snum)
{
	int si = 0;
	int i;
	for (i = 0; i < pnum; i++)
	{
		while (si < snum && smem[si].time <= pak[i].time)
		{
			unsigned char funccode = FUNCTION_STATEINFO;
			fwrite(&funccode, 1, sizeof(unsigned char), rf);
			smem[si].WriteToFile(rf);
			
			si++;
		}
		
		fwrite(&pak[i], sizeof(REPLAY_PACKET), 1, rf);
	}
}

void REPLAY::BuildFuncMem(FUNCTION_MEMORY * &fmem, int & fnm)
{
	if (fmem != NULL)
		delete [] fmem;
	
	ifstream cl;
	cl.open(settings.GetFullDataPath("lists/control_list").c_str());
	int counter = 0;
	string junk = utility.sGetLine(cl);
	while (!cl.eof())
	{
		counter++;
		junk = utility.sGetLine(cl);
	}
	fnm = counter / 2;
	cl.close();
	cl.clear();
	cl.open(settings.GetFullDataPath("lists/control_list").c_str());
	fmem = new FUNCTION_MEMORY [fnm];
	int i;
	for (i = 0; i < fnm; i++)
	{
		junk = utility.sGetLine(cl);
		fmem[i].func_name = utility.sGetLine(cl);
		fmem[i].oldval = -1337.0;
		fmem[i].held = false;
		fmem[i].active = false;
	}
	//cout << controlinfo.num_tokens << endl;
	cl.close();
}

float REPLAY::Recording()
{
	if (!replay)
		return -1;
	
	return (float) num_packets / (float) MAX_PACKETS;
}

//extern void SelectCar(string cfile, bool trim);
extern void LoadWorld();

void REPLAY::PlayStart(string replayname)
{
	PlayStart(replayname, false);
}

void REPLAY::PlayStart(string replayname, bool ngc)
{	
	if (state.GetGameState() != STATE_PLAYING && state.GetGameState() != STATE_INITIALMENU && state.GetGameState() != STATE_STAGING)
		return;
	
	ghostcar = ngc;
	
	if (state.GetGameState() == STATE_INITIALMENU)
		state.SetGameState(STATE_PLAYING);
	
	if (state.GetGameMode() == MODE_TIMETRIAL)
		state.SetGameState(STATE_STAGING);
	
	if (replay)
		Stop();
	
	//cout << sizeof(REPLAY_PACKET) << endl;
	
	rf = fopen((settings.GetSettingsDir() + "/replays/" + replayname + ".vdr").c_str(), "rb");
	
	if (rf == NULL)
		return;
	
	//BuildFuncMem();
	
	cur_packet = 0;
	cur_state = 0;
	
	REPLAY_PACKET tpacket;
	
	//check the file format version
	string ver = REPLAY_VERSION;
	char vs[ver.length()+1];
	fread(&vs, 1, ver.length(), rf);
	vs[ver.length()] = '\0';
	if (strcmp(vs, ver.c_str()) != 0)
	{
		cout << "incompatible replay version: " << vs << endl;
		fclose(rf);
		return;
	}

	//read the embedded car
	int ln;
	fread(&ln, 1, sizeof(ln), rf);
	char carname[256];
	fread(carname, sizeof(char), ln, rf);
	carname[ln] = '\0';
	replaycar = carname;
	
	//read the embedded car paint
	int paint = 0;
	fread(&paint, sizeof(int), 1, rf);
	replaypaint = paint;
	
	//read the embedded track
	fread(&ln, 1, sizeof(ln), rf);
	char trackname[256];
	fread(trackname, sizeof(char), ln, rf);
	trackname[ln] = '\0';
	state.SetTrackName(trackname);
	
	//car_paint = paint;
	state.SetCarPaint(0, paint);
	
	//change cars
	state.SetCarName(0, carname);
	
	//reload the world
	LoadWorld();
	
	timer.Reset();
	
	//build a function cache
	if (funcmem != NULL)
		delete [] funcmem;
	
	fread(&fnum, 1, sizeof(int), rf);
	funcmem = new FUNCTION_MEMORY [fnum];
	int i;
	for (i = 0; i < fnum; i++)
	{
		char fin[FUNCTION_CHARS];
		fread(fin, 1, FUNCTION_CHARS, rf);
		funcmem[i].func_name = fin;
		funcmem[i].oldval = 0.0;
		funcmem[i].held = false;
		funcmem[i].active = false;
	}
	
	playing = true;
	
	//load up packets
	num_packets = 0;
	num_states = 0;
	while (!feof(rf))
	{
		unsigned char nextfunc;
		fread(&nextfunc, sizeof(unsigned char), 1, rf);
		if (!feof(rf))
		{
			if (nextfunc == FUNCTION_STATEINFO)
			{
				//fseek(rf, -1, SEEK_CUR);
				state_mem[num_states].ReadFromFile(rf);
				num_states++;
			}
			else
			{
				fseek(rf, -1, SEEK_CUR);
				fread(&tpacket, sizeof(REPLAY_PACKET), 1, rf);
				packet[num_packets] = tpacket;
				playend = packet[num_packets].time;
				num_packets++;
			}
		}
	}
	
	if (DBG_REPLAY)
		cout << num_packets << " packets, " << num_states << " states." << endl;
	
	ResetWorld(true);
	
	timeindex = 0;
	PlayProcessFrame();
}

void REPLAY::PlayStop()
{
	if (Playing() == -1)
		return;
	
	state_to_load = false;
	
	if (funcmem != NULL)
	{
		delete [] funcmem;
		funcmem = NULL;
	}
	
	playing = false;
	
	fclose(rf);
}

float REPLAY::Playing()
{
	if (!playing)
		return -1;
	
	return (float) timeindex / (float) playend;
}

int REPLAY::GetNumFuncs()
{
	return fnum;
}

void REPLAY::PlayIncrement()
{
	if (cur_packet >= num_packets)
	{
		//cout << "replay complete" << endl;
		PlayStop();
		return;
	}
	
	timeindex += FrameTime();
	
	PlayProcessFrame();
}

void REPLAY::PlayProcessFrame()
{	
	if (DBG_REPLAY && timeindex - FrameTime()/2.0 < packet[cur_packet].time && timeindex + FrameTime()/2.0 > packet[cur_packet].time)
		//cout << packet[cur_packet].time << "," << timeindex << endl;
		printf("%f,%f\n", packet[cur_packet].time, timeindex);
	
	int i;
	for (i = 0; i < fnum; i++)
	{
		if (!funcmem[i].held && funcmem[i].active)
			funcmem[i].active = false;
	}
	
	state_to_load = false;
	
	//while (cur_state < num_states && timeindex - FrameTime()/2.0 < state_mem[cur_state].time && timeindex + FrameTime()/2.0 > state_mem[cur_state].time)
	/*while (cur_state < num_states && timeindex + FrameTime()/2.0 > state_mem[cur_state].time)
	{
		//loadstate = cur_state;
		state_to_load = true;
		loadstate.CopyFrom(state_mem[cur_state]);
		
		for (i = 0; i < fnum; i++)
		{
			funcmem[i].oldval = loadstate.funcmem[i].oldval;
			funcmem[i].newval = loadstate.funcmem[i].newval;
			funcmem[i].held = loadstate.funcmem[i].held;
			funcmem[i].active = loadstate.funcmem[i].active;
			funcmem[i].lastupdateat = loadstate.funcmem[i].lastupdateat;
		}
		
		//cout << loadstate << "/" << num_states << endl;
		//cout << state_mem[cur_state].time << endl;
		cur_state++;
	}*/
	
	CARSTATE * nextstate = GetNextState(timeindex + FrameTime()/2.0);
	while (nextstate != NULL)
	{
		state_to_load = true;
		loadstate.CopyFrom(*nextstate);
		
		for (i = 0; i < fnum; i++)
		{
			funcmem[i].oldval = loadstate.funcmem[i].oldval;
			funcmem[i].newval = loadstate.funcmem[i].newval;
			funcmem[i].held = loadstate.funcmem[i].held;
			funcmem[i].active = loadstate.funcmem[i].active;
			funcmem[i].lastupdateat = loadstate.funcmem[i].lastupdateat;
		}
		
		nextstate = GetNextState(timeindex + FrameTime()/2.0);
	}
	
	//while (cur_packet < num_packets && timeindex - FrameTime()/2.0 < packet[cur_packet].time && timeindex + FrameTime()/2.0 > packet[cur_packet].time)
	/*while (cur_packet < num_packets && timeindex + FrameTime()/2.0 > packet[cur_packet].time)
	{
		//cout << timeindex << endl;
		
		//string curfunc = packet[cur_packet].dofunction;
		//if (CHAR_FUNCNUM < 200)
		{
			string curfunc = funcmem[packet[cur_packet].chardata[CHAR_FUNCNUM]].func_name;
		
			int funcidx = 0;
			for (i = 0; i < fnum; i++)
			{
				if (funcmem[i].func_name == curfunc)
					funcidx = i;
			}
			
			funcmem[funcidx].newval = packet[cur_packet].val;
			if (packet[cur_packet].chardata[CHAR_STATE] == 0)
				funcmem[funcidx].held = false;
			if (packet[cur_packet].chardata[CHAR_STATE] == 1)
				funcmem[funcidx].held = true;
			
			if (packet[cur_packet].chardata[CHAR_STATE] == 2)
				funcmem[funcidx].active = false;
			else
				funcmem[funcidx].active = true;
		}
		
		cur_packet++;
	}*/
	
	REPLAY_PACKET * nextpacket = GetNextPacket(timeindex + FrameTime()/2.0);
	while (nextpacket != NULL)
	{
		string curfunc = funcmem[nextpacket->chardata[CHAR_FUNCNUM]].func_name;
	
		int funcidx = 0;
		for (i = 0; i < fnum; i++)
		{
			if (funcmem[i].func_name == curfunc)
				funcidx = i;
		}
		
		funcmem[funcidx].newval = nextpacket->val;
		if (nextpacket->chardata[CHAR_STATE] == 0)
			funcmem[funcidx].held = false;
		if (nextpacket->chardata[CHAR_STATE] == 1)
			funcmem[funcidx].held = true;
		
		if (nextpacket->chardata[CHAR_STATE] == 2)
			funcmem[funcidx].active = false;
		else
			funcmem[funcidx].active = true;
		
		nextpacket = GetNextPacket(timeindex + FrameTime()/2.0);
	}
}

FUNCTION_MEMORY REPLAY::GetFunc(int idx)
{
	return funcmem[idx];
}

int REPLAY::GetFuncIdx(string funcname)
{
	int i;
	int idx = -1;
	for (i = 0; i < fnum; i++)
	{
		if (funcmem[i].func_name == funcname)
			idx = i;
	}
	
	if (idx < 0)
	{
		cout << "Error in GetFuncIdx: " << funcname << endl;
		return 0;
	}
	else
		return idx;
}

double REPLAY::GetTime()
{
	if (replay || playing)
		return timeindex;
	else
		return 0.0;
}

void CARSTATE::CopyFrom(CARSTATE other)
{
	time = other.time;
	chassispos = other.chassispos;
	chassisorientation = other.chassisorientation;
	chassisvel = other.chassisvel;
	chassisangvel = other.chassisangvel;
	int i;
	for (i = 0; i < 4; i++)
	{
		suspdisp[i] = other.suspdisp[i];
		suspcompvel[i] = other.suspcompvel[i];
		tirespeed[i] = other.tirespeed[i];
	}
	gear = other.gear;
	enginespeed = other.enginespeed;
	clutchspeed = other.clutchspeed;
	enginedrag = other.enginedrag;
	
	segment = other.segment;
	
	if (funcmem != NULL)
	{
		delete [] funcmem;
		funcmem = NULL;
	}
	
	fnum = other.fnum;
	funcmem = new FUNCTION_MEMORY_SYNC [fnum];
	for (i = 0; i < fnum; i++)
	{
		funcmem[i].oldval = other.funcmem[i].oldval;
		funcmem[i].newval = other.funcmem[i].newval;
		funcmem[i].held = other.funcmem[i].held;
		funcmem[i].active = other.funcmem[i].active;
		funcmem[i].lastupdateat = other.funcmem[i].lastupdateat;
	}
}

void REPLAY::RememberCarState()
{
	state_mem[num_states].CopyFrom(curstate);
	/*state_mem[num_states].fnum = fnum;
	if (state_mem[num_states].funcmem != NULL)
	{
		delete [] state_mem[num_states].funcmem;
		state_mem[num_states].funcmem = NULL;
	}
	state_mem[num_states].funcmem = new FUNCTION_MEMORY_SYNC [fnum];
	int i;
	for (i = 0; i < fnum; i++)
	{
		state_mem[num_states].funcmem->oldval = funcmem->oldval;
		state_mem[num_states].funcmem->newval = funcmem->newval;
		state_mem[num_states].funcmem->held = funcmem->held;
		state_mem[num_states].funcmem->active = funcmem->active;
		state_mem[num_states].funcmem->lastupdateat = funcmem->lastupdateat;
	}*/
	
	num_states++;
	
	if (num_states >= MAX_STATES)
	{
		cout << "Maximum number of replay states exceeded, replay is too long" << endl;
		num_states = MAX_STATES-1;
	}
}

void CARSTATE::WriteToFile(FILE * fout)
{
	fwrite(&time, sizeof(double), 1, fout);
	WriteVector(chassispos, fout);
	WriteMatrix(chassisorientation, fout);
	WriteVector(chassisvel, fout);
	WriteVector(chassisangvel, fout);
	
	int i;
	for (i = 0; i < 4; i++)
	{
		fwrite(&suspdisp[i], sizeof(double), 1, fout);
		fwrite(&suspcompvel[i], sizeof(double), 1, fout);
		WriteVector(whlangvel[i], fout);
		fwrite(&tirespeed[i], sizeof(double), 1, fout);
	}
	
	fwrite(&gear, sizeof(int), 1, fout);
	fwrite(&enginespeed, sizeof(double), 1, fout);
	fwrite(&clutchspeed, sizeof(double), 1, fout);
	fwrite(&enginedrag, sizeof(double), 1, fout);
	
	fwrite(&segment, sizeof(int), 1, fout);
	
	//write a function state block
	fwrite(&fnum, sizeof(int), 1, fout);
	
	for (i = 0; i < fnum; i++)
	{
		fwrite(&(funcmem[i]), sizeof(struct FUNCTION_MEMORY_SYNC), 1, fout);
	}
}

void CARSTATE::WriteVector(Vamos_Geometry::Three_Vector wv, FILE * fout)
{
	fwrite(&wv[0], sizeof(double), 1, fout);
	fwrite(&wv[1], sizeof(double), 1, fout);
	fwrite(&wv[2], sizeof(double), 1, fout);
}

void CARSTATE::WriteMatrix(Vamos_Geometry::Three_Matrix wv, FILE * fout)
{
	fwrite(&wv[0][0], sizeof(double), 1, fout);
	fwrite(&wv[0][1], sizeof(double), 1, fout);
	fwrite(&wv[0][2], sizeof(double), 1, fout);
	
	fwrite(&wv[1][0], sizeof(double), 1, fout);
	fwrite(&wv[1][1], sizeof(double), 1, fout);
	fwrite(&wv[1][2], sizeof(double), 1, fout);
	
	fwrite(&wv[2][0], sizeof(double), 1, fout);
	fwrite(&wv[2][1], sizeof(double), 1, fout);
	fwrite(&wv[2][2], sizeof(double), 1, fout);
}

void CARSTATE::ReadFromFile(FILE * fin)
{
	fread(&time, sizeof(double), 1, fin);
	ReadVector(chassispos, fin);
	ReadMatrix(chassisorientation, fin);
	ReadVector(chassisvel, fin);
	ReadVector(chassisangvel, fin);
	
	int i;
	for (i = 0; i < 4; i++)
	{
		fread(&suspdisp[i], sizeof(double), 1, fin);
		fread(&suspcompvel[i], sizeof(double), 1, fin);
		ReadVector(whlangvel[i], fin);
		fread(&tirespeed[i], sizeof(double), 1, fin);
	}
	
	fread(&gear, sizeof(int), 1, fin);
	fread(&enginespeed, sizeof(double), 1, fin);
	fread(&clutchspeed, sizeof(double), 1, fin);
	fread(&enginedrag, sizeof(double), 1, fin);
	
	fread(&segment, sizeof(int), 1, fin);
	
	//read in a function state block
	fread(&fnum, sizeof(int), 1, fin);
	
	if (funcmem != NULL)
	{
		delete [] funcmem;
		funcmem = NULL;
	}
	
	funcmem = new FUNCTION_MEMORY_SYNC [fnum];
	
	for (i = 0; i < fnum; i++)
	{
		fread(&(funcmem[i]), sizeof(struct FUNCTION_MEMORY_SYNC), 1, fin);
	}
}

void CARSTATE::ReadVector(Vamos_Geometry::Three_Vector &wv, FILE * fin)
{
	fread(&wv[0], sizeof(double), 1, fin);
	fread(&wv[1], sizeof(double), 1, fin);
	fread(&wv[2], sizeof(double), 1, fin);
}

void CARSTATE::ReadMatrix(Vamos_Geometry::Three_Matrix &wv, FILE * fin)
{
	fread(&wv.m_mat[0][0], sizeof(double), 1, fin);
	fread(&wv.m_mat[0][1], sizeof(double), 1, fin);
	fread(&wv.m_mat[0][2], sizeof(double), 1, fin);
	
	fread(&wv.m_mat[1][0], sizeof(double), 1, fin);
	fread(&wv.m_mat[1][1], sizeof(double), 1, fin);
	fread(&wv.m_mat[1][2], sizeof(double), 1, fin);
	
	fread(&wv.m_mat[2][0], sizeof(double), 1, fin);
	fread(&wv.m_mat[2][1], sizeof(double), 1, fin);
	fread(&wv.m_mat[2][2], sizeof(double), 1, fin);
}

/*int REPLAY::LoadState()
{
	if (loadstate >= num_states)
		return -1;
	else
		return loadstate;
}*/

CARSTATE * REPLAY::LoadState()
{
	if (state_to_load)
		return &loadstate;
	else
		return NULL;
}

void REPLAY::Jump(double newtime)
{	
	if (Playing() != -1)
	{
		int mult = (int)(newtime / STATE_FREQUENCY);
		newtime = (double) mult * STATE_FREQUENCY;
		
		//cout << "jump: " << newtime << "," << timeindex << endl;
		
		if (newtime > timeindex)
		{
			int prev = cur_state;
			
			//ff through states
			while (cur_state < num_states && state_mem[cur_state].time < newtime - FrameTime()/2.0)
				cur_state++;
			
			//cout << cur_state - prev << " states" << endl;
			//cout << "now on " << cur_state << endl;
			
			prev = cur_packet;
			
			//ff through packets
			while (cur_packet < num_packets && packet[cur_packet].time < newtime - FrameTime()/2.0)
				cur_packet++;
			
			//cout << cur_packet - prev << " packets" << endl;
			//cout << "now on " << cur_packet << endl;
		}
		
		if (newtime < timeindex)
		{
			//rw through states
			while (cur_state > 0 && state_mem[cur_state].time > newtime + FrameTime()/2.0)
				cur_state--;
			
			//rw through packets
			while (cur_packet > 0 && packet[cur_packet].time > newtime + FrameTime()/2.0)
				cur_packet--;
		}
		
		timeindex = newtime;
	}
}

CARSTATE * REPLAY::GetNextState(double until)
{
	if (cur_state < num_states && state_mem[cur_state].time < until)
	{
		if (DBG_REPLAY)
			cout << "Reading state at " << timeindex << endl;
		cur_state++;
		return &(state_mem[cur_state-1]);
	}
	
	return NULL;
}

REPLAY_PACKET * REPLAY::GetNextPacket(double until)
{
	if (cur_packet < num_packets && packet[cur_packet].time < until)
	{
		cur_packet++;
		return &(packet[cur_packet-1]);
	}
	
	return NULL;
}
