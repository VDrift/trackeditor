#ifndef _KEYMAN_H

#include <SDL/SDL.h>

#include "utility.h"
#include "camera.h"
#include "settings.h"
#include "globals.h"

#define MAX_ONE_TIME_KEY_DEPTH 10
#define MAX_KEY_RECS 256

#include <string>

using namespace std;

class KEYREC
{
public:
	string name;
	int key;
};

class KEYMAN
{
private:
	ofstream error_log;
	
public:
	int num_keyrecs;
	KEYREC keyrec[MAX_KEY_RECS];

	int GetKey(string kn);
	string GetKeyName(int kc);
	bool keys[1024];
	bool lastkeys[1024];

	void KeyDown(int key);
	void KeyUp(int key);

	int queuedepth;
	int queue[MAX_ONE_TIME_KEY_DEPTH];
	void OneTime(int key);

	bool freecam;
	

	KEYMAN();
	~KEYMAN();


	void DoOneTimeKeys(CAMERA & cam);
	void DoHeldKeys(float timefactor, float fps, CAMERA & cam);
};

#define _KEYMAN_H
#endif
