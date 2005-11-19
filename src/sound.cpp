/***************************************************************************
 *            sound.cpp
 *
 *  Fri May 13 19:02:21 2005
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
 
#include "sound.h"

void SOUNDMANAGER::LoadAllSoundFiles()
{
	if (disable)
		return;
	
	//LoadSoundFile(settings.GetDataDir() + "/sounds/engine.wav");
	LoadSoundFile(settings.GetFullDataPath("sounds/tire_squeal.wav"));
	
	string fname = settings.GetFullDataPath("cars/car_list.txt");
	ifstream cfile;
	cfile.open(fname.c_str());
	
	if (cfile)
	{
		string cftemp = utility.sGetLine(cfile);
		while (!cfile.eof())
		{
			LoadSoundFile(settings.GetFullDataPath("cars/"+cftemp+"/engine.wav"));
			//cout << "Loaded " << cftemp << endl;
			cftemp = utility.sGetLine(cfile);
		}
		cfile.close();
	}
}

#ifdef HAVE_OPENAL

void SOUNDMANAGER::SetListenerVel(VERTEX vel)
{
	if (disable)
		return;
	
	vel.Scale(VELOCITY_FACTOR);
	alListenerfv(AL_VELOCITY, vel.v3());
}

void SOUNDMANAGER::SetListenerPos(VERTEX pos)
{
	if (disable)
		return;
	
	pos.Scale(DISTANCE_FACTOR);
	alListenerfv(AL_POSITION, pos.v3());
}

void SOUNDMANAGER::SetListenerOrientation(VERTEX at, VERTEX up)
{
	if (disable)
		return;
	
	float f[6];
	f[0] = at.x;
	f[1] = at.y;
	f[2] = at.z;
	f[3] = up.x;
	f[4] = up.y;
	f[5] = up.z;
	
	alListenerfv(AL_ORIENTATION, f);
}

void SOUNDMANAGER::Load()
{
	if (disable)
		return;
	
	alutInit(NULL, 0);
	alGetError();
	
	
	// Position of the Listener.
	//ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };
	
	// Velocity of the Listener.
	//ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };
	
	// Orientation of the Listener. (first 3 elements are "at", second 3 are "up")
	// Also note that these should be units of '1'.
	//ALfloat ListenerOri[] = { 0.0, 0.0, -1.0,  0.0, 1.0, 0.0 };

	//set all sources to free
	
	buffers = new ALuint [MAX_BUFFERS];
	buf_fn = new string [MAX_BUFFERS];
	
	int i;
	for (i = 0; i < MAX_SOURCES; i++)
	{
		source_free[i] = true;
		
		buf_fn[i] = "";
	}
	
	buf_cur = 0;

	// Do another error check and return.

	//if(alGetError() == AL_NO_ERROR)
		//return AL_TRUE;

	//return AL_FALSE;

	VERTEX p;
	SetListenerPos(p);
	SetListenerVel(p);
	//alListenerfv(AL_POSITION,    ListenerPos);
	//alListenerfv(AL_VELOCITY,    ListenerVel);
	//alListenerfv(AL_ORIENTATION, ListenerOri);
	VERTEX u;
	u.y = 1;
	p.z = -1;
	SetListenerOrientation(p, u);
	
	//alSourcePlay(sources[0]);
	
	LoadAllSoundFiles();
	
	//int sid = NewSource("engine.wav");
	//PlaySource(sid);
	
	alDopplerFactor (0.002);
	alDistanceModel (AL_INVERSE_DISTANCE);
}

void SOUNDMANAGER::PlaySource(int idx)
{
	if (disable)
		return;
	
	//alSourceQueueBuffers(sources[idx], 2, buffers);
	alSourcePlay(sources[idx]);
}

SOUNDMANAGER::SOUNDMANAGER()
{
	disable = false;
	master_volume = 1.0;
}

extern bool verbose_output;
SOUNDMANAGER::~SOUNDMANAGER()
{
	if (verbose_output)
		cout << "sound deinit" << endl;
	
	if (!disable)
	{
		int i;
		
		for (i = 0; i < buf_cur; i++)
			alDeleteBuffers(1, &buffers[i]);
		
		//alDeleteBuffers(1, &buffers[0]);
		//alDeleteSources(1, &sources[0]);
		
		for (i = 0; i < MAX_SOURCES; i++)
		{
			if (!source_free[i])
			alDeleteSources(1, &sources[i]);	
		}
		
		// Destroy the sound context and device (Gentoo)
		mSoundContext = alcGetCurrentContext();
		mSoundDevice = alcGetContextsDevice( mSoundContext );
		alcDestroyContext( mSoundContext );
		if ( mSoundDevice)
			alcCloseDevice( mSoundDevice );

		alutExit();
		
		delete [] buffers;
		delete [] buf_fn;
	}
}

ALuint SOUNDMANAGER::NewSource(string buffername)
{
	if (disable)
		return 0;
	
	int i;
	for (i = 0; i < MAX_SOURCES; i++)
	{
		if (source_free[i])
		{
			source_free[i] = false;

			//cout << "old source: " << sources[i] << endl;
			
			// Bind the buffer with the source.
			alGenSources(1, &sources[i]);
			
			// Position of the source sound.
			ALfloat SourcePos[] = { 0.0, 0.0, 0.0 };
			// Velocity of the source sound.
			ALfloat SourceVel[] = { 0.0, 0.0, 0.0 };
			
			int idx, m;
			idx = -1;
			for (m = 0; m < MAX_BUFFERS; m++)
			{
				if (buf_fn[m] == buffername)
					idx = m;
			}
			
			if (idx == -1)
			{
				cout << "Cannot find sound file " << buffername << endl;
			}
			else
			{
				//cout << idx << endl;
				alSourcei (sources[i], AL_BUFFER,   buffers[idx]   );
				alSourcef (sources[i], AL_PITCH,    1.0      );
				alSourcef (sources[i], AL_GAIN,     1.0      );
				alSourcefv(sources[i], AL_POSITION, SourcePos);
				alSourcefv(sources[i], AL_VELOCITY, SourceVel);
				alSourcei (sources[i], AL_LOOPING,  AL_TRUE     );
				//alSourcei (sources[i], AL_LOOPING,  AL_FALSE     );
				
				SetGain(i, 1.0);
			}
			//cout << "new source: " << sources[i] << endl;
			PlaySource(i);
			return i;
		}
	}
	
	cout << "Out of sound sources." << endl;
	return 0;
}

void SOUNDMANAGER::LoadSoundFile(string filename)
{
	if (disable)
		return;
	
	ALenum format;
	ALsizei size;
	ALvoid* data;
	ALsizei freq;
	ALboolean loop;
	
	// Load wav data into a buffer.
	

	//if(alGetError() != AL_NO_ERROR)
		//return AL_FALSE;
	
	//string sfile = settings.GetDataDir() + "/sounds/engine.wav";
	
#ifdef __APPLE__
	alutLoadWAVFile((ALbyte*)(filename).c_str(), &format, &data, &size, &freq);
#else
	alutLoadWAVFile((ALbyte*)(filename).c_str(), &format, &data, &size, &freq, &loop);
#endif
	//bool err = LoadWave(filename, &format, &data, &size, &freq, &loop);
	//if (filename == settings.GetDataDir() + "/sounds/engine.wav")
	{
		//cout << loop << endl;
		//cout << size << ", " << freq << endl;
		/*FILE * ro;
		ro = fopen("out.raw", "wb");
		fwrite ( data, 1, size, ro );
		fclose(ro);*/
	}
	//if (!err)
	{
		alGenBuffers(1, &buffers[buf_cur]);
		alBufferData(buffers[buf_cur], format, data, size, freq);
		//UnloadWave(data);
		alutUnloadWAV(format, data, size, freq);
		buf_fn[buf_cur] = filename;
	
		buf_cur++;
	}
}

void SOUNDMANAGER::SetPitch(int sid, float pitch)
{
	if (disable)
		return;
	
	alSourcef (sources[sid], AL_PITCH, pitch);
}

void SOUNDMANAGER::SetGain(int sid, float gain)
{
	if (disable)
		return;

	gain *= master_volume;
	
	alSourcef (sources[sid], AL_MAX_GAIN, gain);
	alSourcef (sources[sid], AL_GAIN, gain);
	//alSourcef (sources[sid], AL_MIN_GAIN, gain);
}

void SOUNDMANAGER::SetPos(int sid, VERTEX pos)
{
	if (disable)
		return;
	
	pos.Scale(DISTANCE_FACTOR);
	//cout << sources[sid] << endl;
	if (alIsSource(sources[sid]))
	{
		alSourcefv(sources[sid], AL_POSITION, pos.v3());
	}
	else
	{
	}
}

void SOUNDMANAGER::SetVel(int sid, VERTEX vel)
{
	if (disable)
		return;
	
	vel.Scale(VELOCITY_FACTOR);
	if (alIsSource(sources[sid]))
		alSourcefv(sources[sid], AL_VELOCITY, vel.v3());
}

void SOUNDMANAGER::MuteAll()
{
	if (disable)
		return;
	
	int i;
	for (i = 0; i < MAX_SOURCES; i++)
	{
		if (!source_free[i])
		{
			SetGain(i, 0.0f);
		}
	}
}

void SOUNDMANAGER::UnMuteAll()
{
	
}

void SOUNDMANAGER::DisableAllSound()
{
	disable = true;
}

void SOUNDMANAGER::EnableAllSound()
{
	disable = false;
}
/*
bool SOUNDMANAGER::LoadWave(string fname, ALenum *format, ALvoid **data, ALsizei *size,
	ALsizei *freq, ALboolean *loop)
{
    FILE *fp;
	
	bool err = false;

    fp = fopen(fname.c_str(),"rb");
    if (fp)
    {
        ALbyte id[4], *sound_buffer; //four bytes to hold 'RIFF'
        ALint fsize; //32 bit value to hold file size
        ALshort format_tag, channels, block_align, bits_per_sample; //our 16 values
        ALint format_length, sample_rate, avg_bytes_sec;// data_size;//, i; //our 32 bit values
		ALint data_size;
		fsize = 0;
        fread(id, sizeof(ALbyte), 4, fp); //read in first four bytes
        if (!strcmp((const char*)id, "RIFF"))
        { //we had 'RIFF' let's continue
            fread(&fsize, sizeof(ALint), 1, fp); //read in 32bit size value
            fread(id, sizeof(ALbyte), 4, fp); //read in 4 byte string now
            if (!strcmp((const char*)id,"WAVE"))
            { //this is probably a wave file since it contained "WAVE"
                fread(id, sizeof(ALbyte), 4, fp); //read in 4 bytes "fmt ";
                fread(&format_length, sizeof(ALint),1,fp);
                fread(&format_tag, sizeof(ALshort), 1, fp); //check mmreg.h (i think?) for other 
                                                              // possible format tags like ADPCM
                fread(&channels, sizeof(ALshort),1,fp); //1 mono, 2 stereo
                fread(&sample_rate, sizeof(ALint), 1, fp); //like 44100, 22050, etc...
                fread(&avg_bytes_sec, sizeof(ALshort), 1, fp); //probably won't need this
                fread(&block_align, sizeof(ALshort), 1, fp); //probably won't need this
                fread(&bits_per_sample, sizeof(ALshort), 1, fp); //8 bit or 16 bit file?
                fread(id, sizeof(ALbyte), 4, fp); //read in 'data'
                fread(&data_size, sizeof(ALbyte), 4, fp); //how many bytes of sound data we have
                //sound_buffer = (BYTE *) malloc (sizeof(BYTE) * data_size); //set aside sound buffer space
				cout << "boo " << fsize << ", " << data_size << endl;
				sound_buffer = new ALbyte[data_size];
                cout << "boo" << endl;
				fread(sound_buffer, sizeof(ALbyte), data_size, fp); //read in our whole sound data chunk
				cout << "boo" << endl;
				*data = sound_buffer;
				cout << "boo" << endl;
				if (bits_per_sample == 16 && channels == 2)
					*format = AL_FORMAT_STEREO16;
				if (bits_per_sample == 8 && channels == 2)
					*format = AL_FORMAT_STEREO8;
				if (bits_per_sample == 16 && channels == 1)
					*format = AL_FORMAT_MONO16;
				else
					*format = AL_FORMAT_MONO8;
				
				*size = data_size;
				*freq = sample_rate;
				cout << "boo" << endl;
            }
            else
			{
                printf("Error: RIFF file but not a wave file: %s\n", fname.c_str());
				err = true;
			}
        }
        else
		{
            printf("Error: not a RIFF file: %s\n", fname.c_str());
			err = true;
		}
    }
	else
	{
		printf("Can't find sound file: %s\n", fname.c_str());
		err = true;
	}
cout << "milk" << endl;
	return err;
}

void SOUNDMANAGER::UnloadWave(ALvoid* data)
{
	delete [] (ALbyte*)data;
}*/

void SOUNDMANAGER::StopSource(int sid)
{
	alSourceStop(sources[sid]);
	alDeleteSources(1, &sources[sid]);
	source_free[sid] = true;
}

void SOUNDMANAGER::SetPosVel(int sid, VERTEX coord, VERTEX tvel)
{
	//cout << sid << endl;
	//coord.DebugPrint();
	SetPos(sid, coord);
	SetVel(sid, tvel);
}

void SOUNDMANAGER::SetListener(VERTEX campos, VERTEX camvel, VERTEX at, VERTEX up)
{
	//campos.DebugPrint();
	SetListenerPos(campos);
	SetListenerVel(camvel);
	SetListenerOrientation(at, up);
}

#else // !HAVE_OPENAL

void SOUNDMANAGER::Load()
{
	if (disable)
		return;
	
	FSOUND_SetMinHardwareChannels(SOUND_CHANNELS);
	FSOUND_Init(44100, SOUND_CHANNELS, 0);
	//if (verbose_output)
	//cout << "FMOD Init: " << FMOD_ErrorString(FSOUND_GetError()) << endl;
	
	//FSOUND_3D_SetDistanceFactor(DISTANCE_FACTOR);
	FSOUND_3D_SetRolloffFactor(DISTANCE_ROLLOFF);
	
	int i;
	for (i = 0; i < MAX_BUFFERS; i++)
	{	
		buf_fn[i] = "";
	}
	
	buf_cur = 0;

	LoadAllSoundFiles();
}

/*void SOUNDMANAGER::LoadAllSoundFiles()
{
	if (disable)
		return;
	
	LoadSoundFile(settings.GetDataDir() + "/sounds/engine.wav");
	LoadSoundFile(settings.GetDataDir() + "/sounds/tire_squeal.wav");
}*/

void SOUNDMANAGER::PlaySource(int idx)
{
	if (disable)
		return;
	
	cout << "PlaySource is deprecated" << endl;
}

SOUNDMANAGER::SOUNDMANAGER()
{
	disable = false;
	master_volume = 1.0;
}

extern bool verbose_output;
SOUNDMANAGER::~SOUNDMANAGER()
{
	if (verbose_output)
		cout << "sound deinit" << endl;
}

int SOUNDMANAGER::NewSource(string buffername)
{
	if (disable)
		return 0;
	
	int m, idx;
	idx = -1;
	for (m = 0; m < MAX_BUFFERS; m++)
	{
		if (buf_fn[m] == buffername)
			idx = m;
	}
	
	int handle = -1;
	
	if (idx < 0)
	{
		cout << "Sound file not loaded: " << buffername << endl;
	}
	else
	{
		handle = FSOUND_PlaySound(FSOUND_FREE, buffers[idx]);
		/*alSourcei (sources[i], AL_BUFFER,   buffers[idx]   );
		alSourcef (sources[i], AL_PITCH,    1.0      );
		alSourcef (sources[i], AL_GAIN,     1.0      );
		alSourcefv(sources[i], AL_POSITION, SourcePos);
		alSourcefv(sources[i], AL_VELOCITY, SourceVel);
		alSourcei (sources[i], AL_LOOPING,  AL_TRUE     );*/
		
		channel[tochannel(handle)] = idx;
		
		SetGain(handle, 1.0);
		
		//cout << buffername << "[" << handle << "," << idx << "]: " << tochannel(handle) << endl;
	}

	if (handle < 0)
	{
		cout << "Couldn't play sound: " << buffername << endl;
		return 0;
	}
	else
		return handle;
}

void SOUNDMANAGER::LoadSoundFile(string filename)
{
	if (disable)
		return;

	buffers[buf_cur] = FSOUND_Sample_Load(FSOUND_FREE, filename.c_str(), FSOUND_FORCEMONO | FSOUND_HW3D | FSOUND_LOOP_NORMAL, 0, 0);
	
	if (buffers[buf_cur] == NULL)
	{
		cout << "Couldn't load sound: " << filename << endl;
		cout << FMOD_ErrorString(FSOUND_GetError()) << endl;
	}
	else
	{
		int deffreq;
		int defvol;
		int defpan;
		int defpri;
		
		FSOUND_Sample_GetDefaults(buffers[buf_cur], &deffreq,
		 &defvol,
		 &defpan,
		 &defpri);
		
		defpitch[buf_cur] = deffreq;
		
		buf_fn[buf_cur] = filename;		
		buf_cur++;
	}
}

void SOUNDMANAGER::SetPitch(int sid, float pitch)
{
	if (disable)
		return;
	
	int ipitch = (int)(pitch*(float)(defpitch[channel[tochannel(sid)]]));
	
	if (ipitch < 100)
		ipitch = 100;
	else if (ipitch > 700000)
		ipitch = 700000;
	
	FSOUND_SetFrequency(sid, ipitch);
}

void SOUNDMANAGER::SetGain(int sid, float gain)
{
	if (disable)
		return;

	gain *= master_volume;
	
	if (gain > 1.0)
		gain = 1.0;
	if (gain < 0)
		gain = 0;
	int igain = (int)(gain * 255.0);
	FSOUND_SetVolume(sid, igain);
}

void SOUNDMANAGER::MuteAll()
{
	if (disable)
		return;
	
	FSOUND_SetVolume(FSOUND_ALL, 0);
}

void SOUNDMANAGER::UnMuteAll()
{
	
}

void SOUNDMANAGER::DisableAllSound()
{
	disable = true;
}

void SOUNDMANAGER::Update()
{
	FSOUND_Update();
}

void SOUNDMANAGER::SetListener(VERTEX pos, VERTEX vel, VERTEX at, VERTEX up)
{
	FSOUND_3D_Listener_SetAttributes(pos.v3(), vel.v3(), at.x, at.y, at.z, up.x, up.y, up.z);
}

void SOUNDMANAGER::SetPosVel(int sid, VERTEX pos, VERTEX vel)
{
	FSOUND_3D_SetAttributes(sid, pos.v3(), vel.v3());
}

int SOUNDMANAGER::tochannel(int handle)
{
	handle = handle << 20;
	handle = handle >> 20;
	return handle;
}

void SOUNDMANAGER::StopSource(int handle)
{
	FSOUND_StopSound(handle);
}

#endif // HAVE_OPENAL
