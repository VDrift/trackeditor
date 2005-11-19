#include "terrain.h"

//#define PERFORMANCE_PROFILE

#ifdef PERFORMANCE_PROFILE
#include <sys/time.h>
extern suseconds_t GetMicroSeconds();
#endif

//string TERRAINTEXTURE_PATH = settings.GetDataDir() + "/terraintex";
//string TERRAIN_PATH = settings.GetDataDir() + "/terrain";

#define ROAM_ALLIN 0x3f

//because sub loading screens are nice.
extern void LoadingScreen(string loadtext);

TERRAIN::TERRAIN()
{
	error_log.open((settings.GetSettingsDir() + "/logs/terrain.log").c_str());
	
	heightarray = NULL;
	initdone = false;
}

unsigned short swap_bytes(unsigned short indat)
{
	int bfront = indat / 256;
	int brear = indat-bfront*256;
	return brear*256+bfront;
}

void TERRAIN::DeInit()
{
	if (initdone)
	{
		glDeleteTextures(3, texture_handle);
		SDL_FreeSurface(height_data);
		SDL_FreeSurface(height_mask);
		delete [] heightarray;
		
		initdone = false;
	}
}

void TERRAIN::Init(int newsize_x, int newsize_y, int newsize_z, string datafile, string datamaskfile, string texfile, string detailtexfile, string detailtexfile2, string harray, float offsetx, float offsetz, float fadefrom, float fadeto)
{
//	int i;
	
	roam_lmax=ROAM_LMAX;
	
	offset.x = offsetx;
	offset.z = offsetz;
	offset.y = 0;
	
	size_x = newsize_x;
	size_y = newsize_y;
	size_z = newsize_z;
	
	PARAM_FADEFROM = fadefrom;
	PARAM_FADETO = fadeto;
	
	LoadingScreen("Loading...\nLoading terrain\nLoading Heightmaps");
	
	string filepath;
	char buffer[1024];
	
	//filepath = TERRAIN_PATH + "/" + datafile;
	filepath = datafile;
	strcpy(buffer, filepath.c_str());
	// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
	if ((height_data=IMG_Load(buffer)))
	{
	}
	else
	{
		//quit, bitmap not found
		error_log << "Could not find terrain height data: " << buffer << "\n";
	}
	
	//filepath = TERRAIN_PATH + "/" + datamaskfile;
	filepath = datamaskfile;
	strcpy(buffer, filepath.c_str());
	// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
	if ((height_mask=IMG_Load(buffer)))
	{
	}
	else
	{
		//quit, bitmap not found
		error_log << "Could not find terrain mask data: " << buffer << "\n";
	}
	
	/*//LOAD TERRAIN HEIGHTMAP
	filepath = TERRAIN_PATH + "/" + datafile;
	strcpy(buffer, filepath.c_str());

	// Load The terrain heightmap in raw format
	FILE * infile = fopen(buffer, "rb");
	if (infile != NULL)
	{
		//read in raw data
		fread (height_data, 2, H_DATA_SIZE, infile);

		//flip endian-ness
		for (i = 0; i < H_DATA_SIZE; i++)
		{
			//cout << height_data[i] << endl;
			height_data[i] = swap_bytes(height_data[i]);
			//unsigned short converted = height_data[i];
			//char * b = (char *)&converted;
			//height_data[i] = ((unsigned short)b[0] << 8) + b[1];
		}

		fclose(infile);
	}
	else
	{
		error_log << "Could not find terrain height data: " << buffer << "\n";
	}*/
	
	
	
	/*//LOAD TERRAIN HEIGHTMAP MASK
	filepath = TERRAIN_PATH + "/" + datamaskfile;
	strcpy(buffer, filepath.c_str());

	// Load The terrain heightmap in raw format
	infile = fopen(buffer, "rb");
	if (infile != NULL)
	{
		//read in raw data
		fread (height_mask, 2, H_DATA_SIZE, infile);

		//flip endian-ness
		for (i = 0; i < H_DATA_SIZE; i++)
		{
			//cout << height_data[i] << endl;
			height_mask[i] = swap_bytes(height_mask[i]);
			//unsigned short converted = height_data[i];
			//char * b = (char *)&converted;
			//height_data[i] = ((unsigned short)b[0] << 8) + b[1];
		}

		fclose(infile);
	}
	else
	{
		error_log << "Could not find terrain height mask: " << buffer << "\n";
	}*/
	
	
	
	
	LoadingScreen("Loading...\nLoading terrain\nLoading Textures");
	
	
	//LOAD TERRAIN TEXTURE
	texture_handle[0] = utility.TexLoad(texfile, GL_RGB, true);
	texture_handle[1] = utility.TexLoad(detailtexfile, GL_RGB, true);
	texture_handle[2] = utility.TexLoad(detailtexfile2, GL_RGB, true);
	
	//TexLoad(1, "forestfloor.png", GL_RGBA, true);
	//TexLoad(1, "detail.png", GL_LUMINANCE, true);
	//TexLoad(1, "grass2.png", GL_RGBA, true);
	//TexLoad(1, "grass2.png", GL_RGBA, true);
	
	//TexLoad(2, "cloudmap2.png", GL_RGB, true);
	
	//TexLoad(2, "cloudmap2.tga", GL_RGBA, true);
	//TexLoad(2, "cloudsraw.tga", GL_RGBA, true);
	
	//TexLoad(3, "cloudmap3.png", GL_RGB, true);
	//TexLoad(3, "cloudsraw.tga", GL_RGBA, true);
	
	//TexLoad(4, "white.png", GL_RGB, true);
	
	//TexLoad(5, "wetmap.png", GL_RGB, true);
	//TexLoad(5, "watergrad.png", GL_RGB, true);
	//TexLoad(5, "watergrad.png", GL_RGBA, false);
	
	//TexLoad(2, "grass2-darker.png", GL_RGBA, true);
	//TexLoad(2, "fcarpet.png", GL_RGBA, true);
	//TexLoad(2, "carpet2.png", GL_RGBA, true);
	
	//TexLoad(3, "waterold.png", GL_RGB, true);
	//TexLoad(4, "water.png", GL_RGB, true);
	
	//TexLoad(3, "water.bmp", GL_LUMINANCE, true);
	//TexLoad(4, "waterold.bmp", GL_BGR, true);
	
	
	
	
	
	/* generate table of displacement sizes versus level */
    {
        int l;

        for (l=0;l<=ROAM_LMAX;l++)
		{
			level2dzsize[l]=(100.0f)*(0.3f/sqrt((float)((roam_int64)1<<l)));
			//level2dzsize[l]=(100.0f)*(0.3f/sqrt(pow(2.0,(double) l*1.0)));
			
			//level2dzsize_water[l]=6.0f/sqrt(pow(2.0,(double) l*0.9));
			
			//level2dzsize_water[l]=120.0f/sqrt(pow(2.0,(double) l*1.1));
			//level2dzsize_water[l]=30.0f/sqrt(pow(2.0,(double) l*1.0));
		}
    }
	
	/*
	// Create a pointer to store the blank image data
    unsigned int *pTexture = NULL;                                            

    // We need to create a blank texture to render our dynamic texture to.
    // To do this, we just create an array to hold the data and then give this
    // array to OpenGL.  The texture is stored on the video card so we can
    // destroy the array immediately afterwards.
    // This function takes the texture array to store the texture, the
    // size of the texture for width and the heigth, the channels (1, 3 or 4),
    // the type (LUMINANCE, RGB, RGBA, etc..) and the texture ID to assign it too.

    // Allocate and init memory for the image array and point to it from pTexture
	unsigned int sizex, sizey;
	sizex = reflect_res_x;
	sizey = reflect_res_y;
    pTexture = new unsigned int [sizex * sizey * 3];
    memset(pTexture, 0, sizex * sizey * 3 * sizeof(unsigned int));    

    // Register the texture with OpenGL and bind it to the texture ID
    glGenTextures(1, &reflection_texture);                                
    glBindTexture(GL_TEXTURE_2D, reflection_texture);                    
    
    // Create the texture and store it on the video card
    glTexImage2D(GL_TEXTURE_2D, 0, 3, sizex, sizey, 0, GL_RGB, GL_UNSIGNED_INT, pTexture);
    
    // Set the texture quality
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    // Since we stored the texture space with OpenGL, we can delete the image data
    delete [] pTexture;
	*/
	
	
	
	/*LoadingScreen("Loading...\nLoading terrain\nGenerating Noise");
	
	//create water bump texture(s)
	float offs[OFFSET_TEX_SIZE*OFFSET_TEX_SIZE*OFFSET_OCTAVES];
	
	srand(0);
	NoiseInit();
	
	float weights[OFFSET_OCTAVES];
	
	noise_step = 0;
	
	for (i = 0; i < OFFSET_OCTAVES; i++)
		weights[i] = 1.0f;
	weights[0] = 0.0f;
	
	if (OFFSET_OCTAVES == 8)
	{
		weights[0] = 0.0f;//used to be 0.5
		weights[1] = 0.5f;
		weights[2] = 0.5f;
		weights[3] = 0.7f;
		weights[4] = 1.0f;
		weights[5] = 2.0f;
		weights[6] = 2.0f;
		weights[7] = 1.0f;
	}
	
	
	for (i = 0; i < OFFSET_OCTAVES; i++)
	{
		//cout << i << ", " << powl(2,i) << ", " << OFFSET_TEX_SIZE*OFFSET_TEX_SIZE*i << endl;
		GenNoise(offs+OFFSET_TEX_SIZE*OFFSET_TEX_SIZE*i, (int) powl(2,i), 0, OFFSET_OCTAVES, weights, i);
	}
	
	CombineMaps(combomap_static, offs, offset_anim_step, OFFSET_OCTAVES);*/
	
	/*
	LoadingScreen("Loading...\nLoading terrain\nLoading Clouds");
	
	//load up cloud resources
	filepath = TERRAINTEXTURE_PATH + "/" + "cloudsraw.tga";
	strcpy(buffer, filepath.c_str());
	if ((sdl_cloud_map=IMG_Load(buffer)))
	{
		sdl_cloud_map_original = IMG_Load(buffer);
	}
	else
		error_log << "Couldn't find texture: " << buffer << endl;
	
	//construct cloud texture from source texture that was loaded in
	int cloudsize = sdl_cloud_map->h*sdl_cloud_map->w;
	GLbyte *pin, *pout;;
	int pcount;
	for (i = 0; i < cloudsize; i++)
	{
		pin = &(((GLbyte *) sdl_cloud_map_original->pixels)[i*4]);
		pout = &(((GLbyte *) sdl_cloud_map->pixels)[i*4]);
		
		for (pcount = 0; pcount < 4; pcount++)
		{
			pout[pcount] = pin[pcount]+128;
		}
		
		//cout << (int) pout[3] << ", ";
	}
	//cout << endl;
	
	JGL_SELTEX(GL_TEXTURE0_ARB);
	glGenTextures(1, &cloud_tex);
	glBindTexture(GL_TEXTURE_2D, cloud_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sdl_cloud_map->w, sdl_cloud_map->h, 0, GL_RGBA,
			 GL_BYTE, sdl_cloud_map->pixels);
	//texture settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //maybe mipmap this?
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	//construct cloud shadow texture
	JGL_SELTEX(GL_TEXTURE3_ARB);
	glGenTextures(1, &cloud_map);
	glBindTexture(GL_TEXTURE_2D, cloud_map);
	
	GLbyte cloud_map_data[cloudsize];
	GLbyte ptemp;
	for (i = 0; i < cloudsize; i++)
	{
		ptemp = ((GLbyte *) sdl_cloud_map->pixels)[i*4+3];
		//cout << (int) ptemp << ", ";
		cloud_map_data[i] = ptemp;
	}
	//cout << endl;
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE8, sdl_cloud_map->w, sdl_cloud_map->h, 0, GL_LUMINANCE,
			 GL_BYTE, &cloud_map_data);
	//texture settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //maybe mipmap this?
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	LoadingScreen("Loading...\nLoading terrain\nGenerating Water map");
	//new combomap
	if (OFFSET_ANIMATION)
	{
		GLbyte combomap[OFFSET_TEX_SIZE*OFFSET_TEX_SIZE];
		float * mh = water.GetHeightMapPtr();
		for (i = 0; i < OFFSET_TEX_SIZE*OFFSET_TEX_SIZE; i++)
		{
			combomap[i] = (signed int) (mh[i*2]*5.0f);
		}
		BuildOffsetMap(offsetmap, combomap);
	}
	else
	{	
		//if doing a static, one time texture, do all of this junk too
		
		//size up water
		//cout << "resizing water" << endl;
		water.resizeWater(OFFSET_TEX_SIZE,OFFSET_TEX_SIZE);
		
		GLbyte combomap[OFFSET_TEX_SIZE*OFFSET_TEX_SIZE];
		float * mh = water.GetHeightMapPtr();
		for (i = 0; i < OFFSET_TEX_SIZE*OFFSET_TEX_SIZE; i++)
		{
			//combomap[i] = (signed char) (mh[i*2]*4.0f*(OFFSET_TEX_SIZE/64.0f));
			combomap[i] = (unsigned char) (mh[i*2]*2.0f+128);
		}
		BuildOffsetMap(offsetmap, combomap);
		
		int debug = 0;
		
		//DEBUG STUFF
		if (debug)
		{
			FILE * of;
			of = fopen("water.raw", "wb");
			//unsigned char conv;
			fwrite(combomap, sizeof(GLbyte), OFFSET_TEX_SIZE*OFFSET_TEX_SIZE, of);
		
			fclose(of);
		}
		
		//cout << camfor.x << "," << camfor.z << endl;
		float bl;
		float blmax = 0;
		for (i = 0; i < OFFSET_TEX_SIZE*OFFSET_TEX_SIZE; i++)
		{
			bl = (int)offsetmap[i*2] + (int)offsetmap[i*2+1];
			if (bl > blmax)
				blmax = bl;
		}
		for (i = 0; i < OFFSET_TEX_SIZE*OFFSET_TEX_SIZE; i++)
		{
			//less simplified model
			bl = (int)offsetmap[i*2] + (int)offsetmap[i*2+1];
			float maxfactor = 15.0f; //for 512x512
			bl = maxfactor*bl/blmax;
			//if (bl > 0)
			//	bl = -bl;
			bl = bl - maxfactor;
			//alphamap[i] = (signed char) (bl/10.0f+114);
			alphamap[i] = (signed char) (127+bl);//(signed char) (bl/5.0f+100);
			
			//less simplified model
		
			//bl = maxfactor*bl/blmax;
			//if (bl > 0)
			//	bl = -bl;
			//bl = bl - maxfactor;
			//alphamap[i] = (signed char) (bl/10.0f+114);
			//alphamap[i] = (signed char) (127+bl);//(signed char) (bl/5.0f+100);
			
			//simplified model
			//alphamap[i] = (offsetmap[i*2]+100);
		}
		
		if (debug)
		{
			FILE * of;
			of = fopen("water-trans.raw", "wb");
			fwrite(alphamap, sizeof(GLbyte), OFFSET_TEX_SIZE*OFFSET_TEX_SIZE, of);
		
			fclose(of);
		}
		
		//upload
		#ifdef _WIN32
		pglActiveTexture(GL_TEXTURE0_ARB);
		#else
		glActiveTextureARB(GL_TEXTURE0_ARB);
		#endif
		glGenTextures(1, &offset_tex);
		glBindTexture(GL_TEXTURE_2D, offset_tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DSDT8_NV, OFFSET_TEX_SIZE, OFFSET_TEX_SIZE, 0, GL_DSDT_NV,
				 GL_BYTE, &offsetmap);
		//offset texture settings
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		//gluBuild2DMipmaps( GL_TEXTURE_2D, 1, OFFSET_TEX_SIZE, OFFSET_TEX_SIZE, GL_DSDT_NV, GL_BYTE, &offsetmap);
		
		//reinit water with smaller size
		water.resizeWater(64,64);
	}
	
	if (!STATIC_WATERDETAIL)
	{
		JGL_SELTEX(GL_TEXTURE2_ARB);
		glGenTextures(1, &water_alpha_map);
		glBindTexture(GL_TEXTURE_2D, water_alpha_map);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA8, OFFSET_TEX_SIZE, OFFSET_TEX_SIZE, 0, GL_ALPHA,
				 //GL_BYTE, &alphamap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE8, OFFSET_TEX_SIZE, OFFSET_TEX_SIZE, 0, GL_LUMINANCE,
				 GL_BYTE, &alphamap);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE8, OFFSET_TEX_SIZE, OFFSET_TEX_SIZE, 0, GL_LUMINANCE,
		//		 GL_BYTE, &alphamap);
		//offset alpha texture settings
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		//gluBuild2DMipmaps( GL_TEXTURE_2D, 1, OFFSET_TEX_SIZE, OFFSET_TEX_SIZE, GL_ALPHA, GL_UNSIGNED_BYTE, &alphamap);
	}
	else
	{
		//water_alpha_map = TexLoad("water-trans.png", GL_ALPHA, true);
		water_alpha_map = TexLoad("water-trans.png", GL_LUMINANCE, true);
		//water_alpha_map = TexLoad("water-trans.png", GL_RGB, false);
	}
	

	windoffset_x = windoffset_z = 0.0f;
	offset_anim_step = 0.0f; //unused
	*/
	
	LoadingScreen("Loading...\nLoading terrain\nParameterized surface");
	
	//LOAD TERRAIN HEIGHTMAP FOR SPLINE
	//filepath = TERRAIN_PATH + "/" + harray;
	filepath = harray;
	strcpy(buffer, filepath.c_str());
	SDL_Surface * htemp;
	// Load The terrain heightmap in raw format
	if ((htemp=IMG_Load(buffer)))
	{
	}
	else
	{
		//quit, bitmap not found
		error_log << "Could not find terrain height data: " << buffer << "\n";
	}
	
	if (htemp->format->BytesPerPixel != 1)
		error_log << "Terrain height data is not greyscale no-alpha: " << buffer << endl;
	
	heightarray = new float[htemp->w*htemp->h];
	
	unsigned char * ha = (unsigned char *) htemp->pixels;
	
	int x, y;
	int count = 0;
	for (y = 0; y < htemp->h; y++)
	{
		//for (x = 0; x < htemp->h; x++)
		for (x = 0; x < htemp->pitch; x++)
		{
			//heightarray[x+htemp->w*y] = (float)(((unsigned char *) htemp->pixels)[x+htemp->w*y])/255.0;
			if (x < htemp->w)
			{
				//heightarray[x+htemp->w*y] = ((float)(ha[x+htemp->w*y]))/255.0;
				heightarray[count] = ((float)(ha[x+htemp->pitch*y]))/255.0;
				count++;
			}
			
			
			//heightarray[i] = ((float)((char *) htemp->pixels)[i*3]+128.0)/256.0;
			
			if (y == 50)
			{
				//cout << heightarray[x+htemp->w*y] << endl;
				//cout << (int)(((unsigned char*)htemp->pixels)[x+htemp->w*y]) << endl;
				//cout << heightarray[count] << endl;
			}
		}
	}
	
	//cout << htemp->pitch << " " << htemp->w << "," << htemp->h << endl;
	
	hax = htemp->w;
	hay = htemp->h;
	
	if ((hax-1) % 3 != 0 || (hay-1) % 3 != 0)
		error_log << "Parametric height data must be sized to a multiple of 3, + 1: " << buffer << endl;
	
	FixHeightArray();
	
	SDL_FreeSurface(htemp);
	
	LoadingScreen("Loading...\nLoading terrain\nDone");
	initdone = true;
}

void TERRAIN::FixHeightArray()
{
	int x, y;
	
	x = 4;
	y = 4;
	
	//printf("%f %f %f\n", heightarray[x-2+y*hax], heightarray[x-1+y*hax], heightarray[x+y*hax]);
	
	for (y = 0; y < hay; y++)
	{
		for (x = 0; x < hax; x++)
		{
			if (x % 3 == 1 && x != 1)
			{
				heightarray[x+y*hax] = 2.0f*heightarray[x-1+y*hax]-heightarray[x-2+y*hax];
			}
		}
		
		x = 0;
		//cout << heightarray[x+y*hax] << endl;
	}
	
	for (y = 0; y < hay; y++)
	{
		for (x = 0; x < hax; x++)
		{
			if (y % 3 == 1 && y != 1)
			{
				heightarray[x+y*hax] = 2.0f*heightarray[x+(y-1)*hax]-heightarray[x+(y-2)*hax];
			}
		}
	}
	
	x = 4;
	y = 4;
	
	//printf("%f %f %f\n", heightarray[x-2+y*hax], heightarray[x-1+y*hax], heightarray[x+y*hax]);
}

extern bool verbose_output;
TERRAIN::~TERRAIN()
{
	if (verbose_output)
		cout << "terrain deinit" << endl;
	
	if (height_data)	// If Texture Exists
	{
		// Free up any memory we may have used
    	
	}
	
	error_log.close();
	
	if (heightarray != NULL)
		delete [] heightarray;
}

VERTEX TERRAIN::lookup_normal_alwaysup(float xloc, float zloc, int l)
{
	VERTEX normal;
	normal.x = 0;
	normal.y = 1;
	normal.z = 0;
	return normal;
}

void TERRAIN::lookup_texcoord_detailmap(float * tc, float xloc, float zloc)
{
	tc[0] = xloc/(float)0.2;
	tc[1] = zloc/(float)0.2;
}

void TERRAIN::lookup_texcoord_bigmap(float * tc, float xloc, float zloc)
{
	//tc[0] = (xloc-offset.x)/(float)size_x;
	//tc[1] = (zloc-offset.z)/(float)size_z;
	
	//tc[0] = xloc/(float)0.5;
	//tc[1] = zloc/(float)0.5;
	
	//indicates we want multitexturing!
	tc[0] = -1337;
	tc[1] = -1337;
}

VERTEX TERRAIN::lookup_normal_half(float xloc, float zloc, int l)
{	
	//output normal
	VERTEX normal;
	
	float v0[3], v1[3], v2[3];
	
	//float dampening = 0.8f;
	float dampening = 0.5f;
	
	//int datastep = (int)(18.0f/(float)(l));
	int datastep = 4;

	xloc = (xloc-offset.x)*(((float) H_DATA_RES/2.0f)/size_x);
	zloc = (zloc-offset.z)*(((float) H_DATA_RES/2.0f)/size_z);
	
	//clamp
	if (xloc < 0)
		xloc = 0;
	if (zloc < 0)
		zloc = 0;
	if (xloc > size_x)
		xloc = size_x;
	if (zloc > size_z)
		zloc = size_z;
	
	//set this vertex height
	v1[0] = xloc;
	v1[2] = zloc;
	v1[1] = dampening*lookup_height_int((int)v1[0]*2,(int)v1[2]*2);
	
	//get vertex height one pixel to the right
	v0[0] = xloc+datastep;
	v0[2] = zloc;
	v0[1] = dampening*lookup_height_int((int)v0[0]*2,(int)v0[2]*2);
	
	//get vertex height one pixel lower
	v2[0] = xloc;
	v2[2] = zloc+datastep;
	v2[1] = dampening*lookup_height_int((int)v2[0]*2,(int)v2[2]*2);
	
	float vb[3], va[3];
    int i;
    for (i = 0; i < 3; i++)
    {
    	va[i] = v0[i]-v1[i];	
		vb[i] = v2[i]-v1[i];
    }
    float norm[3];
    norm[0] = va[1]*vb[2]-vb[1]*va[2];
    norm[1] = va[2]*vb[0]-vb[2]*va[0];
    norm[2] = va[0]*vb[1]-vb[0]*va[1];
    float normmag = sqrt(norm[0]*norm[0]+norm[1]*norm[1]+norm[2]*norm[2]);
    for (i = 0; i < 3; i++)
    {
    	norm[i]=norm[i]/normmag;
    }
	
	if (norm[1] < 0)
	{
		norm[0] = -norm[0];
		norm[1] = -norm[1];
		norm[2] = -norm[2];
	}
	
	normal.x=norm[0];
	normal.y=norm[1];
	normal.z=norm[2];
	
	//normal.DebugPrint();
	
	return normal;
}

VERTEX TERRAIN::lookup_normal_half_param(float xloc, float zloc, int l)
{	
	//output normal
	VERTEX normal;
	
	float v0[3], v1[3], v2[3];
	
	//float dampening = 0.8f;
	float dampening = 0.5f;
	
	//int datastep = (int)(18.0f/(float)(l));
	int datastep = 4;

	xloc = (xloc-offset.x)*(((float) H_DATA_RES/2.0f)/size_x);
	zloc = (zloc-offset.z)*(((float) H_DATA_RES/2.0f)/size_z);
	
	//clamp
	if (xloc < 0)
		xloc = 0;
	if (zloc < 0)
		zloc = 0;
	if (xloc > size_x)
		xloc = size_x;
	if (zloc > size_z)
		zloc = size_z;
	
	//set this vertex height
	v1[0] = xloc;
	v1[2] = zloc;
	v1[1] = dampening*lookup_height_int((int)v1[0]*2,(int)v1[2]*2);
	
	//get vertex height one pixel to the right
	v0[0] = xloc+datastep;
	v0[2] = zloc;
	v0[1] = dampening*lookup_height_int((int)v0[0]*2,(int)v0[2]*2);
	
	//get vertex height one pixel lower
	v2[0] = xloc;
	v2[2] = zloc+datastep;
	v2[1] = dampening*lookup_height_int((int)v2[0]*2,(int)v2[2]*2);
	
	float vb[3], va[3];
    int i;
    for (i = 0; i < 3; i++)
    {
    	va[i] = v0[i]-v1[i];	
		vb[i] = v2[i]-v1[i];
    }
    float norm[3];
    norm[0] = va[1]*vb[2]-vb[1]*va[2];
    norm[1] = va[2]*vb[0]-vb[2]*va[0];
    norm[2] = va[0]*vb[1]-vb[0]*va[1];
    float normmag = sqrt(norm[0]*norm[0]+norm[1]*norm[1]+norm[2]*norm[2]);
    for (i = 0; i < 3; i++)
    {
    	norm[i]=norm[i]/normmag;
    }
	
	if (norm[1] < 0)
	{
		norm[0] = -norm[0];
		norm[1] = -norm[1];
		norm[2] = -norm[2];
	}
	
	normal.x=norm[0];
	normal.y=norm[1];
	normal.z=norm[2];
	
	//normal.DebugPrint();
	
	return normal;
}

VERTEX TERRAIN::lookup_normal(float xloc, float zloc, int l)
{
	//output normal
	VERTEX normal;
	normal.y = 1;
	return normal;
	/*float v0[3], v1[3], v2[3];
	
	//float dampening = 0.8f;
	//float dampening = 0.5f;
	float dampening = 1.0f;
	
	//int datastep = (int)(18.0f/(float)(l));
	float datastep = 50.0f;
	
	//set this vertex height
	v1[0] = xloc;
	v1[2] = zloc;
	v1[1] = dampening*((*this.*(drawdata.lookup_height_func))(v1[0],v1[2]));
	
	//get vertex height one pixel to the right
	v0[0] = xloc+datastep;
	v0[2] = zloc;
	//v0[1] = dampening*lookup_height_int((int)v0[0],(int)v0[2]);
	v0[1] = dampening*((*this.*(drawdata.lookup_height_func))(v0[0],v0[2]));
	
	//get vertex height one pixel lower
	v2[0] = xloc;
	v2[2] = zloc+datastep;
	//v2[1] = dampening*lookup_height_int((int)v2[0],(int)v2[2]);
	v2[1] = dampening*((*this.*(drawdata.lookup_height_func))(v2[0],v2[2]));
	
	float vb[3], va[3];
    int i;
    for (i = 0; i < 3; i++)
    {
    	va[i] = v0[i]-v1[i];	
		vb[i] = v2[i]-v1[i];
    }
    float norm[3];
    norm[0] = va[1]*vb[2]-vb[1]*va[2];
    norm[1] = va[2]*vb[0]-vb[2]*va[0];
    norm[2] = va[0]*vb[1]-vb[0]*va[1];
    float normmag = sqrt(norm[0]*norm[0]+norm[1]*norm[1]+norm[2]*norm[2]);
    for (i = 0; i < 3; i++)
    {
    	norm[i]=norm[i]/normmag;
    }
	
	if (norm[1] < 0)
	{
		norm[0] = -norm[0];
		norm[1] = -norm[1];
		norm[2] = -norm[2];
	}
	
	normal.x=norm[0];
	normal.y=norm[1];
	normal.z=norm[2];
	
	//normal.DebugPrint();
	
	return normal;*/
}

extern float ACOS(float coeff);

inline void TERRAIN::setwatercolor(float * norm, float * vert)
{
	
	VERTEX n, v;
	n.x = norm[0];
	n.y = norm[1];
	n.z = norm[2];
	
	v.x = vert[0];
	v.y = vert[1];
	v.z = vert[2];
	
	VERTEX sightline, temp;
	temp = temp - cam.position;
	sightline = temp - v;
	//float sl = sightline.len();
	float cosangle = sightline.normalize().dot(n.normalize());
	
	
	if (cosangle < 0)
		cosangle = -cosangle;
	
	//correct answer based on Snell and Fresnel:
	float angle = acos(cosangle);
	float sinangle = sin(angle);
	float tangle = asin(0.75*sin(angle));
	float tfactor = (2.0f*0.75f*sinangle*cosangle)/(sin(angle+tangle)*cos(angle-tangle));
	if (tfactor < 0)
		tfactor = 0.0f;
	
	//super optimized linear model
	/*cosangle = 1-n.y*n.y*n.y;
	float tfactor = 0.855*cosangle;*/
	
	/*if (cosangle > 0.9f)
	{
		cout << temp.x << "," << temp.y << "," << temp.z << endl;
		cout << v.x << "," << v.y << "," << v.z << endl;
		cout << sightline.x << "," << sightline.y << "," << sightline.z << endl;
		cout << n.x << "," << n.y << "," << n.z << endl;
		cout << cosangle << endl;
		cout << tfactor << endl;
		cout << endl;
	}*/
	
	//float tfactor = cosangle/1.0f;
	
	//fudge factor required to try to eliminate problems with transparent overdraw
	// which happens with overlapping waves
	/*
	float sl = sightline.len();
	if (temp.y - v.y < 10.0f && sl > 10.0f)
	{
		if (sl > 20.0f)
			sl = 20.0f;
		float linfac = sl - 10.0f;
		linfac = linfac / 10.0f;
		tfactor = (1.0f - linfac) * tfactor;
	}*/
	//another attempt
	/*float sl = sightline.len();
	float opaquesl = 10.0f;
	float normalsl = 20.0f;
	if (sl <= opaquesl)
		tfactor = 0.0f;
	else if (sl < normalsl)
	{
		tfactor *= (sl - opaquesl)/(normalsl-opaquesl);
	}*/
	
	//float col = n.y*n.y*n.y;
	//float compfactor, offset;
	//compfactor = 0.1f;
	//offset = 0.85f;
	//compfactor = 0.1f;
	//offset = 0.9f;
	//float col = (1-(1.0f-tfactor)*(1.0f-tfactor))*compfactor+offset;
	
	//since we're using an alphamap for surface detail, add some opacity
	//tfactor -= 0.4f;
	
	float col = 1.0f-tfactor*0.4;
	
	float trans = 1.0f - (tfactor*0.1);
	
	//col = windmag/0.003f;
	
	/*float watercolor[3] = {  0.53f, 0.74f, 0.91f  };
	int i;
	for (i = 0; i < 3; i++)
		watercolor[i] *= col;*/
	
	//tfactor = 0.0f;
	
	glColor4f(col, col, col, trans);
	//glColor4f(1.0f-tfactor*0.4,1.0f-tfactor*0.4,1.0f-tfactor*0.4, 1.0f);
	//glColor4f(1.0f,1.0f,1.0f,trans);
}

inline void TERRAIN::setlandcolor(float * n, float * v, float yloc)
{
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	//glColor4f(1.0f, n[1], 1.0f, 1.0f);
	return;
}

void TERRAIN::drawmultitextri(float **allv,
		float **alln,
		float *ally, float *ostx, int flags)
{
	int i;
	
	for ( i = 0; i < 3; i++)
	{
		float xloc, yloc, zloc;
		
		xloc = allv[i][0];
		yloc = ally[i];
		zloc = allv[i][2];
		
		/*float detailscale;
		if (flags == 1)
			detailscale = 0.40f;
		else
			//detailscale = 1.5f;
			detailscale = 5.0f;
		float xscale = size_x;
		float zscale = size_z;
		//float cloudscale = 2000.0f;
		//float cloudoffset_x, cloudoffset_z;
		//use modangle to avoid singularities which occur at 0 and pi (sunup and sunset)
		//float modangle = backdrop.sunpos_lat;
		float modangle = 0;
		if (modangle > 3.141593f)
			modangle = 2.0f*3.141593f-modangle;//wrap around night to day
		//avoid singularities by clamping to reasonable range
		float reasonablefactor = 0.15f;
		if (modangle < reasonablefactor)
			modangle = reasonablefactor;
		if (modangle > 3.141593f-reasonablefactor)
			modangle = 3.141593f-reasonablefactor;
		//who knows why that 2.5 needs to be in there.  *shrug*
		//cloudoffset_x = windoffset_x+((0.1f*cloudscale-WATEROFFSET*2.5f)*
			//			tan(1.570796-modangle))/cloudscale;
		//cloudoffset_z = windoffset_z;
		if (flags == 1)
		{
			//tile more for water texture
			//xscale = zscale = (OFFSET_TEX_SIZE/16.0f);
			xscale = zscale = 2*1.60f;
			//cloudscale = 5000.0f;
			//cloudscale = 500.0f;
			//xscale = zscale = 4.0f;
		}
		
		//cloudmap texture mapping
		//JGL_MTEXCOORD(GL_TEXTURE3_ARB, xloc/cloudscale+cloudoffset_x, zloc/cloudscale+cloudoffset_z);
		
		float offset_x=0, offset_z=0;
		//offset_x = bumpwindoffset_x*8.0f;
		//offset_z = bumpwindoffset_z*8.0f;
		*/
		/*if (flags == 1)
		{
			utility.TexCoord2d2f(0, xloc/xscale+offset_x, zloc/zscale+offset_z);
			utility.TexCoord2d2f(1, ostx[2*i], ostx[2*i+1]);
			utility.TexCoord2d2f(2, xloc/xscale+offset_x, zloc/zscale+offset_z);
			//JGL_MTEXCOORD(GL_TEXTURE0_ARB, xloc/xscale+offset_x, zloc/zscale+offset_z);
			//JGL_MTEXCOORD(GL_TEXTURE1_ARB, ostx[2*i], ostx[2*i+1]);
			//JGL_MTEXCOORD(GL_TEXTURE2_ARB, xloc/xscale+offset_x, zloc/zscale+offset_z);
		}
		else
			utility.TexCoord2d2f(0, xloc/xscale, zloc/zscale);
			//JGL_MTEXCOORD(GL_TEXTURE0_ARB, xloc/xscale, zloc/zscale);
		
		if (flags == 2)
		{
			utility.TexCoord2d2f(1, xloc/detailscale, zloc/detailscale);
			//JGL_MTEXCOORD(GL_TEXTURE1_ARB, xloc/detailscale, zloc/detailscale);
			
			float maxwet = 2.0f;
			float minwet = 0.0f;//WATEROFFSET-2.0f;
			float wettexu = xloc/15.0f;
			float wettexv = (yloc - minwet)/(maxwet-minwet);
			if (wettexv < 0)
			   wettexv = 0;
			//cout << v0[1] << "," << yloc << ": " << wettexu << "," << wettexv << endl;
			utility.TexCoord2d2f(2, wettexu, wettexv);
			//JGL_MTEXCOORD(GL_TEXTURE2_ARB, wettexu, wettexv);
			//JGL_MTEXCOORD(GL_TEXTURE2_ARB, xloc/detailscale, zloc/detailscale);
		}*/
		
		//utility.TexCoord2d2f(0, xloc/xscale+offset_x, zloc/zscale+offset_z);
		utility.TexCoord2d2f(0, (xloc-offset.x)/(float)size_x, (zloc-offset.z)/(float)size_z);
		float detailscale = 100.0;
		utility.TexCoord2d2f(1, (xloc-offset.x)/(size_x/detailscale), (zloc-offset.z)/(size_z/detailscale));
		detailscale = 1000.0;
		utility.TexCoord2d2f(2, (xloc-offset.x)/(size_x/detailscale), (zloc-offset.z)/(size_z/detailscale));
		utility.TexCoord2d2f(3, (xloc-offset.x)/(float)size_x, (zloc-offset.z)/(float)size_z);
		
		//fakies
		setlandcolor(alln[i], allv[i], yloc);
		
		glNormal3fv(alln[i]);
		glVertex3fv(allv[i]);
	}
}
/*
bool TERRAIN::getoffsettexcoords(float **allv,
		float **alln,
		float *ally, float *ostx, bool * clipped)
{
	bool clippin = false;
	
	int i;
	for (i = 0; i < 3; i++)
	{
		//find the projected screen coordinates of the vertex, which are used
		// as tex coords into a previously rendered reflection texture
		float texu, texv;
		GLdouble ox, oy, oz, ow;
		VERTEX v;
		v.x = allv[i][0];
		v.y = allv[i][1];
		v.z = allv[i][2];
		coordProject(v.x, v.y, v.z, temp_matrix, temp_proj, temp_viewport, &ox, &oy, &oz, &ow);

		texu = (ox/temp_viewport[2]);// * size;
		texv = (oy/temp_viewport[3]);// * size;

		if (texu < 0 || texv < 0 || texu > 1.0f || texv > 1.0f)
		{
			clippin = true;
			clipped[i] = true;
		}
		else
			clipped[i] = false;
		
		//add distortion to the texture coords based on the constant rippling_strength
		//float rippling_strength = 0.05f;
		//float rippling_strength = 4.0f;
		//float rippling_strength_max = 0.5f;
		float rippling_strength_max = 0.1f;
		//float rippling_strength_max = 0.5f;
		
		VERTEX sightline, temp;
		temp = temp - cam.position;
		sightline = temp - v;
		float sl = sightline.len();
		
		float ripplescale = 1.0f;
		
		//waves further away shouldn't displace the texture as much
		//float rippling_strength = ripplescale*rippling_strength_max / (sl*sl);
		if (sl < 0.1f)
			sl = 0.1f;
		float rippling_strength = ripplescale*rippling_strength_max / (sl);
		if (rippling_strength > rippling_strength_max)
		{
			rippling_strength = rippling_strength_max;
			//cout << "rippleclip" << endl;
		}
		
		//disable distance-modified ripple scaling
		rippling_strength = rippling_strength_max;
		
		VERTEX sltrans = sightline;
		sltrans.y = 0.0f;
		sltrans = sltrans.normalize();
		rippling_strength = -rippling_strength;
		float trv = alln[i][0]*sltrans.x+alln[i][2]*sltrans.z;
		float tru = alln[i][0]*sltrans.z-alln[i][2]*sltrans.x;
		//texv += sin(sin(sin(trv)))*rippling_strength;
		//texu += sin(sin(sin(tru)))*rippling_strength;
		texv += trv*rippling_strength;
		texu += tru*rippling_strength;
		
		//glMultiTexCoord2fARB(GL_TEXTURE0_ARB, texu, texv);
		//JGL_MTEXCOORD(GL_TEXTURE1_ARB, texu, texv);
		ostx[2*i] = texu;
		ostx[2*i+1] = texv;
	}
	
	return clippin;
}*/

/*
void TERRAIN::ClipToPlane(float polyin[][3], float polyinnorms[][3], int polyinpoints, float polyout[][3], float polyoutnorms[][3], int & polyoutpoints, float * plane)
{
	int curvert = 0;
	float vert1[3], vert2[3], norm1[3], norm2[3];
	
	int m, i;
	
	//assign vert1 = polyin.numpoints - 1
	for (m = 0; m < 3; m++)
	{
		vert1[m] = polyin[polyinpoints-1][m];
		norm1[m] = polyinnorms[polyinpoints-1][m];
	}
	
	//cout << endl << "Init" << endl;
	
	for (i = 0; i < polyinpoints; i++)
	{
		//set vert2 to polyin[i]
		for (m = 0; m < 3; m++)
		{
			vert2[m] = polyin[i][m];
			norm2[m] = polyinnorms[i][m];
		}

		
		//build a bool array defining in what way the vert is clipped
		//true if outside of the clipping plane
		bool clipping_info[2];
		//Ax + By + Cz + D < 0 implies outside of frustum (i think)
		clipping_info[0] = (plane[0]*vert1[0]+
			plane[1]*vert1[1]+
			plane[2]*vert1[2]+
			plane[3] < 0);
		clipping_info[1] = (plane[0]*vert2[0]+
			plane[1]*vert2[1]+
			plane[2]*vert2[2]+
			plane[3] < 0);
		
		//cout << clipping_info[0] << "," << clipping_info[1] << endl;
		
		//if both are inside the frustum
		if (!clipping_info[0] && !clipping_info[1])
		{
			for (m = 0; m < 3; m++)
			{
				polyout[curvert][m] = vert2[m];
				polyoutnorms[curvert][m] = norm2[m];
			}
			curvert++;
		}
		
		//if we're entering or leaving the frustum
		if (clipping_info[0] != clipping_info[1])
		{
			
			//direction of the edge
			float dir[3];
			for (m = 0; m < 3; m++)
				dir[m] = vert2[m]-vert1[m];
			
			float denom = plane[0]*dir[0]+plane[1]*dir[1]+plane[2]*dir[2];
			
			float t;
			
			if (denom == 0)
				t = 1.0f;
			else
				t = (plane[0]*vert1[0]+plane[1]*vert1[1]+plane[2]*vert1[2]+plane[3])/denom;
			
			//cout << t << endl;
			
			if (t < 0)
				t = -t;
			
			//now apply t to find the new vertex
			float newvert[3], newnorm[3];
			for (m = 0; m < 3; m++)
			{
				newvert[m] = vert1[m]+t*dir[m];
				newnorm[m] = (1.0f-t)*norm1[m]+t*norm2[m];
			}
			
			//insert(newvertex) in polyout[curvert]
			for (m = 0; m < 3; m++)
			{
				polyout[curvert][m] = newvert[m];
				polyoutnorms[curvert][m] = newnorm[m];
			}
			curvert++;
			
			//additional steps if entering
			if (clipping_info[0])
			{
				for (m = 0; m < 3; m++)
				{
					polyout[curvert][m] = vert2[m];
					polyoutnorms[curvert][m] = norm2[m];
				}
				curvert++;
			}
		}
		
		for (m=0;m<3;m++)
		{
			vert1[m] = vert2[m];
			norm1[m] = norm2[m];
		}
	}
	
	polyoutpoints = curvert;
	
	//this seems unnecessary
	for (m = 0; m < 3; m++)
	{
		polyout[curvert][m] = polyout[0][m];
		polyoutnorms[curvert][m] = polyoutnorms[0][m];
	}
}
*/

//#define DRAWSUB_DEBUG

void TERRAIN::drawsub(int l,float *v0,float *v1,float *v2,float *n0, float *n1,
		float *n2, float * t0, float * t1, float * t2, int cullflags)
{
	int i;
	float vc[3]; /* new (split) vertex */
	float pvc[3],pnc[3];//,pyheightc; // previous center point
	float nc[3]; //ditto (normal)
	float tc[2]; //ditto (tex coord)
    float ds; /* maximum midpoint displacement */
    float dx,dy,dz; /* difference vector */
    float rd; /* squared sphere bound radius */
    float rc; /* squared distance from vc to camera position */

	#ifdef DRAWSUB_DEBUG
	cout << "boing1" << endl;
	#endif

		
	ds = level2dzsize[l];
	/*cout << "v0: " << v0[0] << "," << v0[1] << "," << v0[2] << endl;
	cout << "v1: " << v1[0] << "," << v1[1] << "," << v1[2] << endl;
	cout << "v2: " << v2[0] << "," << v2[1] << "," << v2[2] << endl;
	cout << l << ": " << vc[0] << "," << vc[1] << "," << vc[2] << endl;*/
	
	
	pvc[0] = vc[0]=0.5f*(v0[0]+v2[0]);
	pvc[1] = 0.5f*(v0[1]+v2[1]); //added for curved surface
	//pvc[1] = vc[1]=yheightc;
    pvc[2] = vc[2]=0.5f*(v0[2]+v2[2]);
	
	//find rectangular projection coordinates
	float recx, recz;
	recx = vc[0];
	recz = vc[2];
	
	float yheightc = (*this.*(drawdata.lookup_height_func))(recx, recz);
	
    /* compute split point of base edge */
    
	//pyheightc = (yheight0 + yheight2)/2.0f;
    //RANDHASHF((unsigned char *)vc,8,dz)
	//dy = 0.0f;
	
	vc[1] = yheightc;
	
	//vc[1] = 0.0f;
	//rect_to_sphere_inline(vc);
	
	//pyheightc = yheightc;
	//setheight(vc, yheightc);
	
	#ifdef DRAWSUB_DEBUG
	cout << "boing2" << endl;
	#endif
	
	dy = dz = 0.0f;

	#ifdef DRAWSUB_DEBUG
	cout << "boing3" << endl;
	#endif
	
	//only look up normals for nearby points
	VERTEX newnorm;
	//if (drawdata.flags == 1)
	{
		newnorm = (*this.*(drawdata.lookup_normal_func))(recx, recz,l);
	}
	/*else
	{
		newnorm.y = 1.0f;
	}*/
	nc[0] = newnorm.x;
	nc[1] = newnorm.y;
	nc[2] = newnorm.z;
	
	//lookup tex coord for new point
	(*this.*(drawdata.lookup_texcoord_func))(tc, recx, recz);
	//(*this.*(drawdata.lookup_texcoord_func))(ptc, recx, recz);
	//(*this.*(drawdata.lookup_texcoord_func))(ptc, pvc[0], pvc[2]);
	
    /* compute radius of diamond bounding sphere (squared) */
    rd=0.0f;
    dx=v0[0]-vc[0]; dy=v0[1]-vc[1]; dz=v0[2]-vc[2];
    rc=dx*dx+dy*dy+dz*dz; if (rc>rd) rd=rc;
		//cout << dx << "," << dy << "," << dz << endl;
    dx=v1[0]-vc[0]; dy=v1[1]-vc[1]; dz=v1[2]-vc[2];
    rc=dx*dx+dy*dy+dz*dz; if (rc>rd) rd=rc;
    dx=v2[0]-vc[0]; dy=v2[1]-vc[1]; dz=v2[2]-vc[2];
    rc=dx*dx+dy*dy+dz*dz; if (rc>rd) rd=rc;
	
	#ifdef DRAWSUB_DEBUG
	cout << "boing4" << endl;
	#endif	
	
	/* update cull flags, stop recursing if OUT */
    if (cullflags!=ROAM_ALLIN) 
	{
        int m;

        for (i=0,m=1;i<6;i++,m<<=1) {
            if (!(cullflags&m)) {
                rc=cam.frustum[i][0]*vc[0]+
                   cam.frustum[i][1]*vc[1]+
                   cam.frustum[i][2]*vc[2]+
                   cam.frustum[i][3];
                if (rc*rc>=rd) {
                    if (rc<0.0f) return;
                    cullflags|=m;
                }
            }
        }
    }

    /* compute distance from split point to camera (squared) */
    dx=vc[0]-xcf; dy=vc[1]-ycf; dz=vc[2]-zcf;
    rc=dx*dx+dy*dy+dz*dz;
	
	/* if not max level and error large on screen, recursively split */
	if (l<drawdata.lmax && ds*ds>rc*drawdata.lfactor*0.25f)
	{
		//gradual pop code
		{
			float lfullpop = drawdata.lfactor * drawdata.popfactor;
			float lipop, lfpop;
			lipop = rc*drawdata.lfactor*0.25f;
			lfpop = rc*lfullpop*0.25f;
			float lmag, ldiff;
			ldiff = ds*ds - lipop;
			lmag = lfpop - lipop;
			float coeff = ldiff / lmag;
			
			if (coeff > 1.0 || coeff < 0.0)
			{
				
			}
			else
			{
				//put coeff through ease curve.  i'm using 3*p^2-2*p^3
				//coeff = 3.0f*coeff*coeff-2.0f*coeff*coeff*coeff;
					
				for (i = 0; i < 3; i++)
				{
					vc[i] = (1.0f - coeff)*pvc[i] + coeff*vc[i];
					pnc[i] = (n0[i]+n2[i])/2.0f;
					nc[i] = (1.0 - coeff)*pnc[i] + coeff*nc[i];
				}
				
				(*this.*(drawdata.lookup_texcoord_func))(tc, vc[0], vc[2]);
				
				//yheightc = (1.0f - coeff)*pyheightc + coeff*yheightc;
			}
		}
		
		
		//drawsub(l+1,v0,vc,v1,n0,nc,n1,t0,tc,t1,cullflags, cam, lmax, lfactor, lookup_height_func, lookup_normal_func, lookup_texcoord_func, flags, yheight0, yheightc, yheight1);
		//drawsub(l+1,v1,vc,v2,n1,nc,n2,t1,tc,t2,cullflags, cam, lmax, lfactor, lookup_height_func, lookup_normal_func, lookup_texcoord_func, flags, yheight1, yheightc, yheight2);
		drawsub(l+1,v0,vc,v1,n0,nc,n1,t0,tc,t1,cullflags);
		drawsub(l+1,v1,vc,v2,n1,nc,n2,t1,tc,t2,cullflags);
		return;
	}
	
	#ifdef DRAWSUB_DEBUG
	cout << "boing5" << endl;
	#endif
	
	//do some precalculations for face culling
	/*bool triup = false;
	float ycomp;
	float *vtemp;
	float va[3], vb[3];
    for (i = 0; i < 3; i++) //build vectors
    {
    	va[i] = v0[i]-v1[i];	
		vb[i] = v2[i]-v1[i];
    }
    ycomp = va[2]*vb[0]-vb[2]*va[0]; //find y component
    //set our triup flag if ycomp is positive
	if (ycomp >= 0)
		triup = true;
	
	//cout << ycomp << " -> " << triup << endl;
	
	//quickly re-order verts to do face culling
	if (triup)
	{
		vtemp = v0;
		v0 = v2;
		v2 = vtemp;
		
		vtemp = n0;
		n0 = n2;
		n2 = vtemp;
		
		vtemp = t0;
		t0 = t2;
		t2 = vtemp;
	}*/
	
	//if we're rendering the reflection, don't render underwater portions
	/*bool renderme = true;
	if (reflection && flags == 2)
	{
		if (!(v0[1] > WATEROFFSET || v1[1] > WATEROFFSET || v2[1] > WATEROFFSET))
			renderme = false;
		
		//check for partially out of water triangles
		if (renderme)
		{
			if (v0[1] < WATEROFFSET)
				v0[1] = WATEROFFSET;
			if (v1[1] < WATEROFFSET)
				v1[1] = WATEROFFSET;
			if (v2[1] < WATEROFFSET)
			{
				//cout << v0[0] << "," << v0[1] << "," << v0[2] << " - ";
				//cout << v1[0] << "," << v1[1] << "," << v1[2] << " - ";
				//cout << v2[0] << "," << v2[1] << "," << v2[2] << endl;
				v2[1] = WATEROFFSET;
			}
		}
	}
	if (!renderme)
		return;*/
		
	/*if (reflection)
	{
		float cdir = (cam.position.x+v0[0])*(cam.position.x+v0[0])+
				(cam.position.z+v0[2])*(cam.position.z+v0[2]);
		if (cdir > 10000)
			return;
	}*/
	
	#ifdef DRAWSUB_DEBUG
	cout << "boing6" << endl;
	#endif
	
	glBegin(GL_TRIANGLES);
	
	/*if (drawdata.flags == 1)
		num_water_tris++;*/
	
	if (t0[0] == -1337)
	{
		float *allv[3];
		float *alln[3];
		float ally[3];

		allv[0] = v0;
		allv[1] = v1;
		allv[2] = v2;
		
		alln[0] = n0;
		alln[1] = n1;
		alln[2] = n2;
		
		/*ally[0] = yheight0;
		ally[1] = yheight1;
		ally[2] = yheight2;*/
		
		//offset texture generation stuff:  only if we're rendering water
		/*float ostx[6];
		bool clipping = false;
		bool clipped[3];
		if (drawdata.flags == 1)
		{
			clipping = getoffsettexcoords(allv, alln, ally, ostx, clipped);
		}
		
		if (clipping)
		{
			//this usually happens to between 0 and 10 triangles
			//if it does happen, we need to tesselate the triangle up into
			// constituent parts which are inside the view frustum

			int max_verts = 10;
			float newpoly_v[max_verts][3];
			float temppoly_v[max_verts][3];
			float newpoly_n[max_verts][3];
			float temppoly_n[max_verts][3];
			int newpoly_numpoints = 0;
			int temppoly_numpoints = 0;
			
			//prime the pump
			for (i = 0; i < 9; i++)
			{
				temppoly_v[i/3][i%3] = allv[i/3][i%3];
				temppoly_n[i/3][i%3] = alln[i/3][i%3];
			}
			temppoly_numpoints = 3;
			
			
			
			ClipToPlane(temppoly_v, temppoly_n, temppoly_numpoints, newpoly_v, newpoly_n, newpoly_numpoints, &(cam.frustum[0][0]));
			ClipToPlane(newpoly_v, newpoly_n, newpoly_numpoints, temppoly_v, temppoly_n, temppoly_numpoints, &(cam.frustum[1][0]));
			ClipToPlane(temppoly_v, temppoly_n, temppoly_numpoints, newpoly_v, newpoly_n, newpoly_numpoints, &(cam.frustum[2][0]));
			ClipToPlane(newpoly_v, newpoly_n, newpoly_numpoints, temppoly_v, temppoly_n, temppoly_numpoints, &(cam.frustum[3][0]));
			ClipToPlane(temppoly_v, temppoly_n, temppoly_numpoints, newpoly_v, newpoly_n, newpoly_numpoints, &(cam.frustum[5][0]));
			
			int n;
			
			//newpoly now contains the clipped polygon
					
			//draw the polygon to the screen
			float * newv[3], * newn[3];
			for(n = 0; n < newpoly_numpoints-2; n++)
			{
				newv[0] = &(newpoly_v[0][0]);
				newv[1] = &(newpoly_v[n+1][0]);
				newv[2] = &(newpoly_v[n+2][0]);
				
				newn[0] = &(newpoly_n[0][0]);
				newn[1] = &(newpoly_n[n+1][0]);
				newn[2] = &(newpoly_n[n+2][0]);
				
				//the y coord (ally, yloc, etc) isn't used for the water, 
				// so don't bother to even pass in valid data for it.
				
				//re-project to get reflection tex coords
				getoffsettexcoords(newv, newn, ally, ostx, clipped);
                
				drawmultitextri(newv, newn, ally, ostx, drawdata.flags);
			}
		}
		else*/
		{
			float ostx[6];
			drawmultitextri(allv, alln, ally, ostx, drawdata.flags);
		}
		
		/*for ( i = 0; i < 3; i++)
		{
			float xloc, yloc, zloc;
			
			xloc = allv[i][0];
			yloc = ally[i];
			zloc = allv[i][2];
			
			float detailscale;
			if (flags == 1)
				detailscale = 4.0f;
			else
				//detailscale = 1.5f;
				detailscale = 5.0f;
			float xscale = size_x;
			float zscale = size_z;
			float cloudscale = 2000.0f;
			float cloudoffset_x, cloudoffset_z;
			//use modangle to avoid singularities which occur at 0 and pi (sunup and sunset)
			float modangle = backdrop.sunpos_lat;
			if (modangle > 3.141593f)
				modangle = 2.0f*3.141593f-modangle;//wrap around night to day
			//avoid singularities by clamping to reasonable range
			float reasonablefactor = 0.15f;
			if (modangle < reasonablefactor)
				modangle = reasonablefactor;
			if (modangle > 3.141593f-reasonablefactor)
				modangle = 3.141593f-reasonablefactor;
			//who knows why that 2.5 needs to be in there.  *shrug*
			cloudoffset_x = windoffset_x+((0.1f*cloudscale-WATEROFFSET*2.5f)*
							tan(1.570796-modangle))/cloudscale;
			cloudoffset_z = windoffset_z;
			if (flags == 1)
			{
				//tile more for water texture
				//xscale = zscale = (OFFSET_TEX_SIZE/16.0f);
				xscale = zscale = 2*16.0f;
				//cloudscale = 5000.0f;
				//cloudscale = 500.0f;
				//xscale = zscale = 4.0f;
			}
			
			//cloudmap texture mapping
			JGL_MTEXCOORD(GL_TEXTURE3_ARB, xloc/cloudscale+cloudoffset_x, zloc/cloudscale+cloudoffset_z);
			
			float offset_x, offset_z;
			offset_x = bumpwindoffset_x*8.0f;
			offset_z = bumpwindoffset_z*8.0f;
			if (flags == 1)
			{
				JGL_MTEXCOORD(GL_TEXTURE0_ARB, xloc/xscale+offset_x, zloc/zscale+offset_z);
				JGL_MTEXCOORD(GL_TEXTURE1_ARB, ostx[i][0], ostx[i][1]);
				JGL_MTEXCOORD(GL_TEXTURE2_ARB, xloc/xscale+offset_x, zloc/zscale+offset_z);
			}
			else
				JGL_MTEXCOORD(GL_TEXTURE0_ARB, xloc/xscale, zloc/zscale);
			
			if (flags == 2)
			{
				JGL_MTEXCOORD(GL_TEXTURE1_ARB, xloc/detailscale, zloc/detailscale);
				
				float maxwet = WATEROFFSET+2.0f;
				float minwet = 0.0f;//WATEROFFSET-2.0f;
				float wettexu = xloc/15.0f;
				float wettexv = (yloc - minwet)/(maxwet-minwet);
				if (wettexv < 0)
				   wettexv = 0;
				//cout << v0[1] << "," << yloc << ": " << wettexu << "," << wettexv << endl;
				JGL_MTEXCOORD(GL_TEXTURE2_ARB, wettexu, wettexv);
				//JGL_MTEXCOORD(GL_TEXTURE2_ARB, xloc/detailscale, zloc/detailscale);
			}
			
			//fakies
			if (flags == 1)
				setwatercolor(alln[i], allv[i]);
			if (flags == 2)
				setlandcolor(alln[i], allv[i], yloc);
			
			glNormal3fv(alln[i]);
			glVertex3fv(allv[i]);
		}*/
		
		
		/*xloc = v1[0];
		yloc = yheight1;//v1[1];
		zloc = v1[2];
		
		//cloudmap texture mapping
		JGL_MTEXCOORD(GL_TEXTURE3_ARB, xloc/cloudscale+cloudoffset_x, zloc/cloudscale+cloudoffset_z);
		if (flags == 1)
		{
			JGL_MTEXCOORD(GL_TEXTURE0_ARB, xloc/xscale+offset_x, zloc/zscale+offset_z);
			JGL_MTEXCOORD(GL_TEXTURE2_ARB, xloc/xscale+offset_x, zloc/zscale+offset_z);
			//JGL_MTEXCOORD(GL_TEXTURE2_ARB, xloc/xscale, zloc/zscale);
			//JGL_MTEXCOORD(GL_TEXTURE0_ARB, xloc/xscale, zloc/zscale);
		}
		else
			JGL_MTEXCOORD(GL_TEXTURE0_ARB, xloc/xscale, zloc/zscale);
		
		if (flags == 2)
		{
    		JGL_MTEXCOORD(GL_TEXTURE1_ARB, xloc/detailscale, zloc/detailscale);
			
			float maxwet = WATEROFFSET+2.0f;
			float minwet = 0.0f;//WATEROFFSET-2.0f;
			float wettexu = xloc/15.0f;
			float wettexv = (yloc - minwet)/(maxwet-minwet);
			if (wettexv < 0)
			   wettexv = 0;
			//cout << texu << "," << texv << endl;
			JGL_MTEXCOORD(GL_TEXTURE2_ARB, wettexu, wettexv);
			//JGL_MTEXCOORD(GL_TEXTURE2_ARB, xloc/detailscale, zloc/detailscale);
		}
		
		//fake lighting!
		if (flags == 1)
			setwatercolor(n1, v1);
		if (flags == 2)
			setlandcolor(n1, v1, yloc);
		
		glNormal3fv(n1);
		glVertex3fv(v1);
		
		xloc = v2[0];
		yloc = yheight2;//v2[1];
		zloc = v2[2];
		
		//cloudmap texture mapping
		JGL_MTEXCOORD(GL_TEXTURE3_ARB, xloc/cloudscale+cloudoffset_x, zloc/cloudscale+cloudoffset_z);
		if (flags == 1)
		{
			JGL_MTEXCOORD(GL_TEXTURE0_ARB, xloc/xscale+offset_x, zloc/zscale+offset_z);
			JGL_MTEXCOORD(GL_TEXTURE2_ARB, xloc/xscale+offset_x, zloc/zscale+offset_z);
			//JGL_MTEXCOORD(GL_TEXTURE2_ARB, xloc/xscale, zloc/zscale);
			//JGL_MTEXCOORD(GL_TEXTURE0_ARB, xloc/xscale, zloc/zscale);
		}
		else
			JGL_MTEXCOORD(GL_TEXTURE0_ARB, xloc/xscale, zloc/zscale);
		
		if (flags == 2)
		{
    		JGL_MTEXCOORD(GL_TEXTURE1_ARB, xloc/detailscale, zloc/detailscale);
			
			float maxwet = WATEROFFSET+2.0f;
			float minwet = 0.0f;//WATEROFFSET-2.0f;
			float wettexu = xloc/15.0f;
			float wettexv = (yloc - minwet)/(maxwet-minwet);
			if (wettexv < 0)
			   wettexv = 0;
			//cout << texu << "," << texv << endl;
			JGL_MTEXCOORD(GL_TEXTURE2_ARB, wettexu, wettexv);
			//JGL_MTEXCOORD(GL_TEXTURE2_ARB, xloc/detailscale, zloc/detailscale);
		}
		
		//fake lighting!
		if (flags == 1)
			setwatercolor(n2, v2);
		if (flags == 2)
			setlandcolor(n2, v2, yloc);

		glNormal3fv(n2);
		glVertex3fv(v2);*/
	}
	else
	{
		/* leaf of recursion (small projected error or max level): draw tri */
		glNormal3fv(n0); glTexCoord2fv(t0); glVertex3fv(v0);
		glNormal3fv(n1); glTexCoord2fv(t1); glVertex3fv(v1);
		glNormal3fv(n2); glTexCoord2fv(t2); glVertex3fv(v2);
	}
	
    glEnd();
}

/*float TERRAIN::lookup_detail_height_nearest(float xloc, float zloc)
{
	//scaling factor
	const float detailprop = 0.001f;
	const float detailpropy = 0.0008f;
	//const float detailpropy = 0.0025f;
	
	unsigned char * imagedat;
	imagedat = (unsigned char *) detail_height_data->pixels;
	
	//calculate position in detail texture
	float xmod = xloc - (float) ((int) (xloc/(detailprop*size_x)))*(detailprop*size_x);
	float zmod = zloc - (float) ((int) (zloc/(detailprop*size_z)))*(detailprop*size_z);
	float xc = (xmod/(size_x*detailprop))*(float)detail_height_data->w;
	float zc = (zmod/(size_z*detailprop))*(float)detail_height_data->h;
	
	int xcoord = (int)xc;
	int zcoord = (int)zc;
	
	float height = (((float)imagedat[zcoord*detail_height_data->w+xcoord])/255.0f)*((float)size_y*detailpropy);
	
	return height;
}*/
/*
VERTEX TERRAIN::lookup_normal_water(float xloc, float zloc, int l)
{
	float scalefactor = WATER_SCALE_XZ;
	VERTEX wnorm;
	
	float v0[3], v1[3], v2[3];
	
	float normoffset;
	float maxfactor = 40000;
	if (l <= 1)
		normoffset = maxfactor;
	else
		normoffset = maxfactor/(l*l*l);
	//normoffset = 0.5f;
	//cout << normoffset << endl;
	
	float heightscale = WATER_SCALE_Y*(windmag/0.001f);
	if (heightscale < 0.01f)
		heightscale = 0.01f;
	float depth = lookup_height_nearest(xloc, zloc);
	float nowavesat = WATEROFFSET;
	float minwaves = 0.3f;
	if (depth > nowavesat)
		depth = nowavesat;
	depth = depth / nowavesat;
	depth = 1.0f - depth;
	if (depth < minwaves)
		depth = minwaves;
	heightscale *= depth;
	
	v0[0] = xloc;
	v0[1] = water.GetHeightAnywhereFFT(xloc/scalefactor,zloc/scalefactor)*heightscale;
	v0[2] = zloc;
	
	v1[0] = xloc+normoffset;
	v1[1] = water.GetHeightAnywhereFFT((xloc+normoffset)/scalefactor,zloc/scalefactor)*heightscale;
	v1[2] = zloc;
	
	v2[0] = xloc;
	v2[1] = water.GetHeightAnywhereFFT(xloc/scalefactor,(zloc+normoffset)/scalefactor)*heightscale;
	v2[2] = zloc+normoffset;
	
	wnorm = normalfrompoints(v0,v1,v2);
	//wnorm.y = 410.0f / (float)(l*l);
	//wnorm.y = 20000.0f / (float)(l*l*l);
	//wnorm = wnorm.normalize();
	
	//wnorm.x = wnorm.z = 0;	wnorm.y = 1;
	
	return wnorm;
	
	//cout << xloc << zloc
	
	//if (l > 30)
	//	cout << xloc << "," << zloc << ": " << wnorm.x << "," << wnorm.y << "," << wnorm.z << endl;
}

float TERRAIN::lookup_height_water(float xloc, float zloc)
{
	float scalefactor = WATER_SCALE_XZ;
	
	float heightscale = WATER_SCALE_Y*(windmag/0.001f);
	if (heightscale < WATER_SCALE_Y / 10.0f)
		heightscale = WATER_SCALE_Y / 10.0f;
	
	float depth = lookup_height_nearest(xloc, zloc);
	
	float nowavesat = WATEROFFSET;
	float minwaves = 0.3f;
	
	if (depth > nowavesat)
		depth = nowavesat;
	depth = depth / nowavesat;
	depth = 1.0f - depth;
	if (depth < minwaves)
		depth = minwaves;
	
	heightscale *= depth;
	
	//return water.GetHeight(xloc/scalefactor, zloc/scalefactor)+WATEROFFSET;
	return heightscale*water.GetHeightAnywhereFFT(xloc/scalefactor,zloc/scalefactor)+WATEROFFSET;
}*/

/*float TERRAIN::lookup_height_detailoffset(float xloc, float zloc)
{
	
	//clamp
	if (xloc < 0)
		xloc = 0;
	if (zloc < 0)
		zloc = 0;
	if (xloc > size_x)
		xloc = size_x;
	if (zloc > size_z)
		zloc = size_z;
	
	//calculate image-relative coords
	int xcoord,zcoord;
	float fxc, fzc;
	fxc = ((xloc / (float) size_x)*(float)H_DATA_RES);
	fzc = ((zloc / (float) size_z)*(float)H_DATA_RES);
	xcoord = (int) fxc;
	zcoord = (int) fzc;
	float dx, dz;
	dx = fxc-xcoord;
	dz = fzc-zcoord;
	
	//bilinear interpolation for main heightmap
	float h1 = lookup_height_int(xcoord,zcoord);
	float h2 = lookup_height_int(xcoord+1,zcoord);
	float h3 = lookup_height_int(xcoord,zcoord+1);
	float h4 = lookup_height_int(xcoord+1,zcoord+1);
	
	float heightline1 = (h1*(1.0f-dx)+h2*dx);
	float heightline2 = (h3*(1.0f-dx)+h4*dx);
	
	//add detail offset to heightmap interpolation solution
	float height = (heightline1*(1.0f-dz)+heightline2*dz)+1.0f*lookup_detail_height_nearest(xloc,zloc)-0.07f;
	
	//if (height > 100)
	//	cout << xloc << "," << zloc << ": " << height << endl;
	
	return height;
}*/

float TERRAIN::lookup_height(float xloc, float zloc)
{
	//calculate image-relative coords
	int xcoord,zcoord;
	float fxc, fzc;
	fxc = ((xloc / (float) size_x)*(float)H_DATA_RES);
	fzc = ((zloc / (float) size_z)*(float)H_DATA_RES);
	xcoord = (int) fxc;
	zcoord = (int) fzc;
	float dx, dz;
	dx = fxc-xcoord;
	dz = fzc-zcoord;
	
	//bilinear interpolation for main heightmap
	float h1 = lookup_height_int(xcoord,zcoord);
	float h2 = lookup_height_int(xcoord+1,zcoord);
	float h3 = lookup_height_int(xcoord,zcoord+1);
	float h4 = lookup_height_int(xcoord+1,zcoord+1);
	
	float heightline1 = (h1*(1.0f-dx)+h2*dx);
	float heightline2 = (h3*(1.0f-dx)+h4*dx);
	
	//add detail offset to heightmap interpolation solution
	float height = (heightline1*(1.0f-dz)+heightline2*dz);//+lookup_detail_height_nearest(xloc,zloc);
	
	return height;
}

float TERRAIN::lookup_height_half(float xloc, float zloc)
{
	//calculate image-relative coords
	int xcoord,zcoord;
	float fxc, fzc;
	fxc = (((xloc-offset.x) / (float) size_x)*((float)H_DATA_RES/2.0f));
	fzc = (((zloc-offset.z) / (float) size_z)*((float)H_DATA_RES/2.0f));
	xcoord = (int) fxc;
	zcoord = (int) fzc;
	float dx, dz;
	dx = fxc-(float)xcoord;
	dz = fzc-(float)zcoord;
	
	//bilinear interpolation for main heightmap
	float h1,h2,h3,h4, height;
	if (dx != 0 || dz != 0)
	{
		h1 = lookup_height_int((xcoord*2),(zcoord*2));
		h2 = lookup_height_int((xcoord+1)*2,zcoord*2);
		h3 = lookup_height_int(xcoord*2,(zcoord+1)*2);
		h4 = lookup_height_int((xcoord+1)*2,(zcoord+1)*2);
		
		float heightline1 = (h1*(1.0f-dx)+h2*dx);
		float heightline2 = (h3*(1.0f-dx)+h4*dx);
		
		//add detail offset to heightmap interpolation solution
		height = (heightline1*(1.0f-dz)+heightline2*dz);
	}
	else
		height = lookup_height_int(xcoord*2,zcoord*2);
	//float height = (heightline2*(1.0f-dz)+heightline1*dz);
	
	//DEBUG
	//if (h1 != h2 && h2 != h3 && h3 != h4 && h1 > 30 && h1 < 50 && dx < 0.5 && dx > 0)
	//if (h1 == 41.0574)
		//cout << h1 << "," << h2 << "," << h3 << "," << h4 << ":" << dx << "," << dz << ":" << xloc << "," << fxc << "," << xcoord << ": " << height << endl;
	
	return height;
}

float TERRAIN::get_param_height(float xloc, float zloc)
{
	VERTEX ah[4][4];
	
	float fxc, fzc;
	fxc = ((xloc-offset.x) / (float) size_x);
	fzc = ((zloc-offset.z) / (float) size_z);
	
	int x,y;
	
	/*int intx,inty;
	intx = (int)(fxc*hax);
	inty = (int)(fzc*hay);
	return heightarray[intx+hax*inty]*size_y;*/
	
	int ix, iy;
	ix = (int) (fxc*((hax-1)/3));
	iy = (int) (fzc*((hay-1)/3));
	
	if (fxc >= 1.0f)
		ix--;
	if (fzc >= 1.0f)
		iy--;
	
	int cx = 0;
	int cy = 0;
	
	for (y = iy*3; y < iy*3+4; y++)
	{
		for (x = ix*3; x < ix*3+4; x++)
		{
			/*ah[cy][cx].x = (float) cx / 4.0f;
			ah[cy][cx].z = (float) cy / 4.0f;
			ah[cy][cx].y = heightarray[x+y*hax];*/
			ah[cx][cy].x = (float) cx / 4.0f;
			ah[cx][cy].z = (float) cy / 4.0f;
			if (x+y*hax >= hax * hay)
			{
				cout << cx << "," << cy << endl;
				cout << x << "," << y << ": " << hax << endl;
				cout << fxc << "," << fzc << endl;
				cout << ix << "," << iy << endl << endl;
			}
			ah[cx][cy].y = heightarray[x+y*hax];
			cx++;
			cx = cx % 4;
		}
		cy++;
	}
	
	float px = fxc*(hax-1)/3.0f-ix;
	float py = fzc*(hay-1)/3.0f-iy;
	//printf("%i: %f %i %f\n", hax, fxc, ix, fxc*hax/4.0f-ix);
	
	//return 0.0;
	//return SurfHeight(ah, px, py) * 300.0;
	return (SurfHeight(ah, 1.0-py, 1.0-px) / 1.0) * size_y;
}

/*  Calculates 3rd degree polynomial based on array of 4 points */
/*  and a single variable (u) which is generally between 0 and 1 */
VERTEX TERRAIN::Bernstein(float u, VERTEX *p) 
{
	VERTEX	a, b, c, d, r;

	a = p[0].ScaleR(pow(u,3));
	b = p[1].ScaleR(3*pow(u,2)*(1-u));
	c = p[2].ScaleR(3*u*pow((1-u),2));
	d = p[3].ScaleR(pow((1-u),3));

	//r = pointAdd(pointAdd(a, b), pointAdd(c, d));
	r = a+b+c+d;

	return r;
}

float TERRAIN::SurfHeight(VERTEX anchors[][4], float px, float py)
{
	VERTEX		temp[4];

	//get splines along x axis
	temp[0] = Bernstein(px, anchors[0]);
	temp[1] = Bernstein(px, anchors[1]);
	temp[2] = Bernstein(px, anchors[2]);
	temp[3] = Bernstein(px, anchors[3]);
	
	return Bernstein(py, temp).y;
	//return 0.0;
}

float TERRAIN::lookup_height_half_param(float xloc, float zloc)
{
	//calculate image-relative coords
	int xcoord,zcoord;
	float fxc, fzc;
	fxc = (((xloc-offset.x) / (float) size_x)*((float)H_DATA_RES/2.0f));
	fzc = (((zloc-offset.z) / (float) size_z)*((float)H_DATA_RES/2.0f));
	xcoord = (int) fxc;
	zcoord = (int) fzc;
	float dx, dz;
	dx = fxc-(float)xcoord;
	dz = fzc-(float)zcoord;
	
	//bilinear interpolation for main heightmap
	float h1,h2,h3,h4, height;
	if (dx != 0 || dz != 0)
	{
		h1 = lookup_height_int((xcoord*2),(zcoord*2));
		h2 = lookup_height_int((xcoord+1)*2,zcoord*2);
		h3 = lookup_height_int(xcoord*2,(zcoord+1)*2);
		h4 = lookup_height_int((xcoord+1)*2,(zcoord+1)*2);
		
		float heightline1 = (h1*(1.0f-dx)+h2*dx);
		float heightline2 = (h3*(1.0f-dx)+h4*dx);
		
		//add detail offset to heightmap interpolation solution
		height = (heightline1*(1.0f-dz)+heightline2*dz);
	}
	else
		height = lookup_height_int(xcoord*2,zcoord*2);
	//float height = (heightline2*(1.0f-dz)+heightline1*dz);
	
	//DEBUG
	//if (h1 != h2 && h2 != h3 && h3 != h4 && h1 > 30 && h1 < 50 && dx < 0.5 && dx > 0)
	//if (h1 == 41.0574)
		//cout << h1 << "," << h2 << "," << h3 << "," << h4 << ":" << dx << "," << dz << ":" << xloc << "," << fxc << "," << xcoord << ": " << height << endl;
	
	if (height > PARAM_FADEFROM)
		return height;
	if (height < PARAM_FADETO)
		return get_param_height(xloc,zloc);
	else
	{
		float pcoeff = (PARAM_FADEFROM - height)/(PARAM_FADEFROM - PARAM_FADETO);
		return height*(1.0f-pcoeff)+get_param_height(xloc,zloc)*pcoeff;
	}
}

float TERRAIN::lookup_height_param(float xloc, float zloc)
{
	//calculate image-relative coords
	int xcoord,zcoord;
	float fxc, fzc;
	fxc = (((xloc-offset.x) / (float) size_x)*((float)H_DATA_RES/1.0f));
	fzc = (((zloc-offset.z) / (float) size_z)*((float)H_DATA_RES/1.0f));
	xcoord = (int) fxc;
	zcoord = (int) fzc;
	float dx, dz;
	dx = fxc-(float)xcoord;
	dz = fzc-(float)zcoord;
	
	//bilinear interpolation for main heightmap
	/*float h1,h2,h3,h4, height;
	if (dx != 0 || dz != 0)
	{
		h1 = lookup_height_int((xcoord),(zcoord));
		h2 = lookup_height_int((xcoord+1),zcoord);
		h3 = lookup_height_int(xcoord,(zcoord+1));
		h4 = lookup_height_int((xcoord+1),(zcoord+1));
		
		float heightline1 = (h1*(1.0f-dx)+h2*dx);
		float heightline2 = (h3*(1.0f-dx)+h4*dx);
		
		//add detail offset to heightmap interpolation solution
		height = (heightline1*(1.0f-dz)+heightline2*dz);
	}
	else
	{
		height = lookup_height_int(xcoord,zcoord);
	}*/
	//float height = (heightline2*(1.0f-dz)+heightline1*dz);
	
	float fx = (xloc-offset.x) / (float) size_x;
	float fz = (zloc-offset.z) / (float) size_z;
	float height = utility.GetValue(height_data, 0, fx, fz, false, false)*size_y;
	
	//DEBUG
	//if (h1 != h2 && h2 != h3 && h3 != h4 && h1 > 30 && h1 < 50 && dx < 0.5 && dx > 0)
	//if (h1 == 41.0574)
		//cout << h1 << "," << h2 << "," << h3 << "," << h4 << ":" << dx << "," << dz << ":" << xloc << "," << fxc << "," << xcoord << ": " << height << endl;
	
	float mask = lookup_mask_int(xcoord, zcoord)-0.1;
	if (mask < 0)
		mask = 0;
	if (mask > 1)
		mask = 1;
	
	/*if (height > PARAM_FADEFROM)
		return height;
	if (height < PARAM_FADETO)
		return get_param_height(xloc,zloc);
	else
	{
		float pcoeff = (PARAM_FADEFROM - height)/(PARAM_FADEFROM - PARAM_FADETO);
		return height*(1.0f-pcoeff)+get_param_height(xloc,zloc)*pcoeff;
	}*/
	if (mask == 1.0)
	//if (mask > 0.98)
		return height;
	else
		return height*(mask)+get_param_height(xloc,zloc)*(1.0f-mask);
	
	//return get_param_height(xloc,zloc);
	
	//return 13.4972;
	//return 0.0;
}

float TERRAIN::lookup_height_nearest(float xloc, float zloc)
{
	//calculate image-relative coords
	int xcoord,zcoord;
	float fxc, fzc;
	fxc = (((xloc-offset.x) / (float) size_x)*(float)H_DATA_RES);
	fzc = (((zloc-offset.z) / (float) size_z)*(float)H_DATA_RES);
	xcoord = (int) fxc;
	zcoord = (int) fzc;
	float dx, dz;
	dx = fxc-(float)xcoord;
	dz = fzc-(float)zcoord;
	
	//bilinear interpolation for main heightmap
	float h1,h2,h3,h4, height;
	if (dx != 0 || dz != 0)
	{
		h1 = lookup_height_int(xcoord,zcoord);
		h2 = lookup_height_int(xcoord+1,zcoord);
		h3 = lookup_height_int(xcoord,zcoord+1);
		h4 = lookup_height_int(xcoord+1,zcoord+1);
		
		float heightline1 = (h1*(1.0f-dx)+h2*dx);
		float heightline2 = (h3*(1.0f-dx)+h4*dx);
		
		//add detail offset to heightmap interpolation solution
		height = (heightline1*(1.0f-dz)+heightline2*dz);
	}
	else
		height = lookup_height_int(xcoord,zcoord);
	//float height = (heightline2*(1.0f-dz)+heightline1*dz);
	
	//DEBUG
	//if (h1 != h2 && h2 != h3 && h3 != h4 && h1 > 30 && h1 < 50 && dx < 0.5 && dx > 0)
	//if (h1 == 41.0574)
		//cout << h1 << "," << h2 << "," << h3 << "," << h4 << ":" << dx << "," << dz << ":" << xloc << "," << fxc << "," << xcoord << ": " << height << endl;
	
	return height;
	
	
	//float h1 = lookup_height_int(xcoord,zcoord);
	//return h1;
	
	//DEBUG
	//return (xloc+zloc)/30.0;
	
	//add detail offset to heightmap interpolation solution
	//float height = h1+lookup_detail_height_nearest(xloc,zloc);
	//return height;
}

float TERRAIN::lookup_mask_int(int xcoord, int zcoord)
{
	//unsigned short int * imagedat;
	//imagedat = (unsigned short int *) height_mask;
	
	unsigned char * imagedat;
	imagedat = (unsigned char *) height_mask->pixels;
	
	//clamp
	if (xcoord < 0)
		xcoord = 0;
	if (zcoord < 0)
		zcoord = 0;
	if (xcoord >= height_mask->w)
		xcoord = height_mask->w-1;
	if (zcoord >= height_mask->h)
		zcoord = height_mask->h-1;
	
	//repeat
	/*xcoord %= H_DATA_RES;
	zcoord %= H_DATA_RES;
	if (zcoord < 0)
		zcoord += H_DATA_RES;
	if (xcoord < 0)
		xcoord += H_DATA_RES;*/
	
	unsigned long idx = zcoord*height_mask->w+xcoord;
	
	float height = (((float)imagedat[idx])/255.0f);
	
	return height;
}

float TERRAIN::lookup_height_int(int xcoord, int zcoord)
{
	unsigned char * imagedat;
	imagedat = (unsigned char *) height_data->pixels;
	
	//clamp
	if (xcoord < 0)
		xcoord = 0;
	if (zcoord < 0)
		zcoord = 0;
	if (xcoord >= height_data->w)
		xcoord = height_data->w-1;
	if (zcoord >= height_data->h)
		zcoord = height_data->h-1;
	
	//repeat
	/*xcoord %= H_DATA_RES;
	zcoord %= H_DATA_RES;
	if (zcoord < 0)
		zcoord += H_DATA_RES;
	if (xcoord < 0)
		xcoord += H_DATA_RES;*/
	
	unsigned long idx = zcoord*height_data->w+xcoord;
	
	float height = (((float)imagedat[idx])/255.0f)*((float)size_y);
	
	return height;
}

void TERRAIN::drawterrain(float *v0,float *v1,float *v2,float *n0, float *n1, 
		float *n2, float * t0, float * t1, float * t2, int cullflags,
		CAMERA * pcam, int lmax, float lfactor,
		float (TERRAIN::*lookup_height_func)(float, float),
		VERTEX (TERRAIN::*lookup_normal_func)(float, float, int),
		void (TERRAIN::*lookup_texcoord_func)(float *, float, float),
		int flags,
		float popfactor
		)
{
	drawdata.lmax = lmax;
	drawdata.lfactor = lfactor;
	drawdata.lookup_height_func = lookup_height_func;
	drawdata.lookup_normal_func = lookup_normal_func;
	drawdata.lookup_texcoord_func = lookup_texcoord_func;
	drawdata.flags = flags;
	drawdata.cam = pcam;
	drawdata.popfactor = popfactor;
	
	drawsub(0,v0,v1,v2, n0, n1, n2, t0, t1, t2, 0);
}

//detail level is zero to 10, with 0 being infinitely high detail
void TERRAIN::Draw(CAMERA & cam, float detail_level, float timefactor, float fps, float day_time)
{
//	int i;
	
	#ifdef PERFORMANCE_PROFILE
	suseconds_t t1, t2;
	t1 = GetMicroSeconds();
	t1 = GetMicroSeconds();
	#endif
	
	cam.ExtractFrustum();
	
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	
	glPushMatrix();
	
	glTranslatef(0.0,-0.2,0);


	
	//debug test of the curved surfaces stuff
	/*float vtest[3][3];
	vtest[0][0] = 1999;
	vtest[0][1] = 0;
	vtest[0][2] = 2000;
	vtest[1][0] = 2000;
	vtest[1][1] = 0;
	vtest[1][2] = 2000;
	rect_to_sphere_inline(vtest[0]);
	rect_to_sphere_inline(vtest[1]);
	cout << "output[0]: " << vtest[0][0] << "," << vtest[0][1] << "," << vtest[0][2] << endl;
	cout << "output[1]: " << vtest[1][0] << "," << vtest[1][1] << "," << vtest[1][2] << endl;
	for (i = 0; i < 3; i++)
		vtest[2][i] = (vtest[0][i]+vtest[1][i])*0.5f;
	cout << "input[2]: " << vtest[2][0] << "," << vtest[2][1] << "," << vtest[2][2] << endl;
	rect_to_sphere_inline(vtest[2]);
	cout << "output[2]: " << vtest[2][0] << "," << vtest[2][1] << "," << vtest[2][2] << endl;
	cout << endl;*/
	
	
	
	//draw_water = false;
	
	//if (!reflection)
		//backdrop.Draw(GetSize());
	
	//glClipPlane(GL_CLIP_PLANE1, equation);
	//if (reflection)
	{
		//glEnable(GL_CLIP_PLANE1);

		/*if (!cullfront)
		{
			//second clip plane to reduce reflection artifacts for
			// positive part of land rendering
			equation[1] = -1;
			equation[3] = (WATEROFFSET)+2.0f;
			glClipPlane(GL_CLIP_PLANE2, equation);
			glEnable(GL_CLIP_PLANE2);
		}*/
	}
	
	
	#ifdef PERFORMANCE_PROFILE
	t2 = GetMicroSeconds();
	cout << "\tInitial setup and matrix extraction: " << t2-t1 << endl;
	t1 = GetMicroSeconds();
	#endif
	
	
//	if (draw_terrain)
	{

		
		glBindTexture(GL_TEXTURE_2D,texture_handle[0]);
		glEnable(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		
		glEnable( GL_LIGHTING );
		
		glColor3f(1.0f,1.0f,1.0f);
	
		//draw the big one
		float v[4][3];
		float n[4][3];
		float t[4][3];
		
		if (detail_level < 0.1f)
			detail_level = 0.1f;
		if (detail_level > 10.0f)
			detail_level = 10.0f;
		
		detail_level = detail_level * ROAM_LFACTOR;
		
		utility.SelectTU(0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture_handle[0]);
		
		if (utility.numTUs() > 1)
		{		
			utility.SelectTU(1);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texture_handle[1]);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_PREVIOUS_ARB);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB_ARB,GL_MODULATE);
			glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, 2);
		}
	
		if (utility.numTUs() > 2)
		{
			utility.SelectTU(2);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texture_handle[2]);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_PREVIOUS_ARB);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB_ARB,GL_MODULATE);
			glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, 2);
		}
		
		/*#ifdef _WIN32
		pglActiveTexture(GL_TEXTURE1_ARB);
		#else
		glActiveTextureARB(GL_TEXTURE1_ARB);
		#endif
		glEnable(GL_TEXTURE_2D);
		
		// Here we turn on the COMBINE properties and increase our RGB
		// gamma for the detail texture.  2 seems to work just right.
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_PREVIOUS_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB_ARB,GL_MODULATE);
		glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, 2);
		
		// Bind the detail texture
		glBindTexture(GL_TEXTURE_2D, texture_handle[1]);
		
		
		//TU2
		//water caustics
		JGL_SELTEX(GL_TEXTURE2_ARB);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture_handle[5]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//causes artifacts in windows version unless not using the non-conformant
		// texture clamping
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE_ARB);
		glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB_ARB,GL_INTERPOLATE_ARB);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB_ARB,GL_PREVIOUS_ARB);
		glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB_ARB,GL_SRC_COLOR);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB_ARB,GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB_ARB,GL_SRC_COLOR);
		//glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE2_RGB_ARB,GL_PRIMARY_COLOR_ARB);
		//glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND2_RGB_ARB,GL_SRC_COLOR);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE2_RGB_ARB,GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND2_RGB_ARB,GL_ONE_MINUS_SRC_ALPHA);
		glTexEnvi(GL_TEXTURE_ENV,GL_RGB_SCALE_ARB,1);
		if (reflection)
			glDisable(GL_TEXTURE_2D);
		
		
		//TU3
		// setup cloud map
		JGL_SELTEX(GL_TEXTURE3_ARB);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, cloud_map);
		glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, 1);
		//glBindTexture(GL_TEXTURE_2D, texture_handle[3]);
		
		#ifdef PERFORMANCE_PROFILE
		t2 = GetMicroSeconds();
		cout << "\tMisc texture setup: " << t2-t1 << endl;
		t1 = GetMicroSeconds();
		#endif
		
		
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
		glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB_ARB,GL_MODULATE);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB_ARB,GL_PREVIOUS_ARB);
		glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB_ARB,GL_SRC_COLOR);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB_ARB,GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB_ARB,GL_ONE_MINUS_SRC_COLOR);
		glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, 1);
*/		
	
		//four_corners_and_normal(v, n, t, WORLD_SIZE, WORLD_SIZE, -WORLD_SIZE, -WORLD_SIZE);
		four_corners_and_normal(v, n, t, size_x+offset.x, size_z+offset.z, offset.x, offset.z);
		//TODO:  should sample image corners to get image corner height
	
		glDisable(GL_CULL_FACE);
		//glEnable(GL_CULL_FACE);
		
		//drawterrain(v, n, t, 0, BIGMAP_LMAX, detail_level, &TERRAIN::lookup_height_detailoffset, &TERRAIN::lookup_normal, &TERRAIN::lookup_texcoord_bigmap, 2, yheight[0], yheight[1], yheight[3]);
		//drawterrain(v[0],v[1],v[3], n[0], n[1], n[3], t[0], t[1], t[3], 0, &cam, ROAM_LMAX, detail_level, &TERRAIN::lookup_height_half_param, &TERRAIN::lookup_normal_half, &TERRAIN::lookup_texcoord_bigmap, 2, LAND_POPFACTOR);
		//drawterrain(v[3],v[2],v[0], n[3], n[2], n[0], t[3], t[2], t[0], 0, &cam, ROAM_LMAX, detail_level, &TERRAIN::lookup_height_half_param, &TERRAIN::lookup_normal_half, &TERRAIN::lookup_texcoord_bigmap, 2, LAND_POPFACTOR);
		drawterrain(v[0],v[1],v[3], n[0], n[1], n[3], t[0], t[1], t[3], 0, &cam, ROAM_LMAX, detail_level, &TERRAIN::lookup_height_param, &TERRAIN::lookup_normal, &TERRAIN::lookup_texcoord_bigmap, 2, LAND_POPFACTOR);
		drawterrain(v[3],v[2],v[0], n[3], n[2], n[0], t[3], t[2], t[0], 0, &cam, ROAM_LMAX, detail_level, &TERRAIN::lookup_height_param, &TERRAIN::lookup_normal, &TERRAIN::lookup_texcoord_bigmap, 2, LAND_POPFACTOR);
		
		glDisable(GL_CULL_FACE);
		
		utility.SelectTU(3);
		glDisable(GL_TEXTURE_2D);
		utility.SelectTU(2);
		glDisable(GL_TEXTURE_2D);
		utility.SelectTU(1);
		glDisable(GL_TEXTURE_2D);
		
		utility.SelectTU(0);
		
		glEnable(GL_TEXTURE_2D);
		
		#ifdef PERFORMANCE_PROFILE
		t2 = GetMicroSeconds();
		cout << "\tGeometry render: " << t2-t1 << endl;
		t1 = GetMicroSeconds();
		#endif
		
//		glEnable(GL_FOG);
		
	}
	
		
	
	
	//draw the detail ones
	/*glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,texture_handle[2]);
	glEnable( GL_LIGHTING );
	glAlphaFunc(GL_GREATER, 0.9f);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	
	int seg;
	float sx, sz;
	float camwidth = 10.0f;
	sx = (- (int) cam.position.x) / (int) camwidth - 1;
	sz = (- (int) cam.position.z) / (int) camwidth - 1;
	
	for (seg = 0; seg < 9; seg++) // draw a 9 segment grid 
	{
		float x1, x2, z1, z2;
		
		//advance position based on seg variable
		x1 = sx + seg % 3;
		z1 = sz + seg / 3;
		
		x2 = x1+1;
		z2 = z1+1;
		
		x1 *= camwidth;
		x2 *= camwidth;
		z1 *= camwidth;
		z2 *= camwidth;
		
		//cout << x1 << "," << x2 << ";" << z1 << "," << z2 << endl;
		
		int i;
		
		four_corners_and_normal(v, n, t, x1, z1, x2, z2);
		for (i=0;i<4;i++)
		{
			v[i][1]=lookup_height_detailoffset(v[i][0],v[i][2]);
			lookup_texcoord_detailmap(t[i], v[i][0], v[i][2]);
		}
		
		int l = DETAILMAP_LMAX;
		
		drawsub(0,v[0],v[1],v[3], n[0], n[1], n[3], t[0], t[1], t[3], 0, cam, l, DETAILMAP_LFACTOR, &TERRAIN::lookup_height_detailoffset, &TERRAIN::lookup_normal, &TERRAIN::lookup_texcoord_detailmap);
		drawsub(0,v[3],v[2],v[0], n[3], n[2], n[0], t[3], t[2], t[0], 0, cam, l, DETAILMAP_LFACTOR, &TERRAIN::lookup_height_detailoffset, &TERRAIN::lookup_normal, &TERRAIN::lookup_texcoord_detailmap);
	}*/
	
	glEnable( GL_LIGHTING );
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glPopAttrib();
	glPopMatrix();
	
	//glClear (GL_DEPTH_BUFFER_BIT);
}

void TERRAIN::four_corners_and_normal(float v[4][3], float n[4][3], float t[4][3], float x1, float z1, float x2, float z2)
{
	//cout << "TERRAIN::four_corners_and_normal: This is the rectangular version of this function and is deprecated." << endl;
	
	/* compute four corners of the base square */
	int i;
	for (i=0;i<4;i++) {
		v[i][0]=((i&1)?(float)x1:x2);
		v[i][2]=((i&2)?(float)z1:z2);
		v[i][1]=0.0f;
		
		t[i][0]=((i&1)?(float)1.0f:0.0f);
		t[i][1]=((i&2)?(float)1.0f:0.0f);
		
		n[i][0] = 0;
		n[i][1] = 1;
		n[i][2] = 0;
	}
}

void TERRAIN::SetFrustum(CAMERA &cam)
{
	VERTEX campos = cam.GetPosition();
	xcf = -campos.x;
	ycf = -campos.y;
	zcf = -campos.z;
}

VERTEX TERRAIN::GetSize()
{
	VERTEX temp;
	temp.x = size_x;
	temp.y = size_y;
	temp.z = size_z;
	
	return temp;
}

VERTEX TERRAIN::normalfrompoints(float * v0, float * v1, float * v2)
{
	VERTEX normal;
	
	float vb[3], va[3];
    int i;
    for (i = 0; i < 3; i++)
    {
    	va[i] = v0[i]-v1[i];	
		vb[i] = v2[i]-v1[i];
    }
    float norm[3];
    norm[0] = va[1]*vb[2]-vb[1]*va[2];
    norm[1] = va[2]*vb[0]-vb[2]*va[0];
    norm[2] = va[0]*vb[1]-vb[0]*va[1];
    float normmag = sqrt(norm[0]*norm[0]+norm[1]*norm[1]+norm[2]*norm[2]);
    for (i = 0; i < 3; i++)
    {
    	norm[i]=norm[i]/normmag;
    }
	
	/*if (norm[1] < 0)
	{
		norm[0] = -norm[0];
		norm[1] = -norm[1];
		norm[2] = -norm[2];
	}*/
	
	normal.x=norm[0];
	normal.y=norm[1];
	normal.z=norm[2];
	
	return normal;
}

GLuint TERRAIN::GetTexture(int index)
{
	index %= NUM_TERRAIN_TEX;
	if (index < 0)
		index += NUM_TERRAIN_TEX;
	return texture_handle[index];
}

void TERRAIN::Update(CAMERA & cam, float timefactor, float fps,  float day_time)
{
//	int i;
	
	/*cam.position.DebugPrint();
	cam.dir.DebugPrint();*/
	
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	
	glEnable(GL_TEXTURE_2D);
	
	cam.ExtractFrustum();
	
	
	//extract opengl matrices for later use in reflection calculations
	/*glPushMatrix();
	cam.dir.GetMat(temp_matrix);
	glLoadMatrixd(temp_matrix);
	glTranslatef(cam.GetPosition().x,cam.GetPosition().y,cam.GetPosition().z);
	glGetDoublev(GL_MODELVIEW_MATRIX, temp_matrix);
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity( );
	gluPerspective( 70.0f, (float)SCREEN_WIDTH/SCREEN_HEIGHT, 0.1f, 10000.0f );
	glGetDoublev(GL_PROJECTION_MATRIX, temp_proj);
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	
	glGetIntegerv(GL_VIEWPORT, temp_viewport);
	glPopMatrix();	*/
	

	//update water depth -- no longer used by anything
	//water.SetCamDepth(WATEROFFSET-lookup_height(-cam.position.x,-cam.position.z));
	
	//re-render heightmap
	//water.animateWater(timefactor, fps);
	
	
	//regen cloud texture map
	/*int cloudsize = sdl_cloud_map->h*sdl_cloud_map->w;
	GLbyte *pin, *pout;
	int cloudthresh = weathersystem.GetCloudCover();
	GLbyte cloudfluffiness = weathersystem.GetCloudFuzziness();
	if (!reflection)
	{
		for (i = 0; i < cloudsize; i++)
		{
			pin = &(((GLbyte *) sdl_cloud_map_original->pixels)[i*4]);
			pout = &(((GLbyte *) sdl_cloud_map->pixels)[i*4]);
			
			
			//slight optimization
			pout[0] = pin[0]+127;
			pout[1] = pout[0];
			pout[2] = pout[0];
			pout[3] = pin[3]+127;
			
			//pout[3] = (pout[3] & 127) << 1;
			if (pout[3] <= cloudthresh-cloudfluffiness)
			{
				pout[3] = 0;
			}
			else if (pout[3] >= cloudthresh+cloudfluffiness)
			{
				pout[3] = 127;
			}
			else
			{
				//blend!
				pout[3] = (127*(pout[3]-(cloudthresh-cloudfluffiness)))/(2*cloudfluffiness);
			}
		}
	}
	
	
	glBindTexture(GL_TEXTURE_2D, cloud_tex);
	
	if (!reflection)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sdl_cloud_map->w, sdl_cloud_map->h, 0, GL_RGBA,
			GL_BYTE, sdl_cloud_map->pixels);
	}
	

	
	
	//regen cloud shadow texture
	glBindTexture(GL_TEXTURE_2D, cloud_map);
	GLbyte cloud_map_data[cloudsize];
	GLbyte ptemp;
	int shiftfade;
	float dayout = day_time;
	float daythresh = 0.1f;
	if (dayout >= 0.5)
		dayout = 0.0f;
	if (dayout < 0.5f && dayout > 0.5f - daythresh)
		dayout = 0.5-dayout;
	dayout /= daythresh;
	if (dayout > 1.0f)
		dayout = 1.0f;
	shiftfade = (int) ((1.0f-dayout)*8+1.0f);
	if (shiftfade > 8)
		shiftfade = 8;
	dayout *= 0.5f;
	for (i = 0; i < cloudsize; i++)
	{
		ptemp = ((GLbyte *) sdl_cloud_map->pixels)[i*4+3];
		//ptemp /= 2;
		//ptemp = ptemp >> shiftfade;
		//ptemp = ptemp >> 1;
		ptemp = (signed char) (ptemp*dayout);
		cloud_map_data[i] = ptemp;
	}
	
	#ifdef PERFORMANCE_PROFILE
	t2 = GetMicroSeconds();
	cout << "\tCloud shadow regen: " << t2-t1 << endl;
	t1 = GetMicroSeconds();
	#endif
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE8, sdl_cloud_map->w, sdl_cloud_map->h, 0, GL_LUMINANCE,
			 GL_BYTE, &cloud_map_data);
	
	#ifdef PERFORMANCE_PROFILE
	t2 = GetMicroSeconds();
	cout << "\tCloud shadow re-upload: " << t2-t1 << endl;
	t1 = GetMicroSeconds();
	#endif
	
	
	glBindTexture(GL_TEXTURE_2D, water_alpha_map);

	//the scale of the water transparency map is also dependent on the camera
	// height.  when the camera is high above the water, the trans map
	// looks tiled and crappy, so i fade it out here.
	float ch[3];
	ch[0] = -cam.position.x;
	ch[1] = -cam.position.y;
	ch[2] = -cam.position.z;
	float camheight = getheight(ch);
	float startat = WATEROFFSET;
	float goneat = startat + 10;
	float coef = camheight - startat;
	coef = coef / (goneat - startat);
	if (coef < 0)
		coef = 0;
	if (coef > 1.0f)
		coef = 1.0f;
	
	coef = 1.0f - coef;
	
	//disable this effect
	coef = 1.0f;
	
	
	
	#ifdef PERFORMANCE_PROFILE
	t2 = GetMicroSeconds();
	cout << "\tAlpha map gain adjustment: " << t2-t1 << endl;
	t1 = GetMicroSeconds();
	#endif


	//move wind
	wind = weathersystem.GetGlobalWind();
	windmag = sqrt(wind.x*wind.x+wind.z*wind.z);
	MAX_REF_DIST2 = 1000.0f*1000.0f;
	//MAX_REF_DIST2 = 10000.0f*10000.0f;
	
	
	float depth = lookup_height_nearest(-cam.position.x, -cam.position.z);
	float nowavesat = WATEROFFSET;
	if (depth > nowavesat)
		depth = nowavesat;
	depth = depth / nowavesat;
	depth = 1.0f - depth;
	bumpwindoffset_x += (timefactor/fps)*wind.x*depth;
	bumpwindoffset_z += (timefactor/fps)*wind.z*depth;
	
	windoffset_x += (timefactor/fps)*wind.x;
	windoffset_z += (timefactor/fps)*wind.z;
	backdrop.SetWindoffset(windoffset_x, windoffset_z);
	//clip to ten -- this is necessary because the backdrop currently uses
	// this going to 10
	while (windoffset_x > 10)
		windoffset_x -= 10.0f;
	while (windoffset_x < 0)
		windoffset_x += 10.0f;
	while (windoffset_z > 10)
		windoffset_z -= 10.0f;
	while (windoffset_z < 0)
		windoffset_z += 10.0f;
	while (bumpwindoffset_x > 10)
		bumpwindoffset_x -= 10.0f;
	while (bumpwindoffset_x < 0)
		bumpwindoffset_x += 10.0f;
	while (bumpwindoffset_z > 10)
		bumpwindoffset_z -= 10.0f;
	while (bumpwindoffset_z < 0)
		bumpwindoffset_z += 10.0f;
	*/
	glPopAttrib();
}

float TERRAIN::GetHeight(float x, float z)
{
	//return (*this.*(drawdata.lookup_height_func))(x,z);
	return lookup_height_param(x,z);
}

VERTEX TERRAIN::GetOffset()
{
	return offset;
}

int TERRAIN::GetRoamLMax()
{
	int val = 14;
	
	if (state.GetTerrainDetail() == "Med")
		val = 16;
	if (state.GetTerrainDetail() == "High")
		val = 18;
	
	return val;
}

float TERRAIN::GetRoamLFactor()
{
	float val = 0.000004f;
	
	if (state.GetTerrainDetail() == "Med")
		val = 0.000003f;
	if (state.GetTerrainDetail() == "High")
		val = 0.000002f;
	
	return val;
}
