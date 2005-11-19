/***************************************************************************
 *            menu.cc
 *
 *  Wed Jun  1 19:32:35 2005
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
 
#include "menu.h"

//extern void SelectCar(string cfile, bool clearold, bool trim);

void MENU::Load()
{
	if (loaded)
	{
		if (disp_data.modes != NULL)
		{
			delete [] disp_data.modes;
			disp_data.modes = NULL;
		}
	
		ClearCFList();
		
		int i;
		
		if (page != NULL)
		{	
			for (i = 0; i < numpages; i++)
			{
				if (page[i].item != NULL && page[i].numitems > 0)
					delete [] page[i].item;
			}
			
			delete [] page;
			
			page = NULL;
		}
		
		if (controlinfo.desc != NULL)
		{
			delete [] controlinfo.desc;
			controlinfo.desc = NULL;
		}
		if (controlinfo.token != NULL)
		{
			delete [] controlinfo.token;
			controlinfo.token = NULL;
		}
		
		glDeleteTextures( 1, &backbox );
		glDeleteTextures( 1, &logo );
		glDeleteTextures( 1, &sphere_reflection );
	}
	
	backbox = utility.TexLoad("gui/box.png", GL_RGBA, false);
	logo = utility.TexLoad("gui/vdrift-logo.png", GL_RGBA, false);
	//tach = utility.TexLoad("tachometer.png", GL_RGBA, false);
	
	//load reflection map used for car display
	sphere_reflection = utility.TexLoad("gui/refmap.png", false);

	int i;

	int videoFlags  = SDL_OPENGL;          /* Enable OpenGL in SDL */
	videoFlags |= SDL_GL_DOUBLEBUFFER; /* Enable double buffering */
	videoFlags |= SDL_HWPALETTE;       /* Store the palette in hardware */
	videoFlags |= SDL_RESIZABLE;       /* Enable window resizing */
	videoFlags |= SDL_HWSURFACE|SDL_ANYFORMAT|SDL_FULLSCREEN;  

	/*int videoFlags = SDL_HWSURFACE;
	videoFlags |= SDL_FULLSCREEN;*/

	SDL_Rect **tmodes;

	/* Get available fullscreen/hardware modes */
	tmodes=SDL_ListModes(NULL, videoFlags);

	disp_data.num_modes = 1;

	int realnum = 0;

	/* Print valid modes */
	for(i=0;tmodes[i];++i)
	{
		if (i > 1 && (tmodes[i]->w != tmodes[i-1]->w || tmodes[i]->h != tmodes[i-1]->h))
    		disp_data.num_modes++;
		realnum++;
	}

	/*i--;
	if (i > 1 && tmodes[i]->w != tmodes[i-1]->w && tmodes[i]->h != tmodes[i-1]->h)
    		disp_data.num_modes++;*/

	//disp_data.modes = new SDL_Rect [disp_data.num_modes];

	int counter = 0;

	/*disp_data.modes[counter].w = tmodes[0]->w;
	disp_data.modes[counter].h = tmodes[0]->h;
	counter++;

	for (i = 0; i < realnum; i++)
	{	
		if (i > 1 && (tmodes[i]->w != tmodes[i-1]->w || tmodes[i]->h != tmodes[i-1]->h))
		{
			disp_data.modes[counter].w = tmodes[i]->w;
			disp_data.modes[counter].h = tmodes[i]->h;
			counter++;
		}
	}*/

	/*i--;	
	if (i > 1 && tmodes[i]->w != tmodes[i-1]->w && tmodes[i]->h != tmodes[i-1]->h)
	{
		disp_data.modes[counter].w = tmodes[i-1]->w;
		disp_data.modes[counter].h = tmodes[i-1]->h;
		counter++;
	}*/

	//cout << SDL_VideoModeOK(800,600, 32, videoFlags) << endl;

	disp_data.modes = NULL;
	disp_data.modes = new SDL_Rect [disp_data.num_modes];

	ifstream vmf;
	counter = 0;
	vmf.open(settings.GetFullDataPath("lists/videomodes").c_str());
	while (!vmf.eof())
	{
		int w, h;
		vmf >> w;
		vmf >> h;
		if (!vmf.eof())
		{
			//cout << w << "x" << h << endl;
			//disp_data.modes[counter].w = w;
			//disp_data.modes[counter].h = h;
			counter++;
		}
	}

	vmf.close();
	vmf.clear();
	vmf.open(settings.GetFullDataPath("lists/videomodes").c_str());
	disp_data.num_modes = counter;
	disp_data.modes = new SDL_Rect [disp_data.num_modes];
	counter = 0;
	for (i = 0; i < disp_data.num_modes; i++)
	{
		int w, h;
		vmf >> w;
		vmf >> h;
		if (SDL_VideoModeOK(w,h, 16, videoFlags))
		{
			//cout << w << "x" << h << endl;
			disp_data.modes[counter].w = w;
			disp_data.modes[counter].h = h;
			counter++;
		}
	}
	
	disp_data.num_modes = counter;

	//load car parts list for customization menu
	string part;
	//bool found;
	vmf.close();
	vmf.clear();
	vmf.open(settings.GetFullDataPath("carparts/parts_list").c_str());
	while( getline(vmf, part, '\n') )
	{
		parts.push_back(part);
		parts_names.push_back(settings.GetCarPartName(part));
		//cout << "Part pathname: " << part << endl;
	}
	ReloadCarParts();

	//load control ui
	BuildControls();

	//load lists
	BuildCFList();
	BuildTSList();

	//load generic pages
	numpages = 32;
	pageslot = 0;
	page = new MENU_PAGE [numpages];
	LoadPage("About");
	LoadPage("AutoClutch");
	LoadPage("ControlOpts");
	LoadPage("Deadzone");
	LoadPage("Help1");
	LoadPage("JoyType");
	LoadPage("Main");
	LoadPage("MouseSettings");
	LoadPage("MouseXAxisSensitivity");
	LoadPage("MouseYAxisDeadzone");
	LoadPage("MouseYAxisSensitivity");
	LoadPage("NextRelease");
	LoadPage("Options");
	LoadPage("QuitConfirm");
	LoadPage("Replay");
	LoadPage("TouchComp");
	LoadPage("ResetBestLapTime");
	LoadPage("RestartConfirm");
	LoadPage("Tools");
	LoadPage("NewGameSetup");
	LoadPage("SelectMode");
	LoadPage("SoundSettings");
	LoadPage("SoundVolume");
	LoadPage("ConnectionError");
	LoadPage("DisplaySettings");
	LoadPage("InputSettings");
	LoadPage("TreeDetail");
	LoadPage("TerrainDetail");
	LoadPage("TexSize");
	LoadPage("MPHorKPH");
	LoadPage("QuitToChange");
	LoadPage("ViewDistance");
	
	loaded = true;
}

MENU::MENU()
{
	error_log.open((settings.GetSettingsDir() + "/logs/menu.log").c_str());
	
	disp_data.num_modes = 0;
	disp_data.sel = 0;
	sel = 0;
	subsel = 0;
	disp_data.bpp32 = false;
	disp_data.fullscreen = false;
	disp_data.modes = NULL;
	
	page = NULL;
	numpages = 0;
	
	controlinfo.desc = NULL;
	controlinfo.token = NULL;
	
	controlwait = false;
	
	cflist = NULL;
	rotation = 0;
	
	car = NULL;
	
	numts = 0;

	net_host = settings.GetIsHosting();
	net_ip = settings.GetServerIP();
	char temp[10];
	sprintf(temp, "%d", settings.GetServerPort());
	port_input = temp;
	ip_or_port = true;
	
	loaded = false;
}

extern bool verbose_output;
MENU::~MENU()
{
	error_log.close();
	
	if (verbose_output)
		cout << "menu deinit" << endl;
	
	if (loaded)
	{
		if (disp_data.modes != NULL)
		{
			delete [] disp_data.modes;
		}
	
		ClearCFList();
		
		int i;
		
		if (page != NULL)
		{	
			for (i = 0; i < numpages; i++)
			{
				if (page[i].item != NULL && page[i].numitems > 0)
					delete [] page[i].item;
			}
			
			delete [] page;
		}
		
		if (controlinfo.desc != NULL)
			delete [] controlinfo.desc;
		if (controlinfo.token != NULL)
			delete [] controlinfo.token;
		
		glDeleteTextures( 1, &backbox );
		glDeleteTextures( 1, &logo );
		glDeleteTextures( 1, &sphere_reflection );
	}
}

void MENU::InMenu(bool new_in_menu)
{
	in_menu = new_in_menu;
	multiplay.in_menu = new_in_menu;
}

bool MENU::InMenu()
{
	return in_menu;
}

/*void MENU::InDisplay(bool new_in_display)
{
	//in_display = new_in_display;
	if (new_in_display)
		curmenu = "Display";
}

bool MENU::InDisplay()
{
	//return in_display;
	return (curmenu == "Display");
}*/

void MENU::MenuDraw()
{
	if (!in_menu)
		return;

	Draw2D(0,0,1,1,backbox,0);
	
	float xp, yp, w, h;
	xp = 0.625;
	w = 0.375;
	//w = 0.4;
	xp = 1.0 - w;
	
	//h = w*1.3333333;
	h = 0.5;
	yp = 1-h;
	Draw2D(xp,yp,xp+w,yp+h,logo,0);
	
	if (In("Display"))
		DrawDisplay();
	else if (In("ControlSetup"))
		DrawControlSetup();
	else if (In("SelectCar"))
		DrawSelectCar();
	else if (In("Calibrate"))
		DrawCalibrate();
	else if (In("SelectTrack"))
		DrawSelectTrack();
	else if (In("NetSetup"))
		DrawNetSetup();
	else if (In("CustomizeCar"))
		DrawCustomizeCar();
	else
		DrawAutomatic();
}

void MENU::Draw2D(float x1, float y1, float x2, float y2, GLuint texid, float rotation)
{
	int sx, sy;
	
	QUATERNION rot;
	rot.Rotate(rotation, 0,0,1);
	
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	
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

extern float timefactor;
extern void ChangeDisplay(int w, int h, int bpp, bool fullscreen, bool reloadtextures);

//extern void SelectTrack(string tfile);

void MENU::MenuKey(SDLKey key)
{
	if (!InMenu())
		return;
	
	MENU_PAGE * cp = curp();
	
	if (In("Display"))
	{
		if (key == SDLK_DOWN)
		{
			disp_data.sel++;
			if (disp_data.sel >= disp_data.num_modes)
				disp_data.sel = 0;
		}
		
		if (key == SDLK_UP)
		{
			disp_data.sel--;
			if (disp_data.sel < 0)
				disp_data.sel = disp_data.num_modes-1;
		}
		
		if (key == 'b')
			disp_data.bpp32 = !disp_data.bpp32;
		
		if (key == 'f')
			disp_data.fullscreen = !disp_data.fullscreen;
		
		if (key == SDLK_RETURN)
		{
			int bpp;
			if (disp_data.bpp32)
				bpp = 32;
			else
				bpp = 16;
			ChangeDisplay(disp_data.modes[disp_data.sel].w, disp_data.modes[disp_data.sel].h, bpp, disp_data.fullscreen, true);
			
			//InMenu(false);
			//In("Display", false);
			//timefactor = 1.0f;
#ifdef _WIN32
			Go("QuitToChange");
#else
#ifdef __APPLE__
			Go("QuitToChange");
#else
			Go("DisplaySettings");
#endif
#endif
		}
	}
	else if (In("ControlSetup"))
	{
		if (!controlwait)
		{
			if (key == SDLK_DOWN)
			{
				sel++;
				if (sel >= controlinfo.num_tokens)
					sel = 0;
			}
			
			if (key == SDLK_UP)
			{
				sel--;
				if (sel < 0)
					sel = controlinfo.num_tokens-1;
			}
			
			if (key == SDLK_RIGHT)
			{
				controlinfo.xsel++;
				if (controlinfo.xsel >= NUM_CONTROL_MAPPING)
					controlinfo.xsel = 0;
			}
			
			if (key == SDLK_LEFT)
			{
				controlinfo.xsel--;
				if (controlinfo.xsel < 0)
					controlinfo.xsel = NUM_CONTROL_MAPPING-1;
			}
		
			if (key == 'k')
			{
				controlwait = true;
				controlwaittype = "key";
			}
			
			if (key == 'a')
			{
				controlwait = true;
				controlwaittype = "joyaxis";
			}
			
			if (key == 'b')
			{
				controlwait = true;
				controlwaittype = "joybutton";
			}
			
			if (key == 't')
			{
				togglemode = 0;
				ToggleControlOpts();
			}
			
			if (key == 'd')
			{
				togglemode = 1;
				ToggleControlOpts();
			}
			
			if (key == 'c')
			{
				UnbindSlot();
			}
			if (key == SDLK_RETURN)
			{
				Go("InputSettings");
			}
		}
		else
		{
			//cout << (int) ((char) Key) << endl;
			if (controlwaittype == "key" && key != SDLK_ESCAPE)
			{
				AssignKey((int) key);
				controlwait = false;
			}
		}
	}
	else if (In("SelectCar"))
	{
		if (key == SDLK_RIGHT)
		{
			sel++;
			if (sel >= numcf)
				sel = 0;
			//SelectCar(cflist[sel], true, false);
			//WriteCarSelection();
			LoadCar(cflist[sel]);
			subsel = 0;
		}
		
		if (key == SDLK_LEFT)
		{
			sel--;
			if (sel < 0)
				sel = numcf-1;
			//SelectCar(cflist[sel], true, false);
			//WriteCarSelection();
			LoadCar(cflist[sel]);
			subsel = 0;
		}
		if (key == SDLK_DOWN)
		{
			subsel++;
			car->SetPaint(subsel);
		}
		if (key == SDLK_UP)
		{
			subsel--;
			car->SetPaint(subsel);
		}
		if (key == SDLK_RETURN)
		{
			//SelectCar(cflist[sel], true, true);
			
			WriteCarSelection();
			ReloadCarParts();

			ClearCar();
			Go("NewGameSetup");
		}
	}
	else if (In("Calibrate"))
	{
		if (key == SDLK_RETURN)
		{
			//SelectCar(cflist[sel], true, true);
			
			//WriteCarSelection();
			//ClearCar();
			gamecontrols.WriteCalibration();
			Go("ControlOpts");
		}
	}
	else if (In("SelectTrack"))
	{
		if (key == SDLK_RIGHT)
		{
			sel++;
			if (sel >= numts)
				sel = 0;
			//SelectCar(cflist[sel], true, false);
			//WriteCarSelection();
			//LoadCar(cflist[sel]);
		}
		
		if (key == SDLK_LEFT)
		{
			sel--;
			if (sel < 0)
				sel = numts-1;
			//SelectCar(cflist[sel], true, false);
			//WriteCarSelection();
			//LoadCar(cflist[sel]);
		}
		
		if (key == SDLK_RETURN)
		{
			//WriteCarSelection();
			//ClearCar();
			//SelectTrack(tslist[sel]);
			state.SetTrackName(tslist[sel]);
			Go("NewGameSetup");
		}
	}
	else if (In("NetSetup"))
	{
		if (key == SDLK_LEFT || key == SDLK_RIGHT)
			net_host = !net_host;
		
		if (!net_host)
		{
			if (key == SDLK_UP || key == SDLK_DOWN)
				ip_or_port = !ip_or_port;

			if (ip_or_port)
			{
				if (key == '0' || key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6' || key == '7' || key == '8' || key == '9' || key == '.')
				{
					if (net_ip.length() < 16)
					{
						string keystr;
						keystr.append((char *) &key);
						net_ip = net_ip + keystr;
					}
				}
			
				if (key == SDLK_BACKSPACE)
				{
					char tempchar[256];
					int len = net_ip.length();
					len --;
					int i;
					if (len >= 0)
					{
						for (i = 0; i < len; i++)
						{
							tempchar[i] = net_ip.c_str()[i];
						}
						tempchar[i] = '\0';
						
						net_ip = tempchar;
					}
				}
			}
			else
			{
				if (key == '0' || key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6' || key == '7' || key == '8' || key == '9')
				{
					if (port_input.length() < 6)
					{
						string keystr;
						keystr.append((char *) &key);
						port_input = port_input + keystr;
					}
				}
			
				if (key == SDLK_BACKSPACE)
				{
					char tempchar[256];
					int len = port_input.length();
					len --;
					int i;
					if (len >= 0)
					{
						for (i = 0; i < len; i++)
						{
							tempchar[i] = port_input.c_str()[i];
						}
						tempchar[i] = '\0';
	
						port_input = tempchar;
					}
				}
			}
		}
		else
		{
			if (key == '0' || key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6' || key == '7' || key == '8' || key == '9')
			{
				if (port_input.length() < 6)
				{
					string keystr;
					keystr.append((char *) &key);
					port_input = port_input + keystr;
				}
			}
			
			if (key == SDLK_BACKSPACE)
			{
				char tempchar[256];
				int len = port_input.length();
				len --;
				int i;
				if (len >= 0)
				{
					for (i = 0; i < len; i++)
					{
						tempchar[i] = port_input.c_str()[i];
					}
					tempchar[i] = '\0';
					
					port_input = tempchar;
				}
			}
		}

		if (key == SDLK_RETURN)
		{
			settings.SetIsHosting(net_host);
			settings.SetServerIP(net_ip);
			settings.SetServerPort(atoi(port_input.c_str()));
			Go("NewGameSetup");
		}
	}
	else if (In("CustomizeCar"))
	{
		if (key == SDLK_SPACE || key == SDLK_RIGHT || key == SDLK_LEFT)
		{
			// toggle a part on or off
			parts_enabled[part_selected] = !parts_enabled[part_selected];
		}
		if (key == SDLK_UP)
		{
			// move up parts list
			// at top, loop to bottom
			if( part_selected == 0 )
			{
				part_selected = parts.size() - 1;
			}
			else
			{
				part_selected--;
			}
		}
		if (key == SDLK_DOWN)
		{
			// move down parts list
			// at bottom, loop to top
			if( part_selected == parts.size() - 1 )
			{
				part_selected = 0;
			}
			else
			{
				part_selected++;
			}
		}
		if (key == SDLK_RETURN)
		{
			vector<string> sv;
			for( unsigned int i = 0; i < parts.size(); i++ )
				if(parts_enabled[i]) sv.push_back(parts[i]);

/*			Vamos_Body::Car *c = world.GetPlayerCar();
			c->SaveParts(sv);
*/
			settings.SaveCarPartsSettings(sv);
			Go("NewGameSetup");
		}
	}
	else if (cp != NULL)
	{
		bool hassel = false;
		int i;
		for (i = 0; i < cp->numitems; i++)
			if (cp->item[i].selectable)
				hassel = true;
		
		//automatically handle keys
		if (hassel)
		{
			if (key == SDLK_DOWN)
			{
				int newsel = sel;
				newsel++;
				int count = 0;
				
				while (count < cp->numitems && !cp->item[newsel].selectable)
				{
					newsel++;
					newsel = newsel % cp->numitems;
					count++;
				}
				
				newsel = newsel % cp->numitems;
				
				//if (newsel > sel)
				if (cp->item[newsel].selectable)
				{
					sel = newsel;
				}
			}
			
			if (key == SDLK_UP)
			{
				int newsel = sel;
				newsel--;
				int count = 0;
				
				while (count < cp->numitems && !cp->item[newsel].selectable)
				{
					newsel--;
					//cout << newsel << endl;
					if (newsel < 0)
						newsel = cp->numitems + newsel;
					count++;
				}
				
				//cout << newsel << endl;
				
				if (newsel < 0)
						newsel = cp->numitems + newsel;
				
				//if (newsel < sel)
				if (cp->item[newsel].selectable)
					sel = newsel;
			}
			
			if (key == SDLK_RETURN)
			{
				Go(cp->item[sel].go_to);
			}
		}
	}
	
	if (key == SDLK_ESCAPE)
	{	
		if (controlwait)
			controlwait = false;
		else
		{
			string last_menu = "Main";
			if( !lastmenu.empty() ) last_menu = lastmenu.back();
			if (In("SelectCar"))
			{
				//SelectCar(cflist[sel], true, true);
				ClearCar();
				Go("NewGameSetup");
			}
			else if (In("ControlSetup"))
			{
				Go("InputSettings");
			}
			else if (In("SelectTrack"))
			{
				//SelectCar(cflist[sel], true, true);
				//ClearCar();
				Go("NewGameSetup");
			}
			else if (In("Calibrate"))
			{
				gamecontrols.LoadCalibration();
				Go("InputSettings");
				cout << "loaded calibration" << endl;
			}
			else if (In("Main"))
			{
				if( state.GetGameState() != STATE_INITIALMENU) Go("Close");
				else Go("QuitConfirm");
			}
			else
			{
				Go(last_menu);
			}
		}
	}
}

MENU_PAGE * MENU::curp()
{
	int i;
	for (i = 0; i < numpages; i++)
	{
		if (curmenu == page[i].name)
			return &page[i];
	}
	
	return NULL;
}

void MENU::DrawDisplay()
{	
	int i;
	char tempchar[128];
	
	//float cx = 0.03;
	float cx = 0.3;
	float cy = 0.02;
	
	string bpp, fs;
	if (disp_data.bpp32)
		bpp = "32bit";
	else
		bpp = "16bit";
	if (disp_data.fullscreen)
		fs = "fullscreen";
	else
		fs = "windowed";
	
	font.Print(cx, cy, "Press B to toggle 16 or 32 bit", 1, 6, 1,1,1, 1);
	cy += 0.03;
	font.Print(cx, cy, "Press F to toggle fullscreen", 1, 6, 1,1,1, 1);
	cy += 0.03;
	font.Print(cx, cy, "ENTER to accept", 1, 6, 1,1,1, 1);
	//cy += 0.03;
	cy += 0.07;
	sprintf(tempchar, "%s  %s", bpp.c_str(), fs.c_str());
	//font.Print(cx, 0.02, tempchar, 1, 6, 1);
	font.Print(cx, cy, tempchar, 1, 6, 1,1,0, 1);
	cy += 0.05;
	
	for (i = 0; i < disp_data.num_modes; i++)
	{
		//sprintf(tempchar, "%ix%i %s %s", disp_data.modes[i].w, disp_data.modes[i].h, bpp.c_str(), fs.c_str());
		sprintf(tempchar, "%ix%i", disp_data.modes[i].w, disp_data.modes[i].h);
		if (i == disp_data.sel)
			font.Print(cx,cy, tempchar, 1, 6, 1,1,0);
		else
			font.Print(cx,cy, tempchar, 1, 6, 0.5,0.5,0, 1);
		//cy += 0.02;
		cy += 0.03;
	}
}

void MENU::MainMenu()
{
	//state.SetGameState(STATE_MENU);
	
	controlwait = false;
	InMenu(true);
	//InDisplay(true);
	//InDisplay(false);
	
	if (multiplay.NumConnected() == 0)
		timefactor = 0.0f;
	sel = 0;
	subsel = 0;
	controlinfo.xsel = 0;
	disp_data.sel = 0;
	//curmenu = "Main";
	Go("Main");
}

bool MENU::In(string menuname)
{
	return (curmenu == menuname);
}

extern void Quit( int returnCode );

extern void ResetWorld(bool fullreset);

extern void ToggleFPSDisplay();

extern bool LoadWorld();

void MENU::Go(string menuname)
{
	string last_menu = "";
	if(!lastmenu.empty()) last_menu = lastmenu.back();
	if( menuname == last_menu )
	{
		if( last_menu != "Main")
		{
			//cout << "ascending menus, popping " << last_menu << endl;
			if(!lastmenu.empty()) lastmenu.pop_back();
		}
	}
	else
	{
		if(curmenu != "")
		{
			//cout << "descending menus, pushing " << curmenu << endl;
			lastmenu.push_back(curmenu);
		}
	}

	curmenu = menuname;

	sel = 0;
	subsel = 0;
	controlinfo.xsel = 0;

	if (curmenu == "Quit")
		Quit( 0 );
	else if (curmenu == "Close")
	{
		if (state.GetGameState() != STATE_INITIALMENU)
		{
			InMenu(false);
			//In("Display", false);
			timefactor = 1.0f;
		}
	}
	else if (curmenu == "JoyTypeWheel")
	{
		gamecontrols.SetJoystickType("wheel");
		Go("ControlOpts");
	}
	else if (curmenu == "JoyTypeStick")
	{
		gamecontrols.SetJoystickType("joystick");
		Go("ControlOpts");
	}
	else if (curmenu == "ResetBest")
	{
		timer.ResetBest();
		Go("Tools");
	}
	else if (curmenu == "StartRecording")
	{
		if (state.GetGameState() != STATE_PLAYING && state.GetGameState() != STATE_STAGING)
		{
			Go("Replay");
			return;
		}
		replay.Start();
		Go("Close");
	}
	else if (curmenu == "StopRecording")
	{
		replay.Stop();
		Go("Close");
	}
	else if (curmenu == "StartReplay")
	{
		if (state.GetGameState() != STATE_PLAYING && state.GetGameState() != STATE_INITIALMENU && state.GetGameState() != STATE_STAGING)
		{
			//cout << state.GetGameState() << endl;
			return;
		}
		replay.PlayStart("1");
		Go("Close");
	}
	else if (curmenu == "StartGhostCarReplay")
	{
		if (state.GetGameState() != STATE_PLAYING && state.GetGameState() != STATE_INITIALMENU && state.GetGameState() != STATE_STAGING)
		{
			//cout << state.GetGameState() << endl;
			return;
		}
		replay.PlayStart("1", true);
		Go("Close");
	}
	else if (curmenu == "StopReplay")
	{
		replay.PlayStop();
		Go("Close");
	}
	else if (curmenu == "TouchCompOff")
	{
		gamecontrols.SetCompensation("off");
		Go("ControlOpts");
	}
	else if (curmenu == "TouchCompLow")
	{
		gamecontrols.SetCompensation("low");
		Go("ControlOpts");
	}
	else if (curmenu == "TouchCompMed")
	{
		gamecontrols.SetCompensation("med");
		Go("ControlOpts");
	}
	else if (curmenu == "TouchCompHigh")
	{
		gamecontrols.SetCompensation("high");
		Go("ControlOpts");
	}
	else if (curmenu == "TouchComp900to200")
	{
		gamecontrols.SetCompensation("900to200");
		Go("ControlOpts");
	}
	else if (curmenu == "DeadzoneOff")
	{
		gamecontrols.SetDeadzone("off");
		Go("ControlOpts");
	}
	else if (curmenu == "DeadzoneLow")
	{
		gamecontrols.SetDeadzone("low");
		Go("ControlOpts");
	}
	else if (curmenu == "DeadzoneMed")
	{
		gamecontrols.SetDeadzone("med");
		Go("ControlOpts");
	}
	else if (curmenu == "DeadzoneHigh")
	{
		gamecontrols.SetDeadzone("high");
		Go("ControlOpts");
	}
	else if (curmenu == "Refuel")
	{
		if (state.GetGameState() == STATE_PLAYING)
		{
			world.FuelPlayerCar();
			Go("Close");
		}
	}
	else if (curmenu == "ResetWorld")
	{
		if (state.GetGameState() == STATE_PLAYING)
		{
			ResetWorld(true);
			Go("Close");
		}
	}
	else if (curmenu == "RestartConfirm")
	{
		if (state.GetGameState() != STATE_PLAYING)
		{
			Go("NewGameSetup");
		}
	}
	else if (curmenu == "FPSToggle")
	{
		ToggleFPSDisplay();
		Go("DisplaySettings");
	}
	else if (curmenu == "HUDToggle")
	{
		//ToggleFPSDisplay();
		world.ToggleHUD();
		Go("DisplaySettings");
	}
	else if (curmenu == "HighViewDistance")
	{
		settings.SetViewDistance( 10000.0 );
		Go("DisplaySettings");
	}
	else if (curmenu == "LowViewDistance")
	{
		settings.SetViewDistance( 500.0 );
		Go("DisplaySettings");
	}
	else if (curmenu == "TexSizeSmall")
	{
		settings.SetTexSize("small");
		Go("QuitToChange");
	}
	else if (curmenu == "TexSizeMedium")
	{
		settings.SetTexSize("medium");
		Go("QuitToChange");
	}
	else if (curmenu == "TexSizeLarge")
	{
		settings.SetTexSize("large");
		Go("QuitToChange");
	}
	else if (curmenu == "Vol0")
	{
		sound.SetMasterVolume(0.0);
		settings.SetSoundVolume(0.0);
		Go("SoundSettings");
	}
	else if (curmenu == "Vol10")
	{
		sound.SetMasterVolume(0.1);
		settings.SetSoundVolume(0.1);
		Go("SoundSettings");
	}
	else if (curmenu == "Vol20")
	{
		sound.SetMasterVolume(0.2);
		settings.SetSoundVolume(0.2);
		Go("SoundSettings");
	}
	else if (curmenu == "Vol30")
	{
		sound.SetMasterVolume(0.3);
		settings.SetSoundVolume(0.3);
		Go("SoundSettings");
	}
	else if (curmenu == "Vol40")
	{
		sound.SetMasterVolume(0.4);
		settings.SetSoundVolume(0.4);
		Go("SoundSettings");
	}
	else if (curmenu == "Vol50")
	{
		sound.SetMasterVolume(0.5);
		settings.SetSoundVolume(0.5);
		Go("SoundSettings");
	}
	else if (curmenu == "Vol60")
	{
		sound.SetMasterVolume(0.6);
		settings.SetSoundVolume(0.6);
		Go("SoundSettings");
	}
	else if (curmenu == "Vol70")
	{
		sound.SetMasterVolume(0.7);
		settings.SetSoundVolume(0.7);
		Go("SoundSettings");
	}
	else if (curmenu == "Vol80")
	{
		sound.SetMasterVolume(0.8);
		settings.SetSoundVolume(0.8);
		Go("SoundSettings");
	}
	else if (curmenu == "Vol90")
	{
		sound.SetMasterVolume(0.9);
		settings.SetSoundVolume(0.9);
		Go("SoundSettings");
	}
	else if (curmenu == "Vol100")
	{
		sound.SetMasterVolume(1.0);
		settings.SetSoundVolume(1.0);
		Go("SoundSettings");
	}
	else if (curmenu == "MouseToggle")
	{
		mouse.SetMouseEnabled(!mouse.GetMouseEnabled());
		settings.SetMouseEnabled(mouse.GetMouseEnabled());
		Go("MouseSettings");
	}
	else if (curmenu == "MouseXSensVL")
	{
		mouse.SetSensitivityX(0.75);
		settings.SetMouseXSens(0.75);
		Go("MouseSettings");
	}
	else if (curmenu == "MouseXSensL")
	{
		mouse.SetSensitivityX(0.9);
		settings.SetMouseXSens(0.9);
		Go("MouseSettings");
	}
	else if (curmenu == "MouseXSensM")
	{
		mouse.SetSensitivityX(1.0);
		settings.SetMouseXSens(1.0);
		Go("MouseSettings");
	}
	else if (curmenu == "MouseXSensH")
	{
		mouse.SetSensitivityX(1.1);
		settings.SetMouseXSens(1.1);
		Go("MouseSettings");
	}
	else if (curmenu == "MouseXSensVH")
	{
		mouse.SetSensitivityX(1.25);
		settings.SetMouseXSens(1.25);
		Go("MouseSettings");
	}
	else if (curmenu == "MouseYDeadVL")
	{
		mouse.SetDeadzoneY(0.0);
		settings.SetMouseYDead(0.0);
		Go("MouseSettings");
	}
	else if (curmenu == "MouseYDeadL")
	{
		mouse.SetDeadzoneY(0.1);
		settings.SetMouseYDead(0.1);
		Go("MouseSettings");
	}
	else if (curmenu == "MouseYDeadM")
	{
		mouse.SetDeadzoneY(0.2);
		settings.SetMouseYDead(0.2);
		Go("MouseSettings");
	}
	else if (curmenu == "MouseYDeadH")
	{
		mouse.SetDeadzoneY(0.3);
		settings.SetMouseYDead(0.3);
		Go("MouseSettings");
	}
	else if (curmenu == "MouseYDeadVH")
	{
		mouse.SetDeadzoneY(0.4);
		settings.SetMouseYDead(0.4);
		Go("MouseSettings");
	}
	else if (curmenu == "MouseYSensVL")
	{
		mouse.SetSensitivityY(0.75);
		settings.SetMouseYSens(0.75);
		Go("MouseSettings");
	}
	else if (curmenu == "MouseYSensL")
	{
		mouse.SetSensitivityY(0.9);
		settings.SetMouseYSens(0.9);
		Go("MouseSettings");
	}
	else if (curmenu == "MouseYSensM")
	{
		mouse.SetSensitivityY(1.0);
		settings.SetMouseYSens(1.0);
		Go("MouseSettings");
	}
	else if (curmenu == "MouseYSensH")
	{
		mouse.SetSensitivityY(1.1);
		settings.SetMouseYSens(1.1);
		Go("MouseSettings");
	}
	else if (curmenu == "MouseYSensVH")
	{
		mouse.SetSensitivityY(1.25);
		settings.SetMouseYSens(1.25);
		Go("MouseSettings");
	}
	else if (curmenu == "SelectCar")
	{
		//if (state.GetGameState() == STATE_PLAYING)
		{
			//SelectCar(cflist[0], true, true);
			LoadCar(cflist[0]);
			//WriteCarSelection();
		}
	}
	else if (curmenu == "BackToGame")
	{
		if (state.GetGameState() == STATE_PLAYING)
			Go("Close");
		else
			Go("NewGameSetup");
	}
	else if (curmenu == "NewGame")
	{
		bool go = true;
		
		multiplay.Disconnect();
		replay.Stop();
		replay.PlayStop();
		
		if (replay.Recording() != -1)
			replay.Stop();
		if (replay.Playing() != -1)
			replay.PlayStop();
		
		if (state.GetGameMode() == MODE_TIMETRIAL)
		{
			LoadWorld();
			state.SetGameState(STATE_STAGING);
		}
		else if (state.GetGameMode() == MODE_NETMULTIFREE)
		{
			if (!net_host)
				go = multiplay.Connect(net_ip);
			else
			{
				go = multiplay.Host();
				LoadWorld();
			}
			if (go)
			{
				state.SetGameState(STATE_PLAYING);
			}
		}
		else
		{
			LoadWorld();
			state.SetGameState(STATE_PLAYING);
		}
		
		if (go)
			Go("Close");
		else
		{
			Go("ConnectionError");
		}
	}
	else if (curmenu == "Calibrate")
	{
		gamecontrols.MinimizeCalibration();
	}
	else if (curmenu == "FreeDrive")
	{
		state.SetGameMode(MODE_FREEDRIVE);
		Go("NewGameSetup");
	}
	else if (curmenu == "TimeTrial")
	{
		state.SetGameMode(MODE_TIMETRIAL);
		Go("NewGameSetup");
	}
	else if (curmenu == "NetMultiFree")
	{
		state.SetGameMode(MODE_NETMULTIFREE);
		//net_ip = "";
		//net_host = true;
		Go("NetSetup");
	}
	else if (curmenu == "TreeDetailMax")
	{
		state.SetTreeDetail("Max");
		Go("DisplaySettings");
	}
	else if (curmenu == "TreeDetailHigh")
	{
		state.SetTreeDetail("High");
		Go("DisplaySettings");
	}
	else if (curmenu == "TreeDetailMed")
	{
		state.SetTreeDetail("Med");
		Go("DisplaySettings");
	}
	else if (curmenu == "TreeDetailLow")
	{
		state.SetTreeDetail("Low");
		Go("DisplaySettings");
	}
	else if (curmenu == "TreeDetailFoliageOnly")
	{
		state.SetTreeDetail("FoliageOnly");
		Go("DisplaySettings");
	}
	else if (curmenu == "TreeDetailOff")
	{
		state.SetTreeDetail("Off");
		Go("DisplaySettings");
	}
	else if (curmenu == "TerrainDetailHigh")
	{
		state.SetTerrainDetail("High");
		Go("DisplaySettings");
	}
	else if (curmenu == "TerrainDetailMed")
	{
		state.SetTerrainDetail("Med");
		Go("DisplaySettings");
	}
	else if (curmenu == "TerrainDetailLow")
	{
		state.SetTerrainDetail("Low");
		Go("DisplaySettings");
	}
	else if (curmenu == "SpeedMPH")
	{
		settings.SetMPH(true);
		Go("DisplaySettings");
	}
	else if (curmenu == "SpeedKPH")
	{
		settings.SetMPH(false);
		Go("DisplaySettings");
	}
	else if (curmenu == "AutoClutchOn")
	{
		settings.SetAutoClutch(true);
		Go("InputSettings");
	}
	else if (curmenu == "AutoClutchOff")
	{
		settings.SetAutoClutch(false);
		Go("InputSettings");
	}
	else
	{
		MENU_PAGE * cp = curp();
		if (cp != NULL)
		{
			bool hassel = false;
			int i;
			for (i = 0; i < cp->numitems; i++)
				if (cp->item[i].selectable)
					hassel = true;
			if (hassel)
			{
				/*//special code to have the current value pre-selected
				if (In("TouchComp"))
				{
					
				}*/
				
				int newsel = sel;
				int count = 0;
				
				while (count < cp->numitems && !cp->item[newsel].selectable)
				{
					newsel++;
					newsel = newsel % cp->numitems;
					count++;
				}
				
				if (newsel > sel)
					sel = newsel;
			}
		}
	}
}

void MENU::LoadPage(string pagename)
{	
	string name, temp, go_to;
	go_to = "nowhere";
	float r,g,b;
	r = g = b = 0.8f;
	bool selectable = false;
	int size = 6;
	float spaceafter = 0.03;
	
	//open the file
	ifstream pf;
	pf.open(settings.GetFullDataPath("lists/menus/" + pagename).c_str());
	if (!pf)
	{
		error_log << "Couldn't load menu: " << settings.GetFullDataPath("lists/menus/" + pagename) << endl;
		return;
	}
	
	MENU_PAGE * cp = &page[pageslot];
	cp->name = pagename;
	pageslot++;
	
	//start counting items
	cp->numitems = 0;
	
	temp = utility.sGetParam(pf);
	
	while (!pf.eof())
	{
		if (temp == "!")
			cp->numitems++;
		//else
			//cout << "non-!" << endl;
		
		temp = utility.sGetParam(pf);
	}
	
	pf.close();
	pf.clear();
	
	//allocate memory
	cp->item = new MENU_ITEM [cp->numitems];
	
	pf.open(settings.GetFullDataPath("lists/menus/" + pagename).c_str());
	
	cp->x = utility.fGetParam(pf);
	cp->y = utility.fGetParam(pf);
	
	int curitem = 0;
	int curplace = 0;
	
	temp = utility.sGetLine(pf);

	while (!pf.eof())
	{
		if (temp == "!")
		{
			//encountered the next item.  write values for this item first
			cp->item[curitem].name = name;
			cp->item[curitem].r = r;
			cp->item[curitem].g = g;
			cp->item[curitem].b = b;
			cp->item[curitem].selectable = selectable;
			cp->item[curitem].size = size;
			cp->item[curitem].go_to = go_to;
			cp->item[curitem].spaceafter = spaceafter;
			
			curplace = 0;
		
			curitem++;
		}
		else
		{
			switch (curplace)
			{
				case 0:
					name = temp;
					break;
				
				case 1:
					if (temp == "false" || temp == "no")
						selectable = false;
					else
					{
						selectable = true;
						go_to = temp;
					}
					break;
				
				case 2:
					size = atoi(temp.c_str());
					break;
				
				case 3:
					spaceafter = atof(temp.c_str());
					break;
				
				case 4:
					r = atof(temp.c_str());
					break;
				
				case 5:
					g = atof(temp.c_str());
					break;
				
				case 6:
					b = atof(temp.c_str());
					break;
			}
			
			curplace++;
		}
		
		if (curplace == 0)
			temp = utility.sGetLine(pf);
		else
			temp = utility.sGetParam(pf);
	}

	//make sure to get last item	
	/*cp->item[curitem].name = name;
	cp->item[curitem].r = r;
	cp->item[curitem].g = g;
	cp->item[curitem].b = b;
	cp->item[curitem].selectable = selectable;
	cp->item[curitem].size = size;
	cp->item[curitem].spaceafter = spaceafter;*/
	
	/*for (int i = 0; i < cp->numitems; i++)
		cout << cp->item[i].name << endl;*/
	
	pf.close();
	
	//cout << "Loaded " << cp->numitems << " menu items." << endl;
}

MENU_PAGE::MENU_PAGE()
{
	item = NULL;
}

void MENU::DrawAutomatic()
{
	int i;
	
	MENU_PAGE * cp = NULL;
	
	for (i = 0; i < numpages; i++)
	{
		if (In(page[i].name))
		{
			cp = &page[i];
		}
	}
	
	float selectmodify = 0.3;
	
	if (cp != NULL)
	{
		float cx, cy;
		cx = cp->x;
		cy = cp->y;
		
		//do the actual drawing
		for (i = 0; i < cp->numitems; i++)
		{
			string drawstring = cp->item[i].name;
			
			//special case for control options page
			if (In("ControlOpts"))
			{
				if (drawstring == "Joystick Type")
					drawstring = drawstring + " - " + gamecontrols.GetJoystickType();
				if (drawstring == "Touchiness Compensation")
					drawstring = drawstring + " - " + gamecontrols.GetCompensation();
				if (drawstring == "Deadzone")
					drawstring = drawstring + " - " + gamecontrols.GetDeadzone();
			}
			
			if (i == sel && cp->item[i].selectable)
				font.Print(cx,cy, drawstring.c_str(), 1, cp->item[i].size, cp->item[i].r+selectmodify,cp->item[i].g+selectmodify,cp->item[i].b+selectmodify);
			else
				font.Print(cx,cy, drawstring.c_str(), 1, cp->item[i].size, cp->item[i].r,cp->item[i].g,cp->item[i].b);
			//cy += 0.02;
			cy += cp->item[i].spaceafter;
		}
	}
}

void MENU::DrawControlSetup()
{
	int i;
	
	float selectmodifyr = 0.7;
	float selectmodifyg = 0.0;
	float selectmodifyb = 0.0;
	//float selectmodify = 0.7;
	selectmodifyr = selectmodifyg = selectmodifyb = 0.3;
	int size = 5;
	float r, g, b;
	r = g = b = 0.8;
	if (!controlwait)
	{
		r = g = b = 0.5;
		g = 0.8;
	}
	float ycoord = 0.3;
	//float rightspace = 0.23;
	//float firstrightspace = 0.25;
	float rightspace = 0.18;
	float firstrightspace = 0.2;
	
	float cx, cy;
	cx = 0.03;
	cy = ycoord;
	
	float ty = 0.05;
	if (controlwait)
	{
		if (controlwaittype == "key")
			font.Print(cx, ty, "Press a key....", 1, 6, 1,1,1);
		else if (controlwaittype == "joybutton")
			font.Print(cx, ty, "Press a joystick/wheel button....", 1, 6, 1,1,1);
		else if (controlwaittype == "joyaxis")
			font.Print(cx, ty, "Move a joystick/wheel axis....", 1, 6, 1,1,1);
	}
	else
	{
		font.Print(cx, ty, "Select the control using the arrow keys", 1, 6, 1,1,1);
		ty += 0.03;
		font.Print(cx, ty, "Press K to input a new keyboard control", 1, 6, 1,1,1);
		ty += 0.03;
		font.Print(cx, ty, "Press A to input a new joystick/wheel axis", 1, 6, 1,1,1);
		ty += 0.03;
		font.Print(cx, ty, "Press B to input a new joystick/wheel button", 1, 6, 1,1,1);
		ty += 0.03;
		font.Print(cx, ty, "Press D to toggle +/- or down/up", 1, 6, 1,1,1);
		ty += 0.03;
		font.Print(cx, ty, "Press T to toggle once/held", 1, 6, 1,1,1);
		ty += 0.03;
		font.Print(cx, ty, "Press C to clear the control", 1, 6, 1,1,1);
		//font.Print(cx, 0.05, "Select the control and slot using the arrow keys", 1, 6, 1,1,1);
	}
	
	float spaceafter = 0.03;
	
	float headr, headg, headb;
	headr = headg = headb = 5;
	headb = 0.8;
	
	font.Print(cx, ycoord-spaceafter, "Control Name", 1, 6, headr,headg,headb);
	font.Print(cx+firstrightspace, ycoord-spaceafter, "Primary", 1, 6, headr,headg,headb);
	font.Print(cx+firstrightspace+rightspace, ycoord-spaceafter, "Secondary", 1, 6, headr,headg,headb);
	font.Print(cx+firstrightspace+rightspace*2.0, ycoord-spaceafter, "Tertiary", 1, 6, headr,headg,headb);
	font.Print(cx+firstrightspace+rightspace*3.0, ycoord-spaceafter, "Quaternary", 1, 6, headr,headg,headb);
	
	spaceafter = 0.02;
	
	//draw left hand column
	for (i = 0; i < controlinfo.num_tokens; i++)
	{
		if (i == sel)
			font.Print(cx,cy, controlinfo.desc[i].c_str(), 1, size, r+selectmodifyr,g+selectmodifyg,b+selectmodifyb);
		else
			font.Print(cx,cy, controlinfo.desc[i].c_str(), 1, size, r,g,b);
		cy += spaceafter;
		//cy += cp->item[i].spaceafter;
	}
	
	cx += firstrightspace;
	
	//draw right columns
	int n;
	for (n = 0; n < NUM_CONTROL_MAPPING; n++)
	{
		cy = ycoord;
		
		for (i = 0; i < controlinfo.num_tokens; i++)
		{
			int num = n+1;
			int count = 0;
			string keystring = "None";
			string token = controlinfo.token[i];
			int j;
			for (j = 0; j < gamecontrols.GetNumControls(); j++)
			{
				if (gamecontrols.GetControls()[j].GetName() == token)
				{
					if (count == num-1)
					{
						char tempchar[32];
						
						if (gamecontrols.GetControls()[j].GetType() == Joy)
						{
							if (gamecontrols.GetControls()[j].GetJoyType() == Button)
								sprintf(tempchar, "Joy%dBut%d", gamecontrols.GetControls()[j].GetJoyNum(), gamecontrols.GetControls()[j].GetJoyButton());
							else if (gamecontrols.GetControls()[j].GetJoyType() == Axis)
								sprintf(tempchar, "Joy%dAxis%d", gamecontrols.GetControls()[j].GetJoyNum(), gamecontrols.GetControls()[j].GetJoyAxis());
							keystring = tempchar;
							if (gamecontrols.GetControls()[j].GetJoyType() == Button)
							{
								if (gamecontrols.GetControls()[j].GetJoyPushDown())
									keystring = keystring + "(down)";
								else
									keystring = keystring + "(up)";
								if (!gamecontrols.GetControls()[j].GetOneTime())
									keystring = keystring + "(held)";
								else
									keystring = keystring + "(once)";
							}
							else if (gamecontrols.GetControls()[j].GetJoyType() == Axis)
							{
								if (gamecontrols.GetControls()[j].GetJoyAxisPlus())
									keystring = keystring + "(+)";
								else
									keystring = keystring + "(-)";
							}
						}
						if (gamecontrols.GetControls()[j].GetType() == Key)
						{
							keystring = keyman.GetKeyName(gamecontrols.GetControls()[j].GetKeyCode());
							if (!gamecontrols.GetControls()[j].GetOneTime())
								keystring = keystring + "(held)";
							else
								keystring = keystring + "(once)";
							if (gamecontrols.GetControls()[j].GetKeyPushDown())
									keystring = keystring + "(down)";
								else
									keystring = keystring + "(up)";
						}
					}
					count++;
				}
			}
			
			if (i == sel && controlinfo.xsel == n)
				font.Print(cx,cy, keystring.c_str(), 1, size, r+selectmodifyr,g+selectmodifyg,b+selectmodifyb);
			else
				font.Print(cx,cy, keystring.c_str(), 1, size, r,g,b);
			cy += spaceafter;
			//cy += cp->item[i].spaceafter;
		}
		
		cx += rightspace;
	}
}

void MENU::AssignKey(int kcode)
{
	if (!In("ControlSetup"))
		return;
	
	string token = controlinfo.token[sel];
	
	bool added = false;
	
	//loop through the controls looking for a control to overwrite
	int num = controlinfo.xsel+1;
	int count = 0;
	int j;
	for (j = 0; j < gamecontrols.GetNumControls(); j++)
	{
		if (gamecontrols.GetControls()[j].GetName() == token)
		{
			if (count == num-1)
			{
				gamecontrols.GetControls()[j].SetType("key");
				gamecontrols.GetControls()[j].SetKeyCode(kcode);
				//cout << kcode << endl;
				gamecontrols.GetControls()[j].SetOneTime(true);
				
				/*char tempchar[32];
				
				if (gamecontrols.GetControls()[j].GetType() == Joy)
				{
					if (gamecontrols.GetControls()[j].GetJoyType() == Button)
						sprintf(tempchar, "Joy%dButton%d", gamecontrols.GetControls()[j].GetJoyNum(), gamecontrols.GetControls()[j].GetJoyButton());
					else if (gamecontrols.GetControls()[j].GetJoyType() == Axis)
						sprintf(tempchar, "Joy%dAxis%d", gamecontrols.GetControls()[j].GetJoyNum(), gamecontrols.GetControls()[j].GetJoyAxis());
					keystring = tempchar;
					if (gamecontrols.GetControls()[j].GetJoyType() == Button)
					{
						if (gamecontrols.GetControls()[j].GetJoyPushDown())
							keystring = keystring + "(down)";
						else
							keystring = keystring + "(up)";
					}
					else if (gamecontrols.GetControls()[j].GetJoyType() == Axis)
					{
						if (gamecontrols.GetControls()[j].GetJoyAxisPlus())
							keystring = keystring + "(+)";
						else
							keystring = keystring + "(-)";
					}
				}
				if (gamecontrols.GetControls()[j].GetType() == Key)
				{
					keystring = keyman.GetKeyName(gamecontrols.GetControls()[j].GetKeyCode());
					if (!gamecontrols.GetControls()[j].GetOneTime())
						keystring = keystring + "(held)";
				}*/
				
				added = true;
			}
			count++;
		}
	}
	
	if (!added)
	{
		CONTROL * newcontrols;
		newcontrols = new CONTROL[gamecontrols.GetNumControls()+1];
		for (j = 0; j < gamecontrols.GetNumControls(); j++)
		{
			newcontrols[j] = gamecontrols.GetControls()[j];
		}
		newcontrols[j].SetType("key");
		newcontrols[j].SetKeyCode(kcode);
		newcontrols[j].SetOneTime(true);
		newcontrols[j].SetName(token);
		gamecontrols.SetControls(newcontrols, gamecontrols.GetNumControls()+1);
		
		added = true;
	}
}

void MENU::BuildControls()
{
	if (controlinfo.desc != NULL)
	{
		delete [] controlinfo.desc;
		controlinfo.desc = NULL;
	}
	if (controlinfo.token != NULL)
	{
		delete [] controlinfo.token;
		controlinfo.token = NULL;
	}
		
	ifstream cl;
	cl.open(settings.GetFullDataPath("lists/control_list").c_str());
	if (!cl)
		error_log << "Couldn't open " + settings.GetFullDataPath("lists/control_list") << endl;
	int counter = 0;
	string junk = utility.sGetLine(cl);
	while (!cl.eof())
	{
		counter++;
		junk = utility.sGetLine(cl);
	}
	if (counter % 2 != 0)
		error_log << "Error parsing " + settings.GetFullDataPath("lists/control_list") << endl;
	controlinfo.num_tokens = counter / 2;
	cl.close();
	cl.clear();
	cl.open(settings.GetFullDataPath("lists/control_list").c_str());
	controlinfo.desc = new string [controlinfo.num_tokens];
	controlinfo.token = new string [controlinfo.num_tokens];
	int i;
	for (i = 0; i < controlinfo.num_tokens; i++)
	{
		controlinfo.desc[i] = utility.sGetLine(cl);
		controlinfo.token[i] = utility.sGetLine(cl);
	}
	//cout << controlinfo.num_tokens << endl;
	cl.close();
}

void MENU::ToggleControlOpts()
{
	if (!In("ControlSetup"))
		return;
	
	string token = controlinfo.token[sel];
	
	int num = controlinfo.xsel+1;
	int count = 0;
	int j;
	for (j = 0; j < gamecontrols.GetNumControls(); j++)
	{
		if (gamecontrols.GetControls()[j].GetName() == token)
		{
			if (count == num-1)
			{
				if (gamecontrols.GetControls()[j].GetType() == Joy)
				{
					if (gamecontrols.GetControls()[j].GetJoyType() == Button)
					{
						if (togglemode == 0)
							gamecontrols.GetControls()[j].SetOneTime(!gamecontrols.GetControls()[j].GetOneTime());
						else if (togglemode == 1)
							gamecontrols.GetControls()[j].SetJoyPushDown(!gamecontrols.GetControls()[j].GetJoyPushDown());
					}
					else if (gamecontrols.GetControls()[j].GetJoyType() == Axis)
					{
						if (togglemode == 1)
							gamecontrols.GetControls()[j].SetJoyAxisPlus(!gamecontrols.GetControls()[j].GetJoyAxisPlus());
					}
				}
				if (gamecontrols.GetControls()[j].GetType() == Key)
				{
					if (togglemode == 0)
						gamecontrols.GetControls()[j].SetOneTime(!gamecontrols.GetControls()[j].GetOneTime());
					else if (togglemode == 1)
							gamecontrols.GetControls()[j].SetKeyPushDown(!gamecontrols.GetControls()[j].GetKeyPushDown());
				}
			}
			count++;
		}
	}
}

void MENU::AssignJoyButton(int joynum, int butnum)
{
	if (!controlwait)
		return;
	
	//cout << joynum << "," << butnum << endl;
	
	if (!In("ControlSetup") || controlwaittype != "joybutton")
		return;
	
	string token = controlinfo.token[sel];
	
	bool added = false;
	
	//loop through the controls looking for a control to overwrite
	int num = controlinfo.xsel+1;
	int count = 0;
	int j;
	for (j = 0; j < gamecontrols.GetNumControls(); j++)
	{
		if (gamecontrols.GetControls()[j].GetName() == token)
		{
			if (count == num-1)
			{
				//gamecontrols.GetControls()[j].SetType("key");
				//gamecontrols.GetControls()[j].SetKeyCode(kcode);
				//cout << kcode << endl;
				//gamecontrols.GetControls()[j].SetOneTime(true);
				gamecontrols.GetControls()[j].SetType("joy");
				gamecontrols.GetControls()[j].SetOneTime(true);
				gamecontrols.GetControls()[j].SetJoyType("button");
				gamecontrols.GetControls()[j].SetJoyNum(joynum);
				gamecontrols.GetControls()[j].SetJoyPushDown(true);
				gamecontrols.GetControls()[j].SetJoyButton(butnum);
				
				added = true;
			}
			count++;
		}
	}
	
	if (!added)
	{
		CONTROL * newcontrols;
		newcontrols = new CONTROL[gamecontrols.GetNumControls()+1];
		for (j = 0; j < gamecontrols.GetNumControls(); j++)
		{
			newcontrols[j] = gamecontrols.GetControls()[j];
		}
		
		newcontrols[j].SetType("joy");
		newcontrols[j].SetOneTime(true);
		newcontrols[j].SetJoyType("button");
		newcontrols[j].SetJoyNum(joynum);
		newcontrols[j].SetJoyPushDown(true);
		newcontrols[j].SetJoyButton(butnum);
		
		newcontrols[j].SetName(token);
		gamecontrols.SetControls(newcontrols, gamecontrols.GetNumControls()+1);
		
		added = true;
	}
	
	controlwait = false;
}

void MENU::AssignJoyAxis(int joynum, int joyaxis, float val)
{
	if (!controlwait)
		return;
	
	//cout << joynum << "," << butnum << endl;
	
	if (!In("ControlSetup") || controlwaittype != "joyaxis")
		return;
	
	string token = controlinfo.token[sel];
	
	bool added = false;
	
	//loop through the controls looking for a control to overwrite
	int num = controlinfo.xsel+1;
	int count = 0;
	int j;
	for (j = 0; j < gamecontrols.GetNumControls(); j++)
	{
		if (gamecontrols.GetControls()[j].GetName() == token)
		{
			if (count == num-1)
			{
				//gamecontrols.GetControls()[j].SetType("key");
				//gamecontrols.GetControls()[j].SetKeyCode(kcode);
				//cout << kcode << endl;
				//gamecontrols.GetControls()[j].SetOneTime(true);
				gamecontrols.GetControls()[j].SetType("joy");
				gamecontrols.GetControls()[j].SetJoyType("axis");
				gamecontrols.GetControls()[j].SetJoyNum(joynum);
				gamecontrols.GetControls()[j].SetJoyAxis(joyaxis);
				if (val >= 0)
					gamecontrols.GetControls()[j].SetJoyAxisPlus(true);
				else
					gamecontrols.GetControls()[j].SetJoyAxisPlus(false);
				
				added = true;
			}
			count++;
		}
	}
	
	if (!added)
	{
		CONTROL * newcontrols;
		newcontrols = new CONTROL[gamecontrols.GetNumControls()+1];
		for (j = 0; j < gamecontrols.GetNumControls(); j++)
		{
			newcontrols[j] = gamecontrols.GetControls()[j];
		}
		
		newcontrols[j].SetType("joy");
		newcontrols[j].SetJoyType("axis");
		newcontrols[j].SetJoyNum(joynum);
		newcontrols[j].SetJoyAxis(joyaxis);
		if (val >= 0)
			newcontrols[j].SetJoyAxisPlus(true);
		else
			newcontrols[j].SetJoyAxisPlus(false);
		
		newcontrols[j].SetName(token);
		gamecontrols.SetControls(newcontrols, gamecontrols.GetNumControls()+1);
		
		added = true;
	}
	
	controlwait = false;
}

void MENU::UnbindSlot()
{
	if (!In("ControlSetup"))
		return;
	
	string token = controlinfo.token[sel];
	
	bool found = false;
	int fslot = 0;
	
	//loop through the controls looking for a control to overwrite
	int num = controlinfo.xsel+1;
	int count = 0;
	int j;
	for (j = 0; j < gamecontrols.GetNumControls(); j++)
	{
		if (gamecontrols.GetControls()[j].GetName() == token)
		{
			if (count == num-1)
			{
				found = true;
				fslot = j;
			}
			count++;
		}
	}
	
	if (found)
	{
		CONTROL * newcontrols;
		newcontrols = new CONTROL[gamecontrols.GetNumControls()-1];
		int outcount = 0;
		for (j = 0; j < gamecontrols.GetNumControls(); j++)
		{
			if (j != fslot)
			{
				newcontrols[outcount] = gamecontrols.GetControls()[j];
				outcount++;
			}
		}
		
		gamecontrols.SetControls(newcontrols, gamecontrols.GetNumControls()-1);
	}
}

void MENU::DrawSelectCar()
{
	GLint t = SDL_GetTicks();
	float time = (float) (t-last_t) / 1000.0;
	rotation += time;
	while (rotation > 2*3.141593)
		rotation -= 2*3.141593;
	last_t = t;
	
//	int i;
	
//	float selectmodify = 0.3;
//	int size = 5;
	float r, g, b;
	r = g = b = 0.5;
	if (!controlwait)
		g = 0.8;
	//float rightspace = 0.23;
	//float firstrightspace = 0.25;
//	float rightspace = 0.18;
//	float firstrightspace = 0.2;
	
	glPushMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glClear (GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	QUATERNION rot;
	VERTEX pos;
	//pos.Set(-5,-5,-5);
	pos.Set(1,0,-8);
	rot.Rotate(rotation, 0,0,1);
	rot.Rotate(-3.14/3, 1,0,0);
	GLdouble temp_matrix[16];
	rot.GetMat(temp_matrix);
	glTranslatef(pos.x, pos.y, pos.z);
	glMultMatrixd(temp_matrix);

	/*if (utility.numTUs() > 1)
	{
		utility.SelectTU(1);
		glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP); 
		glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP); 
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, sphere_reflection);
		float sub[4];
		//sub[0] = sub[1] = sub[2] = 0.8f;
		sub[0] = sub[1] = sub[2] = 0.7f;
		//sub[0] = sub[1] = sub[2] = 0.0f;
		sub[3] = 0.0f;
		glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, sub);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_PREVIOUS_ARB);
		glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB_ARB,GL_SRC_COLOR);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB_ARB,GL_SRC_COLOR);
		glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB_ARB, GL_ADD);
		glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, 1);
		
	}*/
	
	car->SetReflectionTexture(sphere_reflection);
	
	utility.SelectTU(0);

	//world.GetPlayerCar()->draw(false);
	car->draw(false);

	glPopAttrib();
	glPopMatrix();
	
	float cx, cy;
	cx = 0.03;
	cy = 0.05;
	
	font.Print(cx, cy, "Select Car", 1, 8, 1,1,1);
	cy += 0.08;
	
	//font.Print(cx, cy, cflist[sel].c_str(), 1, 7, 1,1,1);
	font.Print(cx, cy, cfnames[sel].c_str(), 1, 6, 0.8,1.0,0.8);
	cy += 0.06;
	
	char tempchar[256];
	sprintf(tempchar, "Power: %.0f HP at %.0f RPM", 
		car->engine()->max_power()*0.00134102209,
		Vamos_Geometry::rad_s_to_rpm (car->engine ()->peak_engine_speed ()));
	font.Print(cx, cy, tempchar, 1, 6, 0.5,0.8,0.5);
	cy += 0.04;
	
	car->chassis().update_center_of_mass();
	
	sprintf(tempchar, "Curb Weight: %.0f lb (%.0f kg)", 
		car->chassis().mass()*2.20462262,
		car->chassis().mass());
	font.Print(cx, cy, tempchar, 1, 6, 0.5,0.8,0.5);
	cy += 0.04;
	
	cy += 0.02;
	
	font.Print(cx, cy, cfabout[sel].c_str(), 1, 5, 0.5,0.8,0.5);
}

void MENU::BuildCFList()
{
	if (cflist != NULL || cfabout != NULL || cfnames != NULL)
		ClearCFList();
	
	numcf = 0;
	
	string fname = settings.GetFullDataPath("cars/car_list.txt");
	ifstream cfile;
	cfile.open(fname.c_str());
	
	string cftemp = utility.sGetLine(cfile);
	while (!cfile.eof())
	{
		numcf++;
		cftemp = utility.sGetLine(cfile);
	}
	
	cfile.close();
	cfile.open(fname.c_str());
	cfile.clear();
	
	cflist = new string [numcf];
	cfabout = new string [numcf];
	cfnames = new string [numcf];
	
	int i;
	
	for (i = 0; i < numcf; i++)
	{
		cflist[i] = utility.sGetLine(cfile);
		
		ifstream about;
		about.open(settings.GetFullDataPath("cars/" + cflist[i] + "/about.txt").c_str());
		cfabout[i] = "";
		if (about)
		{
			string tstr = utility.sGetLine(about);
			
			int count = 0;
			
			if (!about.eof())
				cfnames[i] = tstr;
			
			while (!about.eof())
			{
				if (count != 0)
				{
					if (cfabout[i] == "")
						cfabout[i] = tstr;
					else
						cfabout[i] = cfabout[i] + "\n" + tstr;
				}
				
				tstr = utility.sGetLine(about);
				count++;
			}
			about.close();
		}
		about.clear();
	}
	
	cfile.close();
}

void MENU::ClearCFList()
{
	if (cflist != NULL)
		delete [] cflist;
	
	if (cfabout != NULL)
		delete [] cfabout;
	
	if (cfnames != NULL)
		delete [] cfnames;
	
	cflist = NULL;
	cfabout = NULL;
	cfnames = NULL;
	
	numcf = 0;
}

void MENU::WriteCarSelection()
{
/*	ofstream csfile;
	csfile.open((settings.GetSettingsDir() + "/selected_car").c_str());
	csfile << cflist[sel] << endl;
	csfile << subsel << endl;
	csfile.close();
*/
	settings.SetCarName(cflist[sel]);
	settings.SetCarPaint(subsel);
}

void MENU::LoadCar(string cfile)
{
	//car = new Vamos_Body::Gl_Car (Vamos_Geometry::Three_Vector (11.0, 0.0, 0.6));
	ClearCar();
	car = new Vamos_Body::Gl_Car (Vamos_Geometry::Three_Vector (0.0, 0.0, 0.0));
	car->read (settings.GetDataDir() + "/", cfile);
}

void MENU::ClearCar()
{
	if (car != NULL)
	{
		delete car;
		car = NULL;
	}
}

void MENU::DrawCalibrate()
{
	char tchar[256];
	
	int j;
	
	font.Print(0.01, 0.02, "Move all joystick axes through their entire ranges", 1, 6, 0.5,0.8,0.5);
	font.Print(0.01, 0.05, "ENTER to accept, ESCAPE to cancel", 1, 6, 0.5,0.8,0.5);
	
	for (j = 0; j < SDL_NumJoysticks(); j++)
	{
		string joystring = "";
		sprintf(tchar, "Joy %i\n[axis:min max]\n", j);
		joystring = joystring + tchar;
		
		int i;
		for (i = 0; i < SDL_JoystickNumAxes(gamecontrols.GetJoy(j)); i++)
		{
			//sprintf(tchar, "%i: %f\n", i, (float)SDL_JoystickGetAxis(gamecontrols.GetJoy(j), i)/32768.0f);
			gamecontrols.SetCalibrationPoint(j, i, (float)SDL_JoystickGetAxis(gamecontrols.GetJoy(j), i)/32768.0f);
			sprintf(tchar, "%i:%.3f %.3f\n", i, gamecontrols.GetCalibration(j, i, false), gamecontrols.GetCalibration(j, i, true));
			joystring = joystring + tchar;
		}
		
		//font.Print(0.01+0.15*j,0.11, joystring.c_str(), 0, 5, 1,1,1);
		font.Print(0.01+0.2*j,0.11, joystring.c_str(), 0, 6, 1,1,1);
	}
}

void MENU::DrawSelectTrack()
{
	float cx, cy;
	cx = 0.03;
	cy = 0.05;
	
	font.Print(cx, cy, "Select Track", 1, 8, 1,1,1);
	cy += 0.08;
	
	//font.Print(cx, cy, cflist[sel].c_str(), 1, 7, 1,1,1);
	if (sel < numts && sel >= 0)
		font.Print(cx, cy, tsabout[sel].c_str(), 1, 6, 0.5,0.8,0.5);
}

void MENU::BuildTSList()
{
	if (tslist != NULL || tsabout != NULL)
		ClearTSList();
	
	numts = 0;
	
	string fname = settings.GetFullDataPath("tracks/track_list.txt");
	ifstream tfile;
	tfile.open(fname.c_str());
	
	if (!tfile)
	{
		cout << "Error opening track list: " << settings.GetFullDataPath("tracks/track_list.txt") << endl;
	}
	
	string tstemp = utility.sGetLine(tfile);
	while (!tfile.eof())
	{
		numts++;
		string tstemp = utility.sGetLine(tfile);
	}
	
	tfile.close();
	tfile.open(fname.c_str());
	tfile.clear();
	
	tslist = new string [numts];
	tsabout = new string [numts];
	
	int i;
	
	for (i = 0; i < numts; i++)
	{
		tslist[i] = utility.sGetLine(tfile);
		
		ifstream about;
		about.open(settings.GetFullDataPath("tracks/" + tslist[i] + "/about.txt").c_str());
		tsabout[i] = "";
		if (about)
		{
			string tstr = utility.sGetLine(about);
			
			while (!about.eof())
			{
				if (tsabout[i] == "")
					tsabout[i] = tstr;
				else
					tsabout[i] = tsabout[i] + "\n" + tstr;
				
				tstr = utility.sGetLine(about);
			}
			about.close();
		}
		about.clear();
	}
	
	tfile.close();
}

void MENU::ClearTSList()
{
	if (tslist != NULL)
		delete [] tslist;
	
	if (tsabout != NULL)
		delete [] tsabout;
	
	numts = 0;
}

void MENU::DrawNetSetup()
{
	char tmpchar[256];
	
//	int j;

	float ip_red, ip_green, ip_blue, port_red, port_green, port_blue;

	float cy = 0.02;
	
	font.Print(0.01, cy, "Network Setup", 1, 8, 1,1,1);
	cy += 0.08;
	font.Print(0.01, cy, "Select Host or Client with the left and right arrow keys.", 1, 6, 0.5,0.8,0.5);
	cy += 0.05;
	if (net_host)
	{
		font.Print(0.01, cy, "Host", 1, 6, 0.8,0.5,0.5);
		cy += 0.05;
		
		sprintf(tmpchar, "Port: %s", port_input.c_str());
		font.Print(0.01, cy, tmpchar, 1, 6, 0.8,1.0,0.8);
		cy += 0.05;
	}
	else
	{
		font.Print(0.01, cy, "Client", 1, 6, 0.8,0.5,0.5);
		cy += 0.05;
		
		if( ip_or_port )
		{
			ip_red = 0.8;
			ip_green = 1.0;
			ip_blue = 0.8;
			port_red = 0.5;
			port_green = 0.8;
			port_blue = 0.5;
		}
		else
		{
			ip_red = 0.5;
			ip_green = 0.8;
			ip_blue = 0.5;
			port_red = 0.8;
			port_green = 1.0;
			port_blue = 0.8;
		}

		sprintf(tmpchar, "IP: %s", net_ip.c_str());
		font.Print(0.01, cy, tmpchar, 1, 6, ip_red,ip_green,ip_blue);
		cy += 0.05;

		sprintf(tmpchar, "Port: %s", port_input.c_str());
		font.Print(0.01, cy, tmpchar, 1, 6, port_red,port_green,port_blue);
		cy += 0.05;
	}
	//font.Print(0.01, cy, "Select Host or Client with the arrow keys", 1, 6, 0.5,0.8,0.5);
	//font.Print(0.01, 0.05, "ENTER to accept, ESCAPE to cancel", 1, 6, 0.5,0.8,0.5);
	
	/*string joystring = "";
	sprintf(tchar, "Joy %i\n[axis:min max]\n", j);
	joystring = joystring + tchar;
	
	sprintf(tchar, "%i:%.3f %.3f\n", i, gamecontrols.GetCalibration(j, i, false), gamecontrols.GetCalibration(j, i, true));
	joystring = joystring + tchar;*/
		
	//font.Print(0.01+0.15*j,0.11, joystring.c_str(), 0, 5, 1,1,1);
	//font.Print(0.01+0.2*j,0.11, joystring.c_str(), 0, 6, 1,1,1);
}


void MENU::DrawCustomizeCar()
{
	float cy = 0.05, cx = 0.03;
	string this_part, this_part_path, this_category, this_menu_text;
	float red, green, blue = 0.0f;
	string::size_type div_pos;
	string menu_title = "Customize " + settings.GetCarName() + " Parts";

	font.Print(cx, cy, menu_title.c_str(), 1, 8, 0.8, 0.5, 0.5);

	cy += 0.08;
	string temp = "Here you can install predefined parts to the selected car.";
	font.Print(cx, cy, temp.c_str(), 1, 5, 0.8, 0.8, 0.8);
	cy += 0.025;
	temp = "Use space to toggle a part on or off. Escape cancels, Enter accepts.";
	font.Print(cx, cy, temp.c_str(), 1, 5, 0.8, 0.8, 0.8);
	cy += 0.025;
	temp = "You must start a new game for the parts to be added to the car.";
	font.Print(cx, cy, temp.c_str(), 1, 5, 0.8, 0.8, 0.8);
	cy += 0.04;

	for( unsigned int i = 0; i < parts.size(); i++ )
	{
		if ( i == part_selected )
		{
			red = 0.8f;
			green = 1.0f;
			blue = 0.8f;
		}
		else
		{
			red = 0.5f;
			green = 0.8f;
			blue = 0.5f;
		}

		this_part_path = parts[i];
		div_pos = this_part_path.find("/", 0);

		this_category = this_part_path.substr(0, div_pos);
		this_part = this_part_path.substr(div_pos + 1, this_part_path.length() - (div_pos + 1));

		this_menu_text = this_category + "> " + parts_names[i] + ": ";
		this_menu_text += parts_enabled[i] ? "Installed" : "Not installed";

		font.Print(cx, cy, this_menu_text.c_str(), 1, 6, red, green, blue);

		cy += 0.03;
	}
}

void MENU::ReloadCarParts()
{
	vector<string> temp_vec;
	bool found;
	parts_enabled.clear();
	temp_vec = settings.LoadCarPartsSettings();
	for( unsigned int i = 0; i <  parts.size(); i++ )
	{
		found = false;
		for( unsigned int j = 0; j < temp_vec.size(); j++ )
		{
			if( parts[i] == temp_vec[j] )
			{
				//cout << "found part: " << temp_vec[j] << endl;
				found = true;
				break;
			}
		}
		parts_enabled.push_back(found);
	}
	part_selected = 0;
}
