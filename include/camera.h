#ifndef _CAMERA_H

#include <stdio.h>			// Header File For Standard Input/Output
#include <GL/gl.h>
#include <GL/glu.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <cassert>
//#include "coldet.h"			// Header File For Collision Detection Library
//#include "mesh.h"
#include "quat.h"
//#include "entity.h"
//#include "entitylist.h"

using namespace std;

class CAMERA
{
private:
	//OLD NEWZ
	GLdouble transform_matrix[16];
	GLdouble velocity_matrix[16];

	void GenerateTransform();

public:
	CAMERA();

	VERTEX position;
	VERTEX pos_vel;

	//HOT NEWZ
	QUATERNION dir;
	QUATERNION dir_vel;
	QUATERNION dir_coupled;

	void Update();

	//OLD AND BUSTED
	/*void RotateX(float theta);
	void RotateY(float theta);
	void RotateZ(float theta);*/

	//NEW HOTNESS
	void Rotate(float a, float ax, float ay, float az);
	
	void MoveRelative(float mx, float my, float mz);

	//for FPS movement when we don't want to move up and down
	void MoveRelativeConstrainY(float mx, float my, float mz);

	void Move(float mx, float my, float mz);
	
	void PutTransformInto(GLdouble *matrix16);
	void PutVelocityInto(GLdouble *matrix16);

	void LoadVelocityIdentity();
	
	VERTEX GetPosition();
	void SetPosition(VERTEX newpos);
	VERTEX GetVelocity();
	//VERTEX GetRotation();

	void SetVelocity(VERTEX new_velocity);
	
	void Chase();
	
	//what camera mode we're in
	string cam_mode;
	int chase_ent;
	
	//frustum information
	float frustum[6][4];
	void ExtractFrustum();
};

#define _CAMERA_H
#endif
