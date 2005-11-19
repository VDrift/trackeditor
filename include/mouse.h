#ifndef _MOUSE_H

#include <SDL/SDL.h>
#include "camera.h"
#include "quat.h"
#include "settings.h"

#include "globals.h"

//this should be the same as the #define in terrain.h
//#define EARTH_RADIUS 50000.0
#define EARTH_RADIUS 637800.0

#define ZOOMSPEED 1.0

class MOUSE
{
private:
	float mousezoom;
	QUATERNION viewdir;
	bool mousesteering;
	bool mousebounce;
	void UpdateCamera(CAMERA & cam, int mousePos_x, int mousePos_y, float timefactor, float fps);
	void UpdateSteering(int x, int y);
	float mouse_sensitivity_x, mouse_sensitivity_y;
	float deadzone_y;
	float steer_x, steer_y;
	bool lbounce, rbounce, lastl, lastr;
	void ClearBounce() {lbounce = false; rbounce = false; lastl = false; lastr = false;}
	
public:
	MOUSE();
	void Update(CAMERA & cam, int screenw, int screenh, float timefactor, float fps);
	float GetZoom() {return mousezoom;}
	QUATERNION GetDir() {return viewdir;}
	void InitDir(QUATERNION newd) {viewdir = newd;}
	bool GetMouseControls(float * x, float * y, bool * click_l, bool * click_r);
	void SetDeadzoneY(float newydz) {if (newydz > 1) newydz = 1; if (newydz < 0) newydz = 0; deadzone_y = newydz;}
	void SetSensitivityX(float newxs) {mouse_sensitivity_x = newxs;}
	void SetSensitivityY(float newys) {mouse_sensitivity_x = newys;}
	void SetMouseEnabled(bool mouse) {mousesteering = mouse;}
	float GetDeadzoneY() {return deadzone_y;}
	float GetSensitivityX() {return mouse_sensitivity_x;}
	float GetSensitivityY() {return mouse_sensitivity_y;}
	bool GetMouseEnabled() {return mousesteering;}
	bool ButtonDown(int butnum) {return (SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(butnum));}
};

#define _MOUSE_H
#endif
