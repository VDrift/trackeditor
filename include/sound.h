/***************************************************************************
 *            sound.h
 *
 *  Fri May 13 19:01:29 2005
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
 
#ifndef _SOUND_H

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
#include <stdlib.h>

#ifdef HAVE_OPENAL
#ifdef __APPLE__
       #include <OpenAL/al.h>
       #include <OpenAL/alc.h>
       //#include <OpenAL/alu.h>
       #include <OpenAL/alut.h>
#else
       #include <AL/al.h>
       #include <AL/alc.h>
       //#include <AL/alu.h>
       #include <AL/alut.h>
#endif
#else
#include "fmod/fmod.h"
#include "fmod/fmod_errors.h"
#endif

#include "camera.h"
#include "utility.h"

//#include "backdrop.h"
//#include "weather.h"

#include "globals.h"

using namespace std;

#define MAX_BUFFERS 50

#ifdef HAVE_OPENAL

#define MAX_SOURCES 10

#define DISTANCE_FACTOR 0.025
//#define VELOCITY_FACTOR 1.0/DISTANCE_FACTOR
#define VELOCITY_FACTOR 1.5

class SOUNDMANAGER
{
private:
	// Buffers to hold sound data.
	ALuint * buffers;
	int buf_cur;
	string * buf_fn;
	
	// Sources are points of emitting sound.
	ALuint sources[MAX_SOURCES];
	bool source_free[MAX_SOURCES];

	bool muted;

	bool disable;

	//BROKEN, DO NOT USE
	/*bool LoadWave(string fname, ALenum *format, ALvoid **data, ALsizei *size,
					ALsizei *freq, ALboolean *loop);
	void UnloadWave(ALvoid* data);*/

	float master_volume;
	
public:
	// Gentoo
	ALCdevice* mSoundDevice;
	ALCcontext* mSoundContext;
	
	void Load();
	void LoadAllSoundFiles();
	void LoadSoundFile(string filename);
	~SOUNDMANAGER();
	SOUNDMANAGER();

	void SetListenerPos(VERTEX pos);
	void SetListenerVel(VERTEX vel);
	void SetListenerOrientation(VERTEX at, VERTEX up);

	void SetMasterVolume(float newvol) {master_volume = newvol; if (master_volume < 0) master_volume = 0; if (master_volume > 1) master_volume = 1;}
	float GetMasterVolume() {return master_volume;}

	ALuint NewSource(string buffername);

	void PlaySource(int idx);
	void SetPitch(int sid, float pitch);
	void SetGain(int sid, float gain);
	void SetPos(int sid, VERTEX pos);
	void SetVel(int sid, VERTEX vel);

	void MuteAll();
	void UnMuteAll();
	//ALuint GetSource(string buffername);
	
	void Update() {}
	void StopSource(int sid);

	void DisableAllSound();
	void EnableAllSound();
		
	void SetPosVel(int sid, VERTEX coord, VERTEX tvel);
	void SetListener(VERTEX campos, VERTEX camvel, VERTEX at, VERTEX up);
};
#else // !HAVE_OPENAL

#define SOUND_CHANNELS 8
#define DISTANCE_ROLLOFF 0.05

class SOUNDMANAGER
{
private:
        // Buffers to hold sound data.
        FSOUND_SAMPLE * buffers[MAX_BUFFERS];
        int buf_cur;
        string buf_fn[MAX_BUFFERS];
        int defpitch[MAX_BUFFERS];
        int channel[SOUND_CHANNELS];

        bool muted;

        bool disable;

        //BROKEN, DO NOT USE
        /*bool LoadWave(string fname, ALenum *format, ALvoid **data, ALsizei *size,
                                        ALsizei *freq, ALboolean *loop);
        void UnloadWave(ALvoid* data);*/

        int tochannel(int handle);

	// Gentoo
	float master_volume;
        
public:
        void Load();
        void LoadAllSoundFiles();
        void LoadSoundFile(string filename);
        ~SOUNDMANAGER();
        SOUNDMANAGER();

        //void SetListenerPos(VERTEX pos);
        //void SetListenerVel(VERTEX vel);
        //void SetListenerOrientation(VERTEX at, VERTEX up);
        void SetListener(VERTEX pos, VERTEX vel, VERTEX at, VERTEX up);

        int NewSource(string buffername);
        void PlaySource(int idx);
        void SetPitch(int sid, float pitch);
        void SetGain(int sid, float gain);
        void StopSource(int sid);

	// Gentoo
        void SetMasterVolume(float newvol) {master_volume = newvol; if (master_volume < 0) master_volume = 0; if (master_volume > 1) master_volume = 1;}
	float GetMasterVolume() {return master_volume;}
        
        //void SetPos(int sid, VERTEX pos);
        //void SetVel(int sid, VERTEX vel);
        void SetPosVel(int sid, VERTEX pos, VERTEX vel);

        void MuteAll();
        void UnMuteAll();
        //ALuint GetSource(string buffername);

        void DisableAllSound();
        
        void Update();
};

#endif // HAVE_OPENAL

#define _SOUND_H
#endif
