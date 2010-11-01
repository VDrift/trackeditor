#include "quat.h"

#define DELTA 0.00001

void QUATERNION::LoadMultIdent()
{
	w = 1.0f;
	x = y = z = 0.0f;
	cachematvalid = false;
}

QUATERNION::QUATERNION()
{
	LoadMultIdent();
	cachematvalid = false;
}

float QUATERNION::Norm()
{
	return sqrt(w*w+x*x+y*y+z*z);
}

void QUATERNION::Normalize()
{
	cachematvalid = false;
	
	float len = Norm();
	w = w / len;
	x = x / len;
	y = y / len;
	z = z / len;
}

void QUATERNION::GetMat(float * destmat)
{
	if (!cachematvalid)
	{
		Normalize();
		//NOTE:  We're assuming the quaternion is normalized here!
		
		float xx = x*x;
		float xy = x*y;
		float xz = x*z;
		float xw = x*w;
		
		float yy = y*y;
		float yz = y*z;
		float yw = y*w;
		
		float zz = z*z;
		float zw = z*w;
		
		destmat[0] = 1.0f - 2.0f*(yy+zz);
		destmat[1] = 2.0f*(xy+zw);
		destmat[2] = 2.0f*(xz-yw);
		destmat[3] = 0;
		
		destmat[4] = 2.0f*(xy-zw);
		destmat[5] = 1.0f-2.0f*(xx+zz);
		destmat[6] = 2.0f*(yz+xw);
		destmat[7] = 0;
		
		destmat[8] = 2.0f*(xz+yw);
		destmat[9] = 2.0f*(yz-xw);
		destmat[10] = 1.0f-2.0f*(xx+yy);
		destmat[11] = 0;
		
		destmat[12] = 0;
		destmat[13] = 0;
		destmat[14] = 0;
		destmat[15] = 1;
		
		//build cached matrix
		for (int i = 0; i < 16; i++)
		{
			cachemat[i] = destmat[i];
		}
		cachematvalid = true;
	}
	else
	{
		//send back the cached matrix
		for (int i = 0; i < 16; i++)
		{
			destmat[i] = cachemat[i];
		}
	}
	
	/*xx      = X * X;
    xy      = X * Y;
    xz      = X * Z;
    xw      = X * W;

    yy      = Y * Y;
    yz      = Y * Z;
    yw      = Y * W;

    zz      = Z * Z;
    zw      = Z * W;

    mat[0]  = 1 - 2 * ( yy + zz );
    mat[1]  =     2 * ( xy - zw );
    mat[2]  =     2 * ( xz + yw );

    mat[4]  =     2 * ( xy + zw );
    mat[5]  = 1 - 2 * ( xx + zz );
    mat[6]  =     2 * ( yz - xw );

    mat[8]  =     2 * ( xz - yw );
    mat[9]  =     2 * ( yz + xw );
    mat[10] = 1 - 2 * ( xx + yy );

    mat[3]  = mat[7] = mat[11 = mat[12] = mat[13] = mat[14] = 0;
    mat[15] = 1;*/
}

void QUATERNION::GetMat(GLdouble * destmat)
{
	if (!cachematvalid)
	{
		Normalize();
		//NOTE:  We're assuming the quaternion is normalized here!
		
		GLdouble xx = x*x;
		GLdouble xy = x*y;
		GLdouble xz = x*z;
		GLdouble xw = x*w;
		
		GLdouble yy = y*y;
		GLdouble yz = y*z;
		GLdouble yw = y*w;
		
		GLdouble zz = z*z;
		GLdouble zw = z*w;
		
		destmat[0] = 1.0f - 2.0f*(yy+zz);
		destmat[1] = 2.0f*(xy+zw);
		destmat[2] = 2.0f*(xz-yw);
		destmat[3] = 0;
		
		destmat[4] = 2.0f*(xy-zw);
		destmat[5] = 1.0f-2.0f*(xx+zz);
		destmat[6] = 2.0f*(yz+xw);
		destmat[7] = 0;
		
		destmat[8] = 2.0f*(xz+yw);
		destmat[9] = 2.0f*(yz-xw);
		destmat[10] = 1.0f-2.0f*(xx+yy);
		destmat[11] = 0;
		
		destmat[12] = 0;
		destmat[13] = 0;
		destmat[14] = 0;
		destmat[15] = 1;
		
		//build cached matrix
		for (int i = 0; i < 16; i++)
		{
			cachemat[i] = destmat[i];
		}
		cachematvalid = true;
	}
	else
	{
		//send back the cached matrix
		for (int i = 0; i < 16; i++)
		{
			destmat[i] = cachemat[i];
		}
	}
}

void QUATERNION::GetAxisAngle(float * aangle)
{
	//aangle is (angle of rotation, x, y, z)
	
	/*float scale = x*x+y*y+z*z;
	if (scale < 0.00001)
	{
		aangle[0] = 0;
		aangle[1] = 1;
		aangle[2] = 0;
		aangle[3] = 0;
	}
	else
	{
		aangle[0] = 2*acos(w);
		aangle[1] = x/scale;
		aangle[2] = y/scale;
		aangle[3] = z/scale;
	}*/
	
	
	/*float scale = x*x+y*y+z*z;
	
	float cos_angle  = w;
    aangle[0]      = acos( cos_angle ) * 2;
    float sin_angle  = sqrt( 1.0 - cos_angle * cos_angle );


    if ( fabs( sin_angle ) < 0.00005 )
      scale = 1.0f;

    aangle[1] = x/scale;
    aangle[2] = y/scale;
	aangle[3] = z/scale;*/
	
    GLfloat	len;
    GLfloat tx, ty, tz;

	// cache variables
	tx = x;
	ty = y;
	tz = z;

	len = tx * tx + ty * ty + tz * tz;

    if (len > DELTA) 
	{
		aangle[1] = tx * (1.0f / len);
		aangle[2] = ty * (1.0f / len);
		aangle[3] = tz * (1.0f / len);
	    aangle[0] = (GLfloat)(2.0 * acos(w));
    }

    else {
		aangle[1] = 0.0;
		aangle[2] = 0.0;
		aangle[3] = 1.0;
	    aangle[0] = 0.0;
    }
}



void QUATERNION::Multiply(QUATERNION quat2)
{
	cachematvalid = false;
	
	//slow multiply
	float w2 = quat2.w;
	float x2 = quat2.x;
	float y2 = quat2.y;
	float z2 = quat2.z;
	w = w*w2 - x*x2 - y*y2 - z*z2;
	x = w*x2 + x*w2 + y*z2 - z*y2;
	y = w*y2 + y*w2 + z*x2 - x*z2;
	z = w*z2 + z*w2 + x*y2 - y*x2;
	
	/*//FAST multiply code
	float a = w;
	float b = x;
	float c = y;
	float d = z;
	float w2 = quat2.w;
	float x2 = quat2.x;
	float y2 = quat2.y;
	float z2 = quat2.z;
	
	float tmp_00, tmp_01, tmp_02, tmp_03, tmp_04, tmp_05, tmp_06, tmp_07, tmp_08, tmp_09;
	
	tmp_00 = (d - c) * (z2 - w2);
	tmp_01 = (a + b) * (x2 + y2);
	tmp_02 = (a - b) * (z2 + w2);
	tmp_03 = (c + d) * (x2 - y2);
	tmp_04 = (d - b) * (y2 - z2);
	tmp_05 = (d + b) * (y2 + z2);
	tmp_06 = (a + c) * (x2 - w2);
	tmp_07 = (a - c) * (x2 + w2);
	tmp_08 = tmp_05 + tmp_06 + tmp_07;
	tmp_09 = 0.5 * (tmp_04 + tmp_08);
	
	w = tmp_00 + tmp_09 - tmp_05;
	x = tmp_01 + tmp_09 - tmp_08;
	y = tmp_02 + tmp_09 - tmp_07;
	z = tmp_03 + tmp_09 - tmp_06;*/
}

void QUATERNION::PostMultiply(QUATERNION quat2)
{
	cachematvalid = false;
	
	//slow multiply
	float w1 = quat2.w;
	float x1 = quat2.x;
	float y1 = quat2.y;
	float z1 = quat2.z;
	float w2 = w;
	float x2 = x;
	float y2 = y;
	float z2 = z;
	w = w1*w2 - x1*x2 - y1*y2 - z1*z2;
	x = w1*x2 + x1*w2 + y1*z2 - z1*y2;
	y = w1*y2 + y1*w2 + z1*x2 - x1*z2;
	z = w1*z2 + z1*w2 + x1*y2 - y1*x2;
	
	//FAST multiply code
	/*float a = quat2.w;
	float b = quat2.x;
	float c = quat2.y;
	float d = quat2.z;
	
	float tmp_00, tmp_01, tmp_02, tmp_03, tmp_04, tmp_05, tmp_06, tmp_07, tmp_08, tmp_09;
	
	tmp_00 = (d - c) * (z - w);
	tmp_01 = (a + b) * (x + y);
	tmp_02 = (a - b) * (z + w);
	tmp_03 = (c + d) * (x - y);
	tmp_04 = (d - b) * (y - z);
	tmp_05 = (d + b) * (y + z);
	tmp_06 = (a + c) * (x - w);
	tmp_07 = (a - c) * (x + w);
	tmp_08 = tmp_05 + tmp_06 + tmp_07;
	tmp_09 = 0.5 * (tmp_04 + tmp_08);
	
	w = tmp_00 + tmp_09 - tmp_05;
	x = tmp_01 + tmp_09 - tmp_08;
	y = tmp_02 + tmp_09 - tmp_07;
	z = tmp_03 + tmp_09 - tmp_06;*/
}

QUATERNION QUATERNION::operator* (const QUATERNION &quat2 )
{
	//below line stays true since we're not actually modifying our current quat
	//cachematvalid = false;
	
	QUATERNION qout;
	
	//slow multiply
	float w2 = quat2.w;
	float x2 = quat2.x;
	float y2 = quat2.y;
	float z2 = quat2.z;
	qout.w = w*w2 - x*x2 - y*y2 - z*z2;
	qout.x = w*x2 + x*w2 + y*z2 - z*y2;
	qout.y = w*y2 + y*w2 + z*x2 - x*z2;
	qout.z = w*z2 + z*w2 + x*y2 - y*x2;
		
	return qout;
}

//i believe a is in radians
void QUATERNION::SetAxisAngle(float a, float ax, float ay, float az)
{
	cachematvalid = false;
	
	//normalize axis
	float temp = ax*ax + ay*ay + az*az;
    float dist = (GLfloat)(1.0 / sqrt(temp));

    ax *= dist;
    ay *= dist;
    az *= dist;
	
	/*if (ax < 0.00001 && ay < 0.00001 && az < 0.00001)
	{
		LoadMultIdent();
	}
	else*/
	{
		float sina2 = sin(a/2);
		
		w   =   cos(a/2);
		x   =   ax * sina2;
		y   =   ay * sina2;
		z   =   az * sina2;
	}
	
	
	/*GLfloat temp, dist;

	// normalize
	temp = ax*ax + ay*ay + az*az;

    dist = (GLfloat)(1.0 / sqrt(temp));

    ax *= dist;
    ay *= dist;
    az *= dist;

	x = ax;
	y = ay;
	z = az;

	w = (GLfloat)cos(a / 2.0f);*/
	
}

void QUATERNION::SetEuler(float a, float b, float c)
{
	cachematvalid = false;
	
	QUATERNION qout;
	
    QUATERNION Qx, Qy, Qz;
	Qx.w = cos(a/2); Qx.x = sin(a/2); Qx.y = Qx.z = 0.0f;
    Qy.w = cos(b/2); Qy.x = Qy.z = 0.0f; Qy.y = sin(b/2);
    Qz.w = cos(c/2); Qz.x = Qz.y = 0.0f; Qz.z = sin(c/2);

	qout = Qx * Qy * Qz;
	
	*this = qout;
}

VERTEX QUATERNION::GetEulerZYX() const
{
	VERTEX vout;
	QUATERNION q1 = *this;
	
	vout.x = atan2(2 * (q1.y * q1.z + q1.x * q1.w), -q1.x * q1.x - q1.y * q1.y + q1.z * q1.z + q1.w * q1.w);
	vout.y = asin(-2 * (q1.x * q1.z - q1.y * q1.w));
	vout.z = atan2(2 * (q1.x * q1.y + q1.z * q1.w),  q1.x * q1.x - q1.y * q1.y - q1.z * q1.z + q1.w * q1.w);
	
	return vout;
}

void QUATERNION::SetEulerZYX(const VERTEX & angle)
{
	cachematvalid = false;
	
	QUATERNION qout;
	float cosx2 = cos(angle.x/2);
	float cosy2 = cos(angle.y/2);
	float cosz2 = cos(angle.z/2);
	float sinx2 = sin(angle.x/2);
	float siny2 = sin(angle.y/2);
	float sinz2 = sin(angle.z/2);
	qout.x = sinx2 * cosy2 * cosz2 - cosx2 * siny2 * sinz2;
	qout.y = cosx2 * siny2 * cosz2 + sinx2 * cosy2 * sinz2;
	qout.z = cosx2 * cosy2 * sinz2 - sinx2 * siny2 * cosz2;
	qout.w = cosx2 * cosy2 * cosz2 + sinx2 * siny2 * sinz2;
	
	*this = qout;
}

QUATERNION QUATERNION::ReturnConjugate()
{
	QUATERNION qtemp;
	qtemp.w = w;
	qtemp.x = -x;
	qtemp.y = -y;
	qtemp.z = -z;
	return qtemp;
}

void QUATERNION::LookAt(float eyeX, 
		   float eyeY, 
		   float eyeZ, 
		   float centerX, 
		   float centerY, 
		   float centerZ, 
		   float upX, 
		   float upY, 
		   float upZ)
{
	float tempmat[16];
	
	glPushMatrix();
	glLoadIdentity();
	gluLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ);
	glGetFloatv(GL_MODELVIEW_MATRIX , tempmat);
	glPopMatrix();
	
	SetMat(tempmat);
}

void QUATERNION::SetMat(float * mat)
{
	float S;
	
	float T = 1 + mat[0] + mat[5] + mat[10];
	if ( T > 0.00000001 )
	{
		S = sqrt(T) * 2;
		x = ( mat[9] - mat[6] ) / S;
		y = ( mat[2] - mat[8] ) / S;
		z = ( mat[4] - mat[1] ) / S;
		w = 0.25 * S;
	}
	else
	{
		if ( mat[0] > mat[5] && mat[0] > mat[10] )  {       // Column 0: 
			S  = sqrt( 1.0 + mat[0] - mat[5] - mat[10] ) * 2;
			x = 0.25 * S;
			y = (mat[4] + mat[1] ) / S;
			z = (mat[2] + mat[8] ) / S;
			w = (mat[9] - mat[6] ) / S;
		} else if ( mat[5] > mat[10] ) {                    // Column 1: 
			S  = sqrt( 1.0 + mat[5] - mat[0] - mat[10] ) * 2;
			x = (mat[4] + mat[1] ) / S;
			y = 0.25 * S;
			z = (mat[9] + mat[6] ) / S;
			w = (mat[2] - mat[8] ) / S;
		} else {                                            // Column 2:
			S  = sqrt( 1.0 + mat[10] - mat[0] - mat[5] ) * 2;
			x = (mat[2] + mat[8] ) / S;
			y = (mat[9] + mat[6] ) / S;
			z = 0.25 * S;
			w = (mat[4] - mat[1] ) / S;
		}
	}
}

/*void QUATERNION::SetMat(float * m1)
{
	w = sqrt(1.0 + m1[0] + m1[5] + m1[10]) / 2.0;
	double w4 = (4.0 * w);
	x = (m1[9] - m1[6]) / w4 ;
	y = (m1[2] - m1[8]) / w4 ;
	z = (m1[4] - m1[1]) / w4 ;
}*/

VERTEX QUATERNION::RotateVec(VERTEX vec)
{
	QUATERNION dirconj;
	
	QUATERNION dir;
	dir.w = w;
	dir.x = x;
	dir.y = y;
	dir.z = z;

	dirconj = dir.ReturnConjugate();
	QUATERNION qtemp;
	qtemp.w = 0;
	qtemp.x = vec.x;
	qtemp.y = vec.y;
	qtemp.z = vec.z;
	QUATERNION qout;
	qout = dir * qtemp * dirconj;
	
	VERTEX vout;
	
	vout.x += qout.x;
	vout.y += qout.y;
	vout.z += qout.z;
	
	return vout;
}

float QUATERNION::GetAngleBetween(QUATERNION quat2)
{
	//establish a forward vector
	VERTEX forward;
	forward.x = 0;
	forward.y = 0;
	forward.z = 1;
	
	//create vectors for quats
	VERTEX vec1, vec2;
	vec1 = RotateVec(forward);
	vec2 = quat2.RotateVec(forward);
	
	//return the angle between the vectors
	return acos(vec1.dot(vec2));
}

VERTEX QUATERNION::GetEuler()
{
	VERTEX vout;
	QUATERNION q1 = *this;
	
	float sqw = q1.w*q1.w;
    float sqx = q1.x*q1.x;
    float sqy = q1.y*q1.y;
    float sqz = q1.z*q1.z;
	
    vout.z = atan(2.0 * (q1.x*q1.y + q1.z*q1.w)/(sqx - sqy - sqz + sqw));
    vout.x = atan(2.0 * (q1.y*q1.z + q1.x*q1.w)/(-sqx - sqy + sqz + sqw));
    vout.y = asin(-2.0 * (q1.x*q1.z - q1.y*q1.w));
	
	return vout;
}

QUATERNION QUATERNION::Slerp(QUATERNION quat2, float t)
{
	QUATERNION qout, p1, p2;
	float theta = GetAngleBetween(quat2);
	
	//qout = (*this).MultScalar(sin((1.0f-t)*theta)).Add(quat2.MultScalar(sin(t*theta))).MultScalar(1.0f/sin(theta));
	
	p1 = (*this).MultScalar((sin((1.0f-t)*theta))/sin(theta));
	p2 = quat2.MultScalar(sin(theta*t)/sinh(theta));
	qout = p1.Add(p2);
	
	
	return qout;
}

QUATERNION QUATERNION::Add(QUATERNION quat2)
{
	QUATERNION qout;
	
	qout.w = w + quat2.w;
	qout.x = x + quat2.x;
	qout.y = y + quat2.y;
	qout.z = z + quat2.z;
	
	return qout;
}

QUATERNION QUATERNION::MultScalar(float scalar)
{
	QUATERNION qout;
	
	qout.w = w * scalar;
	qout.x = x * scalar;
	qout.y = y * scalar;
	qout.z = z * scalar;
	
	return qout;
}

QUATERNION QUATERNION::QuatSlerp(QUATERNION quat2, float t)
{
	float           to1[4];
	float        omega, cosom, sinom, scale0, scale1;


	// calc cosine
	cosom = x * quat2.x + y * quat2.y + z * quat2.z
			   + w * quat2.w;


	// adjust signs (if necessary)
	if ( cosom <0.0 ){ cosom = -cosom; to1[0] = - quat2.x;
	to1[1] = - quat2.y;
	to1[2] = - quat2.z;
	to1[3] = - quat2.w;
	} else  {
	to1[0] = quat2.x;
	to1[1] = quat2.y;
	to1[2] = quat2.z;
	to1[3] = quat2.w;
	}


	// calculate coefficients


   if ( (1.0 - cosom) > DELTA ) {
			// standard case (slerp)
			omega = acos(cosom);
			sinom = sin(omega);
			scale0 = sin((1.0 - t) * omega) / sinom;
			scale1 = sin(t * omega) / sinom;


	} else {        
// "from" and "to" quaternions are very close 
	//  ... so we can do a linear interpolation
			scale0 = 1.0 - t;
			scale1 = t;
	}
	// calculate final values
	QUATERNION qout;
	qout.x = scale0 * x + scale1 * to1[0];
	qout.y = scale0 * y + scale1 * to1[1];
	qout.z = scale0 * z + scale1 * to1[2];
	qout.w = scale0 * w + scale1 * to1[3];
	return qout;
}

//i was getting angry because the quats couldn't do this, so i added it....
void QUATERNION::Rotate(float a, float ax, float ay, float az)
{
	//NOTE: ax, ay, az is assumed to be a UNIT VECTOR!!
	
	QUATERNION qtemp;
	qtemp.SetAxisAngle(a, ax, ay, az);
	PostMultiply(qtemp);
}

void QUATERNION::DebugPrint()
{
	cout << x << "," << y << "," << z << "," << w << endl;
}

VERTEX VERTEX::operator+ (VERTEX v)
{
	VERTEX result;
	
	result.x = x + v.x;
	result.y = y + v.y;
	result.z = z + v.z;
	
	return result;
}

VERTEX VERTEX::operator- (VERTEX v)
{
	VERTEX result;
	
	result.x = x - v.x;
	result.y = y - v.y;
	result.z = z - v.z;
	
	return result;
}

void VERTEX::Set(float nx, float ny, float nz)
{
	x = nx;
	y = ny;
	z = nz;
}

float VERTEX::len()
{
	//optimization!
	if (x == 0 && y == 0 && z == 0)
		return 0;
	
	float sl = x*x+y*y+z*z;
	
	if (sl < 0)
	{
		ofstream err((settings.GetSettingsDir() + "/logs/vertex.log").c_str());
		err << x << endl << y << endl << z << endl << sl << endl;
		err.close();
		assert(0);
	}
	else if (sl >=0 )
	{	
		return sqrt(sl);
	}
	else
	{
		//last ditch effort to fix the problem
		if (!(x < 0 || x >= 0))
			x = 1;
		if (!(y < 0 || y >= 0))
			y = 1;
		if (!(z < 0 || z >= 0))
			z = 1;
		sl = x*x+y*y+z*z;
		
		if (!(sl >= 0))
		{
			ofstream err((settings.GetSettingsDir() + "/logs/vertex.log").c_str());
			err << x << endl << y << endl << z << endl << sl << endl;
			err.close();
			assert(0);
			return 0;
		}
		else
			return sqrt(sl);
	}
}

VERTEX VERTEX::cross(VERTEX b)
{
	VERTEX result;

	result.x = y*b.z - z*b.y;
	//result.y = x*b.z - z*b.x;
	result.y = z*b.x - x*b.z;
	result.z = x*b.y - y*b.x;

	return result;
}

VERTEX VERTEX::normalize()
{
	VERTEX new_vec;
	float vec_len = len();
	
	if (vec_len == 0.0f)
	{
		new_vec.x = 0.0f;
		new_vec.y = 0.0f;
		new_vec.z = 0.0f;
		return new_vec;
	}

	new_vec.x = x/vec_len;
	new_vec.y = y/vec_len;
	new_vec.z = z/vec_len;

	return new_vec;
}

VERTEX::VERTEX()
{
	x = y = z = 0.0f;
}

void VERTEX::Scale(float scalar)
{
	x = x*scalar;
	y = y*scalar;
	z = z*scalar;
}

VERTEX VERTEX::ScaleR(float scalar)
{
	VERTEX result;
	
	result.x = x*scalar;
	result.y = y*scalar;
	result.z = z*scalar;
	
	return result;
}

VERTEX VERTEX::InvertR()
{
	VERTEX result;
	
	result.x = -x;
	result.y = -y;
	result.z = -z;
	
	return result;
}

float VERTEX::dot(VERTEX b)
{
	return x*b.x + y*b.y + z*b.z;
}

VERTEX VERTEX::interpolatewith(VERTEX other, float percent)
{
	VERTEX vout;
	
	vout.x = (other.x - x)*percent + x;
	vout.y = (other.y - y)*percent + y;
	vout.z = (other.z - z)*percent + z;
	
	return vout;
}

float * VERTEX::v3()
{
	float3[0] = x;
	float3[1] = y;
	float3[2] = z;
	
	return float3;
}

void VERTEX::DebugPrint()
{
	cout << x << "," << y << "," << z << endl;
}

void VERTEX::zero()
{
	x = y = z = 0.0f;
}

void VERTEX::Set(float * f3)
{
	x = f3[0];
	y = f3[1];
	z = f3[2];
}

VERTEX QUATERNION::RelativeMove(VERTEX input)
{
	QUATERNION dirconj, dir;
	VERTEX position;

	dirconj = ReturnConjugate();
	dir.w = w;
	dir.x = x;
	dir.y = y;
	dir.z = z;
	QUATERNION qtemp;
	qtemp.w = 0;
	qtemp.x = input.x;
	qtemp.y = input.y;
	qtemp.z = input.z;
	QUATERNION qout;
	qout = dirconj * qtemp * dir;
	
	position.x = qout.x;
	position.y = qout.y;
	position.z = qout.z;
	return position;
}

bool VERTEX::nan()
{
	if (!(x < 0 || x >= 0) || !(y < 0 || y >= 0) || !(z < 0 || z >= 0))
		return true;
	
	return false;
}
