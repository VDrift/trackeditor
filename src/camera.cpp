// *** CAMERA.CPP ***
// Definitions for CAMERA class

#include "camera.h"

CAMERA::CAMERA()
{
	dir.LoadMultIdent();
	dir_vel.LoadMultIdent();
	
	//pos and pos_vel are VERTEX types, and so they init themselves to 0 on their own
	
	cam_mode = "free";
}

void CAMERA::MoveRelative(float mx, float my, float mz)
{
	//old system
	/*glPushMatrix();
	
	float transform_matrix[16];
	dir.GetMat(transform_matrix);
	
	glLoadMatrixf(transform_matrix);
	
	glTranslatef(mx, my, mz);
	
	glGetFloatv(GL_MODELVIEW_MATRIX, transform_matrix);
	
	//now, put the new x,y,z vals into our x,y,z
	
	glPopMatrix();*/
	
	
	//new system
	QUATERNION dirconj;

	dirconj = dir.ReturnConjugate();
	QUATERNION qtemp;
	qtemp.w = 0;
	qtemp.x = mx;
	qtemp.y = my;
	qtemp.z = mz;
	QUATERNION qout;
	qout = dirconj * qtemp * dir;
	
	position.x += qout.x;
	position.y += qout.y;
	position.z += qout.z;
}

void CAMERA::MoveRelativeConstrainY(float mx, float my, float mz)
{
	//new system
	QUATERNION dirconj;

	dirconj = dir.ReturnConjugate();
	QUATERNION qtemp;
	qtemp.w = 0;
	qtemp.x = mx;
	qtemp.y = my;
	qtemp.z = mz;
	QUATERNION qout;
	qout = dirconj * qtemp * dir;
	
	//adjust to constrain Y
	VERTEX origmove;
	origmove.x = mx; origmove.y = my; origmove.z = mz;
	float origlength = origmove.len();
	VERTEX newmove;
	newmove.x = qout.x;
	newmove.y = 0;
	newmove.z = qout.z;
	if (origlength > 0)
		newmove.Scale(origlength/newmove.len());
	
	position.x += newmove.x;
	position.y += newmove.y;
	position.z += newmove.z;
}

void CAMERA::Move(float mx, float my, float mz)
{
	//simply move the object (in world coords)
	position.x += mx;
	position.y += my;
	position.z += mz;
}

void CAMERA::Rotate(float a, float ax, float ay, float az)
{
	//NOTE: ax, ay, az is assumed to be a UNIT VECTOR!!
	
	QUATERNION qtemp;
	qtemp.SetAxisAngle(a, ax, ay, az);
	//dir.Multiply(qtemp);
	dir.PostMultiply(qtemp);
}

/*void CAMERA::RotateX(float theta)
{
	rotation.x += theta;

	GenerateTransform();
}

void CAMERA::RotateY(float theta)
{
	rotation.y += theta;

	GenerateTransform();
}

void CAMERA::RotateZ(float theta)
{
	rotation.z += theta;

	GenerateTransform();
}*/

void CAMERA::GenerateTransform()
{
	//we don't really need to do anything here anymore
	
	/*glPushMatrix();
	glLoadIdentity();
	glRotatef(rotation.y, 0.0, -1.0, 0.0);
	glRotatef(rotation.x, -1.0, 0.0, 0.0);
	glTranslatef(-position.x, -position.y, -position.z);
	glGetDoublev(GL_MODELVIEW_MATRIX, transform_matrix);
	glPopMatrix();

	glPushMatrix();
	glLoadIdentity();
	glTranslatef(-pos_vel.x, -pos_vel.y, -pos_vel.z);
	glGetDoublev(GL_MODELVIEW_MATRIX, velocity_matrix);
	glPopMatrix();*/
}

void CAMERA::Update()
{
	position = position + pos_vel;
	
	/*if (strcmp(cam_mode.c_str(), "chase") == 0)
		Chase(entities.entity_array[chase_ent]);
	else if (strcmp(cam_mode.c_str(), "cockpit") == 0)
	{
		dir = entities.entity_array[chase_ent].dir.ReturnConjugate();
		position.x = -entities.entity_array[chase_ent].x;
		position.y = -entities.entity_array[chase_ent].y;
		position.z = -entities.entity_array[chase_ent].z;
	}*/
	
	//position.DebugPrint();
	
	GenerateTransform();
}

void CAMERA::PutTransformInto(GLdouble *matrix16)
{		
	int i;
	float transform_matrix[16];
	QUATERNION qtemp;
	qtemp = dir_coupled.ReturnConjugate();
	qtemp.PostMultiply(dir);
	qtemp.GetMat(transform_matrix);
	
	for (i=0;i<16;i++)
	{
		matrix16[i] = transform_matrix[i];
	}
}

void CAMERA::PutVelocityInto(GLdouble *matrix16)
{
	int i;
	float transform_matrix[16];
	
	dir_vel.GetMat(transform_matrix);
	
	for (i=0;i<16;i++)
	{
		matrix16[i] = transform_matrix[i];
	}
}

void CAMERA::LoadVelocityIdentity()
{
	dir_vel.LoadMultIdent();

	pos_vel.x = 0;
	pos_vel.y = 0;
	pos_vel.z = 0;
}

VERTEX CAMERA::GetPosition()
{
	VERTEX temp_vec;

	temp_vec.x = (float) position.x;
	temp_vec.y = (float) position.y;
	temp_vec.z = (float) position.z;

	return temp_vec;
}

//never want to do this anymore....
/*VERTEX CAMERA::GetRotation()
{
	VERTEX temp_vec;

	temp_vec.x = (float) rotation.x;
	temp_vec.y = (float) rotation.y;
	temp_vec.z = (float) rotation.z;

	return temp_vec;
}*/

VERTEX CAMERA::GetVelocity()
{
	VERTEX temp_vec;

	temp_vec.x = (float) pos_vel.x;
	temp_vec.y = (float) pos_vel.y;
	temp_vec.z = (float) pos_vel.z;

	return temp_vec;
}

void CAMERA::SetVelocity(VERTEX new_velocity)
{
	pos_vel = new_velocity;
}

void CAMERA::Chase()
{

	/*dir = chasethis.dir.ReturnConjugate();
	
	position.x = -chasethis.x;
	position.y = -chasethis.y;
	position.z = -chasethis.z;

	MoveRelative(0,-4.0,-15.0);*/
	//Move(0,-4.0,-15.0);
}

void CAMERA::SetPosition(VERTEX newpos)
{
	position = newpos;
}

void CAMERA::ExtractFrustum()
{
   float   proj[16];
   float   modl[16];
   float   clip[16];
   float   t;

   /* Get the current PROJECTION matrix from OpenGL */
   glGetFloatv( GL_PROJECTION_MATRIX, proj );

   /* Get the current MODELVIEW matrix from OpenGL */
   glGetFloatv( GL_MODELVIEW_MATRIX, modl );

   /* Combine the two matrices (multiply projection by modelview) */
   clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
   clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
   clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
   clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

   clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
   clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
   clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
   clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

   clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
   clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
   clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
   clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

   clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
   clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
   clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
   clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

   /* Extract the numbers for the RIGHT plane */
   frustum[0][0] = clip[ 3] - clip[ 0];
   frustum[0][1] = clip[ 7] - clip[ 4];
   frustum[0][2] = clip[11] - clip[ 8];
   frustum[0][3] = clip[15] - clip[12];

   /* Normalize the result */
   t = sqrt( frustum[0][0] * frustum[0][0] + frustum[0][1] * frustum[0][1] + frustum[0][2] * frustum[0][2] );
   frustum[0][0] /= t;
   frustum[0][1] /= t;
   frustum[0][2] /= t;
   frustum[0][3] /= t;

   /* Extract the numbers for the LEFT plane */
   frustum[1][0] = clip[ 3] + clip[ 0];
   frustum[1][1] = clip[ 7] + clip[ 4];
   frustum[1][2] = clip[11] + clip[ 8];
   frustum[1][3] = clip[15] + clip[12];

   /* Normalize the result */
   t = sqrt( frustum[1][0] * frustum[1][0] + frustum[1][1] * frustum[1][1] + frustum[1][2] * frustum[1][2] );
   frustum[1][0] /= t;
   frustum[1][1] /= t;
   frustum[1][2] /= t;
   frustum[1][3] /= t;

   /* Extract the BOTTOM plane */
   frustum[2][0] = clip[ 3] + clip[ 1];
   frustum[2][1] = clip[ 7] + clip[ 5];
   frustum[2][2] = clip[11] + clip[ 9];
   frustum[2][3] = clip[15] + clip[13];

   /* Normalize the result */
   t = sqrt( frustum[2][0] * frustum[2][0] + frustum[2][1] * frustum[2][1] + frustum[2][2] * frustum[2][2] );
   frustum[2][0] /= t;
   frustum[2][1] /= t;
   frustum[2][2] /= t;
   frustum[2][3] /= t;

   /* Extract the TOP plane */
   frustum[3][0] = clip[ 3] - clip[ 1];
   frustum[3][1] = clip[ 7] - clip[ 5];
   frustum[3][2] = clip[11] - clip[ 9];
   frustum[3][3] = clip[15] - clip[13];

   /* Normalize the result */
   t = sqrt( frustum[3][0] * frustum[3][0] + frustum[3][1] * frustum[3][1] + frustum[3][2] * frustum[3][2] );
   frustum[3][0] /= t;
   frustum[3][1] /= t;
   frustum[3][2] /= t;
   frustum[3][3] /= t;

   /* Extract the FAR plane */
   frustum[4][0] = clip[ 3] - clip[ 2];
   frustum[4][1] = clip[ 7] - clip[ 6];
   frustum[4][2] = clip[11] - clip[10];
   frustum[4][3] = clip[15] - clip[14];

   /* Normalize the result */
   t = sqrt( frustum[4][0] * frustum[4][0] + frustum[4][1] * frustum[4][1] + frustum[4][2] * frustum[4][2] );
   frustum[4][0] /= t;
   frustum[4][1] /= t;
   frustum[4][2] /= t;
   frustum[4][3] /= t;

   /* Extract the NEAR plane */
   frustum[5][0] = clip[ 3] + clip[ 2];
   frustum[5][1] = clip[ 7] + clip[ 6];
   frustum[5][2] = clip[11] + clip[10];
   frustum[5][3] = clip[15] + clip[14];

   /* Normalize the result */
   t = sqrt( frustum[5][0] * frustum[5][0] + frustum[5][1] * frustum[5][1] + frustum[5][2] * frustum[5][2] );
   frustum[5][0] /= t;
   frustum[5][1] /= t;
   frustum[5][2] /= t;
   frustum[5][3] /= t;
}
