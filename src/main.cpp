#define GL_GLEXT_PROTOTYPES

#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <SDL/SDL.h>
#include <math.h>
#include <string>

using namespace std;

#include "font.h"
#include "utility.h"
//#include "backdrop.h"
#include "camera.h"
//#include "weather.h"
#include "keyman.h"
#include "mouse.h"
#include "messageq.h"
//#include "terrain.h"
//#include "particles.h"
#include "settings.h"
//#include "sound.h"
//#include "menu.h"
//#include "timer.h"
//#include "controls.h"
//#include "vamosworld.h"
//#include "replay.h"
//#include "trees.h"
//#include "logo.h"
//#include "gamestate.h"
#include "objects.h"
//#include "multiplay.h"
//#include "net.h"
#include "configfile.h"
#include "bezier.h"
#include "track.h"

#ifdef _WIN32
//#define GL_GLEXT_PROTOTYPES
#include <GL/glext.h>
//#include <GL/wglext.h>
//#include <GL/glprocs.h>
#endif

//#define PERFORMANCE_PROFILE

//fps less than below will not tick the simulation
#define MIN_FPS 20.0

#ifdef PERFORMANCE_PROFILE
#include <sys/time.h>
suseconds_t GetMicroSeconds()
{
	struct timeval tv;
	struct timezone tz;
		
	tz.tz_minuteswest = 0;
	tz.tz_dsttime = 0;
	
	gettimeofday(&tv, &tz);
	
	return tv.tv_usec;
}
#endif


/* Set up some booleans */
#define TRUE  1
#define FALSE 0

/* screen width, height, and bit depth */
//const int SCREEN_WIDTH = 1024;
//const int SCREEN_HEIGHT = 768;
//const int SCREEN_BPP = 32;

int SCREEN_WIDTH = 1024;
int SCREEN_HEIGHT = 768;
int SCREEN_BPP = 32;

struct EDITORDATA
{
	GLuint cursortex;
	VERTEX bezinput[8];
	int numbezinput;
	bool mousebounce[5];
	string activetrack;
	int helppage;
} editordata;

bool verbose_output = false;

//bool in_menu = false;
//bool menu_display = false;

/*const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 1024;
const int SCREEN_BPP = 32;*/

/* This is our SDL surface */
SDL_Surface *surface;

/* Ambient Light Values ( NEW ) */
//GLfloat LightAmbient[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat LightAmbient[]  = { 0.1f, 0.1f, 0.1f, 1.0f };
/* Diffuse Light Values ( NEW ) */
//GLfloat LightDiffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat LightDiffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat LightSpecular[]  = { 0.0f, 0.0f, 0.0f, 0.0f };
/* Light Position ( NEW ) */
GLfloat LightPosition[4];

/*
ENTITYLIST entities;
MESHLIST meshes;
TERRAIN terrain;
TREES foliage;
WATER water;
GAMEDB gamedb;
SHIPS ships;*/
SETTINGS settings;
KEYMAN keyman;
CAMERA cam;
FONT font;
UTILITY utility;
//BACKDROP backdrop;
//WEATHER weathersystem;
MOUSE mouse;
MESSAGEQ mq1;
//TERRAIN terrain;
//PARTICLE particle;
//SOUNDMANAGER sound;
//MENU menu;
//TIMER timer;
//GAMECONTROLS gamecontrols;
//REPLAY replay;
//TREES trees;
//LOGO logo;
//GAMESTATE state;
OBJECTS objects;
//NET net;
//MULTIPLAY multiplay;

TRACK track;
ROADSTRIP * activestrip;

enum VERTMODE
{
	TWOVERTS,
 	THREEVERTS,
 	FOURVERTS
} vertmode = TWOVERTS;

//VAMOSWORLD world;
//Vamos_Track::Strip_Track *road;

ofstream error_log;

#define CAR_Y_OFFSET 1.0

double day_time;
double abs_time;

//for game timing and FPS stuff
static GLint T0;
static GLint Frames;
static GLfloat fps;

bool showfps = settings.GetFPSEnabled();

static int frameno;

float timefactor = 1.0f;

#define AVERAGEFRAMES 30
//#define AVERAGEFRAMES 10

float pfps;
float lfps[AVERAGEFRAMES];
int lfpspos = 0;
bool lfpsfull = false;

#define FRAME_TIME 0.002
double execution_time = 0.0;

bool pauserequest = false;
bool unpauserequest = false;

int screenshots = 0;

float FrameTime()
{
	return FRAME_TIME;
}

int Screenshot(char *filename)
{
	SDL_Surface *screen;
	SDL_Surface *temp = NULL;
	unsigned char *pixels;
	int i;
	
	screen = surface;
	
	if (!(screen->flags & SDL_OPENGL))
	{
		SDL_SaveBMP(temp, filename);
		return 0;
	}
	
	temp = SDL_CreateRGBSurface(SDL_SWSURFACE, screen->w, screen->h, 24,
	#if SDL_BYTEORDER == SDL_LIL_ENDIAN
	0x000000FF, 0x0000FF00, 0x00FF0000, 0
	#else
	0x00FF0000, 0x0000FF00, 0x000000FF, 0
	#endif
	);
	if (temp == NULL)
		return -1;
	
	pixels = (unsigned char *) malloc(3 * screen->w * screen->h);
	if (pixels == NULL)
	{
		SDL_FreeSurface(temp);
		return -1;
	}
	
	glReadPixels(0, 0, screen->w, screen->h, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	
	for (i=0; i<screen->h; i++)
		memcpy(((char *) temp->pixels) + temp->pitch * i, pixels + 3*screen->w * (screen->h-i-1), screen->w*3);
	free(pixels);
	
	SDL_SaveBMP(temp, filename);
	SDL_FreeSurface(temp);
	return 0;
}

void ResetWorld(bool fullreset);

/* function to release/destroy our resources and restoring the old desktop */
void Quit( int returnCode )
{
	glDeleteTextures(1, &editordata.cursortex);
	
	if (verbose_output)
		cout << "Quit called" << endl;
	
	error_log.close();
	
    /* clean up the window */
    SDL_Quit( );

	if (verbose_output)
		cout << "SDL_Quit finished" << endl;
	
    /* and exit appropriately */
    exit( returnCode );
}

/* function to reset our viewport after a window resize */
int resizeWindow( int width, int height )
{
    /* Height / width ration */
    GLfloat ratio;
 
    /* Protect against a divide by zero */
    if ( height == 0 )
	height = 1;

    ratio = ( GLfloat )width / ( GLfloat )height;

    /* Setup our viewport. */
    glViewport( 0, 0, ( GLint )width, ( GLint )height );

    /* change to the projection matrix and set our viewing volume. */
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );

    /* Set our perspective */
    gluPerspective( 45.0f, ratio, 0.1f, 10000.0f );

    /* Make sure we're chaning the model view and not the projection */
    glMatrixMode( GL_MODELVIEW );

    /* Reset The View */
    glLoadIdentity( );

    return( TRUE );
}

void ChangeDisplay(int w, int h, int bpp, bool fullscreen, bool reloadtextures)
{
	SCREEN_WIDTH = w;
	SCREEN_HEIGHT = h;
	SCREEN_BPP = bpp;
	
	/* the flags to pass to SDL_SetVideoMode */
    int videoFlags  = SDL_OPENGL;          /* Enable OpenGL in SDL */
    videoFlags |= SDL_GL_DOUBLEBUFFER; /* Enable double buffering */
    videoFlags |= SDL_HWPALETTE;       /* Store the palette in hardware */
    videoFlags |= SDL_RESIZABLE;       /* Enable window resizing */

    /* This checks to see if surfaces can be stored in memory */
    /*if ( videoInfo->hw_available )
		videoFlags |= SDL_HWSURFACE;
    else
		videoFlags |= SDL_SWSURFACE;

    if ( videoInfo->blit_hw )
	videoFlags |= SDL_HWACCEL;*/

	if (fullscreen)
	{
	  videoFlags |= SDL_HWSURFACE|SDL_ANYFORMAT|SDL_FULLSCREEN;  
	}	
	else
	{
	  videoFlags |= SDL_SWSURFACE|SDL_ANYFORMAT;
	}
	
    /* get a SDL surface */
	if (surface != NULL)
	{
		SDL_FreeSurface(surface);
		surface = NULL;
	}
    surface = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP,
				videoFlags );
	
	resizeWindow( SCREEN_WIDTH, SCREEN_HEIGHT );
	//if (fullscreen)
	
/*
	ofstream vconf;
	vconf.open((settings.GetSettingsDir() + "/videoconfig").c_str());
	vconf << w << " ";
	vconf << h << " ";
	vconf << bpp << " ";
	vconf << fullscreen << " ";
	vconf.close();
*/
	settings.SetDisplayX(w);
	settings.SetDisplayY(h);
	settings.SetDisplayDepth(bpp);
	settings.SetFullscreenEnabled(fullscreen);

	if (reloadtextures)
	{
        font.Load();
	}
}

void Update()
{
	//cam.position.DebugPrint();
	
	bool mainloop = false;
	
	//std::cout << "Mainloop" << std::endl;
	
	//if unpaused and getting a reasonable framerate
	//if (fps > MIN_FPS && timefactor != 0.0f)
	if (fps > 0.0f && timefactor != 0.0f)
	{
		double time_increment = timefactor / fps;
		//std::cout << "Time elapsed: " << execution_time << " + " << time_increment << std::endl;
		
		execution_time += time_increment;
		//int num_updates = (int) (execution_time / FRAME_TIME);
//		double this_frame = (double) num_updates * FRAME_TIME;
		
		mouse.Update(cam, SCREEN_WIDTH, SCREEN_HEIGHT, timefactor, fps);

		cam.dir = mouse.GetDir();
		
		while (execution_time > FRAME_TIME)
		{
			//std::cout << "Frame start: " << execution_time << std::endl;
			
			#ifdef PERFORMANCE_PROFILE
			suseconds_t t1, t2;
			t1 = GetMicroSeconds();
			t1 = GetMicroSeconds();
			#endif
			
			//multiplay.Update(FRAME_TIME);
			
			//weather tick
			
			#ifdef PERFORMANCE_PROFILE
			t2 = GetMicroSeconds();
			cout << "multiplay.Update() ticks: " << t2-t1 << endl;
			t1 = GetMicroSeconds();
			#endif
			
			cam.Update();
			
			//handle input
			//keyman.DoHeldKeys(timefactor, fps, cam);
			keyman.DoHeldKeys(1.0, 1.0/FRAME_TIME, cam);
			
			#ifdef PERFORMANCE_PROFILE
			t1 = GetMicroSeconds();
			t1 = GetMicroSeconds();
			#endif

			#ifdef PERFORMANCE_PROFILE
			t2 = GetMicroSeconds();
			cout << "world update ticks: " << t2-t1 << endl;
			t1 = GetMicroSeconds();
			#endif
			
			keyman.DoOneTimeKeys(cam);
			
			//particle.Update(timefactor, fps);
			//particle.Update(1.0, 1.0/FRAME_TIME);
			
			cam.ExtractFrustum();
			
			//float timepassed = (timefactor/fps)/86400.0;
			float timepassed = (FRAME_TIME)/86400.0;
			/*if (keyman.keys[keyman.GetKey("AccelTimeVFast")])
				timefactor = 10000.0;
			else if (keyman.keys[keyman.GetKey("AccelTimeFast")])
				timefactor = 1000.0;
			else
				timefactor = 1.0;*/
			//float timepassed = (timefactor/fps)/60.0;
			//float timepassed = (timefactor/fps)/10.0;
			abs_time += timepassed;
			day_time += timepassed;
			if (day_time > 1.0f)
				day_time -= 1.0f;
			
			#ifdef PERFORMANCE_PROFILE
			t2 = GetMicroSeconds();
			cout << "Time increment ticks: " << t2-t1 << endl;
			t1 = GetMicroSeconds();
			#endif
			
			mainloop = true;
			
			//terrain.Update(cam, timefactor, fps, day_time);
			//terrain.Update(cam, 1.0, 1.0/FRAME_TIME, day_time);
			
			execution_time -= FRAME_TIME;
			//num_updates++;
			
			//replay.IncrementFrame();
		}
	}
	else
	{
		//print "paused"
	}
	
	if (pauserequest)
	{
		if (timefactor != 0.0f)
			timefactor = 0.0f;
		pauserequest = false;
	}
	if (unpauserequest)
	{
		if (timefactor == 0.0f)
			timefactor = 1.0f;
		unpauserequest = false;
	}

	/*if (!mainloop)
		multiplay.Update(0);*/ //don't need this anymore because we're stopping things from being paused in multiplayer mode
}

void LoadingScreen(string loadtext);

void snap_screenshot()
{
	char scrname[64];
	sprintf(scrname, "%s/screenshots/shot%03d.bmp", (settings.GetSettingsDir()).c_str(), settings.GetNumShots());
	Screenshot(scrname);
	settings.SetNumShots(settings.GetNumShots() + 1);
}

void MainPause()
{
}

void MainUnpause()
{
	unpauserequest = true;
}

void AutoTrace()
{
	if (activestrip != NULL)
	{
		BEZIER * lastbez = activestrip->GetLastPatch();
		if (lastbez != NULL && editordata.numbezinput == 4)
		{
			VERTEX tvec[4];
			/*VERTEX fl = lastbez->points[0][0];
			VERTEX fr = lastbez->points[0][3];
			VERTEX bl = lastbez->points[3][0];
			VERTEX br = lastbez->points[3][3];*/
			
			bool success(true);
			BEZIER patch;
			if (vertmode == TWOVERTS)
			{
				success = success && objects.AutoFindClosestVert(lastbez->points[0][0], lastbez->points[0][3], (lastbez->points[0][0]-lastbez->points[3][0]), tvec[0]);
				success = success && objects.AutoFindClosestVert(lastbez->points[0][3], lastbez->points[0][0], (lastbez->points[0][3]-lastbez->points[3][3]), tvec[1]);
				
				if (success)
				{
					patch.SetFromCorners(tvec[0], tvec[1], editordata.bezinput[0], editordata.bezinput[3]);
				}
			}
			else if (vertmode == THREEVERTS)
			{
				success = success && objects.AutoFindClosestVert(lastbez->points[0][0], lastbez->points[0][1], (lastbez->points[0][0]-lastbez->points[3][0]), tvec[0]);
				success = success && objects.AutoFindClosestVert(lastbez->points[0][1], lastbez->points[0][3], (lastbez->points[0][1]-lastbez->points[3][1]), tvec[1]);
				tvec[2] = tvec[1];
				success = success && objects.AutoFindClosestVert(lastbez->points[0][3], lastbez->points[0][1], (lastbez->points[0][3]-lastbez->points[3][3]), tvec[3]);
				
				if (success)
				{
					for (int i = 0; i < 4; i++)
						patch.points[3][i] = editordata.bezinput[i];
					for (int i = 0; i < 4; i++)
						patch.points[0][i] = tvec[i];
					patch.CalculateMiddleRows();
				}
			}
			else if (vertmode == FOURVERTS)
			{
				for (int i = 0; i < 4; i++)
				{
					int nextvert = i + 1;
					if (nextvert >= 4)
						nextvert = 2;
					success = success && objects.AutoFindClosestVert(lastbez->points[0][i], lastbez->points[0][nextvert], (lastbez->points[0][i]-lastbez->points[3][i]), tvec[i]);
				}
				
				if (success)
				{
					for (int i = 0; i < 4; i++)
						patch.points[3][i] = editordata.bezinput[i];
					for (int i = 0; i < 4; i++)
						patch.points[0][i] = tvec[i];
					patch.CalculateMiddleRows();
				}
			}
			else
				assert(0);
			
			if (success)
			{
				activestrip->Add(patch);
				
				editordata.numbezinput = 4;
				
				editordata.bezinput[0] = patch.points[0][0];
				editordata.bezinput[1] = patch.points[0][1];
				editordata.bezinput[2] = patch.points[0][2];
				editordata.bezinput[3] = patch.points[0][3];
				
				mq1.AddMessage("Auto-traced road");
			}
			else
			{
				mq1.AddMessage("Can't auto-trace road: found no candidate points");
			}
		}
		else
		{
			mq1.AddMessage("Can't auto-trace road: must start road first");
		}
	}
	else
	{
		mq1.AddMessage("Can't auto-trace road: no roads selected");
	}
}

/* function to handle key press events */
void handleKeyPress( SDL_keysym *keysym )
{
	VERTEX tvec1, tvec2, fr, fl, bl, br, camray, selvert;
	QUATERNION trot;
	BEZIER * lastbez;
	BEZIER * colbez;
	ROADSTRIP * tempstrip;
	BEZIER patch;
	bool l;
	int i;
	string tstr;
	
	//else if (timefactor == 0 && !menu.InMenu()) timefactor = 0;
		//		else timefactor = 1.0;
//	else
	{
		switch ( keysym->sym )
		{
		case SDLK_ESCAPE:
			/* ESC key was pressed */
			Quit( 0 );
			//menu.MainMenu();
			break;
		/*case SDLK_F7:
			
			tvec1 = tvec1.rotate(0,0,6.21652);
			cout << tvec1[0] << "," << tvec1[1] << endl;
		
			
			tvec2.Set(44.464,-126.8737,0);
			
			trot.Rotate(6.21652, 0, 0, 1);
			tvec2 = trot.RotateVec(tvec2);
			tvec2.DebugPrint();
			break;*/
		/*case SDLK_F7:
			if (timefactor != 100.0f)
				timefactor = 100.0f;
			else
				timefactor = 1.0f;
			break;*/
		/*case SDLK_PAGEUP:
			//tmpvert = cam.GetVelocity();
			cam.MoveRelative(0.0f, 0.0f, -2.0f);
			//cam.Update();
			//cam.LoadVelocityIdentity();
			break;
		case SDLK_PAGEDOWN:
			//tmpvert = cam.GetVelocity();
			cam.MoveRelative(0.0f, 0.0f, 2.0f);
			//cam.Update();
			//cam.LoadVelocityIdentity();
			break;*/
			
		case SDLK_BACKSPACE:
			if (activestrip != NULL)
			{
				if (activestrip->DeleteLastPatch())
				{
					if (activestrip->NumPatches() == 0)
					{
						mq1.AddMessage("Deleted patch and empty road");
						track.Delete(activestrip);
						activestrip = NULL;
						editordata.numbezinput = 0;
					}
					else
					{
						mq1.AddMessage("Deleted patch");
						lastbez = activestrip->GetLastPatch();
						if (lastbez != NULL)
						{
							editordata.bezinput[0] = lastbez->points[0][0];
							editordata.bezinput[1] = lastbez->points[0][1];
							editordata.bezinput[2] = lastbez->points[0][2];
							editordata.bezinput[3] = lastbez->points[0][3];
						}
						else
							editordata.numbezinput = 0;
					}
				}
				else
					mq1.AddMessage("Can't delete patch: no patches on this road");
			}
			else
			{
				mq1.AddMessage("Can't delete patch: no road selected");
			}
			break;
			
		case 'h':
			editordata.helppage++;
			break;
		
		case 'd':
			if (activestrip != NULL)
			{
				track.Delete(activestrip);
				mq1.AddMessage("Deleted road.");
			}
			else
			{
				mq1.AddMessage("Can't delete road: no road selected.");
			}
			activestrip = NULL;
			editordata.numbezinput = 0;
			break;
		
		case 'n':
			//create a new road
			mq1.AddMessage("Ready to create new road, add patches.");
			activestrip = NULL;
			editordata.numbezinput = 0;
			break;
			
		case 's':
			mq1.AddMessage("Saved to file");
			track.Write(editordata.activetrack);
			break;
		
		case 'l':
			track.SetStart(cam.position.ScaleR(-1));
			track.SetStartOrientation(cam.dir.ReturnConjugate());
			mq1.AddMessage("Added start location");
			break;
		
		case 'k':
			track.RemoveStart();
			track.RemoveStartOrientation();
			mq1.AddMessage("Removed start location");
			break;
		
		case 'r':
			//mq1.AddMessage("Saved to file");
			tvec1.zero();
			tvec1.z = -1;
			tvec2 = cam.dir.ReturnConjugate().RotateVec(tvec1);
			l = track.Collide(cam.position.ScaleR(-1.0), tvec2, tvec1, true, activestrip, colbez);
			if (!l)
			{
				mq1.AddMessage("Can't select road: no road found under cursor");
			}
			break;
		
		case 'a':
			AutoTrace();
			break;
			
		case 'f':
			for (i = 0; i < 25; i++)
			{
				AutoTrace();
			}
			break;
		
		case 'i':
			camray.z = -1;
			camray = cam.dir.ReturnConjugate().RotateVec(camray);
			if (objects.FindClosestVert(cam.position.ScaleR(-1.0), camray, selvert, tstr))
				cout << tstr << endl;
			break;
			
		case '-':
			track.ClearLapSequence();
			mq1.AddMessage("All lap sequence markers cleared");
			break;
		
		case 'e':
			//mq1.AddMessage("Saved to file");
			tvec1.zero();
			tvec1.z = -1;
			tvec2 = cam.dir.ReturnConjugate().RotateVec(tvec1);
			l = track.Collide(cam.position.ScaleR(-1.0), tvec2, tvec1, true, tempstrip, colbez);
			if (!l)
			{
				mq1.AddMessage("Can't create lap sequence marker: no road found under cursor");
			}
			else
			{
				track.AddLapSequence(colbez);
			}
			break;
			
		case '2':
			mq1.AddMessage("Two-vertex selection mode");
			vertmode = TWOVERTS;
			break;
			
		case '3':
			mq1.AddMessage("Three-vertex selection mode");
			vertmode = THREEVERTS;
			break;
			
		case '4':
			mq1.AddMessage("Four-vertex selection mode");
			vertmode = FOURVERTS;
			break;
		
		//case SDLK_F11:
			//SDL_WM_ToggleFullScreen( surface );
			//ChangeDisplay(1280,1024,32,true);
			//menu.DisplayMenu();//DisplaySelect();
			//break;
		
		//case SDLK_F12:
			//menu.MainMenu();
			//break;
		
		default:
			keyman.OneTime(keysym->sym);
			break;
		}
	}
	
    return;
}



void glSetup()
{


	/* Enable Texture Mapping ( NEW ) */
    glEnable( GL_TEXTURE_2D );
    /* Enable smooth shading */
    glShadeModel( GL_SMOOTH );
    /* Set the background black */
    //glClearColor( 0.53f, 0.74f, 0.91f, 0.0f );
	glClearColor(0,0,0,0);
    /* Depth buffer setup */
    glClearDepth( 1.0f );
    /* Enables Depth Testing */
    glEnable( GL_DEPTH_TEST );
    /* The Type Of Depth Test To Do */
    glDepthFunc( GL_LEQUAL );
    /* Really Nice Perspective Calculations */
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
	/* Enable Lighting */
	glEnable( GL_LIGHTING );
	
	//set the clipping planes
	//glFrustum(1000,1000,1000,1000,1,1000);
	
	//utility.Tex2D(3, false);
	//utility.Tex2D(2, false);
	//utility.Tex2D(1, false);
	//utility.Tex2D(0, true);
}

//string otrack_file;
//string car_file;
string data_dir;
//int car_paint;

//int number_of_opponents;
//int focused_car;

bool UnloadWorld()
{
	//these should have error catching such that if they're called before
	// init, it's OK
	
	return true;
}

bool LoadWorld()
{
	UnloadWorld();
	
	editordata.helppage = 0;
	editordata.numbezinput = 0;
	editordata.mousebounce[1] = false;
	
	//teststrip = track.AddNewRoad();
	
	//begin loading world
	
	LoadingScreen("Loading...\nConfiguration files");
	
	CONFIGFILE setupfile;
	setupfile.Load(settings.GetDataDir() + "/tracks/editor.config");
	setupfile.GetParam("active track", editordata.activetrack);
	if (editordata.activetrack == "")
		editordata.activetrack = "default";
	
	objects.LoadObjectsFromFolder(settings.GetDataDir() + "/tracks/" + 
			editordata.activetrack + "/objects/");
	
	track.Load(editordata.activetrack);
	
	//float aangle[] = {0, 1, 0, 0};
	//track.GetStartOrientation(0).GetAxisAngle(aangle);
	//cam.Rotate(aangle[0], aangle[1], aangle[2], aangle[3]);
	VERTEX start = track.GetStart(0);
	cam.Move(-start.x, -start.y, -start.z);
	cam.Update();
	cam.LoadVelocityIdentity();
	
	//car_file = "s2000";
	//ifstream csfile;
	//car_file = "";
	//csfile.open((settings.GetSettingsDir() + "/selected_car").c_str());
	//car_paint = 0;
/*	if (csfile)
	{
		state.SetCarName(0, utility.sGetLine(csfile));
		state.SetCarPaint(0, utility.iGetParam(csfile));
		csfile.close();
	}
*/
	
	//LoadingScreen("Loading...\nLoading scenery objects");
	//objects.LoadObjectsFromFolder(settings.GetDataDir() + "/tracks/" + state.GetTrackName() + "/objects");
	
	/*trees.DeleteAll();
	int numtrees = 200;
	for (i = 0; i < numtrees/2; i++)
	{
		VERTEX tp;
		tp.x = ((float) rand()/RAND_MAX)*(param[0]/2.0)+param[0]/4.0;
		tp.z = ((float) rand()/RAND_MAX)*(param[2]/2.0)+param[2]/4.0;
		tp.x += param[3];
		tp.z += param[4];
		tp.y = terrain.GetHeight(tp.x, tp.z);
		trees.Add(tp, 40.0, 0, 5);
	}
	
	for (i = 0; i < numtrees/2; i++)
	{
		VERTEX tp;
		tp.x = ((float) rand()/RAND_MAX)*(param[0]/2.0)+param[0]/4.0;
		tp.z = ((float) rand()/RAND_MAX)*(param[2]/2.0)+param[2]/4.0;
		tp.x += param[3];
		tp.z += param[4];
		tp.y = terrain.GetHeight(tp.x, tp.z);
		trees.Add(tp, 60.0, 1, 5);
	}*/
	
	LoadingScreen("Loading...\nDone");
	
	mq1.AddMessage("Editor started, press H for help");
	
	return true;
}

bool InitGameData()
{
	//LoadingScreen("Loading...\nInitializing utilities");
	utility.Init();
	
	LoadingScreen("Loading...\nSetting time");
	LightPosition[0] = 2.0f;
	LightPosition[1] = 2.0f;
	LightPosition[2] = 2.0f;
	LightPosition[3] = 0.0f;
	
	day_time = abs_time = 0.1;
	//day_time = abs_time = 0.03;
	//day_time = abs_time = 0.48;
	//day_time = abs_time = 0.75;
	
	/*
	LoadingScreen("Loading...\nLoading Meshes");
	//load scene objects
	string listval;
	listval = settings.GetSettingsDir() + "/meshlist.txt";
	int ret;
	ret = meshes.LoadList(listval);
	if (!ret)
	{
		error_log << "Fatal error:  could not load mesh list\n";
		return FALSE;
	}
	
	//LoadingScreen("Loading...\nLoading game database");
	//gamedb.LoadAll();
	
	LoadingScreen("Loading...\nLoading new game state");
	*/
	
	LoadingScreen("Loading...\nStarting message queue");
	mq1.SetPersist(5.0f);
	mq1.SetDepth(3);
	mq1.SetPos(0.01,0.01,5,1);
	mq1.SetBuildUp(true);
	//mq1.SetTimePrint(true);
	

	
	/*
	//LoadingScreen("Loading...\nLoading sound");
	//sound.Load(&cam);
	
	//LoadingScreen("Loading...\nLoading HUD");
	//playerhud.Load();
	
	LoadingScreen("Loading...\nLoading water");
	water.initWater(64,64,1,3.0f);
	//water.initWater(32,32,16,100.0f);
	//water.initWater(64,64,8,10.0f);
*/

	editordata.cursortex = utility.TexLoad("gui/cursor.png", false);

	LoadWorld();
	
	return true;
}

/* general OpenGL initialization function */
int initGL()
{
	//warning:  this must be done BEFORE anything important
	//or else it will override it
	font.Load();
	
    /* Enable Texture Mapping ( NEW ) */
    glEnable( GL_TEXTURE_2D );

    /* Enable smooth shading */
    glShadeModel( GL_SMOOTH );

    /* Set the background black */
    //glClearColor( 0.46f, 0.54f, 0.64f, 0.0f );
	glClearColor(0,0,0,0);

    /* Depth buffer setup */
    glClearDepth( 1.0f );

    /* Enables Depth Testing */
    glEnable( GL_DEPTH_TEST );

    /* The Type Of Depth Test To Do */
    glDepthFunc( GL_LEQUAL );

    /* Really Nice Perspective Calculations */
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

	/* Position The Light */
    glLightfv( GL_LIGHT1, GL_POSITION, LightPosition );
	
    /* Setup The Diffuse Light */
    glLightfv( GL_LIGHT1, GL_DIFFUSE, LightDiffuse );
	glLightfv( GL_LIGHT1, GL_SPECULAR, LightSpecular );
	
	/* Setup The Ambient Light */
    glLightfv( GL_LIGHT1, GL_AMBIENT, LightAmbient );

    /* Enable Light One */
    glEnable( GL_LIGHT1 );
	
	/* Enable Lighting */
	glEnable( GL_LIGHTING );
	
	//our perspective matrix
	glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );
	gluPerspective( 45.0f, (float)SCREEN_WIDTH/SCREEN_HEIGHT, 0.1f, 10000.0f );
	glMatrixMode( GL_MODELVIEW );
	
	
	
	// Enable front face culling, since that's what Quake3 does
    //glCullFace(GL_FRONT);
    //glEnable(GL_CULL_FACE);

	if (!InitGameData())
		return (FALSE);

    return( TRUE );
}

void LoadingScreen(string loadtext)
{
	if (verbose_output)
    	cout << loadtext << endl;

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();
	
	utility.Tex2D(3, false);
	utility.Tex2D(2, false);
	utility.Tex2D(1, false);
	utility.Tex2D(0, true);
	
	glSetup();
	
	glEnable(GL_TEXTURE_2D);
	
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer

    Frames++;

	font.Print(0,0,loadtext.c_str(),1,6,1,1,1);
	
	/* Draw it to the screen */
    SDL_GL_SwapBuffers( );

	GLint t = SDL_GetTicks();
	//if (t - T0 >= 50) 
	{
		GLfloat seconds = (t - T0) / 1000.0;
		fps = 1 / seconds;
		//printf("%d frames in %g seconds = %g FPS\n", Frames, seconds, fps);
		T0 = t;
		//Frames = 0;
	}
	
	glPopMatrix();
	glPopAttrib();
}

/* Here goes our drawing code */
int drawGLScene()
{
	#ifdef PERFORMANCE_PROFILE
	suseconds_t t1, t2;
	t1 = GetMicroSeconds();
	t1 = GetMicroSeconds();
	#endif
	
	//glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	#ifdef PERFORMANCE_PROFILE
	t2 = GetMicroSeconds();
	cout << "RenderReflectedScene() ticks: " << t2-t1 << endl;
	t1 = GetMicroSeconds();
	#endif
	
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	gluPerspective( 45.0f, (float)SCREEN_WIDTH/SCREEN_HEIGHT, 0.1f, settings.GetViewDistance() );
	glMatrixMode( GL_MODELVIEW );
	
	glSetup();
	
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f ); 
	//glClearColor( 255.0f, 0.0f, 0.0f, 0.0f ); 
	
	#ifdef PERFORMANCE_PROFILE
	t2 = GetMicroSeconds();
	cout << "glSetup() ticks: " << t2-t1 << endl;
	t1 = GetMicroSeconds();
	#endif
	
	utility.Tex2D(3, false);
	utility.Tex2D(2, false);
	utility.Tex2D(1, false);
	utility.Tex2D(0, true);
	
	glStencilMask(~0);
	glClearStencil(0);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);		// Clear Screen And Depth Buffer
	
	glDisable(GL_STENCIL_TEST);

	
	GLdouble temp_matrix[16];
	int i;
	
	

	cam.PutTransformInto(temp_matrix);
	glLoadMatrixd(temp_matrix);
	
	//reset sun position so it's in the correct frame
	float lp[4];
	lp[0] = LightPosition[0];
	lp[1] = LightPosition[1];
	lp[2] = LightPosition[2];
	lp[3] = 0;
	
	//lighting hardcoded to reasonable position
	lp[0] = 3;
	lp[1] = 3;
	lp[2] = 3;
	lp[3] = 0;
	glLightfv( GL_LIGHT1, GL_POSITION, lp );
	
	#ifdef PERFORMANCE_PROFILE
	t2 = GetMicroSeconds();
	cout << "Matrix setup and light setup ticks: " << t2-t1 << endl;
	t1 = GetMicroSeconds();
	#endif
	
	// model the highlighted vertex belongs to
	OBJECTMODEL * highlighted_model = NULL;
	//if (state.GetGameState() != STATE_INITIALMENU)
	{
		
//		bool normal = true;
		
		glClear (GL_DEPTH_BUFFER_BIT);
		
		#ifdef PERFORMANCE_PROFILE
		t2 = GetMicroSeconds();
		cout << "DrawSky() ticks: " << t2-t1 << endl;
		t1 = GetMicroSeconds();
		#endif
		
		glTranslatef(cam.GetPosition().x,cam.GetPosition().y,cam.GetPosition().z);
	
		/*GLdouble equation[4];
		VERTEX earthnormal = temp - cam.position;
		earthnormal.y += EARTH_RADIUS;
		earthnormal = earthnormal.normalize();
		equation[0] = earthnormal.x;
		equation[1] = -earthnormal.y;
		equation[2] = earthnormal.z;
		equation[3] = (waterheight);
		glClipPlane(GL_CLIP_PLANE1, equation);
		if (underwater)
			glEnable(GL_CLIP_PLANE1);
		else
			glDisable(GL_CLIP_PLANE1);
		*/
		
				
		#ifdef PERFORMANCE_PROFILE
		t2 = GetMicroSeconds();
		cout << "terrain.SetFrustum() ticks: " << t2-t1 << endl;
		t1 = GetMicroSeconds();
		#endif
		
		//glPolygonOffset(1.0,1.0);
		//glPolygonOffset(0.0,10.0);
		//glEnable(GL_POLYGON_OFFSET_FILL);
	
		/*if (normal)
		{
			//GLfloat LightAmbient2[]  = { 0.3f, 0.3f, 0.3f, 1.0f };
			//glLightfv( GL_LIGHT1, GL_AMBIENT, LightAmbient2 );
			glDisable(GL_STENCIL_TEST);
			
			#ifdef PERFORMANCE_PROFILE
			t2 = GetMicroSeconds();
			cout << "terrain.Draw() ticks: " << t2-t1 << endl;
			t1 = GetMicroSeconds();
			#endif
			
			utility.SelectTU(3);
			glDisable(GL_TEXTURE_2D);
			utility.SelectTU(0);
			glEnable(GL_TEXTURE_2D);

			#ifdef PERFORMANCE_PROFILE
			t2 = GetMicroSeconds();
			cout << "trees.Draw() ticks: " << t2-t1 << endl;
			t1 = GetMicroSeconds();
			#endif
			objects.Draw();
			#ifdef PERFORMANCE_PROFILE
			t2 = GetMicroSeconds();
			cout << "objects.Draw() ticks: " << t2-t1 << endl;
			t1 = GetMicroSeconds();
			#endif
			
			//glLightfv( GL_LIGHT1, GL_AMBIENT, LightAmbient );
		}
		
		glDisable(GL_POLYGON_OFFSET_FILL);*/
		
		//glClear (GL_DEPTH_BUFFER_BIT);
		
		//glDisable(GL_LIGHTING);
		objects.Draw();
		
		#ifdef PERFORMANCE_PROFILE
		t2 = GetMicroSeconds();
		cout << "normal draw done" << t2-t1 << endl;
		t1 = GetMicroSeconds();
		#endif
	
		/*
		//draw trees
		foliage.Draw(terrain, cam);
		
		#ifdef PERFORMANCE_PROFILE
		t2 = GetMicroSeconds();
		cout << "foliage.Draw() ticks: " << t2-t1 << endl;
		t1 = GetMicroSeconds();
		#endif
	
		//glDisable(GL_CLIP_PLANE1);
		ships.Draw(false);
	
		terrain.Draw(cam, 1.0f, false, false, true, false, timefactor, fps, day_time);
		//terrain.Draw(cam, 1.0f, false, false, true, false, timefactor, fps, day_time);
		
		//rain is drawn over everything else
		if (!underwater)
			backdrop.DrawRain(day_time);
	
		
		#ifdef PERFORMANCE_PROFILE
		t2 = GetMicroSeconds();
		cout << "DrawRain() ticks: " << t2-t1 << endl;
		t1 = GetMicroSeconds();
		#endif
		
		
		glDisable(GL_CLIP_PLANE1);
	*/	
	
		
		//glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		/*if (0)
		{	
			//experimental bezier stuff
			BEZIER patch;
			VERTEX fl, fr, bl, br;
			//fl.Set(20,40,0);
			fl.Set(5,10,0);
			fr = fl;
			bl = fl;
			br = fl;
			
			fr.x += 15;
			fr.y += 3;
			
			br.x += 12;
			br.z += 8;
			
			bl.z += 10;
			bl.y -= 2;
			
			patch.SetFromCorners(fl, fr, bl, br);
			
			BEZIER nextpatch;
			VERTEX offset;
			offset.x += -7;
			offset.z += -10;
			fl = fl + offset;
			fr = fr + offset;
			bl = bl + offset;
			br = br + offset;
			
			nextpatch.SetFromCorners(fl, fr, bl, br);
			
			//patch.Attach(nextpatch);
			
			BEZIER thirdpatch;
			offset.zero();
			offset.y += -3;
			offset.z += -10;
			fl = fl + offset;
			fr = fr + offset;
			bl = bl + offset;
			br = br + offset;
			
			thirdpatch.SetFromCorners(fl, fr, bl, br);
			
			//nextpatch.Attach(thirdpatch);
			
			TRACK track;
			ROADSTRIP * teststrip = track.AddNewRoad();
			teststrip->Add(patch);
			teststrip->Add(nextpatch);
			teststrip->Add(thirdpatch);
			
			//teststrip.DeleteLastPatch();
					
			track.VisualizeRoads(true, true);
			
			VERTEX down;
			down.y = -1;
			VERTEX colpoint;
			if (patch.Collide(cam.position.ScaleR(-1.0), down, colpoint))
			{
				//colpoint.DebugPrint();
			}
		}*/
		
		VERTEX camray;
		camray.z = -1;
		camray = cam.dir.ReturnConjugate().RotateVec(camray);
		/*camray.z = 1;
		camray = cam.dir.RotateVec(camray);*/
		VERTEX selvert;
		bool highlightedvert = false;
		if (objects.FindClosestVert(cam.position.ScaleR(-1.0), camray, selvert, highlighted_model))
		{
			/*cam.position.ScaleR(-1.0).DebugPrint();
			selvert.DebugPrint();
			cout << endl;*/
			
			//draw a highlighted vert
			
			highlightedvert = true;
			
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glDisable(GL_LIGHTING);
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_DEPTH_TEST);
			//glEnable(GL_BLEND);
			//glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			glPointSize(4.0);
			//glColor4f(1,1,0,0.75);
			glColor4f(1,1,0,1);
			glBegin(GL_POINTS);
			glVertex3fv(selvert.v3());
			glEnd();
			glDisable(GL_BLEND);
			glColor4f(1,1,1,1);
			glPopAttrib();
		}
		
		//left click
		if (highlightedvert && mouse.ButtonDown(1) && !editordata.mousebounce[1])
		{
			int oldnumbezinput = editordata.numbezinput;
			
			//left click on a highlighted vert
			if (vertmode == TWOVERTS)
			{
				if (editordata.numbezinput == 0)
					editordata.numbezinput = 3;
				else if (editordata.numbezinput <= 3)
					editordata.numbezinput = 4;
				else if (editordata.numbezinput <= 4)
					editordata.numbezinput = 7;
				else if (editordata.numbezinput <= 7)
					editordata.numbezinput = 8;
			}
			else
			{
				editordata.numbezinput++;
				if (vertmode == THREEVERTS && (editordata.numbezinput == 2 || editordata.numbezinput == 6))
					editordata.numbezinput++;
			}
			
			if (editordata.numbezinput >= 8)
			{
				//create bezier patch
				BEZIER patch;
				
				if (vertmode == TWOVERTS)
					patch.SetFromCorners(editordata.bezinput[4], selvert, editordata.bezinput[0], editordata.bezinput[3]);
				else
				{
					//copy the front and back selected rows to the patch, then tell it to do the math to find the other points
					// plus center and radius calculations
					
					editordata.bezinput[7] = selvert;
					
					for (int i = 0; i < 4; i++)
						patch.points[3][i] = editordata.bezinput[i];
					for (int i = 0; i < 4; i++)
						patch.points[0][i] = editordata.bezinput[i+4];
					
					/*if (vertmode == THREEVERTS)
					{
						//recalculate the middle two verts
						for (int i = 0; i < 4; i += 3)
						{
							if ((patch.points[i][1] - patch.points[i][2]).len() < 0.0001)
							{
								VERTEX leftslope = patch.points[i][1] - patch.points[i][0];
								VERTEX rightslope = patch.points[i][2] - patch.points[i][3];
								patch.points[i][1] = patch.points[i][0] + leftslope.ScaleR(0.5);
								patch.points[i][2] = patch.points[i][3] + rightslope.ScaleR(0.5);
							}
						}
					}*/
					
					patch.CalculateMiddleRows();
				}
				
				if (activestrip == NULL)
				{
					activestrip = track.AddNewRoad();
					mq1.AddMessage("New road created to hold new patch.");
				}
				
				activestrip->Add(patch);
				
				//editordata.numbezinput = 0;
				editordata.numbezinput = 4;
				
				editordata.bezinput[0] = patch.points[0][0];
				editordata.bezinput[1] = patch.points[0][1];
				editordata.bezinput[2] = patch.points[0][2];
				editordata.bezinput[3] = patch.points[0][3];
			}
			else
			{
				editordata.bezinput[oldnumbezinput] = selvert;
				if (vertmode == THREEVERTS && (oldnumbezinput == 1 || oldnumbezinput == 5))
					editordata.bezinput[oldnumbezinput+1] = selvert;
			}
		}
		
		editordata.mousebounce[1] = mouse.ButtonDown(1);
		
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
		glPointSize(4.0);
		glColor4f(1,0,0,1);
		glBegin(GL_POINTS);
		for (i = 0; i < editordata.numbezinput; i++)
		{
			if (vertmode != TWOVERTS || i == 0 || i == 3 || i == 4 || i == 7)
			{
				glVertex3fv(editordata.bezinput[i].v3());
			}
		}
		glEnd();
		glColor4f(1,1,1,1);
		glPopAttrib();
		
		track.VisualizeRoads(true, false, activestrip);
		
		for (int l = 0; l < track.NumLapSeqs(); l++)
		{
			BEZIER * lapbez = track.GetLapSeq(l);
			if (lapbez != NULL)
			{
				VERTEX reddish;
				reddish.x = 0.5;
				reddish.y = 0.2;
				reddish.z = 0.2;
				lapbez->Visualize(true, false, reddish);
			}
		}
		
		
		//image in the framebuffer is now complete.
		
		
		/*//add a brightness/contrast adjustment
		glClear (GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glDisable(GL_FOG);
		glDisable(GL_LIGHTING);
		glTranslatef(0,0,-40.0f);
		glBlendFunc(GL_ONE, GL_SRC_ALPHA);
		
		float rd = (float) weathersystem.GetRainDrops();
		
		float rainscale = 0.7f;
		float clearscale = 1.1f;
		float rainbias = 0.035f;
		float clearbias = 0.0f;
		float rainmax = 50.0f;
		
		float rainfactor = (rd/rainmax);
		if (rainfactor > 1.0f)
			rainfactor = 1.0f;
	
		float scale = rainfactor*rainscale+(1.0f-rainfactor)*clearscale;
		float bias = rainfactor*rainbias+(1.0f-rainfactor)*clearbias;
	
	
		glColor4f(bias, bias, bias, scale);
		float x1, y1, x2, y2;
		x1 = -30;
		y1 = -30;
		x2 = 30;
		y2 = 30;
		
		if (scale > 1.0) 
		{
			float remainingScale;
			
			remainingScale = scale;
			glBlendFunc(GL_DST_COLOR, GL_ONE);
			if (remainingScale > 2.0) 
			{
				glColor4f(1, 1, 1, 1);
				while (remainingScale > 2.0) 
				{
					glRectf(x1,y1,x2,y2);
					remainingScale /= 2.0;
				}
			}
			glColor4f(remainingScale - 1,
				remainingScale - 1, remainingScale - 1, 1);
			glRectf(x1,y1,x2,y2);
			glBlendFunc(GL_ONE, GL_ONE);
			if (bias != 0)
			{
				if (bias > 0) 
				{
					glColor4f(bias, bias, bias, 0.0);
				} 
				else 
				{
					glColor4f(-bias, -bias, -bias, 0.0);
					//can't do bias < 0
				}
				glRectf(x1,y1,x2,y2);
			}
		}
		else 
		{
			if (bias > 0) 
			{
				glColor4f(bias, bias, bias, scale);
			}
			else 
			{
				glColor4f(-bias, -bias, -bias, scale);
				//can't do bias < 0
			}
			glBlendFunc(GL_ONE, GL_SRC_ALPHA);
			glRectf(x1,y1,x2,y2);
		}
		
		glPopAttrib();*/
	
		//timer.Draw();
	}
	
	float w, h;
	w = 0.02;
	h = w*(4/3.0);
	utility.Tex2D(0,true);
	utility.Draw2D(0.5-w, 0.5-h, 0.5+w, 0.5+h, editordata.cursortex);
	
	if (fps > 0.0f)
		mq1.Draw(timefactor, fps, font);
	
	
	#ifdef PERFORMANCE_PROFILE
	t2 = GetMicroSeconds();
	cout << "Brightness/contrast adjustment ticks: " << t2-t1 << endl;
	t1 = GetMicroSeconds();
	#endif

	
	
    Frames++;
	frameno++;
	if (frameno >= 30011)
		frameno -= 30011;
	lfps[lfpspos] = pfps;
	lfpspos++;
	if (lfpspos >= AVERAGEFRAMES)
	{
		lfpspos = lfpspos % AVERAGEFRAMES;
		lfpsfull = true;
	}
	
	float tfps = 0.0f;
	int tnum = 0;
	for (i = 0; i < AVERAGEFRAMES; i++)
	{
		if (!(!lfpsfull && i >= lfpspos))
		{
			tfps += lfps[i];
			tnum++;
		}
	}
	fps = std::min(1000.0f,tfps / (float) tnum);
	
	/*lfps += pfps;
    {
		//const int freq = (int) MIN_FPS;
		const int freq = 60;
		if (Frames >= freq)
		{
			fps = lfps / freq;
			Frames = 0;
			lfps = 0;
		}
	}*/
	
	char tempchar[1024];
	sprintf(tempchar, "Frames per second: %f\n", fps);
	
	//font.Print(0.5,0,tempchar,0,0,1,1,0);
	if (showfps)
		font.Print( 0.75, 0.0, tempchar, 1, 5, 1.0 );
	
	// print camera position
	VERTEX pos = cam.GetPosition();
	sprintf(tempchar, "Position: %0.2f, %0.2f, %0.2f\n", -pos.x, -pos.y, -pos.z);
	font.Print( 0.75, 0.025, tempchar, 1, 5, 1.0 );
	
	//VERTEX ang = cam.dir.GetEulerZYX();
	//sprintf(tempchar, "Angle: %0.2f, %0.2f, %0.2f\n", 180/M_PI*ang.x, -180/M_PI*ang.y, 180/M_PI*ang.z);
	//font.Print( 0.75, 0.05, tempchar, 1, 5, 1.0 );
	
	// print object name the highlighted vertex belongs to
	if (highlighted_model)
	{
		string modelname = highlighted_model->name;
		sprintf(tempchar, "Model: %s\n", modelname.c_str());
		font.Print( 0.75, 0.05, tempchar, 1, 5, 1.0 );
	}
	
	#ifdef PERFORMANCE_PROFILE
	t2 = GetMicroSeconds();
	cout << "font.Print() ticks: " << t2-t1 << endl;
	t1 = GetMicroSeconds();
	#endif
	
	
	//draw help screen(s)
	if (editordata.helppage > 2)
		editordata.helppage = 0;
	if (editordata.helppage == 1)
	{
		font.Print(0.1, 0.1, "VDrift Track Editor Keys (press H again for more help)\n"
	"Mouse left click\n"
	"Arrow keys\n"
	"Page Up\n"
	"A, F\n"
	"L\n"
	"K\n"
	"N\n"
	"R\n"
	"S\n"
	"I\n"
	"E\n"
	"- (minus)\n"
	"2, 3, 4\n"
	"BACKSPACE\n"
	"ESCAPE\n"
	, 1, 5, 1.0);
		font.Print(0.3, 0.1, "\n"
	"Select highligted vertex\n"
	"Move around\n"
	"Move forward very fast\n"
	"Automatically try to create the next bezier patch on this road (F: 25 at a time)\n"
	"Add the current camera position to the list of car start locations\n"
	"Remove the last start location from the list\n"
	"Create a new road (the new road is created once you add patches to it)\n"
	"Select the road under the cursor\n"
	"Save the track\n"
	"Print the object that owns the selected vertex to the console\n"
	"Mark a road segment as part of a lap sequence\n"
	"Clear all lap sequences\n"
	"Select vertices 2 at a time, 3 at a time, 4 at a time\n"
	"Delete the last bezier patch on this road\n"
	"Quit the editor\n"
	, 1, 5, 1.0);
	}
	if (editordata.helppage == 2)
		font.Print(0.1, 0.1, "VDrift Track Editor Textual Help\n"
	"The editor starts in vertex selection mode.  Vertices are highlighted yellow as the mouse pointer\n"
	"gets close to them.  Press the left mouse button to select the highlighted vertex.  It will turn\n"
	"red.  Select a vertex on the left side of the road, then a vertex on the right side of the road,\n"
	"with left and right determined by the direction you'd like to create the road in.  You can now select\n"
	"the next two vertices (left and then right), and a green box will be created to indicate a bezier\n"
	"patch has been created there.  Notice that the last two vertices you select are still colored red\n"
	"(selected).  You can now select the next two vertices (left and right), and continue to create bezier\n"
	"patches around the track.  Once you have created a bezier patch, you can alternatively press A to\n"
	"have the editor try to automatically select the next two vertices and auto-create the next patch.\n"
	"This works well in straight areas, not so well in curvy areas.  Press BACKSPACE at any time to delete\n"
	"the last patch that was created."
	, 1, 5, 1.0);


	/* Draw it to the screen */
    SDL_GL_SwapBuffers( );

    
		GLint t = SDL_GetTicks();
		//if (t - T0 >= 50) 
		{
			GLfloat seconds = (t - T0) / 1000.0;
			pfps = 1 / seconds;
			//printf("%d frames in %g seconds = %g FPS\n", Frames, seconds, fps);
			T0 = t;
			//Frames = 0;
		}
		
	#ifdef PERFORMANCE_PROFILE
	t2 = GetMicroSeconds();
	cout << "SwapBuffers() ticks: " << t2-t1 << endl;
	t1 = GetMicroSeconds();
	cout << endl;
	#endif
		
    return( TRUE );
}

void ToggleFPSDisplay()
{
	showfps = !showfps;
	settings.SetFPSEnabled(showfps);
}

// This function returns true if the extension is there.
bool isExtensionSupported(string extstring)
{
    char * temp = (char *) glGetString(GL_EXTENSIONS);
	if (temp == 0)
	{
		cout << "Error getting extensions.  Continuing anyway, errors may follow!" << endl;
		return true;
	}
    
	string s = temp;
	string::size_type temppos = s.find(extstring);

	bool hasext = (temppos <= s.length());
	
	if (!hasext)
	{
		cout << "Extension not supported: " << extstring << endl;
	}
	
	//cout << s << endl;
	
    return hasext;
}

void ResetWorld(bool fullreset)
{
}

int main( int argc, char **argv )
{
    /* Flags to pass to SDL_SetVideoMode */
    int videoFlags;
    /* main loop variable */
    int done = FALSE;
    /* used to collect events */
    SDL_Event event;
    /* this holds some info about our display */
    const SDL_VideoInfo *videoInfo;
    /* whether or not the window is active */
    int isActive = TRUE;

	surface = NULL;
	
	//load error log
	error_log.open((settings.GetSettingsDir() + "/logs/main.log").c_str());
	
	//create trig lookups
	//CreateLookupTable();

	int i;
	for (i = 0; i < argc; i++)
	{
		if (strcmp(argv[i],"-verbose") == 0)
			verbose_output = true;
	}

	//set frame stats to zero
	Frames = 0;
	T0 = 0;
	fps = 0;
	frameno = 0;
	
	
	
    /* initialize SDL */
    if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0 )
	{
	    fprintf( stderr, "SDL initialization failed: %s\n",
		     SDL_GetError( ) );
	    Quit( 1 );
	}
	
	cout << "Run with -verbose for troubleshooting." << endl;
	
	/*SDL_Joystick *joystick;
	SDL_JoystickEventState(SDL_ENABLE);
	joystick = SDL_JoystickOpen(0);
	printf("Joystick 0 opened.\n\n");*/
	
	SDL_JoystickEventState(SDL_ENABLE);
	
	SDL_WM_SetCaption("VDrift - Track Editor", NULL);
	SDL_WM_SetIcon(IMG_Load(settings.GetFullDataPath("textures/small/icons/vdrift-32x32.png").c_str()), NULL);
	SDL_ShowCursor(SDL_DISABLE);

    /* Fetch the video info */
    videoInfo = SDL_GetVideoInfo( );

    if ( !videoInfo )
	{
	    fprintf( stderr, "Video query failed: %s\n",
		     SDL_GetError( ) );
	    Quit( 1 );
	}

    /* the flags to pass to SDL_SetVideoMode */
    videoFlags  = SDL_OPENGL;          /* Enable OpenGL in SDL */
    videoFlags |= SDL_GL_DOUBLEBUFFER; /* Enable double buffering */
    videoFlags |= SDL_HWPALETTE;       /* Store the palette in hardware */
    videoFlags |= SDL_RESIZABLE;       /* Enable window resizing */

    /* This checks to see if surfaces can be stored in memory */
    if ( videoInfo->hw_available )
	videoFlags |= SDL_HWSURFACE;
    else
	videoFlags |= SDL_SWSURFACE;

    /* This checks if hardware blits can be done */
    if ( videoInfo->blit_hw )
	videoFlags |= SDL_HWACCEL;

    /* Sets up OpenGL double buffering */
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
    
    //set up the depth buffer to be 24 bits, 16 bits are NOT enough
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );

/*	ifstream vconf;
	vconf.open((settings.GetSettingsDir() + "/videoconfig").c_str());
	if (vconf)
	{
		vconf >> w;
		vconf >> h;
		vconf >> bpp;
		vconf >> fs;
		vconf.close();
	}
*/
	int w, h, bpp, fs;
	w = settings.GetDisplayX();
	h = settings.GetDisplayY();
	bpp = settings.GetDisplayDepth();
	fs = settings.GetFullscreenEnabled();
	
	if (SDL_VideoModeOK(w, h, bpp, videoFlags) != 0)
	{
		ChangeDisplay(w, h, bpp, fs, true);
	}
	else
	{
		surface = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP,
					videoFlags );
	}
	
    /* Verify there is a surface */
    if ( !surface )
	{
	    fprintf( stderr,  "Video mode set failed: %s\n", SDL_GetError( ) );
	    Quit( 1 );
	}

    /* initialize OpenGL */
    initGL( );

    /* resize the initial window */
    resizeWindow( SCREEN_WIDTH, SCREEN_HEIGHT );

	if (!(isExtensionSupported("GL_ARB_multitexture")))
	{
		cout << "OpenGL extensions not present." << endl;
		Quit(0);
		return(0);
	}
	
	if (verbose_output)
	{
		#ifdef HAVE_OPENAL
		cout << "Using OpenAL for sound" << endl;
		#else
		cout << "Using FMOD for sound" << endl;
		#endif
	}

	// grab & hide mouse
    //SDL_WM_GrabInput(SDL_GRAB_ON);
    //if ( SDL_ShowCursor(SDL_DISABLE) != SDL_DISABLE )
      //cerr << SDL_GetError() << endl;

	timefactor = 1.0f;
	
    /* wait for events */
    while ( !done )
	{
	    /* handle the events in the queue */

	    while ( SDL_PollEvent( &event ) )
		{
//			float v;
			
		    switch( event.type )
			{
			case SDL_ACTIVEEVENT:
			    /* Something's happend with our focus
			     * If we lost focus or we are iconified, we
			     * shouldn't draw the screen
			     */
			    if ( event.active.gain == 0 )
				isActive = FALSE;
			    else
				isActive = TRUE;
			    break;			    
			case SDL_VIDEORESIZE:
			    /* handle resize event */
			    surface = SDL_SetVideoMode( event.resize.w,
							event.resize.h,
							16, videoFlags );
			    if ( !surface )
				{
				    fprintf( stderr, "Could not get a surface after resize: %s\n", SDL_GetError( ) );
				    Quit( 1 );
				}
			    resizeWindow( event.resize.w, event.resize.h );
			    break;
			case SDL_KEYDOWN:
			    /* handle key presses */
			    handleKeyPress( &event.key.keysym );
				keyman.KeyDown(event.key.keysym.sym);
			    break;
			case SDL_KEYUP:
				keyman.KeyUp(event.key.keysym.sym);
				break;
			case SDL_QUIT:
			    /* handle quit requests */
			    done = 1;
			    break;
			case SDL_JOYBUTTONDOWN:
				break;
			case SDL_JOYAXISMOTION:
				break;
			default:
			    break;
			}
		}

	    /* do the game logic & draw the screen*/
	    //if ( isActive )
		{
			Update();
			drawGLScene( );
		}
	}
	
    /* clean ourselves up and exit */
    Quit( 0 );

    /* Should never get here */
    return( 0 );
}
