/***************************************************************************
 *            controls.h
 *
 *  Sat Jun  4 08:17:40 2005
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
 
#ifndef _CONTROLS_H

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

#include "utility.h"
#include "keyman.h"
#include "settings.h"

#include "globals.h"

#define MAX_JOYSTICKS 32
#define MAX_AXES 10

using namespace std;

enum ControllerEnum { Joy, Key, Unknown };
enum JoyTypeEnum { Button, Axis, UnknownType };

class CONTROL
{
private:
	string controlname;
	ControllerEnum controltype;

	int joynum;
	int joyaxis;
	bool joyaxisplus;
	int joybutton;
	bool joybuttonlaststate;
	JoyTypeEnum joytype;
	bool joypushdown;

	int keycode; //requires lookup in keyman database
	bool onetime;
	bool keypushdown;

public:
	void SetName(string cname) {controlname = cname;}
	string GetName(string cname) {return controlname;}
	void SetType(string type) {if (type == "joy") controltype = Joy; else if (type == "key") controltype = Key; else {controltype = Unknown; cout << "Unknown control type: " << type << endl;}}
	ControllerEnum GetType() {return controltype;}
	string GetName() {return controlname;}
	void SetKeyCode(int nkey) {keycode = nkey;}
	int GetKeyCode() {return keycode;}
	void SetOneTime(bool ot) {onetime = ot;}
	bool GetOneTime() {return onetime;}
	void SetKeyPushDown(bool npd) {keypushdown = npd;}
	bool GetKeyPushDown() {return keypushdown;}
	void SetJoyNum(int jn) {joynum = jn;}
	int GetJoyNum() {return joynum;}
	void SetJoyAxis(int ja) {joyaxis = ja;}
	void SetJoyAxisPlus(bool ap) {joyaxisplus = ap;}
	int GetJoyAxis() {return joyaxis;}
	bool GetJoyAxisPlus() {return joyaxisplus;}
	void SetJoyButton(int jb) {joybutton = jb;}
	int GetJoyButton() {return joybutton;}
	void SetJoyType(string type) {if (type == "button") joytype = Button; else if (type == "axis") joytype = Axis; else {joytype = UnknownType; cout << "Unknown joystick input type: " << type << endl;}}
	JoyTypeEnum GetJoyType() {return joytype;}
	void Dump();
	void SetJoyButtonLastState(bool ls) {joybuttonlaststate = ls;}
	bool GetJoyButtonLastState() {return joybuttonlaststate;}
	void SetJoyPushDown(bool pd) {joypushdown = pd;}
	bool GetJoyPushDown() {return joypushdown;}
};

class GAMECONTROLS
{
private:
	CONTROL * controls;
	int numcontrols;
	void ControlParseError(int line, ifstream & ffrom);
	ofstream error_log;
	string joysticktype; //wheel or joystick
	string compensation;
	string deadzone;
	float calibration[MAX_JOYSTICKS][MAX_AXES][2];
	SDL_Joystick * js[MAX_JOYSTICKS];

public:
	GAMECONTROLS();
	~GAMECONTROLS();
	void LoadControls();
	CONTROL * GetControls();
	int GetNumControls();
	string GetJoystickType() {return joysticktype;}
	void SetJoystickType(string newtype) {joysticktype = newtype;}
	void SetControls(CONTROL * newcontrols, int newnum);
	void WriteControlFile();
	string GetCompensation() {return compensation;}
	void SetCompensation(string newcomp) {compensation = newcomp;}
	string GetDeadzone() {return deadzone;}
	void SetDeadzone(string newdeadzone) {deadzone = newdeadzone;}
	
	void LoadCalibration();
	void WriteCalibration();
	float GetCalibration(int joynum, int joyaxis, bool max);
	void MinimizeCalibration();
	void SetCalibrationPoint(int joynum, int joyaxis, float val);
	
	SDL_Joystick * GetJoy(int joynum) {return js[joynum];}
	SDL_Joystick ** Get_js() {return js;}
	void InitJoy();
	void DeinitJoy();
};

#define _CONTROLS_H
#endif /* _CONTROLS_H */
