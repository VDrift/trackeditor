#include "backdrop.h"

BACKDROP::BACKDROP()
{
	//TEXTURE_PATH = settings.GetDataDir() + "/tex";
	error_log.open("logs/backdrop.log");
	initdone = false;
}

extern bool verbose_output;
BACKDROP::~BACKDROP()
{
	if (verbose_output)
		cout << "backdrop deinit" << endl;
	
	error_log.close();
}

void BACKDROP::DeInit()
{
	if (initdone)
	{
		glDeleteTextures(1, &sun_texture);
		glDeleteTextures(1, &sun_texture_ref);
		glDeleteTextures(1, &sky_texture);
		glDeleteTextures(1, &rain_texture);
		glDeleteTextures(1, &cloud_texture);
		glDeleteTextures(1, &moon_texture);
		glDeleteTextures(1, &stars_texture);
		
		initdone = false;
	}
}

void BACKDROP::Init()
{
	fogdensity = 0.0005f;
	
	//*** Load Texture ***
	string filepath = "weather/star.png";
	sun_texture = utility.TexLoad(filepath, false);
	
	/*SDL_Surface *TextureImage[1];					// Create Storage Space For The Texture
	
	int format = GL_RGB;

	// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
	if ((TextureImage[0]=IMG_Load(buffer)))
	{
		//SDL_SetAlpha(TextureImage[0], 0, 0);
		
		glGenTextures(1, &sun_texture);					// Create Texture
		
		// Create MipMapped Texture
		glBindTexture(GL_TEXTURE_2D, sun_texture);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexImage2D( GL_TEXTURE_2D, 0, format, TextureImage[0]->w, TextureImage[0]->h, 0, format, GL_UNSIGNED_BYTE, TextureImage[0]->pixels );
		}
	}
	else
	{
		//quit, bitmap not found
		error_log << "Could not find texture: " << buffer << "\n";
	}

	if (TextureImage[0])								// If Texture Exists
	{
		// Free up any memory we may have used
    	SDL_FreeSurface( TextureImage[0] );
	}*/
	
	
	filepath = "weather/star_reflection.png";
	//strcpy(buffer, filepath.c_str());
	sun_texture_ref = utility.TexLoad(filepath, false);
	/*format = GL_RGBA;

	// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
	if ((TextureImage[0]=IMG_Load(buffer)))
	{
		//SDL_SetAlpha(TextureImage[0], 0, 0);
		
		glGenTextures(1, &sun_texture_ref);					// Create Texture
		
		// Create MipMapped Texture
		glBindTexture(GL_TEXTURE_2D, sun_texture_ref);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexImage2D( GL_TEXTURE_2D, 0, format, TextureImage[0]->w, TextureImage[0]->h, 0, format, GL_UNSIGNED_BYTE, TextureImage[0]->pixels );
		}
	}
	else
	{
		//quit, bitmap not found
		error_log << "Could not find texture: " << buffer << "\n";
	}
	if (TextureImage[0])								// If Texture Exists
	{
		// Free up any memory we may have used
    	SDL_FreeSurface( TextureImage[0] );
	}*/
	
	filepath = settings.GetFullDataPath("textures/" + settings.GetTexSize() + "/weather/clearSky-joe.png");
	//strcpy(buffer, filepath.c_str());

	//sky_texture = utility.TexLoad(filepath, false);

	int format = GL_RGBA;

	// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
	if ((sky_tex_img=IMG_Load(filepath.c_str())))
	{
		//SDL_SetAlpha(TextureImage[0], 0, 0);
		
		glGenTextures(1, &sky_texture);					// Create Texture
		
		// Create Texture
		glBindTexture(GL_TEXTURE_2D, sky_texture);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexImage2D( GL_TEXTURE_2D, 0, format, sky_tex_img->w, sky_tex_img->h, 0, format, GL_UNSIGNED_BYTE, sky_tex_img->pixels );
	}
	else
	{
		//quit, bitmap not found
		error_log << "Could not find texture: " << filepath << "\n";
	}
	
	
	filepath = "weather/raindrop.tga";
	//strcpy(buffer, filepath.c_str());
	rain_texture = utility.TexLoad(filepath, false);
	/*format = GL_RGBA;
	// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
	if ((TextureImage[0]=IMG_Load(buffer)))
	{
		//SDL_SetAlpha(TextureImage[0], 0, 0);
		
		glGenTextures(1, &rain_texture);					// Create Texture
		
		// Create Texture
		glBindTexture(GL_TEXTURE_2D, rain_texture);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexImage2D( GL_TEXTURE_2D, 0, format, TextureImage[0]->w, TextureImage[0]->h, 0, format, GL_UNSIGNED_BYTE, TextureImage[0]->pixels );
	}
	else
	{
		error_log << "Could not find texture: " << buffer << "\n";
	}*/
	//cout << "boo" << endl;
	
	cloud_texture = utility.TexLoad("weather/clouds.png", true);
	
	
	//check for volumetric fog
	/*char extensions[ 16384 ];
	int i;
	int num_ext = 2;
    string extnames[num_ext];
	bool hasext[num_ext];
	bool extsufficient = true;
	extnames[0]	= "GL_ARB_fog_coord";
	extnames[1]	= "GL_ARB_multitexture";
	//extnames[2]	= "GL_NV_register_combiners";
	//extnames[3]	= "GL_NV_texture_shader";
    char *buf;
	
	for (i = 0; i < num_ext; i++)
		hasext[i] = false;

    sprintf( extensions, "%s", (char *)glGetString( GL_ARBENSIONS ) );
	//cout << extensions << endl;
    buf = strtok( extensions, " " );
    while( buf != NULL )
    {
		for (i = 0; i < num_ext; i++)
		{
			if( !strcmp( extnames[i].c_str(), buf ) )
			{
				hasext[i] = true;
			}
		}
		
		buf = strtok( NULL, " " );
    }
	
	for (i = 0; i < num_ext; i++)
	{
		if (!hasext[i])
			extsufficient = false;
	}

	if (!extsufficient)
	{
    	error_log << "You are missing required extensions.  Expect a crash.\n" << endl;
		error_log << "You need at least a Geforce 3 for these extensions.\n" << endl;
		for (i = 0; i < num_ext; i++)
		{
			error_log << extnames[i] << ": ";
			if (hasext[i])
				error_log << "YES" << endl;
			else
				error_log << "NO" << endl;
		}
	}*/

	//set up fog
	float fogcolor[4] = {  0.53f, 0.74f, 0.91f, 0.0f  };
	glFogfv(GL_FOG_COLOR, fogcolor);
	glFogf(GL_FOG_START, 100);
	glFogf(GL_FOG_END, 1000);
	glFogf(GL_FOG_DENSITY, 0.0005f);
	//glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogi(GL_FOG_MODE, GL_EXP);
	glEnable(GL_FOG);

	
	//set up fog
	/*glEnable(GL_FOG);													// Enable Fog
	glFogi(GL_FOG_MODE, GL_LINEAR);										// Fog Fade Is Linear
	float fogcolor[4];
	fogcolor[0] = 0.09f;
	fogcolor[1] = 0.25f;
	fogcolor[2] = 0.38f;
	fogcolor[3] = 0.2f;
	int i;
	for (i = 0; i < 4; i++)
		fogcolor[i] *= 0.5f;
	glFogfv(GL_FOG_COLOR, fogcolor);
	glFogf(GL_FOG_START,  1.0f);										// Set The Fog Start
	glFogf(GL_FOG_END,    0.0f);										// Set The Fog End
	glHint(GL_FOG_HINT, GL_NICEST);										// Per-Pixel Fog Calculation
	glFogi(GL_FOG_COORDINATE_SOURCE_ARB, GL_FOG_COORDINATE_ARB);*/
	
	sunpos_lng = 0.0f;
	sunpos_lat = 0.7f;
	
	moonpos_lng = 0.0f;
	moonpos_lat = 0.2f;
	
	moon_texture = utility.TexLoad("weather/moon.png", false);
	stars_texture = utility.TexLoad("weather/stars.png", false);
}

void BACKDROP::Draw(VERTEX tersize)
{
	//setup gl flags
	//glEnable(GL_BLEND);
	glDisable(GL_BLEND);
	glDisable( GL_TEXTURE_2D );
	glDisable( GL_LIGHTING);
	//glDisable(GL_FOG);
	//glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glDisable(GL_DEPTH_TEST);
	/*glBegin(GL_QUADS);
	glEnable(GL_FOG);
	
	//setup custom vars
	float watercol[4];
	watercol[0] = 0.0f;
	watercol[1] = 0.0f;
	watercol[2] = 0.0f;
	watercol[3] = 1.0f;
	glColor4fv(watercol);
	//float yheight = 3.9f;
	float yheightb = 0.1f;
	float xw2 = 5000;
	float yw2 = 5000;
	float xw1 = tersize.x;
	float yw1 = tersize.z;
	
	//draw caustics
	float yheightf = 0.0f;
	float counter = 0.0001f;
	float yheight;
	float ystart = 0.0f;

	yheightf = ystart;
	yheight = yheightf*tersize.y;
	glColor4fv(watercol);
	//glNormal3f(0,1,0);
	glVertex3f(-xw2,yheight,-yw2);
	glVertex3f(-xw2,yheight,yw2);
	glVertex3f(xw2,yheight,yw2);
	glVertex3f(xw2,yheight,-yw2);
	
	glEnd();*/

	glEnable(GL_FOG);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	glEnable( GL_TEXTURE_2D );
	glEnable( GL_LIGHTING);
}

void BACKDROP::DrawSky(double daytime, float transparency)
{
	daytime_cache = daytime;
	sunpos_lat = daytime*3.141593f*2.0f;
	
	moonpos_lat = (daytime+0.4)*3.141593f*2.0f;
	
	//setup gl flags
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_TEXTURE_2D );
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_FOG);
	
	QUATERNION cor, qx, qy, qz, qt;
	double temp_radius, temp_long, temp_lat;
	GLdouble temp_matrix[16];
	temp_radius = sqrt(cam.position.x*cam.position.x+cam.position.z*cam.position.z+(cam.position.y+EARTH_RADIUS)*(cam.position.y+EARTH_RADIUS));
	temp_long = atan2((double)(cam.position.y+EARTH_RADIUS),(double)cam.position.x);
	temp_lat = acos(cam.position.z/temp_radius);
	temp_lat -= 1.570796f;
	temp_long -= 1.570796f;
	qz.SetAxisAngle(-temp_long, 0, 0, 1);
	qx.SetAxisAngle(temp_lat, 1, 0, 0);
	qt = qx*qz;
	qt.GetMat(temp_matrix);
	
	DrawStars(daytime, transparency, temp_matrix);
	DrawMoon(daytime, transparency, temp_matrix);
	DrawSkyGradient(daytime, transparency, temp_matrix);
	/*glColorMask(1,1,1,1);
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f ); 
	glClear(GL_COLOR_BUFFER_BIT);*/
	DrawSun(daytime, transparency, temp_matrix);
	DrawClouds(daytime, transparency, temp_matrix);
	
	glPopAttrib();
}

void BACKDROP::DrawSkyGradient(double daytime, float transparency, GLdouble *temp_matrix)
{
	//setup gl flags
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFunc(GL_SRC_ALPHA, GL_DST_COLOR);
	glEnable( GL_TEXTURE_2D );
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_FOG);
	
	//variables to help with hemisphere generation
	int lng,lat;
	int num_longitudes = 12;
	int num_lattitudes = 12;
	float hem_radius = 10.0f;
	//float safety_factor = 0.15f;
	float safety_factor = 0.0f;
	float hem_height = hem_radius;
	float pi = 3.141593f;
	
	int i;
	
	glPushMatrix();
	glMultMatrixd(temp_matrix);
	
	float colhoriz[3], colzenith[3];
	colhoriz[0] = .53f;
	colhoriz[1] = .74f;
	colhoriz[2] = .91f;
	
	/*colzenith[0] = 0.0f;
	colzenith[1] = 100.0f/255.0f;
	colzenith[2] = 200.0f/255.0f;*/
	colzenith[0] = 0.0f;
	colzenith[1] = 50.0f/255.0f;
	colzenith[2] = 100.0f/255.0f;
	
	VERTEX * oldset;
	VERTEX * curset;
	oldset = new VERTEX [num_longitudes+1];
	curset = new VERTEX [num_longitudes+1];
	VERTEX oldrot;
	
	QUATERNION qrot, qtemp;
	
	glBindTexture(GL_TEXTURE_2D,sky_texture);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	glBegin(GL_QUADS);

	int num_repeats = 0;
	int old_repeats = 0;
	
	glColor4f(1.0f,1.0f,1.0f,transparency);

	int pass;
	for (pass = 0; pass < 2; pass++)
	{
		for (lat = 0; lat <= num_lattitudes; lat++)
		{
			qrot.SetAxisAngle((pi/2.0f)*((float)lat/(float)num_lattitudes), 0, 0, 1);
			VERTEX rotvec;
			rotvec.x = hem_radius;
			rotvec = qrot.RotateVec(rotvec);
			
			for (lng = 0; lng <= num_longitudes; lng++)
			{
				VERTEX currot;
				float curcol[4];
				
				qtemp.SetAxisAngle(2.0f*pi*((float)lng/(float)num_longitudes), 0, 1, 0);
				currot = qtemp.RotateVec(rotvec);
				//cout << currot.x << "," << currot.y << "," << currot.z << endl;
				
				if (lat > 0 && lng > 0)
				{
					//glColor4f(0.53f, 0.74f, 0.91f, 1.0f);
					float rheight;
	
					rheight = 1.0f-currot.y/hem_height;
					//rheight = (rheight-1)*(rheight-1)*(rheight-1)+1;
	
					for (i = 0; i < 3; i++)
						curcol[i] = colhoriz[i]*(1.0f-rheight)+colzenith[i]*rheight;
					//glColor4fv(curcol);
					glTexCoord2f(daytime,rheight);
					
					glVertex3f(oldrot.x, oldrot.y-safety_factor, oldrot.z);
					glVertex3f(currot.x, currot.y-safety_factor, currot.z);
	
					rheight = 1.0f-oldset[lng].y/hem_height;
					//rheight = (rheight-1)*(rheight-1)*(rheight-1)+1;
					
					for (i = 0; i < 3; i++)
						curcol[i] = colhoriz[i]*(1.0f-rheight)+colzenith[i]*(rheight);
					//glColor4fv(curcol);
					glTexCoord2f(daytime,rheight);	
					glVertex3f(oldset[lng].x, oldset[lng].y-safety_factor, oldset[lng].z);
					glVertex3f(oldset[lng-1].x, oldset[lng-1].y-safety_factor, oldset[lng-1].z);
				}
				
				oldrot = currot;
				curset[lng] = currot;
			}
			
			VERTEX * tptr = curset;
			curset = oldset;
			oldset = tptr;
			old_repeats = num_repeats;
		}
		
		hem_radius = -hem_radius;
		hem_height = hem_radius;
	}
	
	delete [] curset;
	delete [] oldset;
	
	glEnd();
	glPopMatrix();
	glPopAttrib();
}

void BACKDROP::DrawSun(double daytime, float transparency, GLdouble *temp_matrix)
{
	int i;
	float hem_radius = 10.0f;
	float hem_height = hem_radius;
	
	//okay, now plop on the SUN
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glClear(GL_DEPTH_BUFFER_BIT);
	float sunsize = 2.0f;
	/*float reflection_elongation_y = 1.0f; //how much the sun is stretched by in
										// the reflection rendering
	float reflection_elongation_x = 1.0f;*/
	float sunsize_x = sunsize;
	float sunsize_y = sunsize;
	
	VERTEX sunquad[4];
	for (i = 0; i < 4; i++)	//generate the sun quad
	{
		if (i == 0 || i == 3)
		{
			sunquad[i].y = -sunsize_y;
		}
		else
		{
			sunquad[i].y = sunsize_y; //only elongate in -y direction
		}
		if (i < 2)
			sunquad[i].x = -sunsize_x;
		else
			sunquad[i].x = sunsize_x;
	}
	QUATERNION sunquat, sunquat2;
	//float sunheight = atan2(LightPosition[1],LightPosition[2]); //sun angular height in radians
	float transform_matrix[16];
	glPushMatrix();
	glMultMatrixd(temp_matrix);
	
	//float sundir = atan2(LightPosition[0],LightPosition[2]);
	sunquat.SetAxisAngle(-sunpos_lng, 0, 1, 0);
	sunquat.GetMat(transform_matrix); //crank it around the sky
	glMultMatrixf(transform_matrix);
	
	sunquat2.SetAxisAngle(sunpos_lat, 0, 0, 1);
	sunquat2.GetMat(transform_matrix); //crank it up into the sky
	glMultMatrixf(transform_matrix);
	
	glTranslatef(hem_height,0,0); //put the sun at an arm's length

	//compensate
	sunquat2.ReturnConjugate().GetMat(transform_matrix);
	glMultMatrixf(transform_matrix);
	
	//if (reflection)
	//	sunquat.GetMat(transform_matrix);
	//else	
		sunquat.ReturnConjugate().GetMat(transform_matrix);
	glMultMatrixf(transform_matrix);

	cam.dir.ReturnConjugate().GetMat(transform_matrix); //set the sun's orientation to the camera's direction
	glMultMatrixf(transform_matrix);
	
	glColor4f(1.0f, 1.0f, 1.0f, transparency);
	glBindTexture(GL_TEXTURE_2D, sun_texture_ref);
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	//then draw the thing
	glBegin(GL_QUADS);
	for (i = 0; i < 4; i++)
	{
		switch(i)
		{
			case 0:
				glTexCoord2f(0,0);
				break;
			case 1:
				glTexCoord2f(1,0);
				break;
			case 2:
				glTexCoord2f(1,1);
				break;
			case 3:
				glTexCoord2f(0,1);
				break;
		}
		
		glVertex3f(sunquad[i].x, sunquad[i].y, sunquad[i].z);
	}
	glEnd();
	glPopMatrix();
	
	glPopAttrib();
}

void BACKDROP::DrawClouds(double daytime, float transparency, GLdouble *temp_matrix)
{
	//draw the cloud layer
	cloudthresh = weathersystem.GetCloudCover();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	
	glEnable(GL_FOG);
	float oldfog = fogdensity;
	SetFogStrength(fogdensity*300.0f);
	RefreshFog();

	glClear(GL_DEPTH_BUFFER_BIT);
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	//glBlendFunc( GL_SRC_ALPHA, GL_ONE );
	float origalpha = transparency;
	float fadealpha = 1.0f;
	float cloudcolor = sin(sunpos_lat);
	float cloudscale = 20000.0f;//this should be the same as the variable
							   // of the same name in terrain.drawsub() for the
							   // the clouds to line up.
	//the higher the number, the higher the clouds seem.  don't set above 10.
	// in fact, keep it at 1.0, always.
	float cloudrep = 1.0f;
	float cloudoffset_x, cloudoffset_z;
	cloudoffset_x = windx;//10.0f;
	//0.1 is the glscale y factor
	cloudoffset_z = windz;//10.0f;
	cloudcolor += 0.4f;
	if (cloudcolor < 0)
		cloudcolor *= 10.0f; //to simulate sun disappearing behind horizon
	if (cloudcolor < 0.1f)
		cloudcolor = 0.1f;
	if (cloudcolor > 1.0f)
		cloudcolor = 1.0f;
	glColor4f(cloudcolor, cloudcolor, cloudcolor, origalpha);
	glTranslatef(0,-0.4f,0);
	
	/*temp_radius = sqrt(cam.position.x*cam.position.x+cam.position.z*cam.position.z+(cam.position.y+EARTH_RADIUS)*(cam.position.y+EARTH_RADIUS));
	temp_long = atan2((double)(cam.position.y+EARTH_RADIUS),(double)cam.position.x);
	temp_lat = acos(cam.position.z/temp_radius);
	temp_lat -= 1.570796f;
	temp_long -= 1.570796f;
	qz.SetAxisAngle(-temp_long, 0, 0, 1);
	qx.SetAxisAngle(temp_lat, 1, 0, 0);
	qt = qx*qz;
	qt.GetMat(temp_matrix);*/
	glMultMatrixd(temp_matrix);
	
	glScalef(1.0,0.1f,1.0f);
	//glTranslatef(0,-15.0f,0);
	//glScalef(10.0,10.0f,10.0f);
	int num_repeats = 0;
	int old_repeats = 0;
	int num_longitudes = 12;
	int num_lattitudes = 12;
	glBindTexture(GL_TEXTURE_2D, cloud_texture);
	
	VERTEX * oldset = new VERTEX [num_longitudes+1];
	VERTEX * curset = new VERTEX [num_longitudes+1];
	float hem_height = 10.0f;
	float hem_radius = 10.0f;
	int pass, lat, lng;
	QUATERNION qrot, qtemp;
	VERTEX oldrot;
	float safety_factor = 0.0f;
	float pi = 3.141593f;
	glBegin(GL_QUADS);
	for (pass = 0; pass < 2; pass++)
	{
		for (lat = 0; lat <= num_lattitudes; lat++)
		{
			if (pass == 0)
				qrot.SetAxisAngle((pi/2.0f)*((float)lat/(float)num_lattitudes), 0, 1, 0);
			else
				qrot.SetAxisAngle((pi/2.0f)*((float)lat/(float)num_lattitudes), 0, -1, 0);
			VERTEX rotvec;
			rotvec.z = hem_radius;
			rotvec = qrot.RotateVec(rotvec);
			
			for (lng = 0; lng <= num_longitudes; lng++)
			{
				VERTEX currot;
				float curcol[4];
				
				curcol[4] = 1.0f;
				
				//if (pass == 0)
					qtemp.SetAxisAngle(2.0f*pi*((float)lng/(float)num_longitudes), 1, 0, 0);
				//else
				//	qtemp.SetAxisAngle(2.0f*pi*((float)lng/(float)num_longitudes), 0, 0, -1);
				currot = qtemp.RotateVec(rotvec);

				if (lat > 0 && lng > 0)
				{
					float rheight;

					float vtile = 1.0f;
					//float utile = 4.0f;
					
					float texu, texv, cx, cv;
					
					rheight = 1.0f-vtile*currot.x/hem_height;
					
					float fadeterm = 25.0f;
					float alphafactor;
					alphafactor = oldrot.x*oldrot.x+oldrot.z*oldrot.z;
					alphafactor /= (hem_radius*hem_radius-fadeterm);
					if (alphafactor > 1)
						alphafactor = 1.0f;
					alphafactor = 1.0f - alphafactor;
					glColor4f(cloudcolor, cloudcolor, cloudcolor, 
						alphafactor*origalpha+(1.0f-alphafactor)*fadealpha);
					
					//texu = utile*(lng-1)/(float)num_longitudes+cloudoffset_x;
					//texv = rheight+cloudoffset_z;
					cx = oldrot.x;
					cv = oldrot.z;
					texu = (cx/hem_height+cloudoffset_x)*cloudrep+(-cam.position.x/cloudscale)/cloudrep;
					texv = (cv/hem_height+cloudoffset_z)*cloudrep+(-cam.position.z/cloudscale)/cloudrep;
					glTexCoord2f(texu, texv);
					glVertex3f(oldrot.x, oldrot.y-safety_factor, oldrot.z);
					
					alphafactor = currot.x*currot.x+currot.z*currot.z;
					alphafactor /= (hem_radius*hem_radius-fadeterm);
					if (alphafactor > 1)
						alphafactor = 1.0f;
					alphafactor = 1.0f - alphafactor;
					glColor4f(cloudcolor, cloudcolor, cloudcolor, 
						alphafactor*origalpha+(1.0f-alphafactor)*fadealpha);
					
					//texu = utile*(lng)/(float)num_longitudes+cloudoffset_x;
					//texv = rheight+cloudoffset_z;
					cx = currot.x;
					cv = currot.z;
					texu = (cx/hem_height+cloudoffset_x)*cloudrep+(-cam.position.x/cloudscale)/cloudrep;
					texv = (cv/hem_height+cloudoffset_z)*cloudrep+(-cam.position.z/cloudscale)/cloudrep;
					glTexCoord2f(texu, texv);
					glVertex3f(currot.x, currot.y-safety_factor, currot.z);
	
					rheight = 1.0f-vtile*oldset[lng].x/hem_height;

					alphafactor = oldset[lng].x*oldset[lng].x+oldset[lng].z*oldset[lng].z;
					alphafactor /= (hem_radius*hem_radius-fadeterm);
					if (alphafactor > 1)
						alphafactor = 1.0f;
					alphafactor = 1.0f - alphafactor;
					glColor4f(cloudcolor, cloudcolor, cloudcolor, 
						alphafactor*origalpha+(1.0f-alphafactor)*fadealpha);
					
					//texu = utile*(lng)/(float)num_longitudes+cloudoffset_x;
					//texv = rheight+cloudoffset_z;
					cx = oldset[lng].x;
					cv = oldset[lng].z;
					texu = (cx/hem_height+cloudoffset_x)*cloudrep+(-cam.position.x/cloudscale)/cloudrep;
					texv = (cv/hem_height+cloudoffset_z)*cloudrep+(-cam.position.z/cloudscale)/cloudrep;
					glTexCoord2f(texu, texv);
					glVertex3f(oldset[lng].x, oldset[lng].y-safety_factor, oldset[lng].z);
					
					alphafactor = oldset[lng-1].x*oldset[lng-1].x+oldset[lng-1].z*oldset[lng-1].z;
					alphafactor /= (hem_radius*hem_radius-fadeterm);
					if (alphafactor > 1)
						alphafactor = 1.0f;
					alphafactor = 1.0f - alphafactor;
					glColor4f(cloudcolor, cloudcolor, cloudcolor, 
						alphafactor*origalpha+(1.0f-alphafactor)*fadealpha);
					
					//texu = utile*(lng-1)/(float)num_longitudes+cloudoffset_x;
					//texv = rheight+cloudoffset_z;
					cx = oldset[lng-1].x;
					cv = oldset[lng-1].z;
					texu = (cx/hem_height+cloudoffset_x)*cloudrep+(-cam.position.x/cloudscale)/cloudrep;
					texv = (cv/hem_height+cloudoffset_z)*cloudrep+(-cam.position.z/cloudscale)/cloudrep;
					glTexCoord2f(texu, texv);
					glVertex3f(oldset[lng-1].x, oldset[lng-1].y-safety_factor, oldset[lng-1].z);
				}
				
				oldrot = currot;
				curset[lng] = currot;
			}
			
			VERTEX * tptr = curset;
			curset = oldset;
			oldset = tptr;
			old_repeats = num_repeats;
		}

		//hem_height = -hem_height;
		//hem_radius = -hem_radius;
	}
	delete [] curset;
	delete [] oldset;
	glEnd();
	
	/*GLUquadricObj *quadratic;
	quadratic=gluNewQuadric();			// Create A Pointer To The Quadric Object ( NEW )
	gluQuadricNormals(quadratic, GLU_SMOOTH);	// Create Smooth Normals ( NEW )
	gluQuadricTexture(quadratic, GL_TRUE);		// Create Texture Coords ( NEW )
	gluSphere(quadratic,10.0f,32,32);
	gluDeleteQuadric(quadratic);*/
	
	SetFogStrength(oldfog);
	RefreshFog();
	glPopAttrib();
	glPopMatrix();
}

void BACKDROP::DrawStars(double daytime, float transparency, GLdouble *temp_matrix)
{
	//draw the star layer
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	
	glClear(GL_DEPTH_BUFFER_BIT);
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	//glBlendFunc( GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR );
	//glBlendFunc( GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA );
	//glBlendFunc( GL_SRC_ALPHA, GL_ONE );
	//float cloudscale = 20000.0f;//this should be the same as the variable
							   // of the same name in terrain.drawsub() for the
							   // the clouds to line up.
	//the higher the number, the higher the clouds seem.  don't set above 10.
	// in fact, keep it at 1.0, always.
	//float cloudrep = 40.0f;
	//float cloudtile = 10.0f;
	//cloudtile = 2.0f;
	//cloudscale = 1000.0f;
	float fogcolor[4];
	GetSkyColor(fogcolor);
	glColor4f(1.0,1.0,1.0,1.0-fogcolor[3]);
	glTranslatef(0,-0.4f,0);
	
	/*temp_radius = sqrt(cam.position.x*cam.position.x+cam.position.z*cam.position.z+(cam.position.y+EARTH_RADIUS)*(cam.position.y+EARTH_RADIUS));
	temp_long = atan2((double)(cam.position.y+EARTH_RADIUS),(double)cam.position.x);
	temp_lat = acos(cam.position.z/temp_radius);
	temp_lat -= 1.570796f;
	temp_long -= 1.570796f;
	qz.SetAxisAngle(-temp_long, 0, 0, 1);
	qx.SetAxisAngle(temp_lat, 1, 0, 0);
	qt = qx*qz;
	qt.GetMat(temp_matrix);*/
	glMultMatrixd(temp_matrix);
	
	//glScalef(1.0,0.1f,1.0f);
	glScalef(1.0,1.0f,1.0f);
	
	glBindTexture(GL_TEXTURE_2D, stars_texture);
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glScalef(10.0f,10.0f,10.0f);
	
	glMatrixMode(GL_MODELVIEW);
	glRotatef(90,1,0,0);
	GLUquadricObj *quadratic;
	quadratic=gluNewQuadric();
	gluQuadricTexture(quadratic, GL_TRUE);
	gluSphere(quadratic,1.0f,32,32);
	gluDeleteQuadric(quadratic);
	
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	
	//glTranslatef(0,-15.0f,0);
	//glScalef(10.0,10.0f,10.0f);
	/*int num_repeats = 0;
	int old_repeats = 0;
	int num_longitudes = 12;
	int num_lattitudes = 12;
	glBindTexture(GL_TEXTURE_2D, stars_texture);
	float cloudoffset_x, cloudoffset_z;
	cloudoffset_x = cloudoffset_z = 0.0f;
	VERTEX * oldset = new VERTEX [num_longitudes+1];
	VERTEX * curset = new VERTEX [num_longitudes+1];
	float hem_height = 10.0f;
	float hem_radius = 10.0f;
	int pass, lat, lng;
	QUATERNION qrot, qtemp;
	VERTEX oldrot;
	float safety_factor = 0.0f;
	float pi = 3.141593f;
	glBegin(GL_QUADS);
	for (pass = 0; pass < 2; pass++)
	{
		for (lat = 0; lat <= num_lattitudes; lat++)
		{
			if (pass == 0)
				qrot.SetAxisAngle((pi/2.0f)*((float)lat/(float)num_lattitudes), 0, 1, 0);
			else
				qrot.SetAxisAngle((pi/2.0f)*((float)lat/(float)num_lattitudes), 0, -1, 0);
			VERTEX rotvec;
			rotvec.z = hem_radius;
			rotvec = qrot.RotateVec(rotvec);
			
			for (lng = 0; lng <= num_longitudes; lng++)
			{
				VERTEX currot;
				float curcol[4];
				
				curcol[4] = 1.0f;
				
				//if (pass == 0)
					qtemp.SetAxisAngle(2.0f*pi*((float)lng/(float)num_longitudes), 1, 0, 0);
				//else
				//	qtemp.SetAxisAngle(2.0f*pi*((float)lng/(float)num_longitudes), 0, 0, -1);
				currot = qtemp.RotateVec(rotvec);

				if (lat > 0 && lng > 0)
				{
					float rheight;

					float vtile = 1.0f;
					//float vtile = 0.5f;
					//float utile = 4.0f;
					
					float texu, texv, cx, cv;
					
					rheight = 1.0f-vtile*currot.x/hem_height;
					
					float fadeterm = 25.0f;
					float alphafactor;
					alphafactor = oldrot.x*oldrot.x+oldrot.z*oldrot.z;
					alphafactor /= (hem_radius*hem_radius-fadeterm);
					if (alphafactor > 1)
						alphafactor = 1.0f;
					alphafactor = 1.0f - alphafactor;
					//glColor4f(cloudcolor, cloudcolor, cloudcolor, 
						//alphafactor*origalpha+(1.0f-alphafactor)*fadealpha);
					
					//texu = utile*(lng-1)/(float)num_longitudes+cloudoffset_x;
					//texv = rheight+cloudoffset_z;
					cx = oldrot.x;
					cv = oldrot.z;
					texu = (cx/hem_height+cloudoffset_x)*cloudtile+(-cam.position.x/cloudscale)/cloudtile;
					texv = (cv/hem_height+cloudoffset_z)*cloudtile+(-cam.position.z/cloudscale)/cloudtile;
					glTexCoord2f(texu, texv);
					glVertex3f(oldrot.x, oldrot.y-safety_factor, oldrot.z);
					
					alphafactor = currot.x*currot.x+currot.z*currot.z;
					alphafactor /= (hem_radius*hem_radius-fadeterm);
					if (alphafactor > 1)
						alphafactor = 1.0f;
					alphafactor = 1.0f - alphafactor;
					//glColor4f(cloudcolor, cloudcolor, cloudcolor, 
						//alphafactor*origalpha+(1.0f-alphafactor)*fadealpha);
					
					//texu = utile*(lng)/(float)num_longitudes+cloudoffset_x;
					//texv = rheight+cloudoffset_z;
					cx = currot.x;
					cv = currot.z;
					texu = (cx/hem_height+cloudoffset_x)*cloudtile+(-cam.position.x/cloudscale)/cloudtile;
					texv = (cv/hem_height+cloudoffset_z)*cloudtile+(-cam.position.z/cloudscale)/cloudtile;
					glTexCoord2f(texu, texv);
					glVertex3f(currot.x, currot.y-safety_factor, currot.z);
	
					rheight = 1.0f-vtile*oldset[lng].x/hem_height;

					alphafactor = oldset[lng].x*oldset[lng].x+oldset[lng].z*oldset[lng].z;
					alphafactor /= (hem_radius*hem_radius-fadeterm);
					if (alphafactor > 1)
						alphafactor = 1.0f;
					alphafactor = 1.0f - alphafactor;
					//glColor4f(cloudcolor, cloudcolor, cloudcolor, 
						//alphafactor*origalpha+(1.0f-alphafactor)*fadealpha);
					
					//texu = utile*(lng)/(float)num_longitudes+cloudoffset_x;
					//texv = rheight+cloudoffset_z;
					cx = oldset[lng].x;
					cv = oldset[lng].z;
					texu = (cx/hem_height+cloudoffset_x)*cloudtile+(-cam.position.x/cloudscale)/cloudtile;
					texv = (cv/hem_height+cloudoffset_z)*cloudtile+(-cam.position.z/cloudscale)/cloudtile;
					glTexCoord2f(texu, texv);
					glVertex3f(oldset[lng].x, oldset[lng].y-safety_factor, oldset[lng].z);
					
					alphafactor = oldset[lng-1].x*oldset[lng-1].x+oldset[lng-1].z*oldset[lng-1].z;
					alphafactor /= (hem_radius*hem_radius-fadeterm);
					if (alphafactor > 1)
						alphafactor = 1.0f;
					alphafactor = 1.0f - alphafactor;
					//glColor4f(cloudcolor, cloudcolor, cloudcolor, 
						//alphafactor*origalpha+(1.0f-alphafactor)*fadealpha);
					
					//texu = utile*(lng-1)/(float)num_longitudes+cloudoffset_x;
					//texv = rheight+cloudoffset_z;
					cx = oldset[lng-1].x;
					cv = oldset[lng-1].z;
					texu = (cx/hem_height+cloudoffset_x)*cloudtile+(-cam.position.x/cloudscale)/cloudtile;
					texv = (cv/hem_height+cloudoffset_z)*cloudtile+(-cam.position.z/cloudscale)/cloudtile;
					glTexCoord2f(texu, texv);
					glVertex3f(oldset[lng-1].x, oldset[lng-1].y-safety_factor, oldset[lng-1].z);
				}
				
				oldrot = currot;
				curset[lng] = currot;
			}
			
			VERTEX * tptr = curset;
			curset = oldset;
			oldset = tptr;
			old_repeats = num_repeats;
		}

		//hem_height = -hem_height;
		//hem_radius = -hem_radius;
	}
	delete [] curset;
	delete [] oldset;
	glEnd();*/
	
	/*GLUquadricObj *quadratic;
	quadratic=gluNewQuadric();			// Create A Pointer To The Quadric Object ( NEW )
	gluQuadricNormals(quadratic, GLU_SMOOTH);	// Create Smooth Normals ( NEW )
	gluQuadricTexture(quadratic, GL_TRUE);		// Create Texture Coords ( NEW )
	gluSphere(quadratic,10.0f,32,32);
	gluDeleteQuadric(quadratic);*/
	
	glPopAttrib();
	glPopMatrix();
}

void BACKDROP::RefreshFog()
{
	//set up fog
	float fogcolor[4];
	GetHorizonColor(fogcolor);
	fogcolor[3] = 0.0f;
	
	//desaturate fog based on cloud cover
	float colmean = (fogcolor[0] + fogcolor[1] + fogcolor[2])/3.0f;
	float cloudiness;
	if (cloudthresh >= 0)
		cloudiness = 0.0f;
	else
		cloudiness = -(cloudthresh/100.0f); //decrease 50.0f to make the fog
											//desaturate faster
	
	//clamp
	if (cloudiness > 1.0f)
		cloudiness = 1.0f;
	if (cloudiness < 0.0f)
		cloudiness = 0.0f;
	
	int i;
	for (i = 0; i < 3; i++)
	{
		fogcolor[i] = fogcolor[i]*(1.0f-cloudiness) + colmean*cloudiness;
	}
	
	glFogfv(GL_FOG_COLOR, fogcolor);
	glFogf(GL_FOG_DENSITY, fogdensity);
	glFogi(GL_FOG_MODE, GL_EXP);
	glEnable(GL_FOG);
}

struct pix24
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

struct pix32
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};

void BACKDROP::GetHorizonColor(float * color3)
{
	/*color3[0] = 0.53f;
	color3[1] = 0.74f;
	color3[2] = 0.91f;*/
	
	struct pix32 * pix;
		
	pix = (struct pix32 *) sky_tex_img->pixels;
	
	float dx;
	int x;
	int sky_tex_size = 16;
	dx = (daytime_cache*sky_tex_size);
	x = (int)dx;
	dx = dx - x;
	x %= sky_tex_size;
	int idx1 = x + 15*sky_tex_size;
	x++;
	x %= sky_tex_size;
	int idx2 = x + 15*sky_tex_size;
	
	color3[0] = (1.0f-dx)*pix[idx1].r/255.0f+dx*pix[idx2].r/255.0f;
	color3[1] = (1.0f-dx)*pix[idx1].g/255.0f+dx*pix[idx2].g/255.0f;
	color3[2] = (1.0f-dx)*pix[idx1].b/255.0f+dx*pix[idx2].b/255.0f;
	color3[3] = (1.0f-dx)*pix[idx1].a/255.0f+dx*pix[idx2].a/255.0f;
}

void BACKDROP::GetSkyColor(float * color3)
{
	/*color3[0] = 0.53f;
	color3[1] = 0.74f;
	color3[2] = 0.91f;*/
	
	struct pix32 * pix;
	
	pix = (struct pix32 *) sky_tex_img->pixels;
	
	float dx;
	int x;
	int sky_tex_size = 16;
	dx = (daytime_cache*sky_tex_size);
	x = (int)dx;
	dx = dx - x;
	x %= sky_tex_size;
	//int idx1 = x + 15*sky_tex_size;
	int idx1 = x;
	x++;
	x %= sky_tex_size;
	//int idx2 = x + 15*sky_tex_size;
	int idx2 = x;
	
	color3[0] = (1.0f-dx)*pix[idx1].r/255.0f+dx*pix[idx2].r/255.0f;
	color3[1] = (1.0f-dx)*pix[idx1].g/255.0f+dx*pix[idx2].g/255.0f;
	color3[2] = (1.0f-dx)*pix[idx1].b/255.0f+dx*pix[idx2].b/255.0f;
	color3[3] = (1.0f-dx)*pix[idx1].a/255.0f+dx*pix[idx2].a/255.0f;
}

void BACKDROP::SetFogStrength(float density)
{
	fogdensity = density;
	glFogf(GL_FOG_DENSITY, fogdensity);
}

void BACKDROP::SetWindoffset(float windoffset_x, float windoffset_z)
{
	windx = windoffset_x;
	windz = windoffset_z;
}

void BACKDROP::SetCloudCover(int newcloudthresh)
{
	cloudthresh = newcloudthresh;
}

void BACKDROP::DrawRain(float day_time)
{
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glDisable(GL_FOG);
	glDisable(GL_LIGHTING);
	glTranslatef(0,0,-40.0f);
	//glBlendFunc(GL_ONE, GL_SRC_ALPHA)
	glBindTexture(GL_TEXTURE_2D, rain_texture);
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	glBegin(GL_TRIANGLES);
	
	VERTEX pos;
	float alpha;
	int numdrops = weathersystem.GetRainDrops();
	int i;
	
	VERTEX up, forward, right;
	up.y = 1.0f;
	up = cam.dir.RotateVec(up);
	up.y = sin(up.y);
	forward.z = 1.0f;
	forward = cam.dir.RotateVec(forward);
	if ((forward.y >= 0 && forward.z >= 0) || (forward.y < 0 && forward.z < 0))
		forward.y = 1.0f;
	else
		forward.y = -1.0f;
	
	float randperm;
	float lum;
	
	float dayout = day_time;
	float daythresh = 0.1f;
	if (dayout >= 0.5)
		dayout = 0.0f;
	if (dayout < 0.5f && dayout > 0.5f - daythresh)
		dayout = 0.5-dayout;
	dayout /= daythresh;
	if (dayout > 1.0f)
		dayout = 1.0f;
	dayout = dayout*0.75f+0.25f;
	
	for (i = 0; i < numdrops; i++)
	{
		pos.x = (rand()/(float)RAND_MAX-0.5f)*50.0f;
		pos.y = (rand()/(float)RAND_MAX-0.5f)*40.0f;
		pos.z = (rand()/(float)RAND_MAX);
		alpha = (rand()/(float)RAND_MAX)/4.0f + 0.2f;
		lum = (rand()/(float)RAND_MAX)/2.0f + 0.5f;
		lum *= dayout;
		glColor4f(lum,lum,lum,alpha);
		randperm = (rand()/(float)RAND_MAX-0.5f)*1.0f;
		DrawDrop(pos, randperm, up.y, forward.y);
	}
	
	glEnd();
	glPopAttrib();
	glPopMatrix();
}

void BACKDROP::DrawDrop(VERTEX pos, float rndperm, float yscale, float zheight)
{
	float dropwidth = 0.25f;
	bool negy;
	if (zheight >= 0)
		negy = false;
	else
		negy = true;
	
	float dropheight = 10.0f*pos.z*yscale;
	if (dropheight < 0.5f)
		dropheight = 0.5f;
	float xoffset;
	if (negy)
		xoffset = -(1.0f-yscale)*0.2f*pos.x*pos.z;
	else
		xoffset = (1.0f-yscale)*0.2f*pos.x*pos.z;
	
	glTexCoord2f(0,0);
	glVertex3f(pos.x-dropwidth, pos.y+dropheight, 0.0f);
	glTexCoord2f(0,1);
	glVertex3f(pos.x-dropwidth+rndperm+xoffset, pos.y-dropheight, 0.0f);
	glTexCoord2f(1,1);
	glVertex3f(pos.x+dropwidth+rndperm+xoffset, pos.y-dropheight, 0.0f);
}

void BACKDROP::DrawMoon(double daytime, float transparency, GLdouble *temp_matrix)
{
	int i;
	float hem_radius = 10.0f;
	float hem_height = hem_radius;
	
	//okay, now plop on the MOON
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glEnable(GL_TEXTURE_2D);
	//glEnable(GL_BLEND);
	glClear(GL_DEPTH_BUFFER_BIT);
	float sunsize = 0.3f;
	/*float reflection_elongation_y = 1.0f; //how much the sun is stretched by in
										// the reflection rendering
	float reflection_elongation_x = 1.0f;*/
	float sunsize_x = sunsize;
	float sunsize_y = sunsize;
	
	VERTEX sunquad[4];
	for (i = 0; i < 4; i++)	//generate the sun quad
	{
		if (i == 0 || i == 3)
		{
			sunquad[i].y = -sunsize_y;
		}
		else
		{
			sunquad[i].y = sunsize_y; //only elongate in -y direction
		}
		if (i < 2)
			sunquad[i].x = -sunsize_x;
		else
			sunquad[i].x = sunsize_x;
	}
	QUATERNION sunquat, sunquat2;
	//float sunheight = atan2(LightPosition[1],LightPosition[2]); //sun angular height in radians
	float transform_matrix[16];
	glPushMatrix();
	glMultMatrixd(temp_matrix);
	
	//float sundir = atan2(LightPosition[0],LightPosition[2]);
	sunquat.SetAxisAngle(-moonpos_lng, 0, 1, 0);
	sunquat.GetMat(transform_matrix); //crank it around the sky
	glMultMatrixf(transform_matrix);
	
	sunquat2.SetAxisAngle(moonpos_lat, 0, 0, 1);
	sunquat2.GetMat(transform_matrix); //crank it up into the sky
	glMultMatrixf(transform_matrix);
	
	glTranslatef(hem_height,0,0); //put the sun at an arm's length

	//compensate
	sunquat2.ReturnConjugate().GetMat(transform_matrix);
	glMultMatrixf(transform_matrix);
	
	//if (reflection)
	//	sunquat.GetMat(transform_matrix);
	//else	
		sunquat.ReturnConjugate().GetMat(transform_matrix);
	glMultMatrixf(transform_matrix);

	cam.dir.ReturnConjugate().GetMat(transform_matrix); //set the sun's orientation to the camera's direction
	glMultMatrixf(transform_matrix);
	
	glColor4f(1.0f, 1.0f, 1.0f, transparency);
	glBindTexture(GL_TEXTURE_2D, moon_texture);
	//glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	//then draw the thing
	glBegin(GL_QUADS);
	for (i = 0; i < 4; i++)
	{
		switch(i)
		{
			case 0:
				glTexCoord2f(0,0);
				break;
			case 1:
				glTexCoord2f(1,0);
				break;
			case 2:
				glTexCoord2f(1,1);
				break;
			case 3:
				glTexCoord2f(0,1);
				break;
		}
		
		glVertex3f(sunquad[i].x, sunquad[i].y, sunquad[i].z);
	}
	glEnd();
	glPopMatrix();
	
	glPopAttrib();
}
