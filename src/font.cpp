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
 
#include "font.h"

FONT::FONT()
{
	error_log.open((settings.GetSettingsDir() + "/logs/font.log").c_str());
	loaded = false;
}

extern bool verbose_output;
FONT::~FONT()
{
	if (verbose_output)
		cout << "font deinit" << endl;
	
	error_log.close();
	
	if (loaded)
	{
		glDeleteLists(base, 256);
		glDeleteTextures( 1, &texture );
	}
}



void FONT::Load()
{
	if (loaded)
	{
		glDeleteLists(base, 256);
		glDeleteTextures( 1, &texture );
	}
	
	loaded = true;
	
	base = glGenLists(256);
	
	ifstream spf;
	spf.open(settings.GetFullDataPath("lists/fontspacing.1").c_str());
	int c;
	for (c = 0; c < 128; c++)
	{
		spf >> spacing[0][c];
	}
	spf.close();
	
	spf.open(settings.GetFullDataPath("lists/fontspacing.2").c_str());
	for (c = 0; c < 128; c++)
	{
		spf >> spacing[1][c];
	}
	spf.close();
	
	texture = utility.TexLoad("gui/newfontt3.png", GL_RGBA, true);
	
	/*// *** Load Texture ***
	string filepath;
	char buffer[1024];
	string filename;
	filename = "newfont.png";
	bool mipmapping = true;
	int format = GL_RGB;
	filepath = settings.GetDataDir() + "/tex/" + filename;
	strcpy(buffer, filepath.c_str());

	SDL_Surface *TextureImage[1];					// Create Storage Space For The Texture

	// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
	if ((TextureImage[0]=SDL_LoadBMP(buffer)))
	{
		glGenTextures(1, &texture);					// Create Texture
		
		//mesh_error_log << filename << endl;
		
		// Linear Filtering, no MipMapping
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		#ifdef _WIN32
		glTexImage2D( GL_TEXTURE_2D, 0, 3, TextureImage[0]->w, TextureImage[0]->h, 0, format, GL_UNSIGNED_BYTE, TextureImage[0]->pixels );
		#else
		glTexImage2D( GL_TEXTURE_2D, 0, 3, TextureImage[0]->w, TextureImage[0]->h, 0, format, GL_UNSIGNED_BYTE, TextureImage[0]->pixels );
		#endif
		
		if (mipmapping)
		{
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
			gluBuild2DMipmaps( GL_TEXTURE_2D, 3, TextureImage[0]->w, TextureImage[0]->h, GL_BGR, GL_UNSIGNED_BYTE, TextureImage[0]->pixels );
		}
	}
	else
	{
		//quit, bitmap not found
		error_log << "Could not find font texture: " << buffer << "\n";
	}*/
	
	
	//*** Create Display Lists ***
	
	float cx;
	float cy;
	
	glBindTexture(GL_TEXTURE_2D, texture);
	
	// Loop Through All 256 Lists
    for ( int loop = 0; loop < 256; loop++ )
        {
	    /* NOTE:
	     *  BMPs are stored with the top-leftmost pixel being the
	     * last byte and the bottom-rightmost pixel being the first
	     * byte. So an image that is displayed as
	     *    1 0
	     *    0 0
	     * is represented data-wise like
	     *    0 0
	     *    0 1
	     * And because SDL_LoadBMP loads the raw data without
	     * translating to how it is thought of when viewed we need
	     * to start at the bottom-right corner of the data and work
	     * backwards to get everything properly.
	     */

	    /* X Position Of Current Character */
	    cx = 1 - ( float )( loop % 16 ) / 16.0f;
	    /* Y Position Of Current Character */
	    cy = 1 - ( float )( loop / 16 ) / 16.0f;

		//compute size of character
		int maxx;
		int loopr = ( 255 - loop );
		if (loopr > 127)
		  maxx = spacing[1][loopr-128];
		else
		  maxx = spacing[0][loopr];
		float xscale = 1.0f/4.3f;
		//float space = ((float)maxx+15)*xscale;
		float space = ((float)maxx)*xscale;
		
		float texcspace = 0.0625f;
		//float offsetamountx = 0.01f;
		//float offsetamounty = 0.0025f;
		
		//float offsetamountx = -texcspace / 8.0f;
		//float offsetamounty = -texcspace / 8.0f;
		
		float offsetamountx = 0.002;
		float offsetamounty = 0.001;
		//float texcspace = 0.06f;
		/*float texcspace = 0.05f;
		float offsetamount = 0.0625f - texcspace;*/
		//texcspace = (float)maxx/(float)FONT_RES;
		//cout << texcspace << " ";

        /* Start Building A List */
	    glNewList( base + ( 255 - loop ), GL_COMPILE );
	      /* Use A Quad For Each Character */
	      glBegin( GL_QUADS );
	        /* Texture Coord (Bottom Left) */
	        glTexCoord2f( cx - texcspace-offsetamountx, cy -offsetamounty);
			/* Vertex Coord (Bottom Left) */
			glVertex2i( 0, 0 );
	
			/* Texture Coord (Bottom Right) */
			glTexCoord2f( cx-offsetamountx, cy-offsetamounty);
			/* Vertex Coord (Bottom Right) */
			glVertex2i( 16, 0 );
	
			/* Texture Coord (Top Right) */
			glTexCoord2f( cx-offsetamountx, cy - texcspace -offsetamounty);
			/* Vertex Coord (Top Right) */
			glVertex2i( 16, 16 );
	
			/* Texture Coord (Top Left) */
			glTexCoord2f( cx - texcspace-offsetamountx, cy - texcspace -offsetamounty);
			/* Vertex Coord (Top Left) */
			glVertex2i( 0, 16 );
	      glEnd( );

	      /* Move To The Right Of The Character */
	      glTranslated( space+1, 0, 0 );
		  //cout << maxx*xscale << " ";
	    glEndList( );
        }
		/*cout << endl;
		cout << spacing[1]['D'-32] << endl;
		cout << spacing[1]['i'-32] << endl;
		cout << spacing[1]['f'-32] << endl;*/
}

void FONT::Print( float px, float py, const char *string, int set, int size, float r, float g, float b )
{
	Print(px, py, string, set, size, r, g, b, 1);
}

void FONT::Print( float px, float py, const char *string, int set, int size, float r, float g, float b, float trans )
{
	int sx, sy;
	glPushAttrib(GL_ALL_ATTRIB_BITS);
    if ( set > 1 )
	set = 1;
	
	if (size < 0)
		size = 0;
	
	if (size > 10)
		size = 10;
	
	if (size == 0)
	{
		sx = 1600;
		sy = 1200;
	}
	else if (size == 1)
	{
		sx = 1440;
		sy = 1080;
	}
	else if (size == 2)
	{
		sx = 1280;
		sy = 960;
	}
	else if (size == 3)
	{
		sx = 1138;
		sy = 853;
	}
	else if (size == 4)
	{
		sx = 1024;
		sy = 768;
	}
	else if (size == 5)
	{
		sx = 931;
		sy = 698;
	}
	else if (size == 6)
	{
		sx = 640;
		sy = 480;
	}
	else if (size == 7)
	{
		sx = 480;
		sy = 360;
	}
	else if (size == 8)
	{
		sx = 320;
		sy = 240;
	}
	else if (size == 9)
	{
		sx = 213;
		sy = 160;
	}
	else if (size == 10)
	{
		sx = 142;
		sy = 107;
	}
	else
	{
		sx = 1138;
		sy = 853;
	}
	
	int x = (int) (px*(float)sx);
	int y = (int) ((1.0-py)*(float)sy)-20;
	
    // Select our texture
    glBindTexture( GL_TEXTURE_2D, texture );

    // Disable depth testing 
    glDisable( GL_DEPTH_TEST );
	//and lighting...
	glDisable( GL_LIGHTING);
	glBlendFunc( GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR);
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_BLEND );
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

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
	
	int num_crs = 0;
	
	glColor4f(r,g,b,trans);
	
	for (unsigned int i = 0; i < strlen(string); i++)
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
	}

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

void FONT::Print( float px, float py, const char *string, int set, int size, float opacity )
{
	Print( px, py, string, set, size, 1,1,1, opacity);
}
