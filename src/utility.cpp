/***************************************************************************
 *            utility.cc
 *
 *  Sat Mar 26 08:52:54 2005
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

#define GL_GLEXT_PROTOTYPES

#ifdef _WIN32
//#define GL_GLEXT_PROTOTYPES
//#include <GL/glext.h>
//#include <GL/glut.h>
//#include <GL/wglext.h>
//#include <GL/glprocs.h>
#include <GL/glew.h>
#endif

#include "utility.h"

/*#ifdef _WIN32
PFNGLMULTITEXCOORD2FARBPROC     pglMultiTexCoord2f     = NULL;
PFNGLMULTITEXCOORD4FARBPROC     pglMultiTexCoord4f     = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC	pglActiveTexture       = NULL;
PFNGLACTIVETEXTUREARBPROC       pglClientActiveTexture = NULL;
#endif*/

void UTILITY::SelectTU(int TU)
{
	if (!initdone)
	{
	    initerror();
	    return;
	}

	GLenum tuenum;
	
	if (TU == 0)
		tuenum = GL_TEXTURE0_ARB;
	else if (TU == 1)
		tuenum = GL_TEXTURE1_ARB;
	else if (TU == 2)
		tuenum = GL_TEXTURE2_ARB;
	else if (TU == 3)
		tuenum = GL_TEXTURE3_ARB;
	else
		tuenum = GL_TEXTURE0_ARB;
	
	#ifdef _WIN32
	//pglActiveTexture(tuenum);
	glActiveTextureARB(tuenum);
	#else
	glActiveTextureARB(tuenum);
	#endif
}

void UTILITY::TexCoord2d2f(int TU, float u, float v)
{
	if (!initdone)
	{
	    initerror();
	    return;
	}

	GLenum tuenum;
	
	if (TU+1 > nb_multitexture)
		return;
	
	if (TU == 0)
		tuenum = GL_TEXTURE0_ARB;
	else if (TU == 1)
		tuenum = GL_TEXTURE1_ARB;
	else if (TU == 2)
		tuenum = GL_TEXTURE2_ARB;
	else if (TU == 3)
		tuenum = GL_TEXTURE3_ARB;
	else
		tuenum = GL_TEXTURE0_ARB;
	
	#ifdef _WIN32
	//pglMultiTexCoord2f(tuenum, u, v);
	glMultiTexCoord2fARB(tuenum, u, v);
	#else
	glMultiTexCoord2fARB(tuenum, u, v);
	#endif
}

GLuint UTILITY::TexLoad(string filename, int format, bool mipmap, int &w, int &h, const bool supressederror, bool &err)
{
	return TexLoad(filename, format, mipmap, w, h, supressederror, err, 0);
}

GLuint UTILITY::TexLoad(string filename, int format, bool mipmap, int &w, int &h, const bool supressederror, bool &err, int attempt)
{
	string filepath;
	char buffer[1024];
	//filepath = DATA_DIR + "/tex/" + filename;
	
	switch (attempt)
	{
		case 0:
			filepath = filename;
			break;
		
		case 1:
			filepath = settings.GetFullDataPath(filename);
			break;
		
		case 2:
			filepath = settings.GetFullDataPath("textures/" + settings.GetTexSize() + "/" + filename);
			break;
		
		default:
			filepath = filename;
			break;
	}

	strcpy(buffer, filepath.c_str());
	
	GLuint new_handle = 0;
	
	//*** Load Texture ***
	SDL_Surface *TextureImage[1];					// Create Storage Space For The Texture

	// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
	if ((TextureImage[0]=IMG_Load(buffer)))
	{
		//SDL_SetAlpha(TextureImage[0], 0, 0);
		
		w = TextureImage[0]->w;
		h = TextureImage[0]->h;
		
		switch (TextureImage[0]->format->BytesPerPixel)
		{
			case 1:
				format = GL_LUMINANCE;
				break;
			case 2:
				format = GL_LUMINANCE_ALPHA;
				break;
			case 3:
				format = GL_RGB;
				break;
			case 4:
				format = GL_RGBA;
				break;
			default:
				break;
		}
		
		glGenTextures(1, &new_handle);					// Create Texture
		
		// Create MipMapped Texture
		glBindTexture(GL_TEXTURE_2D, new_handle);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		
		if (mipmap)
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
			gluBuild2DMipmaps( GL_TEXTURE_2D, format, TextureImage[0]->w, TextureImage[0]->h, format, GL_UNSIGNED_BYTE, TextureImage[0]->pixels );
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexImage2D( GL_TEXTURE_2D, 0, format, TextureImage[0]->w, TextureImage[0]->h, 0, format, GL_UNSIGNED_BYTE, TextureImage[0]->pixels );
		}
		
		err = false;
	}
	else
	{
		/*err = true;
		
	    //try once more
		//cout << "initially failed: " << buffer << endl;
		int try2 = -1;
		if (filepath.find(settings.GetDataDir() + "/textures") > filepath.length())
		{
			try2 = TexLoad(settings.GetFullDataPath("textures/" + settings.GetTexSize() + "/" + filepath), format, mipmap, w, h, supressederror, err);
		}
		//quit, bitmap not found
		if (err)
		{
			if (!supressederror) error_log << "Could not find texture: " << buffer << "\n";
			return 0;
		}
		else
		{
			err = false;
			return try2;
		}*/
		
		attempt++;
		
		if (attempt > 2)
		{
			err = true;
			if (!supressederror) error_log << "Could not find texture: " << filename << "\n";
			return 0;
		}
		else
		{
			return TexLoad(filename, format, mipmap, w, h, supressederror, err, attempt);
		}
	}

	if (TextureImage[0]) // If Texture Exists
	{
		// Free up any memory we may have used
		SDL_FreeSurface( TextureImage[0] );
	}
	
	return new_handle;
}

GLuint UTILITY::TexLoad(string texfile, bool mipmap)
{
	return TexLoad(texfile, GL_RGBA, mipmap);
}

GLuint UTILITY::TexLoad(string filename, int format, bool mipmap)
{
	int dw, dh, new_handle;
	bool err = false;
	
	new_handle = TexLoad(filename, format, mipmap, dw, dh, false, err);
	
	return new_handle;
}

UTILITY::UTILITY()
{
	error_log.open((settings.GetSettingsDir() + "/logs/utility.log").c_str());
	initdone = false;
	
	srand(1337);
}

extern bool verbose_output;
UTILITY::~UTILITY()
{
	if (verbose_output)
		cout << "utility deinit" << endl;
	
	error_log.close();
}

void UTILITY::Init()
{
	//cout << "utility init test" << endl;

#ifdef _WIN32
	GLenum glew_err = glewInit();
	if ( glew_err != GLEW_OK )
	{
		cerr << "GLEW failed to initialize: " << glewGetErrorString ( glew_err ) << endl;
	}
	else
	{
		cout << "Using GLEW " << glewGetString ( GLEW_VERSION ) << endl;
	}
	/*pglMultiTexCoord4f = (PFNGLMULTITEXCOORD4FARBPROC) wglGetProcAddress("glMultiTexCoord4fARB");
	pglMultiTexCoord2f = (PFNGLMULTITEXCOORD2FARBPROC) wglGetProcAddress("glMultiTexCoord2fARB");
	pglActiveTexture = (PFNGLCLIENTACTIVETEXTUREARBPROC) wglGetProcAddress("glActiveTextureARB");
	pglClientActiveTexture = (PFNGLACTIVETEXTUREARBPROC) wglGetProcAddress("glClientActiveTextureARB");
	
	if (pglMultiTexCoord4f == NULL)
		error_log << "main, WIN32: wglGetProcAddress(\"glMultiTexCoord4fARB\") failed." << endl;
	if (pglMultiTexCoord2f == NULL)
		error_log << "main, WIN32: wglGetProcAddress(\"glMultiTexCoord2fARB\") failed." << endl;
	if (pglActiveTexture == NULL)
		error_log << "main, WIN32: wglGetProcAddress(\"glActiveTextureARB\") failed." << endl;
	if (pglClientActiveTexture == NULL)
		error_log << "main, WIN32: wglGetProcAddress(\"glClientActiveTextureARB\") failed." << endl;
		*/
		//cout << "utility init test" << endl;
#endif


	glGetIntegerv( GL_MAX_TEXTURE_UNITS_ARB,&nb_multitexture );
	
	//nb_multitexture = 2;
	
	cout << "Multitexture units (4 are required for all effects): " << nb_multitexture << endl;
	if (nb_multitexture < 2)
		cout << "You have less than the recommended number of texture units." << endl << "Some effects will not be rendered." << endl << "Upgrade your graphics card!" << endl;
	else if (nb_multitexture <= 3)
		cout << "You have less than the recommended number of texture units." << endl << "Some textures will lack detail." << endl << "Upgrade your graphics card!" << endl;
	else
		cout << "Your GPU meets the texture unit requirements." << endl;
	
	cout << endl << "Note to user:  All error messages will be put in " + settings.GetSettingsDir() + "/logs/." << endl;
	
	initdone = true;
}

string UTILITY::sGetLine(ifstream &ffrom)
{
	string trashstr;
	char trashchar[1024];

	//ffrom >> trashstr;
	ffrom.getline(trashchar,1024,'\n');

	while ((trashchar[0] == '#' || strlen(trashchar) < 1 ) && !ffrom.eof())
	{
		ffrom.getline(trashchar, 1024, '\n');
		//ffrom >> trashstr;
		//ffrom.getline(trashchar,1024,'\n');
	}
	
	trashstr = trashchar;

	if (ffrom.eof() && trashstr.length() == 0)
		return ENDOFFILESTRING;
	else
		return trashstr;
}

string UTILITY::sGetParam(ifstream &ffrom)
{
	string trashstr;
	char trashchar[1024];

	ffrom >> trashstr;

	while (trashstr.c_str()[0] == '#' && !ffrom.eof() && trashstr != "")
	{
		ffrom.getline(trashchar, 1024, '\n');
		ffrom >> trashstr;
	}
	
	if (ffrom.eof() && trashstr.c_str()[0] == '#')
		return ENDOFFILESTRING;
	else
		return trashstr;
}

int UTILITY::iGetParam(ifstream &ffrom)
{
	string trashstr;
	char trashchar[1024];

	ffrom >> trashstr;

	while (trashstr.c_str()[0] == '#' && !ffrom.eof())
	{
		ffrom.getline(trashchar, 1024, '\n');
		ffrom >> trashstr;
	}

	return atoi(trashstr.c_str());
}

float UTILITY::fGetParam(ifstream &ffrom)
{
	string trashstr;
	char trashchar[1024];

	ffrom >> trashstr;

	while (trashstr.c_str()[0] == '#' && !ffrom.eof())
	{
		ffrom.getline(trashchar, 1024, '\n');
		ffrom >> trashstr;
	}

	return atof(trashstr.c_str());
}

void UTILITY::initerror()
{
	//error_log << "Utility library not yet initialized" << endl;
}

void UTILITY::Tex2D(int TU, bool enable)
{
	if (!initdone)
	{
	    initerror();
	    return;
	}

	SelectTU(TU);
	if (enable)
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);
}

bool UTILITY::bGetParam(ifstream &ffrom)
{
	string trashstr;
	char trashchar[1024];

	ffrom >> trashstr;

	while (trashstr.c_str()[0] == '#' && !ffrom.eof())
	{
		ffrom.getline(trashchar, 1024, '\n');
		ffrom >> trashstr;
	}
	
	if (trashstr == "true" || trashstr == "1")
		return true;
	else
		return false;
}

int UTILITY::numTUs()
{
	if (!initdone)
	{
	    initerror();
	    return 0;
	}
	
	return nb_multitexture;
}

void UTILITY::Draw2D(float x1, float y1, float x2, float y2, GLuint texid)
{
	Draw2D(x1, y1, x2, y2, texid, 0);
}

void UTILITY::Draw2D(float x1, float y1, float x2, float y2, GLuint texid, float rotation)
{
	int sx, sy;
	
	QUATERNION rot;
	rot.Rotate(rotation, 0,0,1);
	
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	
	sx = 1600;
	sy = 1200;
	
	int x = (int) (x1*(float)sx);
	int y = (int) ((1.0-y1)*(float)sy);
	
	int w = (int) ((x2-x1)*(float)sx);
	int h = (int) ((y2-y1)*(float)sy);

	// Select our texture
	glBindTexture( GL_TEXTURE_2D, texid );

	// Disable depth testing 
	glDisable( GL_DEPTH_TEST );
	//and lighting...
	glDisable( GL_LIGHTING);
	glBlendFunc( GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR);
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_BLEND );

	// Select The Projection Matrix
	glMatrixMode( GL_PROJECTION );
	// Store The Projection Matrix
	glPushMatrix( );

	// Reset The Projection Matrix 
	glLoadIdentity( );
	// Set Up An Ortho Screen 
	glOrtho( 0, sx, 0, sy, -1, 1 );

	// Select The Modelview Matrix
	glMatrixMode( GL_MODELVIEW );
	// Stor the Modelview Matrix
	glPushMatrix( );
	// Reset The Modelview Matrix
	glLoadIdentity( );

	// Position The Text (0,0 - Bottom Left)
	glTranslated( x, y, 0 );

	/*// Choose The Font Set (0 or 1)
	glListBase( base - 32 + ( 128 * set ) );

	// Write The Text To The Screen
	glCallLists( strlen( string ), GL_BYTE, string );*/
	
	glColor3f(1,1,1);
	
	/*for (unsigned int i = 0; i < strlen(string); i++)
	{
		if (string[i] == 32)
			glTranslated( 10, 0, 0 );
		else if (string[i] == '\n')
		{
			num_crs++;
			glLoadIdentity( );
			glTranslated(x,y-20*num_crs,0);
		}
		else
		{
			int pos = base - 32 + (128*set) + string[i];
			if (pos < 0)
				pos = 0;
			glCallList(pos);
		}
	}*/
	
	VERTEX v[4];
	v[0].Set(0,0,0);
	v[1].Set(w,0,0);
	v[2].Set(w,-h,0);
	v[3].Set(0,-h,0);
	
	int i;
	
	for (i = 0; i < 4; i++)
	{
		v[i].x -= w/2.0;
		v[i].y += h/2.0;
	}
	
	for (i = 0; i < 4; i++)
		v[i] = rot.RotateVec(v[i]);
	
	for (i = 0; i < 4; i++)
	{
		v[i].x += w/2.0;
		v[i].y -= h/2.0;
	}
	
	//draw box
	glBegin( GL_QUADS );
	        /* Texture Coord (Bottom Left) */
	        glTexCoord2f( 0, 0);
			/* Vertex Coord (Bottom Left) */
			glVertex2f( v[0].x, v[0].y );
	
			/* Texture Coord (Bottom Right) */
			glTexCoord2f( 1, 0);
			/* Vertex Coord (Bottom Right) */
			glVertex2f( v[1].x, v[1].y );
	
			/* Texture Coord (Top Right) */
			glTexCoord2f( 1, 1);
			/* Vertex Coord (Top Right) */
			glVertex2f( v[2].x, v[2].y );
	
			/* Texture Coord (Top Left) */
			glTexCoord2f( 0, 1);
			/* Vertex Coord (Top Left) */
			glVertex2f( v[3].x, v[3].y );
	      glEnd( );

	// Select The Projection Matrix
	glMatrixMode( GL_PROJECTION );
	// Restore The Old Projection Matrix
	glPopMatrix( );
	
	// Select the Modelview Matrix
	glMatrixMode( GL_MODELVIEW );
	// Restore the Old Projection Matrix
	glPopMatrix( );

	// Re-enable Depth Testing
	glEnable( GL_DEPTH_TEST );
	//and lighting...
	glEnable( GL_LIGHTING);
	glDisable(GL_BLEND);
	
	glPopAttrib();
}

float UTILITY::GetValue(SDL_Surface * surf, int channel, float x, float y, bool interpolate)
{
	return GetValue(surf, channel, x, y, interpolate, false);
}

float UTILITY::GetValue(SDL_Surface * surf, int channel, float x, float y, bool interpolate, bool wrap)
{
	//check for NANs
	if (!(x < 0 || x >= 0) || !(y < 0 || y >= 0))
		return 0;
	
	if (channel >= surf->format->BytesPerPixel || channel < 0)
	{
		error_log << "Asked for channel " << channel << ", image only has " << surf->format->BytesPerPixel << endl;
		channel = surf->format->BytesPerPixel - 1;
	}
	
	{
		if (x < 0)
			x = 0;
		if (x > 1)
			x = 1;
		if (y < 0)
			y = 0;
		if (y > 1)
			y = 1;
		int ix, iy;
		ix = (int)(x * surf->w);
		iy = (int)(y * surf->h);
		float dx, dy;
		dx = x - ix;
		dy = y - iy;
		
		unsigned char * pix;
		pix = (unsigned char *) surf->pixels;
		
		if (ix >= surf->w)
		{
			if (wrap)
				ix = ix % surf->w;
			else
				ix = surf->w - 1;
		}
		if (iy >= surf->h)
		{
			if (wrap)
				iy = iy % surf->h;
			else
				iy = surf->h - 1;
		}
		
		if (!interpolate)
		{
			return pix[(ix+surf->pitch*iy)*surf->format->BytesPerPixel+channel] / 255.0f;
		}
		else
		{
			float ul, ur, ll, lr;
			
			int rx, ry;
			
			rx = ix + 1;
			if (rx >= surf->w)
			{
				if (wrap)
					rx = rx % surf->w;
				else
					rx = ix;
			}
			
			ry = iy + 1;
			if (ry >= surf->h)
			{
				if (wrap)
					ry = ry % surf->h;
				else
					ry = iy;
			}
			
			rx = ix;
			ry = iy;
			
			ul = pix[(ix+surf->pitch*iy)*surf->format->BytesPerPixel+channel] / 255.0f;
			ur = pix[(rx+surf->pitch*iy)*surf->format->BytesPerPixel+channel] / 255.0f;
			ll = pix[(ix+surf->pitch*ry)*surf->format->BytesPerPixel+channel] / 255.0f;
			lr = pix[(rx+surf->pitch*ry)*surf->format->BytesPerPixel+channel] / 255.0f;
			
			float u, l;
			
			u = ul*(1.0-dx)+ur*dx;
			l = ll*(1.0-dx)+lr*dx;
			
			return u*(1.0-dy)+l*dy;
		}
	}
}

bool UTILITY::FileExists(string filename)
{
	ifstream test;
	test.open(filename.c_str());
	if (test)
	{
		test.close();
		return true;
	}
	else
		return false;
}

int UTILITY::IntersectTriangleD(double orig[3], double dir[3],
                   double vert0[3], double vert1[3], double vert2[3],
                   double *t, double *u, double *v)
{
   double edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
   double det,inv_det;

   /* find vectors for two edges sharing vert0 */
   SUB(edge1, vert1, vert0);
   SUB(edge2, vert2, vert0);

   /* begin calculating determinant - also used to calculate U parameter */
   CROSS(pvec, dir, edge2);

   /* if determinant is near zero, ray lies in plane of triangle */
   det = DOT(edge1, pvec);

#ifdef TEST_CULL           /* define TEST_CULL if culling is desired */
   if (det < EPSILON)
      return 0;

   /* calculate distance from vert0 to ray origin */
   SUB(tvec, orig, vert0);

   /* calculate U parameter and test bounds */
   *u = DOT(tvec, pvec);
   if (*u < 0.0 || *u > det)
      return 0;

   /* prepare to test V parameter */
   CROSS(qvec, tvec, edge1);

    /* calculate V parameter and test bounds */
   *v = DOT(dir, qvec);
   if (*v < 0.0 || *u + *v > det)
      return 0;

   /* calculate t, scale parameters, ray intersects triangle */
   *t = DOT(edge2, qvec);
   inv_det = 1.0 / det;
   *t *= inv_det;
   *u *= inv_det;
   *v *= inv_det;
#else                    /* the non-culling branch */
   if (det > -EPSILON && det < EPSILON)
     return 0;
   inv_det = 1.0 / det;

   /* calculate distance from vert0 to ray origin */
   SUB(tvec, orig, vert0);

   /* calculate U parameter and test bounds */
   *u = DOT(tvec, pvec) * inv_det;
   if (*u < 0.0 || *u > 1.0)
     return 0;

   /* prepare to test V parameter */
   CROSS(qvec, tvec, edge1);

   /* calculate V parameter and test bounds */
   *v = DOT(dir, qvec) * inv_det;
   if (*v < 0.0 || *u + *v > 1.0)
     return 0;

   /* calculate t, ray intersects triangle */
   *t = DOT(edge2, qvec) * inv_det;
#endif
   return 1;
}

int UTILITY::IntersectTriangleF(float orig[3], float dir[3],
                   float vert0[3], float vert1[3], float vert2[3],
                   float *t, float *u, float *v)
{
   float edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
   float det,inv_det;

   /* find vectors for two edges sharing vert0 */
   SUB(edge1, vert1, vert0);
   SUB(edge2, vert2, vert0);

   /* begin calculating determinant - also used to calculate U parameter */
   CROSS(pvec, dir, edge2);

   /* if determinant is near zero, ray lies in plane of triangle */
   det = DOT(edge1, pvec);

#ifdef TEST_CULL           /* define TEST_CULL if culling is desired */
   if (det < EPSILON)
      return 0;

   /* calculate distance from vert0 to ray origin */
   SUB(tvec, orig, vert0);

   /* calculate U parameter and test bounds */
   *u = DOT(tvec, pvec);
   if (*u < 0.0 || *u > det)
      return 0;

   /* prepare to test V parameter */
   CROSS(qvec, tvec, edge1);

    /* calculate V parameter and test bounds */
   *v = DOT(dir, qvec);
   if (*v < 0.0 || *u + *v > det)
      return 0;

   /* calculate t, scale parameters, ray intersects triangle */
   *t = DOT(edge2, qvec);
   inv_det = 1.0 / det;
   *t *= inv_det;
   *u *= inv_det;
   *v *= inv_det;
#else                    /* the non-culling branch */
   if (det > -EPSILON && det < EPSILON)
     return 0;
   inv_det = 1.0 / det;

   /* calculate distance from vert0 to ray origin */
   SUB(tvec, orig, vert0);

   /* calculate U parameter and test bounds */
   *u = DOT(tvec, pvec) * inv_det;
   if (*u < 0.0 || *u > 1.0)
     return 0;

   /* prepare to test V parameter */
   CROSS(qvec, tvec, edge1);

   /* calculate V parameter and test bounds */
   *v = DOT(dir, qvec) * inv_det;
   if (*v < 0.0 || *u + *v > 1.0)
     return 0;

   /* calculate t, ray intersects triangle */
   *t = DOT(edge2, qvec) * inv_det;
#endif
   return 1;
}

float UTILITY::randf(float min, float max)
{
	if (min > max)
	{
		float temp = min;
		min = max;
		max = temp;
	}
	
	float r = (float) rand() / (float) RAND_MAX;

	r *= max-min;
	r += min;
	return r;
}
