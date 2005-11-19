#include "keyman.h"

//no-whitespace version of get string param
string sGetParamNW(ifstream &ffrom)
{
	string trashstr;
	char trashchar[1024];

	ffrom >> trashstr;
	//ffrom.getline(trashchar,1024,'\n');

	while (trashstr.c_str()[0] == '#')
	{
		ffrom.getline(trashchar, 1024, '\n');
		ffrom >> trashstr;
		//ffrom.getline(trashchar,1024,'\n');
	}

	return trashstr;
}

void KEYMAN::KeyDown(int key)
{
	keys[key] = true;
}

void KEYMAN::KeyUp(int key)
{
	keys[key] = false;
}

void KEYMAN::OneTime(int key)
{
	if (queuedepth < MAX_ONE_TIME_KEY_DEPTH - 1)
	{
		queue[queuedepth] = key;
		queuedepth++;
	}
}


KEYMAN::KEYMAN()
{
	for (int i = 0; i < 1024; i++)
	{
		keys[i] = false;
	}
	
	queuedepth = 0;
	
	error_log.open((settings.GetSettingsDir() + "/logs/keyman.log").c_str());
	
	ifstream kdb((settings.GetDataDir() + "/lists/keys").c_str());
	
	if (!kdb)
		error_log << "Couldn't find key database." << endl;
	
	num_keyrecs = utility.iGetParam(kdb);
	//error_log << num_keyrecs << endl;
	
	for (int i = 0; i < num_keyrecs; i++)
	{
		keyrec[i].name = sGetParamNW(kdb);
		//error_log << keyrec[i].name << endl;
		string nk = sGetParamNW(kdb);
		if (strlen(nk.c_str()) == 1)
			keyrec[i].key = nk.c_str()[0];
		else
			keyrec[i].key = atoi(nk.c_str());
		//error_log << keyrec[i].key << endl;
	}
	
	kdb.close();
	
	freecam = true;
}

int KEYMAN::GetKey(string kn)
{
	for (int i = 0; i < num_keyrecs; i++)
	{
		if (strcmp(kn.c_str(), keyrec[i].name.c_str()) == 0)
		{
			return keyrec[i].key;
		}
	}
	
	return '!';
}

extern bool verbose_output;
KEYMAN::~KEYMAN()
{
	if (verbose_output)
		cout << "keyman deinit" << endl;
	
	error_log.close();
}

void KEYMAN::DoOneTimeKeys(CAMERA & cam)
{
	int i;
	
	for (i = 0; i < queuedepth; i++)
	{

	}
	
	//flush queue
	queuedepth = 0;
	
	//save last key states
	for (i = 0; i < 1024; i++)
	{
		lastkeys[i] = keys[i];
	}
}

void KEYMAN::DoHeldKeys(float timefactor, float fps, CAMERA & cam)
{
	//fps camera
	
	float playermovespeedz = 6.0f;
	float playermovespeedx = 3.0f;
	
	//camera movement
	
	VERTEX moveamount;
	
	if (keys[GetKey("UP")])
		moveamount.z += playermovespeedz;
	if (keys[GetKey("PAGEUP")])
		moveamount.z += 50.0*playermovespeedz;
	if (keys[GetKey("INSERT")])
		moveamount.z -= 50.0*playermovespeedz;
	if (keys[GetKey("DOWN")])
		moveamount.z += -playermovespeedz;
	if (keys[GetKey("RIGHT")])
		moveamount.x += -playermovespeedx;
	if (keys[GetKey("LEFT")])
		moveamount.x += playermovespeedx;
	
	moveamount.Scale(3.0f);
	
	moveamount.Scale(timefactor/fps);

	if (freecam)
		cam.MoveRelative(moveamount.x, moveamount.y, moveamount.z);
	VERTEX campos = cam.GetPosition();
	cam.SetPosition(campos);
}

string KEYMAN::GetKeyName(int kc)
{
	int i;
	for (i = 0; i < num_keyrecs; i++)
	{
		if (keyrec[i].key == kc)
			return keyrec[i].name;
	}
	
	string numval;
	char tempchar[32];
	sprintf(tempchar, "KeyCode%d", kc);
	//return "UNDEFINED";
	numval = tempchar;
	return numval;
}
