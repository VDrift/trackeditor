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
 
#include "logo.h"

LOGO::LOGO()
{
	num_logos = 0;
	ltime = 0;
}

void LOGO::AddLogo(string filename)
{
	logo_fn[num_logos] = filename;
	logo_tex[num_logos] = utility.TexLoad(filename, false);
	
	num_logos++;
}

bool LOGO::IncrementTime(float fps)
{
	ltime += 1.0f/fps;
	
	//cout << ltime << endl;
	
	if (ltime > SECS_PER_LOGO * num_logos)
	{
		//gamestate.SetGameState(GAMESTATE_MENU);
		Cleanup();
		//cout << "Cleanup" << endl;
		return false;
	}
	
	return true;
}

void LOGO::Cleanup()
{
	int i;
	
	for (i = 0; i < num_logos; i++)
		glDeleteTextures(1, &(logo_tex[i]));
}

void LOGO::Draw()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer
	
	int curlogo = (int)(ltime / SECS_PER_LOGO);
	
	glEnable(GL_TEXTURE_2D);
	
	/*int w = configsys.GetScreenW();
	int h = configsys.GetScreenH();*/
	int w, h;
	//w = (int) (LOGO_DIM*1.333333f);
	w = LOGO_DIM;
	h = LOGO_DIM;
	
	// Select our texture
    glBindTexture( GL_TEXTURE_2D, logo_tex[curlogo] );

    // Disable depth testing 
    glDisable( GL_DEPTH_TEST );
	//and lighting...
	glDisable( GL_LIGHTING);
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_BLEND );

    // Select The Projection Matrix
    glMatrixMode( GL_PROJECTION );
    // Store The Projection Matrix
    glPushMatrix( );

    // Reset The Projection Matrix 
    glLoadIdentity( );
    // Set Up An Ortho Screen 
    glOrtho( 0, w, 0, h, -1, 1 );

    // Select The Modelview Matrix
    glMatrixMode( GL_MODELVIEW );
    // Stor the Modelview Matrix
    glPushMatrix( );
    // Reset The Modelview Matrix
    glLoadIdentity( );

    // Position The Text (0,0 - Bottom Left)
    glTranslated( (w-LOGO_DIM)/2, (h-LOGO_DIM)/2, 0 );
	
	float trans = (ltime - (curlogo*SECS_PER_LOGO))/LOGO_FADE_SECS;
	if (trans > 1.0f)
		trans = 1.0f;
	
	//cout << trans << endl;
	
	if (FADE_OUT && SECS_PER_LOGO - (ltime - (curlogo*SECS_PER_LOGO)) < LOGO_FADE_SECS)
	{
		trans = (SECS_PER_LOGO - (ltime - (curlogo*SECS_PER_LOGO)))/LOGO_FADE_SECS;
	}
	
	glColor4f(1.0f, 1.0f, 1.0f, trans);
	
	//draw logo
	glBegin( GL_QUADS );

		/* Texture Coord (Top Right) */
		glTexCoord2f( 1, 1 );
		/* Vertex Coord (Top Right) */
		glVertex2i( LOGO_DIM, 0 );
		
		/* Texture Coord (Top Left) */
		glTexCoord2f( 0, 1);
		/* Vertex Coord (Top Left) */
		glVertex2i( 0, 0 );

		/* Texture Coord (Bottom Left) */
		glTexCoord2f( 0, 0 );
		/* Vertex Coord (Bottom Left) */
		glVertex2i( 0, LOGO_DIM );	

		/* Texture Coord (Bottom Right) */
		glTexCoord2f( 1, 0);
		/* Vertex Coord (Bottom Right) */
		glVertex2i( LOGO_DIM, LOGO_DIM );		
	
	glEnd( );
	
	// Select The Projection Matrix
    glMatrixMode( GL_PROJECTION );
    // Restore The Old Projection Matrix
    glPopMatrix( );
	
	// Select the Modelview Matrix
    glMatrixMode( GL_MODELVIEW );
    // Restore the Old modelview Matrix
	glPopMatrix();
	
	glPopAttrib();
}

void LOGO::run()
{
	//string settings.GetDataDir() = DATA_DIR;
	AddLogo(settings.GetFullDataPath("textures/" + settings.GetTexSize() + "/gui/logo_opengl.png"));
	//AddLogo(settings.GetDataDir() + "/tex/splash.jpg");
	AddLogo(settings.GetFullDataPath("textures/" + settings.GetTexSize() + "/gui/splash.png"));
	//AddLogo(settings.GetDataDir() + "/tex/logo_openal.png");
	
	//GLint t = SDL_GetTicks();
	float fps = 100.0;
	GLint T0 = SDL_GetTicks();
	
	bool lp = true;
	
	while (lp)
	{
		//glClearColor(1,1,1,0);
		//glClearColor(0,0,0,0);
		Draw();
		SDL_GL_SwapBuffers( );
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//t = SDL_GetTicks();
		//fps = 1000000.0/(t - ot);
		//cout << fps << endl;

		GLint t = SDL_GetTicks();
		GLfloat seconds = (t - T0) / 1000.0;
		fps = 1.0 / seconds;
		T0 = t;
		
		t = T0;
		lp = IncrementTime(fps);
		//cout << lp << endl;
		
		SDL_Event event;
		while ( SDL_PollEvent( &event ) )
		{
		    switch( event.type )
			{
				case SDL_KEYDOWN:
					lp = false;
					break;
			}
		}
	}
}
