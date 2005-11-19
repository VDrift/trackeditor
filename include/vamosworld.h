/***************************************************************************
 *            vamosworld.h
 *
 *  Sat Mar 26 14:05:40 2005
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
 
#ifndef _VAMOSWORLD_H

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

#include "font.h"
#include "quat.h"
#include "keyman.h"
#include "camera.h"
#include "particles.h"
#include "messageq.h"
#include "terrain.h"
#include "sound.h"
#include "timer.h"
#include "controls.h"
#include "replay.h"
#include "mouse.h"
#include "trees.h"
#include "gamestate.h"
#include "multiplay.h"

#include "globals.h"

#include <vamos/world/World.h>
#include <vamos/body/Gl_Car.h>
#include <vamos/body/Car.h>
#include <vamos/body/Wheel.h>
#include <vamos/body/Fuel_Tank.h>

#define MAX_TRIM_LOOPS 5000

#define AUTO_CLUTCH_THRESH 200.0f
#define AUTO_CLUTCH_MARGIN 100.0f
#define AUTO_CLUTCH_ENGAGE_TIME 1.0f

enum CameraMode { CMChaseRigid=0, CMChase=1, CMOrbit=2, CMHood=3, CMFree=4, CMInCar=5, CMExternal=6, CMExtFollow=7 };

enum CONTROLLERTYPE { CONT_NONE=0, CONT_PLAYERLOCAL=1, CONT_PLAYERREMOTE=2, CONT_AI=-1, CONT_REPLAY=3 };

class VAMOSWORLD
{
private:
	ofstream error_log;
	
	//int focused_car;

	Vamos_World::World * world;

	void draw_track();
	void draw_cars(bool draw_interior, bool draw_focused_car);

	void ProcessControls(SDL_Joystick ** js, float timefactor, float fps);

	void DrawHUD();
	void DrawDigitalHUD();
	GLuint tachbase, tachredband, tachband, speedo, needle, fuelgauge, fgbox;
	GLuint m_ballh, m_sliderh, m_ballv, m_sliderv;
	GLuint sphere_reflection;
	GLuint car_shadow;
	//void Draw2D(float x1, float y1, float x2, float y2, GLuint texid);
	//void Draw2D(float x1, float y1, float x2, float y2, GLuint texid, float rotation);
	
	void build_track_shot();
	
	CameraMode cammode;
	
	double l_timefactor, l_fps;
	
	bool joyinfo;
	void DisplayJoyInfo();
	SDL_Joystick * joyinfo_js;
	SDL_Joystick ** joyinfo_jsarray;
	
	bool propsteer;
	
	Vamos_Track::Strip_Track* track_ptr;
	
	float steerpos;
	void steer_to(float val, float timefactor, float fps);
	void steer_set(float val, bool left);
	float steervals[2];
	void steer_commit();
	
	VERTEX cam_lastpos;
	VERTEX cam_lastpos2;
	VERTEX cam_lastvel;
	VERTEX cam_lastaccel;
	VERTEX cam_jerk;
	//VERTEX car_lastpos;
	
	int tire_source[4];
	VERTEX oldtirepos[4];
	
	int oldseg;
	
	void DoOp(Vamos_World::Car_Information * c, string dofunction, float dovalue, float dotime, bool held, float timefactor, float fps);
	
	//used by the external camera mode
	float lastcamchange;
	VERTEX oldcampos;
	bool camneedupdate;
	
	void PhysUpdate(float dt);
	
	bool display_hud;
	bool initdone;
	
	void draw_shadows();
	
	bool CheckMouseControls();
	
	float shift_time;

public:
	VAMOSWORLD();
	~VAMOSWORLD();
	void Init (Vamos_Track::Strip_Track* track);

	void Draw();
	void DrawShadows();
	void DrawCars();
	void DrawTopLevel();
	
	void Update(float timefactor, float fps, SDL_Joystick ** js);
	
	void set_focused_car(int carnum);
	void add_car (Vamos_Body::Car* car);
	void SetCameraMode(CameraMode newmode);
	void reset();

	Vamos_Body::Car * GetPlayerCar();

	Vamos_World::Car_Information * GetCar(CONTROLLERTYPE p);
	
	void FuelPlayerCar();

	void clear_cars();
	void Trim(float frametime, SDL_Joystick ** js);

	void ToggleHUD();

void DeInit();
};

#define _VAMOSWORLD_H
#endif /* _VAMOSWORLD_H */
