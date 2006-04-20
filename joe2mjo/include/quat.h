#ifndef QUAT_H

#include <cmath>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <cassert>
#include <fstream>

using namespace std;

class VERTEX
{
private:
	//buffer for float[3] conversions
	float float3[3];
	
public:
	float x;
	float y;
	float z;
	VERTEX operator+ (VERTEX v);
	VERTEX operator- (VERTEX v);
	VERTEX();

	float len();
	VERTEX normalize();
	VERTEX cross(VERTEX b);
	void Scale(float scalar);
	VERTEX ScaleR(float scalar);
	float dot(VERTEX b);
	void Set(float nx, float ny, float nz);
	void Set(float * f3);
	VERTEX InvertR();
	float * v3();
	void zero();
	bool equals(VERTEX other);

	inline void flabs() {if (x < 0) x = -x; if (y < 0) y = -y; if (z < 0) z = -z;}

	inline float v(int i) {if (i == 2) return z; if (i == 1) return y; return x;}

	void DebugPrint();

	VERTEX interpolatewith(VERTEX other, float percent);
	
	bool nan();
};

class VERTEXD
{
private:
	//buffer for double[3] conversions
	double double3[3];
	
public:
	double x;
	double y;
	double z;
	VERTEXD operator+ (VERTEXD v);
	VERTEXD operator- (VERTEXD v);
	VERTEXD();
	VERTEXD(VERTEX other);
	VERTEXD& operator= (const VERTEX &other);

	double len();
	VERTEXD normalize();
	VERTEXD cross(VERTEXD b);
	void Scale(double scalar);
	VERTEXD ScaleR(double scalar);
	double dot(VERTEXD b);
	void Set(double nx, double ny, double nz);
	void Set(double * f3);
	void Set(float * f3);
	VERTEXD InvertR();
	double * v3();
	void zero();

	void DebugPrint();

	VERTEXD interpolatewith(VERTEXD other, double percent);
	
	bool nan();
};

class MATRIX3
{
public:
	void ProjectionMatrix(VERTEX normal);
	VERTEX Multiply(VERTEX vert);
//private:
	VERTEX row[3];
};

class QUATERNION
{
private:
	bool cachematvalid;
	float cachemat[16];
	
public:
	//the 4 vectors we need to represent the quaternion
	float w;
	float x;
	float y;
	float z;

	QUATERNION();			//the constructor calls loadmultident
	void LoadMultIdent();	//load the [1,(0,0,0)] quaternion
	float Norm();			//return the magnitude of the quaternion
	void Normalize();		//normalize the quaternion
	void GetMat(float * destmat);	//convert to a matrix and store in (float) destmat[16]
	void GetMat(GLdouble * destmat);	//convert to a matrix and store in (gldouble) destmat[16]
	void GetAxisAngle(float * aangle);	//convert to axis angle form and store in (float) aangle[4] (a,x,y,z)
	void Multiply(QUATERNION quat2);	//multiply this quaternion by quat2
	void PostMultiply(QUATERNION quat2);	//multiply quat2 by this quaternion
	void SetAxisAngle(float a, float ax, float ay, float az);
	void Rotate(float a, float ax, float ay, float az);
	void SetEuler(float a, float b, float c);
	QUATERNION ReturnConjugate();
	void LookAt(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, 
		   float centerZ, float upX, float upY, float upZ);
	void SetMat(float * m1);
	VERTEX RotateVec(VERTEX vec);
	float GetAngleBetween(QUATERNION quat2);
	VERTEX GetEuler();
	//spherical linear interpolation between this quat and another quat
	QUATERNION Slerp(QUATERNION quat2, float t);
	QUATERNION QuatSlerp(QUATERNION quat2, float t);
	QUATERNION Add(QUATERNION quat2);
	QUATERNION MultScalar(float scalar);
	void DebugPrint();
	VERTEX RelativeMove(VERTEX input);

	QUATERNION operator*(const QUATERNION &quat2 );	//multiply override
};

//axis-aligned bounding box
class AABB
{
private:
	VERTEX pos;
	VERTEX center;
	VERTEX size;

	float fabs(float num) {if (num >= 0) return num; else return -num;}

public:
	void SetFromCorners(VERTEX c1, VERTEX c2);
	bool IntersectRay(VERTEX orig, VERTEX d);
	bool IntersectSegment(VERTEX orig, VERTEX d, float seglen);
	bool IntersectAABB(AABB & other);

	VERTEX GetPos() {return pos;}
	VERTEX GetSize() {return size;}
	VERTEX GetCenter() {return center;}
	
	void DebugPrint();
	
	void GetVerts(VERTEX * eightverts);
};

#define QUAT_H
#endif
