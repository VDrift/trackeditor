#ifndef MODEL_H

#include <stdio.h>			// Header File For Standard Input/Output
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <cassert>
#include <stdint.h>

#include <string>
#include <vector>

#ifdef __APPLE__
#include <machine/endian.h>
#endif

using namespace std;

#include "quat.h"
//#include "settings.h"
//#include "terrain.h"

#define NUM_COLLIDE_CACHE 10

#define INTERSECT_FUNCTION utility.IntersectTriangleF

#define JOE_MAX_TRIANGLES       4096
#define JOE_MAX_VERTICES        2048
#define JOE_MAX_TEXCOORDS       2048
#define JOE_MAX_FRAMES          512

#define JOE_VERSION 3

//uv coords
#define JOE_MAX_TEXTURES 1
#define MAX_TEXTURE_UNITS 4

#define MAX_FILENAME_LEN 1024

#define TEXTUREMODE_NOTEX 0
#define TEXTUREMODE_TEX 1
#define TEXTUREMODE_REFLECTION 2
#define TEXTUREMODE_ADD 3

//texture files
//#define NUM_TEXTURES 2

//#define MODEL_SCALE 0.63
#define MODEL_SCALE 1.0
//#define MODEL_SCALE 0.1
//#define MODEL_SCALE 0.7

#if BYTE_ORDER == LITTLE_ENDIAN
#define ENDIAN_SWAP_16(A)  (A)
#define ENDIAN_SWAP_32(A)  (A)
#define ENDIAN_SWAP_FLOAT(A)  (A)
#else
#define ENDIAN_SWAP_16(A)  ((((uint16_t)(A) & 0xff00) >> 8) | \
                   (((uint16_t)(A) & 0x00ff) << 8))
#define ENDIAN_SWAP_32(A)  ((((uint32_t)(A) & 0xff000000) >> 24) | \
                   (((uint32_t)(A) & 0x00ff0000) >> 8)  | \
                   (((uint32_t)(A) & 0x0000ff00) << 8)  | \
                   (((uint32_t)(A) & 0x000000ff) << 24))
#define ENDIAN_SWAP_FLOAT(A)  LoadLEFloat(&(A))

inline float LoadLEFloat( float *f )
{
       #define __stwbrx( value, base, index ) \
               __asm__ ( "stwbrx %0, %1, %2" :  : "r" (value), "b%" (index), "r" (base) : "memory" )

       union
       {
               long            i;
               float           f;
       } transfer;

       //load the float into the integer unit
       unsigned int    temp = ((long*) f)[0];

       //store it to the transfer union, with byteswapping
       __stwbrx( temp,  &transfer.i, 0);       

       //load it into the FPU and return it
       return transfer.f;
}
#endif

typedef unsigned char byte;

// This holds the header information that is read in at the beginning of the file
struct JOEHeader
{ 
	int magic;                   // This is used to identify the file
	int version;                 // The version number of the file
	int num_faces;	            // The number of faces (polygons)
	int num_frames;               // The number of animation frames
};

// This is used to store the vertices that are read in for the current frame
struct JOEVertex
{
	float vertex[3];
};

// This stores the indices into the vertex and texture coordinate arrays
struct JOEFace
{
	short vertexIndex[3];
	short normalIndex[3];
	//short textureIndex[JOE_MAX_TEXTURES*3];
	short textureIndex[JOE_MAX_TEXTURES*3];
};

// This stores UV coordinates
struct JOETexCoord
{
	float u, v;
};

// This stores the frames vertices after they have been transformed
struct JOEFrame
{
	int num_verts;
	int num_texcoords;
	int num_normals;
	
	JOEFace * faces;
	JOEVertex * verts;
	JOEVertex * normals;
	JOETexCoord * texcoords;
};

/*// This holds the information for a material.  It may be a texture map of a color.
// Some of these are not used, but I left them because you will want to eventually
// read in the UV tile ratio and the UV tile offset for some models.
struct tMaterialInfo
{
    char  strName[255];         // The texture name
    char  strFile[255];         // The texture file name (If this is set it's a texture map)
    byte  color[3];             // The color of the object (R, G, B)
    int   texureId;             // the texture ID
    float uTile;                // u tiling of texture  (Currently not used)
    float vTile;                // v tiling of texture  (Currently not used)
    float uOffset;              // u offset of texture  (Currently not used)
    float vOffset;              // v offset of texture  (Currently not used)
} ;*/

// This holds all the information for our model/scene. 
struct JOEObject 
{
	JOEHeader info;
	JOEFrame * frames;
};

// This class handles all of the loading code
class JOEMODEL
{

public:
    JOEMODEL();                             // This inits the data members
	~JOEMODEL();

    bool Load(string strFileName) {return Load(strFileName, true);}
	bool Load(string strFileName, bool autoloadtexture);
	void Draw(int frame, float t);
	void Draw(int frame, int nextframe, float t);
	void DrawStatic(); //draw frame 0, optimized for speed

	void NewDraw(int frame, float t, VERTEX lightdir, QUATERNION rotation, int pass);
	void NewDraw(int frame, int nextframe, float t, VERTEX lightdir, QUATERNION rotation, int pass);

	//GLuint GetTex(int tnum);
	//void SetBaseTexture(int tid);

	void NoTexture(int tid) {if (tid < MAX_TEXTURE_UNITS) texturemode[tid] = TEXTUREMODE_NOTEX;}
	void Texture(string texturename, int tid);
	void ReflectionTexture(string texturename, int tid);
	void AdditiveTexture(string texturename, int tid);
	
	void TextureID(GLuint texid, int tid) {if (tid < MAX_TEXTURE_UNITS) {texturemode[tid] = TEXTUREMODE_TEX; textureid[tid] = texid; autounload[tid] = false;}}
	void ReflectionTextureID(GLuint texid, int tid) {if (tid < MAX_TEXTURE_UNITS) {texturemode[tid] = TEXTUREMODE_REFLECTION; textureid[tid] = texid; autounload[tid] = false;}}
	void AdditiveTextureID(GLuint texid, int tid) {if (tid < MAX_TEXTURE_UNITS) {texturemode[tid] = TEXTUREMODE_ADD; textureid[tid] = texid; autounload[tid] = false;}}
	
	void SetTU(int tuid, bool tue) {tuenable[tuid] = tue;}

	float GetRadius() {return radius + 0.5f;}
	float GetRadiusXZ() {return radiusxz;}
	
	unsigned int GetFaces() {if (!loadedfile) return 0; else return pObject->info.num_faces;}
	
	bool Collide(VERTEX origin, VERTEX direction, VERTEX &outtri, bool closest);
	
	inline float * GetVert(short index) {return pObject->frames[0].verts[index].vertex;}
	short * GetFace(short index) {return pObject->frames[0].faces[index].vertexIndex;}
	
	inline float * GetNorm(short index) {return pObject->frames[0].normals[index].vertex;}
	short * GetNormIdx(short index) {return pObject->frames[0].faces[index].normalIndex;}

//private:
	
	int collidecache[NUM_COLLIDE_CACHE];
	int collidecachepos;

	AABB bbox;
	
	string FileToPNG(string filename);
	string FileToTexturesizePNG(string filename);
	
	float radius; //used for frustum culling
	float radiusxz; //used to get an object's vertical profile
	
	GLuint static_list;

	bool loadedfile;
	
	bool goodnorms;
	
	inline void ColorFromNormal(VERTEX &norm, VERTEX &ldir);
	inline void TexCoordFromNormal(VERTEX &norm, VERTEX &ldir);

	void CorrectEndian(struct JOEFace * p, int num);
	void CorrectEndian(struct JOEVertex *p, int num);
	void CorrectEndian(struct JOETexCoord *p, int num);

	// This reads in the data from the MD2 file and stores it in the member variables
	void ReadData();

	// This frees memory and closes the file
	void CleanUp();
	
	// The file pointer
	FILE *m_FilePointer;

	//GLuint texid[NUM_TEXTURES];
	GLuint textureid[MAX_TEXTURE_UNITS];
	int texturemode[MAX_TEXTURE_UNITS];
	bool tuenable[MAX_TEXTURE_UNITS];
	bool autounload[MAX_TEXTURE_UNITS];

	//bool has_paint;
	//int numtex;
	string modelpath;
	//int active_base;
	//bool has_illum;
	//GLuint illum_tex;
	
	// Member variables

	JOEObject * pObject;
	
	/*tMd2Header              m_Header;           // The header data
	tMd2Skin                *m_pSkins;          // The skin data
	tMd2TexCoord            *m_pTexCoords;      // The texture coordinates
	tMd2Face                *m_pTriangles;      // Face index information
	tMd2Frame               *m_pFrames;         // The frames of animation (vertices)
	
	
	int numOfObjects;                   // The number of objects in the model
	int numOfMaterials;                 // The number of materials for the model
	int numOfAnimations;                // The number of animations in this model (NEW)
	int currentAnim;                    // The current index into pAnimations list (NEW)
	//int currentFrame;                   // The current frame of the current animation (NEW)
	vector<tAnimationInfo> pAnimations; // The list of animations (NEW)
	//vector<tMaterialInfo> pMaterials;   // The list of material information (Textures and colors)
	vector<t3DObject> pObject;          // The object list for our model*/
};

#define MODEL_H
#endif
