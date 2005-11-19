#ifndef _TERRAIN_H

#include <stdio.h>			// Header File For Standard Input/Output
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <cassert>

#include "camera.h"
#include "utility.h"
#include "gamestate.h"

//#include "backdrop.h"
//#include "weather.h"

#include "globals.h"

using namespace std;

/* maximum bintree level for which index-to-float conversions work */
//#define ROAM_LMAX 18
//#define ROAM_LMAX 14
#define ROAM_LMAX (GetRoamLMax())
//#define ROAM_LMAX 30

#define LAND_POPFACTOR 2.0
#define WATER_POPFACTOR 2.0

//#define BIGMAP_LMAX 21
//#define BIGMAP_LMAX 32
//#define BIGMAP_LMAX 35
//#define DETAILMAP_LMAX 15
//#define ROAM_LMAX 32
//#define ROAM_LMAX 21

//#define ROAM_LFACTOR 0.0000002f
//#define ROAM_LFACTOR 0.000001f
//#define ROAM_LFACTOR 0.000003f
#define ROAM_LFACTOR (GetRoamLFactor())

#define H_DATA_RES 512
#define H_DATA_SIZE (512*512)

//#define H_DATA_RES 1024
//#define H_DATA_SIZE (1024*1024)

#define NUM_TERRAIN_TEX 3

//#define EARTH_RADIUS 50000.0
//#define EARTH_RADIUS 637800.0

//#define WORLD_SIZE 100000.0f

//#define WATEROFFSET 40.0f
//#define WATEROFFSET 31.0f
//#define WATER_LMAX 36
//#define WATER_LMAX 38
/*#define WATER_SCALE_XZ 0.64f
#define WATER_SCALE_Y 0.01f*/

//#define WATER_SCALE_XZ 0.064f
//#define WATER_SCALE_Y 0.001f
//#define WATER_SCALE_XZ 0.064f*4.0f
//#define WATER_SCALE_Y 0.001f*4.0f

//#define WATER_LFACTOR 0.000005f

//#define WATER_LFACTOR .0000000005f
//#define WATER_LFACTOR .000000001f
//#define WATER_LFACTOR .000000005f

//maximum reflection distance squared
//this is now a variable
//#define MAX_REF_DIST2 80000.0f
//#define MAX_REF_DIST2 800000.0f
//#define MAX_REF_DIST2 40000.0f

//size of water bump map
//#define OFFSET_TEX_SIZE 64
//#define OFFSET_TEX_SIZE 512
//whether or not to dynamically animate the water offset texture and alphamap
//#define OFFSET_ANIMATION 1
//#define STATIC_WATERDETAIL 0

//number of octaves in offset water bump map
//#define OFFSET_OCTAVES 7

//size of map to help with noise generation... larger numbers mean fewer
// repeated patterns
//#define NOISEMAP_SIZE 5000

//needed for projection (water reflection uses this)
//extern const int SCREEN_WIDTH;
//extern const int SCREEN_HEIGHT;

/* 64-bit signed and unsigned integers */

#ifdef _WIN32
typedef signed __int64 roam_int64;
typedef unsigned __int64 roam_uint64;
#else
typedef signed long long int roam_int64;
typedef unsigned long long int roam_uint64;
#endif

/* other int types */

typedef int roam_int32;
typedef unsigned int roam_uint32;
typedef short roam_int16;
typedef unsigned short roam_uint16;
typedef signed char roam_int8;
typedef unsigned char roam_uint8;


/*
 * typedefs of various structures referenced
 * should appear before structures are defined
 */

typedef struct roamdm_structdef roamdm_struct,*roamdm;
typedef struct roam_structdef roam_struct,*roam;


#define DMSTOREN     65536 /* number of diamonds in storage pool            */
#define IQMAX         4096 /* number of buckets in priority queue           */
#define TRI_IMAX     65536 /* number of triangle-chunk slots                */
#define DMCHECKLISTN 65536 /* check list size                               */


/* frustum cull bit masks */
#define CULL_ALLIN 0x3f
#define CULL_OUT   0x40


/* misc flags bits */
#define ROAM_SPLIT    0x01
#define ROAM_TRI0     0x04
#define ROAM_TRI1     0x08
#define ROAM_CLIPPED  0x10
#define ROAM_SPLITQ   0x40
#define ROAM_MERGEQ   0x80
#define ROAM_ALLQ     0xc0
#define ROAM_UNQ      0x00


/* parent/kid split flags */
#define SPLIT_K0      0x01
#define SPLIT_K       0x0f
#define SPLIT_P0      0x10
#define SPLIT_P       0x30


/*
 * per-diamond record (carefully packed for alignment)
 */

struct roamdm_structdef {
    roam rm;               /* global record that owns this diamond          */
    roamdm p[2];           /* diamond's parents (two corners)               */
    roamdm a[2];           /* other two corners (a[0]=quadtree parent)      */
    roamdm k[4];           /* diamond's kids                                */
    roamdm q0,q1;          /* prev and next links on queue or free list     */
    float v[3];            /* vertex position                               */
	float n[3];            /* normal                                        */
    float r_bound;         /* radius of sphere bound squared (-1 if NEW)    */
    float r_error;         /* radius of pointwise error squared             */
    roam_int16 iq;         /* index in priority-queue array                 */
    roam_uint16 tri_i[2];  /* triangle output chunk indices (side p0,p1)    */
    roam_int8 i[2];        /* our kid index within each of our parents      */
    roam_int8 l;           /* level of resolution                           */
    roam_uint8 cull;       /* IN/OUT bits for frustum culling               */
    roam_uint8 flags;      /* misc. bit flags (splits etc)                  */
    roam_uint8 lockcnt;    /* number of references (0=FREE)                 */
    roam_uint8 splitflags; /* parent and kid split flags                    */
    roam_uint8 framecnt;   /* frame count of most recent priority update    */
    char padding[2];       /* filler to make 8-byte alignment               */
};


/*
 * global record per ROAM optimizer
 */

struct roam_structdef {
    int lmax;              /* maximum bintree refinement level              */
    float xc,yc,zc;        /* float copy of camera position in world space  */
    float frust_planes[6][4]; /* copy of frustum plane coefficients         */
    float level2dzsize[50+1]; /* max midpoint displacement per level */
    roamdm dmstore;        /* storage pool of diamond records               */
    int dmstoren;          /*   number of diamonds in store                 */
    roamdm dm_free0,dm_free1; /* least and most recent unlocked diamonds    */
    int dmfreecnt;         /*   number of elements on free list             */
    int framecnt;          /* current frame count                           */
    roamdm dm_b0[4][4];    /* base diamonds level 0 (b0[1][1] is domain)    */
    roamdm dm_b1[4][4];    /* base diamonds level -1,-2...                  */
    int iqfine;            /* fine limit on priority index                  */
    roamdm splitq[IQMAX];  /* split queue buckets                           */
    int iqsmax;            /*   max occupied bucket                         */
    roamdm mergeq[IQMAX];  /* merge queue buckets                           */
    int iqmmin;            /*   min occupied bucket                         */
    int tri_imax;          /* max number of tri chunks allowed              */
    int trifreecnt;        /* number of triangles on free list              */
    int tricnt;            /* number of triangles drawn (on tri_in list)    */
    int tricntmax;         /* target triangle count                         */
    int tri_free;          /* first free tri chunk                          */
    int *tri_dmij;         /* packed diamond index and side                 */
    int fixtab[256];       /* correction to float->int conversions          */
    void *step_data;       /* data attachment specific to a library step    */
};

class TERRAIN
{
private:
	int roam_lmax;          /* current max recursion level                      */
	float xcf,ycf,zcf;      /* float copy of camera position in world space     */
	float frust_planes[6][4]; /* copy of frustum plane coefficients             */
	float level2dzsize[50+1]; /* max midpoint displacement per level     */

	//information about current mesh set
	float size_x;
	float size_y;
	float size_z;

	float PARAM_FADEFROM;
	float PARAM_FADETO;

	VERTEX offset;

	//texture info
	SDL_Surface *height_data;
	SDL_Surface *height_mask;

	//unsigned short int height_data[H_DATA_SIZE];
	//unsigned short int height_mask[H_DATA_SIZE];

	//SDL_Surface *detail_height_data;
	//SDL_Surface *sdl_cloud_map_original; //holds UNMODIFIED texture map sdl 
										//  surface
	//SDL_Surface *sdl_cloud_map; //this is the TEXTURE MAP sdl surface
						//the texture map opengl handle is cloud_tex
	GLuint texture_handle[NUM_TERRAIN_TEX];
	//GLuint water_alpha_map;
	//GLuint offset_tex;
	//GLuint cloud_map; //this is the SHADOW MAP
	//GLuint cloud_tex; //this is the cloud texture

	//texture utility
	//void TexLoad(int slot, string filename, int format, bool mipmap);
	//GLuint TexLoad(string filename, int format, bool mipmap);

	//actual roam data structure
	roam_structdef data;

	//recursive draw routine
	
	struct TERRAINDRAWDATA
	{
		int lmax;
		float lfactor;
		float (TERRAIN::*lookup_height_func)(float, float);
		VERTEX (TERRAIN::*lookup_normal_func)(float, float, int);
		void (TERRAIN::*lookup_texcoord_func)(float *, float, float);
		int flags;
		CAMERA * cam;
		float popfactor;
	} drawdata;
	
	void drawterrain(float *v0,float *v1,float *v2,float *n0, float *n1, 
		float *n2, float * t0, float * t1, float * t2, int cullflags,
		CAMERA * pcam, int lmax, float lfactor,
		float (TERRAIN::*lookup_height_func)(float, float),
		VERTEX (TERRAIN::*lookup_normal_func)(float, float, int),
		void (TERRAIN::*lookup_texcoord_func)(float *, float, float),
		int flags,
		float popfactor
		);
	void drawsub(int l,float *v0,float *v1,float *v2,float *n0, float *n1,
		float *n2, float * t0, float * t1, float * t2, int cullflags
		);
	/*void drawsub(int l,float *v0,float *v1,float *v2,float *n0, float *n1, 
		float *n2, float * t0, float * t1, float * t2, int cullflags, 
		CAMERA & cam, int lmax, float lfactor,
		float (TERRAIN::*lookup_height_func)(float, float),
		VERTEX (TERRAIN::*lookup_normal_func)(float, float, int),
		void (TERRAIN::*lookup_texcoord_func)(float *, float, float),
		int flags, float yheight0, float yheight1, float yheight2
		);*/
		
	//function to draw triangles which have multitexturing applied
	void drawmultitextri(float **allv,
		float **alln,
		float *ally, float *ostx, int flags);
	/*bool getoffsettexcoords(float **allv,
		float **alln,
		float *ally, float *ostx, bool * clipped);*/
		
	//clipping functions
	//void ClipToPlane(float polyin[][3], float polyinnorms[][3], int polyinpoints, float polyout[][3], float polyoutnorms[][3], int & polyoutpoints, float * plane);

	//height data lookup helper functions
	//float lookup_detail_height_nearest(float xloc, float zloc);
	float lookup_height_int(int xloc, int zloc);
	float lookup_mask_int(int xloc, int zloc);
	
	//height lookup functions
	//float lookup_height_detailoffset(float xloc, float zloc);
	
	//normal lookup functions
	VERTEX lookup_normal(float xloc, float zloc, int l);
	VERTEX lookup_normal_half(float xloc, float zloc, int l);
	VERTEX lookup_normal_half_param(float xloc, float zloc, int l);
	VERTEX lookup_normal_alwaysup(float xloc, float zloc, int l);
	//VERTEX lookup_normal_water(float xloc, float zloc, int l);
	
	//texcoord lookup functions
	void lookup_texcoord_bigmap(float * tc, float xloc, float zloc);
	void lookup_texcoord_detailmap(float * tc, float xloc, float zloc);
	
	//math utility
	void four_corners_and_normal(float v[4][3], float n[4][3], float t[4][3], float x1, float z1, float x2, float z2);
	void four_corners_and_normal_curved(float v[4][3], float n[4][3], float t[4][3], float x1, float z1, float x2, float z2, float height);
	VERTEX normalfrompoints(float * v0, float * v1, float * v2);
	//this function doesn't really convert coordinate spaces, it actually just
	// corrects the vertex so it has the proper radius for its lat/long
	//void rect_to_sphere_inline(float * v);
	//void get_latlong(float * v, float * olat, float * olong);
	/*GLint coordProject(GLdouble objx, GLdouble objy, GLdouble objz, 
	      const GLdouble modelMatrix[16], 
	      const GLdouble projMatrix[16],
              const GLint viewport[4],
	      GLdouble *winx, GLdouble *winy, GLdouble *winz, GLdouble *winw);*/
		  
	//water coloring, transparency (and more)
	inline void setwatercolor(float * norm, float * v);
	
	//land coloration
	inline void setlandcolor(float * n, float * v, float yloc);

	ofstream error_log;
	
	//storage for calculation of reflection map
	GLdouble temp_matrix[16];
	GLdouble temp_proj[16];
	GLint temp_viewport[4];
	
	//whether or not we're drawing a reflection
	//bool reflection;
	
	//reflection bump map texture data
	/*GLbyte offsetmap[OFFSET_TEX_SIZE*OFFSET_TEX_SIZE*2];
	GLbyte combomap_static[OFFSET_TEX_SIZE*OFFSET_TEX_SIZE];
	GLbyte alphamap[OFFSET_TEX_SIZE*OFFSET_TEX_SIZE];
	float bumpwindoffset_x; //bump map texture offset due to wind
	float bumpwindoffset_z;
	float windoffset_x; //cloud map texture offset due to wind
	float windoffset_z;
	int noise_step;
	float offset_anim_step;
	float MAX_REF_DIST2;*/
	
	//noise stuff used for bump map generation
	/*void GenNoise(float * offsmap, int octave, int step, int num_octaves, 
		float * octave_weighting, int cur_octave);
	GLbyte Noise3D(int x, int y, int z);
	GLbyte Noise3DInterp(float x, float y, int z);
	GLbyte Noise3DInterpTile(float x, float y, int z, int max_x, int max_y);
	void NoiseInit();
	void CombineMaps(GLbyte * newmap, float * sourcemaps, float anim_step, int num_octaves);
	void BuildOffsetMap(GLbyte * offsetmap, GLbyte * heightmap);
	unsigned char noisemap[NOISEMAP_SIZE];*/
	
	//wind stuff
	//VERTEX wind;
	//float windmag;
	
	//performance data
	//int num_water_tris;
	
	//feedback stuff
	//GLfloat feedBuffer[65536];
	//no one told me feedback was insanely slow!
	
	float lookup_height(float xloc, float zloc);
	float lookup_height_nearest(float xloc, float zloc);
	float lookup_height_half(float xloc, float zloc);
	float lookup_height_half_param(float xloc, float zloc);
	float lookup_height_param(float xloc, float zloc);
	//float lookup_height_water(float xloc, float zloc);
	
	float get_param_height(float xloc, float zloc);
	
	float * heightarray;
	
	void FixHeightArray();
	int hax;
	int hay;
	float SurfHeight(VERTEX anchors[][4], float px, float py);
	VERTEX Bernstein(float u, VERTEX *p);
	bool initdone;
	
	int GetRoamLMax();
	float GetRoamLFactor();
	
public:
	TERRAIN();
	~TERRAIN();
	void Init(int newsize_x, int newsize_y, int newsize_z, string datafile, string datamaskfile, string texfile, string detailtexfile, string detailtexfile2, string harray, float offsetx, float offsetz, float fadefrom, float fadeto);
	void DeInit();
	void SetFrustum(CAMERA &cam);
	//void SetTriCount(int trimax);
	//void SetMaxRefinement(int lmax);
	//void SetAccuracy(int iqfine);
	void Update(CAMERA & cam, float timefactor, float fps,  float day_time);
	void Draw(CAMERA & cam, float detail_level, float timefactor, float fps,  float day_time);
	VERTEX GetSize();
	VERTEX GetOffset();
	GLuint GetTexture(int index);
	//GLuint GetCloudTexture();

	//GLuint reflection_texture;

	float GetHeight(float x, float z);
};

#define _TERRAIN_H
#endif
