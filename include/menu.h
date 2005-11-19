/***************************************************************************
 *            menu.h
 *
 *  Wed Jun  1 19:32:04 2005
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
 
#ifndef _MENU_H

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
#include <list>
#include <vector>

using namespace std;

#include "quat.h"
#include "utility.h"
#include "font.h"
#include "controls.h"
#include "keyman.h"

#include "globals.h"

#define NUM_CONTROL_MAPPING 4

class MENU_CONTROLS
{
public:
	string * desc;
	string * token;
	int num_tokens;
	int xsel;
};

class MENU_DISPLAY
{
public:
	SDL_Rect * modes;
	int num_modes;

	bool bpp32;
	bool fullscreen;

	int sel;
};

class MENU_ITEM
{
public:
	string name;
	float r, g, b;
	bool selectable;
	int size;
	float spaceafter;
	string go_to;
};

class MENU_PAGE
{
public:
	MENU_PAGE();
	string name;
	int numitems;
	MENU_ITEM * item;
	float x, y;
};

class MENU
{
private:
	bool in_menu;
	bool in_display;
	GLuint backbox;
	GLuint logo;

	int sel;
	int subsel;

	string curmenu;
	list<string> lastmenu;

	void Draw2D();

	MENU_DISPLAY disp_data;

	MENU_PAGE * page;

	int numpages;
	int pageslot;

	ofstream error_log;

	void DrawDisplay();
	void DrawAutomatic();
	MENU_PAGE * curp();
	
	MENU_CONTROLS controlinfo;
	void DrawControlSetup();
	bool controlwait;
	string controlwaittype;
	void AssignKey(int kcode);
	void BuildControls();
	void ToggleControlOpts();
	int togglemode;
	
	void DrawSelectCar();
	string * cflist;
	string * cfabout;
	string * cfnames;
	int numcf;
	void BuildCFList();
	void ClearCFList();
	float rotation;
	GLint last_t;
	GLuint sphere_reflection;
	void WriteCarSelection();
	
	void LoadCar(string cfile);
	void ClearCar();
	string selectedcar;
	
	void DrawCalibrate();
	
	void DrawSelectTrack();
	void BuildTSList();
	void ClearTSList();
	string * tslist;
	string * tsabout;
	int numts;
	
	void DrawNetSetup();
	bool net_host;
	string net_ip;
	string port_input;
	bool ip_or_port;

	void DrawCustomizeCar();
	vector<string> parts;
	vector<string> parts_names;
	vector<bool> parts_enabled;
	unsigned int part_selected;
	void ReloadCarParts();
	
	bool loaded;
	
public:
	MENU();
	~MENU();

	void Load();
	void LoadPage(string pagename);
	
	bool InMenu();
	void InMenu(bool new_in_menu);

	//bool InDisplay();
	//void InDisplay(bool new_in_display);

	bool In(string menuname);
	void Go(string menuname);

	void MenuKey(SDLKey key);
	void MenuDraw();

	void Draw2D(float x1, float y1, float x2, float y2, GLuint texid, float rotation);

	void MainMenu();
	
	void AssignJoyButton(int joynum, int butnum);
	void AssignJoyAxis(int joynum, int joyaxis, float val);
	
	void UnbindSlot();
};

#define _MENU_H
#endif /* _MENU_H */
