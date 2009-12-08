#define GL_GLEXT_PROTOTYPES

#include "model.h"

#include <cstring>

JOEMODEL::JOEMODEL()
{
	// Set the pointers to null
	pObject = NULL;
	loadedfile = false;
//	has_paint = false;
	
	radius = 0.0;
	radiusxz = 0.0;
	
	int i;
	for (i = 0; i < MAX_TEXTURE_UNITS; i++)
	{
		texturemode[i] = TEXTUREMODE_NOTEX;
		tuenable[i] = true;
		autounload[i] = true;
	}
	
	collidecachepos = 0;
	for (i = 0; i < NUM_COLLIDE_CACHE; i++)
		collidecache[i] = 0;
}

bool JOEMODEL::Load(string strFileName, bool autoloadtexture)
{
	modelpath = strFileName;
	
	if (loadedfile)
	{
		int i;
		
		for (i = 0; i < MAX_TEXTURE_UNITS; i++)
		{
			texturemode[i] = TEXTUREMODE_NOTEX;
			tuenable[i] = true;
			autounload[i] = true;
		}
		
		loadedfile = false;
		
		if (pObject != NULL)
		{	
			for (i = 0; i < pObject->info.num_frames; i++)
			{
				delete [] pObject->frames[i].faces;
				delete [] pObject->frames[i].verts;
				delete [] pObject->frames[i].normals;
				delete [] pObject->frames[i].texcoords;
			}
			
			delete [] pObject->frames;
			
			delete pObject;
		}
		
		pObject = NULL;
		
		for (i = 0; i < MAX_TEXTURE_UNITS; i++)
		{
			if (texturemode[i] != TEXTUREMODE_NOTEX)
			{
				glDeleteTextures(1, &textureid[i]);
			}
			
			texturemode[i] = TEXTUREMODE_NOTEX;
		}
		
		glDeleteLists(static_list, 1);
		static_list = 0;
	}
	
	collidecachepos = 0;
	
	char strMessage[255] = {0};
	
	//numtex = 0;
	//has_illum = false;
	//active_base = 0;
	
	//string filename = settings.GetDataDir() + "/models/";
	//filename = filename + strFileName;
	string filename = strFileName;
	
	//cout << strFileName << endl;

	/*bool err = true;
	int w, h;
	if (filename.find("body.joe") < filename.length())
	{
		has_paint = true;
		modelpath = filepath;
		
		char fn[256];
		numtex = 0;
		sprintf(fn, "%stextures/%s/body%02d.png", filepath.c_str(), settings.GetTexSize().c_str(), numtex);
		ifstream test;
		test.open(fn);
		while (test && numtex < 100)
		{
			numtex++;
			test.close();
			test.clear();
			sprintf(fn, "%stextures/%s/body%02d.png", filepath.c_str(), settings.GetTexSize().c_str(), numtex);
			test.open(fn);
		}
		//cout << numtex << endl;

		texid = //utility.TexLoad(filepath + "textures/" + settings.GetTexSize() + "/body00.png", true);

		illum_tex = //utility.TexLoad(filepath + "textures/" + settings.GetTexSize() + "/brake.png", GL_RGB, true, w, h, true, err);
		if (!err)
		{
			has_illum = true;
			//illum_tex = numtex;
			//numtex++;
			//cout << "Loaded brake lights for: " << filename << endl;
		}
		//else cout << "no brakelights for: " << filename << endl;
	}
	else if (filename.find("glass.joe") < filename.length())
	{
		texid = //utility.TexLoad(filepath + "textures/" + settings.GetTexSize() + "/glass.png", true);
		
		illum_tex = //utility.TexLoad(filepath + "textures/" + settings.GetTexSize() + "/brake-glass.png", GL_RGB, true, w, h, true, err);
		if (!err)
		{
			has_illum = true;
			//illum_tex = numtex;
			//numtex++;
			//cout << "Loaded brake lights for: " << filename << endl;
		}
		//else cout << "no brakelights for: " << filename << endl;
	}
	else if (filename.find("interior.joe") < filename.length())
	{
		texid = //utility.TexLoad(filepath + "textures/" + settings.GetTexSize() + "/interior.png", true);
	}
	else if (filename.find("wheel_front.joe") < filename.length())
	{
		texid = //utility.TexLoad(filepath + "textures/" + settings.GetTexSize() + "/wheel_front.png", true);
	}
	else if (filename.find("wheel_rear.joe") < filename.length())
	{
		texid = //utility.TexLoad(filepath + "textures/" + settings.GetTexSize() + "/wheel_rear.png", true);
	}
	else*/
		
	//texid[1] = //utility.TexLoad(strFileName + "-brake.png", true);
	
    // Open the MD2 file in binary
    m_FilePointer = fopen(filename.c_str(), "rb");

    // Make sure we have a valid file pointer (we found the file)
    if(!m_FilePointer) 
    {
        // Display an error message and don't load anything if no file was found
        sprintf(strMessage, "Unable to find the file: %s!", strFileName.c_str());
		loadedfile = false;
        cerr << strMessage << endl;
		return false;
    }
	
	{
		//texid = //utility.TexLoad(strFileName + ".png", GL_RGBA, true);
		//texid = //utility.TexLoad(newfn, true);
		if (autoloadtexture)
		{
/*			if (//utility.FileExists(FileToPNG(filename)))
			{
				textureid[0] = //utility.TexLoad(FileToPNG(filename), true);
				texturemode[0] = TEXTUREMODE_TEX;
			}
			else
			{
				if (//utility.FileExists(FileToTexturesizePNG(filename)))
				{
					textureid[0] = //utility.TexLoad(FileToTexturesizePNG(filename), true);
					texturemode[0] = TEXTUREMODE_TEX;
				}
			}*/
		}
	}
	
	//create a new object
	pObject = new JOEObject;

    // Read the header data and store it in our variable
    fread(&pObject->info, sizeof(JOEHeader), 1, m_FilePointer);
	
	pObject->info.magic = ENDIAN_SWAP_32(pObject->info.magic);
	pObject->info.version = ENDIAN_SWAP_32(pObject->info.version);
	pObject->info.num_faces = ENDIAN_SWAP_32(pObject->info.num_faces);
	pObject->info.num_frames = ENDIAN_SWAP_32(pObject->info.num_frames);
	
    // Make sure the version is what we expect or else it's a bad egg
    if(pObject->info.version != JOE_VERSION)
    {
        // Display an error message for bad file format, then stop loading
        sprintf(strMessage, "Invalid file format (Version is %d not %d): %s!", pObject->info.version, JOE_VERSION, strFileName.c_str());
		loadedfile = false;
        cerr << strMessage << endl;
		delete [] pObject;
		pObject = NULL;
		return false;
    }
	
	if (pObject->info.num_faces > 32000)
	{
		cout << filename << " has " << pObject->info.num_faces << " faces (max 32768), crashing or other problems may occur." << endl;
	}
	
	loadedfile = true;

	//cout << pObject->info.version << "," << pObject->info.num_faces << endl;
	
    // Read in the model and animation data
	ReadData();

    // Clean up after everything
    CleanUp();

	//optimize frame zero into a static display list
	static_list = glGenLists(1);
	glNewList (static_list, GL_COMPILE);
	Draw(0,0,0);
	glEndList ();
	
    // Return a success
    return true;
}

void JOEMODEL::ReadData()
{
	int num_frames = pObject->info.num_frames;
	int num_faces = pObject->info.num_faces;
	
	pObject->frames = new JOEFrame [num_frames];
	
	int i;
	for (i = 0; i < num_frames; i++)
	//for (i = 0; i < 1; i++)
	{
		pObject->frames[i].faces = new JOEFace [num_faces];
		
		fread(pObject->frames[i].faces, sizeof(JOEFace), num_faces, m_FilePointer);
		CorrectEndian(pObject->frames[i].faces, num_faces);
		
		fread(&pObject->frames[i].num_verts, sizeof(int), 1, m_FilePointer);
		pObject->frames[i].num_verts = ENDIAN_SWAP_32(pObject->frames[i].num_verts);
		fread(&pObject->frames[i].num_texcoords, sizeof(int), 1, m_FilePointer);
		pObject->frames[i].num_texcoords = ENDIAN_SWAP_32(pObject->frames[i].num_texcoords);
		fread(&pObject->frames[i].num_normals, sizeof(int), 1, m_FilePointer);
		pObject->frames[i].num_normals = ENDIAN_SWAP_32(pObject->frames[i].num_normals);
		//cout << pObject->frames[i].num_verts << "," << pObject->frames[i].num_texcoords << "," << pObject->frames[i].num_normals << endl;
		
		pObject->frames[i].verts = new JOEVertex [pObject->frames[i].num_verts];
		pObject->frames[i].normals = new JOEVertex [pObject->frames[i].num_normals];
		pObject->frames[i].texcoords = new JOETexCoord [pObject->frames[i].num_texcoords];
		
		fread(pObject->frames[i].verts, sizeof(JOEVertex), pObject->frames[i].num_verts, m_FilePointer);
		CorrectEndian(pObject->frames[i].verts, pObject->frames[i].num_verts);
		fread(pObject->frames[i].normals, sizeof(JOEVertex), pObject->frames[i].num_normals, m_FilePointer);
		CorrectEndian(pObject->frames[i].normals, pObject->frames[i].num_normals);
		fread(pObject->frames[i].texcoords, sizeof(JOETexCoord), pObject->frames[i].num_texcoords, m_FilePointer);
		CorrectEndian(pObject->frames[i].texcoords, pObject->frames[i].num_texcoords);
		
		//cout << pObject->frames[i].texcoords[0].u << "," << pObject->frames[i].texcoords[0].v << endl;
	}
	
	float maxv[3];
	float minv[3];
	bool havevals[6];
	int n = 0;
	for (n = 0; n < 6; n++)
		havevals[n] = false;
	
	//go do scaling and flip axes
	for (i = 0; i < num_frames; i++)
	{
		int v;
		
		for (v = 0; v < pObject->frames[i].num_verts; v++)
		{
			VERTEX temp;
			
			temp.Set(pObject->frames[i].verts[v].vertex);
			temp.Scale(MODEL_SCALE);
			
			//cache for bbox stuff
			for (n = 0; n < 3; n++)
			{
				if (temp.v3()[n] > maxv[n] || !havevals[n])
				{
					maxv[n] = temp.v3()[n];
					havevals[n] = true;
				}
				if (temp.v3()[n] < minv[n] || !havevals[n+3])
				{
					minv[n] = temp.v3()[n];
					havevals[n+3] = true;
				}
			}
			
			float r = temp.len();
			float rxz = sqrt(temp.x*temp.x+temp.z*temp.z);
			if (r > radius)
				radius = r;
			if (rxz > radiusxz)
				radiusxz = rxz;
			pObject->frames[i].verts[v].vertex[0] = temp.y;
			pObject->frames[i].verts[v].vertex[1] = temp.z;
			pObject->frames[i].verts[v].vertex[2] = -temp.x;
			
			if (modelpath == "data/tracks/test3/objects/ruud01.dof-00.joe")
			{
				temp.DebugPrint();
			}
		}
	}
	
	//make sure vertex ordering is consistent with normals
	for (i = 0; i < pObject->info.num_faces; i++)
	{
		short vi[3];
		VERTEX tri[3];
		VERTEX norms[3];
		for (unsigned int v = 0; v < 3; v++)
		{
			vi[v] = GetFace(i)[v];
			tri[v].Set(GetVert(vi[v]));
			norms[v].Set(GetNorm(GetNormIdx(i)[v]));
		}
		VERTEX norm;
		for (unsigned int v = 0; v < 3; v++)
			norm = norm + norms[v];
		norm = norm.normalize();
		VERTEX tnorm = (tri[2] - tri[0]).cross(tri[1] - tri[0]);
		if (norm.dot(tnorm) > 0)
		{
			/*short tvi = pObject->frames[0].faces[i].vertexIndex[1];
			pObject->frames[0].faces[i].vertexIndex[1] = pObject->frames[0].faces[i].vertexIndex[2];
			pObject->frames[0].faces[i].vertexIndex[2] = tvi;
			
			tvi = pObject->frames[0].faces[i].normalIndex[1];
			pObject->frames[0].faces[i].normalIndex[1] = pObject->frames[0].faces[i].normalIndex[2];
			pObject->frames[0].faces[i].normalIndex[2] = tvi;
			
			tvi = pObject->frames[0].faces[i].textureIndex[1];
			pObject->frames[0].faces[i].textureIndex[1] = pObject->frames[0].faces[i].textureIndex[2];
			pObject->frames[0].faces[i].textureIndex[2] = tvi;*/
			
			/*short tvi = vi[1];
			vi[1] = vi[2];
			vi[2] = tvi;*/
		}
	}
	
	VERTEX minvals, maxvals;
	minvals.Set(minv);
	maxvals.Set(maxv);
	bbox.SetFromCorners(minvals, maxvals);
	
	
    //cout << m_Header.numTriangles << endl;
    //cout << m_Header.numFrames << endl;
    
    // Here we allocate all of our memory from the header's information
    /*m_pSkins     = new tMd2Skin [m_Header.numSkins];
    m_pTexCoords = new tMd2TexCoord [m_Header.numTexCoords];
    m_pTriangles = new tMd2Face [m_Header.numTriangles];
    m_pFrames    = new tMd2Frame [m_Header.numFrames];

    // Next, we start reading in the data by seeking to our skin names offset
    fseek(m_FilePointer, m_Header.offsetSkins, SEEK_SET);
    
    // Depending on the skin count, we read in each skin for this model
    fread(m_pSkins, sizeof(tMd2Skin), m_Header.numSkins, m_FilePointer);
    
    // Move the file pointer to the position in the file for texture coordinates
    fseek(m_FilePointer, m_Header.offsetTexCoords, SEEK_SET);
    
    // Read in all the texture coordinates in one fell swoop
    fread(m_pTexCoords, sizeof(tMd2TexCoord), m_Header.numTexCoords, m_FilePointer);

    // Move the file pointer to the triangles/face data offset
    fseek(m_FilePointer, m_Header.offsetTriangles, SEEK_SET);
    
    // Read in the face data for each triangle (vertex and texCoord indices)
    fread(m_pTriangles, sizeof(tMd2Face), m_Header.numTriangles, m_FilePointer);
            
    // Move the file pointer to the vertices (frames)
    fseek(m_FilePointer, m_Header.offsetFrames, SEEK_SET);

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

    // In the last tutorial we just read in one frame of animation here.  Now we are going
    // to extract every key frame from the .MD2 model.  These key frames will be used to
    // interpolate between each other to form the somewhat smooth animation.
    // The only thing different is that we are putting a for loop in front, then changing
    // the '0' for the m_pFrames[0] in the last tutorial, to a 'i'.  The loop will
    // continue until 'i' has reached the number of key frames for this model.

	int i;
    for (i=0; i < m_Header.numFrames; i++)
    {
        // Assign our alias frame to our buffer memory
        tMd2AliasFrame *pFrame = (tMd2AliasFrame *) buffer;

        // Allocate the memory for the first frame of animation's vertices
        m_pFrames[i].pVertices = new tMd2Triangle [m_Header.numVertices];

		//cout << "about to read raw data" << endl;
		
        // Read in the first frame of animation
        int numread = fread(pFrame, m_Header.frameSize, 1, m_FilePointer);
		
		//cout << "read raw data: " << m_Header.frameSize << "," << numread << endl;

        // Copy the name of the animation to our frames array
        strcpy(m_pFrames[i].strName, pFrame->name);
            
        // Store off a vertex array pointer to cut down large lines of code
        tMd2Triangle *pVertices = m_pFrames[i].pVertices;
		
		//cout << "reading verts: " << m_Header.numVertices << endl;
		
		int j;

        // Go through all of the number of vertices and assign the scale and translations.
        // Store the vertices in our current frame's vertex list array, while swapping Y and Z.
        // Notice we also negate the Z axis as well to make the swap correctly.
        for (j=0; j < m_Header.numVertices; j++)
        {
			//cout << "verts" << endl;
			float temp = pFrame->aliasVertices[j].vertex[0];
			//cout << "read temp: " << temp << endl;
			pVertices[j].vertex[0] = temp;
			//cout << "assigned temp" << endl;
            pVertices[j].vertex[2] = (pFrame->aliasVertices[j].vertex[1]);
            pVertices[j].vertex[1] = pFrame->aliasVertices[j].vertex[2];
			//cout << "norms" << endl;
			pVertices[j].normal[0] = pFrame->aliasVertices[j].normal[0];
            pVertices[j].normal[2] = (pFrame->aliasVertices[j].normal[1]);
            pVertices[j].normal[1] = pFrame->aliasVertices[j].normal[2];
			
			//cout << pVertices[j].vertex[0] << "," << pVertices[j].vertex[1] << "," << pVertices[j].vertex[2] << endl;
        }
		//cout << "finished reading verts" << endl;
    }
	*/
	//cout << "done" << endl;
}

void JOEMODEL::CleanUp()
{
    // This just just the regular cleanup or our md2 model class.  We can free
    // all of this data because we already have it stored in our own structures.

    fclose(m_FilePointer);                      // Close the current file pointer
}

bool verbose_output = false;
JOEMODEL::~JOEMODEL()
{
	if (verbose_output)
		cout << "joemodel deinit" << endl;
	
	int i;
	
	// Free the faces, normals, vertices, and texture coordinates.
	/*
	delete [] pObject[i].pFaces;
	delete [] pObject[i].pNormals;
	delete [] pObject[i].pVerts;
	delete [] pObject[i].pTexVerts;*/
	
	if (pObject != NULL)
	{	
		for (i = 0; i < pObject->info.num_frames; i++)
		{
			delete [] pObject->frames[i].faces;
			delete [] pObject->frames[i].verts;
			delete [] pObject->frames[i].normals;
			delete [] pObject->frames[i].texcoords;
		}
		
		delete [] pObject->frames;
		
		delete pObject;
	}
	
//	int i;
	
	/*glDeleteTextures(1, &texid);
	
	if (has_illum)
		glDeleteTextures(1, &illum_tex);*/
	
	for (i = 0; i < MAX_TEXTURE_UNITS; i++)
	{
		if (texturemode[i] != TEXTUREMODE_NOTEX && autounload[i])
		{
			glDeleteTextures(1, &textureid[i]);
		}
		
		texturemode[i] = TEXTUREMODE_NOTEX;
	}
	
	glDeleteLists(static_list, 1);
}

///////////////////////////////// ANIMATE MD2 MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   This draws and animates the .md2 model by interpoloated key frame animation
/////
///////////////////////////////// ANIMATE MD2 MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void JOEMODEL::Draw(int currentFrame, float t)
{
	Draw(currentFrame, currentFrame+1, t);
}

void JOEMODEL::NewDraw(int currentFrame, float t, VERTEX lightdir, QUATERNION rotation, int pass)
{
	NewDraw(currentFrame, currentFrame+1, t, lightdir, rotation, pass);
}

void JOEMODEL::Draw(int currentFrame, int nextframe, float t)
{
	//utility.SelectTU(0);
	glEnable(GL_TEXTURE_2D);
	
	int num_frames = pObject->info.num_frames;
	int num_faces = pObject->info.num_faces;
	JOEFrame * pFrame, * pNextFrame;
	
	if (currentFrame >= num_frames)
	{
		currentFrame = num_frames-1;
	}
	
	if (nextframe >= num_frames)
		nextframe = num_frames-1;
	
	//cout << currentFrame << "," << nextframe << endl;
	
	pFrame = &pObject->frames[currentFrame];
	pNextFrame = &pObject->frames[nextframe];
	
	// Start rendering triangles
    glBegin(GL_TRIANGLES);

        // Go through all of the faces (polygons) of the current frame and draw them
        for(int j = 0; j < num_faces; j++)
        {
		
			//calculate normals for flat shading
			VERTEX a, b, c;
			a.Set(pFrame->verts[pFrame->faces[j].vertexIndex[0]].vertex);
			b.Set(pFrame->verts[pFrame->faces[j].vertexIndex[1]].vertex);
			c.Set(pFrame->verts[pFrame->faces[j].vertexIndex[2]].vertex);
			
			float v0[3], v1[3], v2[3];
			v0[0] = a.x; v0[1] = a.y; v0[2] = a.z;
			v1[0] = b.x; v1[1] = b.y; v1[2] = b.z;
			v2[0] = c.x; v2[1] = c.y; v2[2] = c.z;
			
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
			
			//glNormal3fv(norm);
			
			
            // Go through each corner of the triangle and draw it.
            for(int whichVertex = 0; whichVertex < 3; whichVertex++)
            {
				
                // Get the index for each point of the face
                int vertIndex = pFrame->faces[j].vertexIndex[whichVertex];
				int nextvertIndex = pNextFrame->faces[j].vertexIndex[whichVertex];

                // Get the index for each texture coordinate for this face
                int texIndex  = pFrame->faces[j].textureIndex[whichVertex];				
				
				int normIndex = pFrame->faces[j].normalIndex[whichVertex];
				int nextnormIndex = pNextFrame->faces[j].normalIndex[whichVertex];
				                     
				// Pass in the texture coordinate for this vertex
				//glTexCoord2f(pFrame->texcoords[ texIndex ].u, pFrame->texcoords[ texIndex ].v);
				//utility.TexCoord2d2f(0, pFrame->texcoords[ texIndex ].u, pFrame->texcoords[ texIndex ].v);
				//utility.TexCoord2d2f(1, pFrame->texcoords[ texIndex ].u, pFrame->texcoords[ texIndex ].v);
				//utility.TexCoord2d2f(2, pFrame->texcoords[ texIndex ].u, pFrame->texcoords[ texIndex ].v);
				//utility.TexCoord2d2f(3, pFrame->texcoords[ texIndex ].u, pFrame->texcoords[ texIndex ].v);
				
				//JGL_2MTEXCOORD(GL_TEXTURE0_ARB, pFrame->texcoords[ texIndex ].u, pFrame->texcoords[ texIndex ].v);
				//JGL_2MTEXCOORD(GL_TEXTURE1_ARB, pFrame->texcoords[ texIndex ].u, pFrame->texcoords[ texIndex ].v);
				//JGL_2MTEXCOORD(GL_TEXTURE2_ARB, pFrame->texcoords[ texIndex ].u, pFrame->texcoords[ texIndex ].v);
				//JGL_2MTEXCOORD(GL_TEXTURE3_ARB, pFrame->texcoords[ texIndex ].u, pFrame->texcoords[ texIndex ].v);

                // Now we get to the interpolation part! (*Bites his nails*)
                // Below, we first store the vertex we are working on for the current
                // frame and the frame we are interpolating too.  Next, we use the
                // linear interpolation equation to smoothly transition from one
                // key frame to the next.
                
				VERTEX vPoint1, vPoint2;
				vPoint1.Set(pFrame->normals[ normIndex ].vertex);
                vPoint2.Set(pNextFrame->normals[ nextnormIndex ].vertex);
				vPoint1 = vPoint1.normalize();
				vPoint2 = vPoint2.normalize();
				//glNormal3f(vPoint1.x + t * (vPoint2.x - vPoint1.x), // Find the interpolated X
                //           vPoint1.y + t * (vPoint2.y - vPoint1.y), // Find the interpolated Y
                //           vPoint1.z + t * (vPoint2.z - vPoint1.z));// Find the interpolated Z
			
				VERTEX n;
				n.x = vPoint1.x + t * (vPoint2.x - vPoint1.x);
				n.y = vPoint1.y + t * (vPoint2.y - vPoint1.y);
				n.z = vPoint1.z + t * (vPoint2.z - vPoint1.z);
				
				//n.Set(norm[0], norm[1], norm[2]);
				n = n.normalize();
				//n = rotation.RotateVec(n);
				
				//if (pass == 1)
				/*if (1)
					TexCoordFromNormal(n, lightdir);
				else
				{
					if (pass == 1)
						ColorFromNormal(n, lightdir);
					else
						TexCoordFromNormal(n, lightdir);
				}*/

				/*QUATERNION goofyfoot;
				goofyfoot.Rotate(-3.141593/2.0, 1,0,0);
				n = goofyfoot.ReturnConjugate().RotateVec(n);*/
				//glNormal3fv(n.v3());
				glNormal3f(n.y,-n.x,n.z);
				
				//if (pass == 2)
				
				
                // Store the current and next frame's vertex
                vPoint1.Set(pFrame->verts[ vertIndex ].vertex);
                vPoint2.Set(pNextFrame->verts[ nextvertIndex ].vertex);
				
				//CVector3 vNorm1 = pFrame->pNormals[ vertIndex ];
                //CVector3 vNorm2 = pNextFrame->pNormals[ vertIndex ];
				
				//glNormal3f(vNorm1.x + t * (vNorm2.x - vNorm1.x), // Find the interpolated X
                //           vNorm1.y + t * (vNorm2.y - vNorm1.y), // Find the interpolated Y
                //           vNorm1.z + t * (vNorm2.z - vNorm1.z));// Find the interpolated Z

				//cout << vNorm1.x << "," << vNorm1.y << "," << vNorm1.z << endl;

				//glNormal3f(pFirstFrame->pNormals[vertIndex].x,pFirstFrame->pNormals[vertIndex].y,pFirstFrame->pNormals[vertIndex].z);
				
                // By using the equation: p(t) = p0 + t(p1 - p0), with a time t
                // passed in, we create a new vertex that is closer to the next key frame.
				VERTEX finv;
				finv.x = vPoint1.x + t * (vPoint2.x - vPoint1.x);
				finv.y = vPoint1.y + t * (vPoint2.y - vPoint1.y);
				finv.z = vPoint1.z + t * (vPoint2.z - vPoint1.z);
				
				//tree shadows
				////utility.TexCoord2d2f(3, (finv.x-terrain.GetOffset().x)/terrain.GetSize().x, (finv.y-terrain.GetOffset().z)/terrain.GetSize().z);
				////utility.TexCoord2d2f(3, pFrame->texcoords[ texIndex ].u, pFrame->texcoords[ texIndex ].v);
                glVertex3f(finv.x, finv.z, finv.y); //required for compatibility with goofy vamos z=up
						   
				
            }
        }

    // Stop rendering the triangles
    glEnd();
}

void JOEMODEL::NewDraw(int currentFrame, int nextframe, float t, VERTEX lightdir, QUATERNION rotation, int pass)
{
/*	int num_frames = pObject->info.num_frames;
	int num_faces = pObject->info.num_faces;
	JOEFrame * pFrame, * pNextFrame;
	
	if (currentFrame >= num_frames)
	{
		currentFrame = num_frames-1;
	}
	
	if (nextframe >= num_frames)
		nextframe = num_frames-1;
	
	//cout << currentFrame << "," << nextframe << endl;
	
	pFrame = &pObject->frames[currentFrame];
	pNextFrame = &pObject->frames[nextframe];
	
	// Start rendering triangles
    glBegin(GL_TRIANGLES);

        // Go through all of the faces (polygons) of the current frame and draw them
        for(int j = 0; j < num_faces; j++)
        {
		
			//calculate normals for flat shading
			VERTEX a, b, c;
			a.Set(pFrame->verts[pFrame->faces[j].vertexIndex[0]].vertex);
			b.Set(pFrame->verts[pFrame->faces[j].vertexIndex[1]].vertex);
			c.Set(pFrame->verts[pFrame->faces[j].vertexIndex[2]].vertex);
			
			float v0[3], v1[3], v2[3];
			v0[0] = a.x; v0[1] = a.y; v0[2] = a.z;
			v1[0] = b.x; v1[1] = b.y; v1[2] = b.z;
			v2[0] = c.x; v2[1] = c.y; v2[2] = c.z;
			
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
			
			//glNormal3fv(norm);
			
			
            // Go through each corner of the triangle and draw it.
            for(int whichVertex = 0; whichVertex < 3; whichVertex++)
            {
				
                // Get the index for each point of the face
                int vertIndex = pFrame->faces[j].vertexIndex[whichVertex];
				int nextvertIndex = pNextFrame->faces[j].vertexIndex[whichVertex];

                // Get the index for each texture coordinate for this face
                int texIndex  = pFrame->faces[j].textureIndex[whichVertex];				
				
				int normIndex = pFrame->faces[j].normalIndex[whichVertex];
				int nextnormIndex = pNextFrame->faces[j].normalIndex[whichVertex];
				                     
				// Pass in the texture coordinate for this vertex
				//glTexCoord2f(pFrame->texcoords[ texIndex ].u, pFrame->texcoords[ texIndex ].v);
				
				JGL_2MTEXCOORD(GL_TEXTURE0_ARB, pFrame->texcoords[ texIndex ].u, pFrame->texcoords[ texIndex ].v);
				JGL_2MTEXCOORD(GL_TEXTURE1_ARB, pFrame->texcoords[ texIndex ].u, pFrame->texcoords[ texIndex ].v);
				JGL_2MTEXCOORD(GL_TEXTURE2_ARB, pFrame->texcoords[ texIndex ].u, pFrame->texcoords[ texIndex ].v);
				JGL_2MTEXCOORD(GL_TEXTURE3_ARB, pFrame->texcoords[ texIndex ].u, pFrame->texcoords[ texIndex ].v);

                // Now we get to the interpolation part! (*Bites his nails*)
                // Below, we first store the vertex we are working on for the current
                // frame and the frame we are interpolating too.  Next, we use the
                // linear interpolation equation to smoothly transition from one
                // key frame to the next.
                
				VERTEX vPoint1, vPoint2;
				vPoint1.Set(pFrame->normals[ normIndex ].vertex);
                vPoint2.Set(pNextFrame->normals[ nextnormIndex ].vertex);
				vPoint1 = vPoint1.normalize();
				vPoint2 = vPoint2.normalize();
				//glNormal3f(vPoint1.x + t * (vPoint2.x - vPoint1.x), // Find the interpolated X
                //           vPoint1.y + t * (vPoint2.y - vPoint1.y), // Find the interpolated Y
                //           vPoint1.z + t * (vPoint2.z - vPoint1.z));// Find the interpolated Z
			
				VERTEX n;
				n.x = vPoint1.x + t * (vPoint2.x - vPoint1.x);
				n.y = vPoint1.y + t * (vPoint2.y - vPoint1.y);
				n.z = vPoint1.z + t * (vPoint2.z - vPoint1.z);
				
				//n.Set(norm[0], norm[1], norm[2]);
				n = n.normalize();
				n = rotation.RotateVec(n);
				
				//if (pass == 1)
				if (1)
					TexCoordFromNormal(n, lightdir);
				else
				{
					if (pass == 1)
						ColorFromNormal(n, lightdir);
					else
						TexCoordFromNormal(n, lightdir);
				}
				
				//glNormal3fv(n.v3());
				
				//if (pass == 2)
				
				
                // Store the current and next frame's vertex
                vPoint1.Set(pFrame->verts[ vertIndex ].vertex);
                vPoint2.Set(pNextFrame->verts[ nextvertIndex ].vertex);
				
				//CVector3 vNorm1 = pFrame->pNormals[ vertIndex ];
                //CVector3 vNorm2 = pNextFrame->pNormals[ vertIndex ];
				
				//glNormal3f(vNorm1.x + t * (vNorm2.x - vNorm1.x), // Find the interpolated X
                //           vNorm1.y + t * (vNorm2.y - vNorm1.y), // Find the interpolated Y
                //           vNorm1.z + t * (vNorm2.z - vNorm1.z));// Find the interpolated Z

				//cout << vNorm1.x << "," << vNorm1.y << "," << vNorm1.z << endl;

				//glNormal3f(pFirstFrame->pNormals[vertIndex].x,pFirstFrame->pNormals[vertIndex].y,pFirstFrame->pNormals[vertIndex].z);
				
                // By using the equation: p(t) = p0 + t(p1 - p0), with a time t
                // passed in, we create a new vertex that is closer to the next key frame.
                glVertex3f(vPoint1.x + t * (vPoint2.x - vPoint1.x), // Find the interpolated X
                           vPoint1.y + t * (vPoint2.y - vPoint1.y), // Find the interpolated Y
                           vPoint1.z + t * (vPoint2.z - vPoint1.z));// Find the interpolated Z
						   
				
            }
        }

    // Stop rendering the triangles
    glEnd();
	*/
	/*
    // Now comes the juice of our tutorial.  Fear not, this is actually very intuitive
    // if you drool over it for a while (stay away from the keyboard though...).
    // What's going on here is, we are getting our current animation that we are
    // on, finding the current frame of that animation that we are on, then interpolating
    // between that frame and the next frame.  To make a smooth constant animation when
    // we get to the end frame, we interpolate between the last frame of the animation 
    // and the first frame of the animation.  That way, if we are doing the running 
    // animation let's say, when the last frame of the running animation is hit, we don't
    // have a huge jerk when going back to the first frame of that animation.  Remember,
    // because we have the texture and face information stored in the first frame of our
    // animation, we need to reference back to this frame every time when drawing the
    // model.  The only thing the other frames store is the vertices, but no information
    // about them.
    
    // Make sure we have valid objects just in case. (size() is in the vector class)
    if(pObject.size() <= 0) return;

    // Here we grab the current animation that we are on from our model's animation list
    //tAnimationInfo *pAnim = &(pAnimations[0]);

    // This gives us the current frame we are on.  We mod the current frame plus
    // 1 by the current animations end frame to make sure the next frame is valid.
    // If the next frame is past our end frame, then we go back to zero.  We check this next.
    //int nextFrame = (currentFrame + 1) % pAnim->endFrame;
	int nextFrame = nextframe;

	//cout << currentFrame << "," << nextFrame << endl;
	
    // If the next frame is zero, that means that we need to start the animation over.
    // To do this, we set nextFrame to the starting frame of this animation.
    //if(nextFrame == 0) 
    //    nextFrame =  pAnim->startFrame;

    // Get the current key frame we are on
    t3DObject *pFrame =      &pObject[currentFrame];

    // Get the next key frame we are interpolating too
    t3DObject *pNextFrame =  &pObject[nextFrame];

    // Get the first key frame so we have an address to the texture and face information
    t3DObject *pFirstFrame = &pObject[0];

    // Next, we want to get the current time that we are interpolating by.  Remember,
    // if t = 0 then we are at the beginning of the animation, where if t = 1 we are at the end.
    // Anyhing from 0 to 1 can be thought of as a percentage from 0 to 100 percent complete.
    //float t = ReturnCurrentTime(pModel, nextFrame);

    // Start rendering lines or triangles, depending on our current rendering mode (Lft Mouse Btn)
    glBegin(GL_TRIANGLES);

        // Go through all of the faces (polygons) of the current frame and draw them
        for(int j = 0; j < pFrame->numOfFaces; j++)
        {
			//calculate normals for flat shading
			CVector3 a = pFrame->pVerts[pFirstFrame->pFaces[j].vertIndex[0]];
			CVector3 b = pFrame->pVerts[pFirstFrame->pFaces[j].vertIndex[1]];
			CVector3 c = pFrame->pVerts[pFirstFrame->pFaces[j].vertIndex[2]];
			
			float v0[3], v1[3], v2[3];
			v0[0] = a.x; v0[1] = a.y; v0[2] = a.z;
			v1[0] = b.x; v1[1] = b.y; v1[2] = b.z;
			v2[0] = c.x; v2[1] = c.y; v2[2] = c.z;
			
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
			
			//glNormal3fv(norm);
			
			
            // Go through each corner of the triangle and draw it.
            for(int whichVertex = 0; whichVertex < 3; whichVertex++)
            {
                // Get the index for each point of the face
                int vertIndex = pFirstFrame->pFaces[j].vertIndex[whichVertex];

                // Get the index for each texture coordinate for this face
                int texIndex  = pFirstFrame->pFaces[j].coordIndex[whichVertex];
				
				//int normIndex = pFirstFrame->pFaces[j].normalIndex[whichVertex];
                        
                // Make sure there was a UVW map applied to the object.  Notice that
                // we use the first frame to check if we have texture coordinates because
                // none of the other frames hold this information, just the first by design.
                if(pFirstFrame->pTexVerts) 
                {
                    // Pass in the texture coordinate for this vertex
                    //glTexCoord2f(pFirstFrame->pTexVerts[ texIndex ].x, pFirstFrame->pTexVerts[ texIndex ].y);
					
					JGL_2MTEXCOORD(GL_TEXTURE0_ARB, pFirstFrame->pTexVerts[ texIndex ].x, pFirstFrame->pTexVerts[ texIndex ].y);
					JGL_2MTEXCOORD(GL_TEXTURE1_ARB, pFirstFrame->pTexVerts[ texIndex ].x, pFirstFrame->pTexVerts[ texIndex ].y);
					JGL_2MTEXCOORD(GL_TEXTURE2_ARB, pFirstFrame->pTexVerts[ texIndex ].x, pFirstFrame->pTexVerts[ texIndex ].y);
					JGL_2MTEXCOORD(GL_TEXTURE3_ARB, pFirstFrame->pTexVerts[ texIndex ].x, pFirstFrame->pTexVerts[ texIndex ].y);
                }

                // Now we get to the interpolation part! (*Bites his nails*)
                // Below, we first store the vertex we are working on for the current
                // frame and the frame we are interpolating too.  Next, we use the
                // linear interpolation equation to smoothly transition from one
                // key frame to the next.
                
				CVector3 vPoint1 = pFrame->pNormals[ vertIndex ];
                CVector3 vPoint2 = pNextFrame->pNormals[ vertIndex ];
				vPoint1.normalize();
				vPoint2.normalize();
				//glNormal3f(vPoint1.x + t * (vPoint2.x - vPoint1.x), // Find the interpolated X
                //           vPoint1.y + t * (vPoint2.y - vPoint1.y), // Find the interpolated Y
                //           vPoint1.z + t * (vPoint2.z - vPoint1.z));// Find the interpolated Z
				
				VERTEX n;
				n.x = vPoint1.x + t * (vPoint2.x - vPoint1.x);
				n.y = vPoint1.y + t * (vPoint2.y - vPoint1.y);
				n.z = vPoint1.z + t * (vPoint2.z - vPoint1.z);
				
				//n.Set(norm[0], norm[1], norm[2]);
				n = n.normalize();
				n = rotation.RotateVec(n);
				
				ColorFromNormal(n, lightdir);
				
                // Store the current and next frame's vertex
                vPoint1 = pFrame->pVerts[ vertIndex ];
                vPoint2 = pNextFrame->pVerts[ vertIndex ];
*/				
				/*CVector3 vNorm1 = pFrame->pNormals[ vertIndex ];
                CVector3 vNorm2 = pNextFrame->pNormals[ vertIndex ];
				
				glNormal3f(vNorm1.x + t * (vNorm2.x - vNorm1.x), // Find the interpolated X
                           vNorm1.y + t * (vNorm2.y - vNorm1.y), // Find the interpolated Y
                           vNorm1.z + t * (vNorm2.z - vNorm1.z));// Find the interpolated Z

				cout << vNorm1.x << "," << vNorm1.y << "," << vNorm1.z << endl;*/
/*				
				//glNormal3f(pFirstFrame->pNormals[vertIndex].x,pFirstFrame->pNormals[vertIndex].y,pFirstFrame->pNormals[vertIndex].z);
				
                // By using the equation: p(t) = p0 + t(p1 - p0), with a time t
                // passed in, we create a new vertex that is closer to the next key frame.
                glVertex3f(vPoint1.x + t * (vPoint2.x - vPoint1.x), // Find the interpolated X
                           vPoint1.y + t * (vPoint2.y - vPoint1.y), // Find the interpolated Y
                           vPoint1.z + t * (vPoint2.z - vPoint1.z));// Find the interpolated Z
            }
        }

    // Stop rendering the triangles
    glEnd();*/
}

inline void JOEMODEL::ColorFromNormal(VERTEX &norm, VERTEX &ldir)
{
/*	VERTEX lpos;
	norm.z = 1.0f;
	
	lpos = norm.ScaleR(2*ldir.dot(norm))-ldir;
	//glColor3fv(lpos.normalize().v3());
	
	lpos = lpos.normalize();
	VERTEX tmp;
	tmp.Set(0.5,0.5,0.5);
	lpos = lpos.ScaleR(0.5)+tmp;
	glColor3fv(lpos.v3());*/
}

inline void JOEMODEL::TexCoordFromNormal(VERTEX &norm, VERTEX &ldir)
{
/*	VERTEX lpos;
	//norm.z = 1.0f;
	
	//lpos = norm.ScaleR(2*ldir.dot(norm))-ldir;
	//R = 2 ( N [dot] V ) V - N
	lpos = ldir.ScaleR(2*norm.dot(ldir))-norm;
	//R = N - ( 2 * N [dot] V ) V
	//lpos = norm - ldir.ScaleR(2*norm.dot(ldir));
	//lpos = norm;
	
	JGL_3MTEXCOORD(GL_TEXTURE0_ARB, lpos.x, lpos.y, lpos.z);*/
}

/*void JOEMODEL::DrawStatic() //draw frame 0, optimized for speed
{
	int i;
	
	for (i = 0; i < MAX_TEXTURE_UNITS && i < //utility.numTUs(); i++)
	{
		//utility.SelectTU(i);
		glDisable(GL_TEXTURE_2D);
	}
	
	for (i = 0; i < MAX_TEXTURE_UNITS && i < //utility.numTUs(); i++)
	{
		if (texturemode[i] != TEXTUREMODE_NOTEX)
		{
			//utility.SelectTU(i);
			glBindTexture(GL_TEXTURE_2D, textureid[i]);
			if (tuenable[i])
				glEnable(GL_TEXTURE_2D);
			else
				glDisable(GL_TEXTURE_2D);
		
			if (texturemode[i] == TEXTUREMODE_ADD)
			{
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_PREVIOUS_ARB);
				glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB_ARB,GL_SRC_COLOR);
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
				glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB_ARB,GL_SRC_COLOR);
				if (i > 0)
					glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB_ARB, GL_ADD);
				else
					glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB_ARB, GL_REPLACE);
				
				glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, 1);
			}
			
			if (texturemode[i] == TEXTUREMODE_REFLECTION)
			{
				glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP); 
				glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP); 
				glEnable(GL_TEXTURE_GEN_S);
				glEnable(GL_TEXTURE_GEN_T);
				glEnable(GL_TEXTURE_2D);
				float sub[4];
				//sub[0] = sub[1] = sub[2] = 0.8f;
				sub[0] = sub[1] = sub[2] = 0.7f;
				//sub[0] = sub[1] = sub[2] = 0.0f;
				sub[3] = 0.0f;
				glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, sub);
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_PREVIOUS_ARB);
				glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB_ARB,GL_SRC_COLOR);
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
				glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB_ARB,GL_SRC_COLOR);
				//glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB_ARB, GL_CONSTANT_ARB);
				//glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND2_RGB_ARB,GL_SRC_COLOR);
				//glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB_ARB, GL_INTERPOLATE);
				glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, 1);
				
				if (i > 0)
					glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB_ARB, GL_ADD);
				else
					glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB_ARB, GL_REPLACE);
			}
		}
		
		//cout << modelpath << ": " << i << ": " << texturemode[i] << endl;
	}
	
	//glBindTexture(GL_TEXTURE_2D, texid[texnum]);
	////utility.SelectTU(0);
	
	if (!loadedfile)
		return;
	
	//tex unit 1 is used by the reflections
	
    glCallList(static_list);
	
	for (i = 0; i < MAX_TEXTURE_UNITS && i < //utility.numTUs(); i++)
	{
		if (texturemode[i] != TEXTUREMODE_NOTEX && i > 0)
		{
			//utility.SelectTU(i);
			glDisable(GL_TEXTURE_2D);
			
			if (texturemode[i] == TEXTUREMODE_REFLECTION)
			{
				glDisable(GL_TEXTURE_GEN_S);
				glDisable(GL_TEXTURE_GEN_T);
			}
		}
	}
}*/

/*GLuint JOEMODEL::GetTex(int tnum)
{
	return texid[tnum];
}*/

/*void JOEMODEL::SetBaseTexture(int tid)
{
	//cout << tid << ",";
	
	if (tid < 0)
	{
		tid = -tid;
		tid = tid % numtex;
		tid = numtex - tid;
	}
	//active_base = tid % numtex;
	tid = tid % numtex;
	
	if (loadedfile && has_paint && numtex > 1)
	{
		glDeleteTextures(1, &texid);
		char fn[256];
		sprintf(fn, "%stextures/%s/body%02d.png", modelpath.c_str(), settings.GetTexSize().c_str(), tid);
		texid = //utility.TexLoad(fn, true);
	}
	
	//cout << tid << endl;
}*/

void JOEMODEL::CorrectEndian(struct JOEFace * p, int num)
{
	int i;
	
	for (i = 0; i < num; i++)
	{
		int d;
		for (d = 0; d < 3; d++)
		{
			p[i].vertexIndex[d] = ENDIAN_SWAP_16(p[i].vertexIndex[d]);
			p[i].normalIndex[d] = ENDIAN_SWAP_16(p[i].normalIndex[d]);
			p[i].textureIndex[d] = ENDIAN_SWAP_16(p[i].textureIndex[d]);
		}
	}
}

void JOEMODEL::CorrectEndian(struct JOEVertex *p, int num)
{
	int i;
	
	for (i = 0; i < num; i++)
	{
		int d;
		for (d = 0; d < 3; d++)
		{
			p[i].vertex[d] = ENDIAN_SWAP_FLOAT(p[i].vertex[d]);
		}
	}
}

void JOEMODEL::CorrectEndian(struct JOETexCoord *p, int num)
{
	int i;
	
	for (i = 0; i < num; i++)
	{
		p[i].u = ENDIAN_SWAP_FLOAT(p[i].u);
		p[i].v = ENDIAN_SWAP_FLOAT(p[i].v);
	}
}

/*void JOEMODEL::Texture(string texturename, int tid)
{
	if (tid < MAX_TEXTURE_UNITS)
	{
		if (texturemode[tid] != TEXTUREMODE_NOTEX && autounload[tid])
		{
			glDeleteTextures(1, &textureid[tid]);
		}

		if (//utility.FileExists(FileToPNG(texturename)))
		{
			textureid[tid] = //utility.TexLoad(FileToPNG(texturename), true);
			texturemode[tid] = TEXTUREMODE_TEX;
			autounload[tid] = true;
		}
		else
		{
			if (//utility.FileExists(FileToTexturesizePNG(texturename)))
			{
				textureid[tid] = //utility.TexLoad(FileToTexturesizePNG(texturename), true);
				texturemode[tid] = TEXTUREMODE_TEX;
				autounload[tid] = true;
			}
			else
			{
				//couldn't find the texture
				cout << "Model " << modelpath << ": can't load " << FileToPNG(texturename) << " or " << FileToTexturesizePNG(texturename) << endl;
			}
		}
	}
}

void JOEMODEL::ReflectionTexture(string texturename, int tid)
{
	if (tid < MAX_TEXTURE_UNITS)
	{
		if (texturemode[tid] != TEXTUREMODE_NOTEX && autounload[tid])
		{
			glDeleteTextures(1, &textureid[tid]);
		}
		
		if (//utility.FileExists(FileToPNG(texturename)))
		{
			textureid[tid] = //utility.TexLoad(FileToPNG(texturename), true);
			texturemode[tid] = TEXTUREMODE_REFLECTION;
			autounload[tid] = true;
		}
		else
		{
			if (//utility.FileExists(FileToTexturesizePNG(texturename)))
			{
				textureid[tid] = //utility.TexLoad(FileToTexturesizePNG(texturename), true);
				texturemode[tid] = TEXTUREMODE_REFLECTION;
				autounload[tid] = true;
			}
		}
	}
}*/

string JOEMODEL::FileToPNG(string filename)
{
	int l;
	
	char newfn[MAX_FILENAME_LEN];
	strcpy(newfn, filename.c_str());
	l = strlen(newfn);
	newfn[l-1] = 'g';
	newfn[l-2] = 'n';
	newfn[l-3] = 'p';
	
	//return settings.GetFullDataPath(newfn);
	return newfn;
}

string JOEMODEL::FileToTexturesizePNG(string filename)
{
	string filepath = "";
	string accum = "";
	int i, l;
	for (i = 0; i < (int) filename.length(); i++)
	{
		accum += filename.c_str()[i];
		
		if (filename.c_str()[i] == '/')
		{
			filepath = filepath + accum;
			accum = "";
		}
	}
	
	string pathless_filename = accum;
	
	char newpf[MAX_FILENAME_LEN];
	strcpy(newpf, pathless_filename.c_str());
	l = strlen(newpf);
	newpf[l-1] = 'g';
	newpf[l-2] = 'n';
	newpf[l-3] = 'p';

	return newpf;
}

/*void JOEMODEL::AdditiveTexture(string texturename, int tid)
{
	if (tid < MAX_TEXTURE_UNITS)
	{
		if (texturemode[tid] != TEXTUREMODE_NOTEX && autounload[tid])
		{
			glDeleteTextures(1, &textureid[tid]);
		}
			
		if (//utility.FileExists(FileToPNG(texturename)))
		{
			textureid[tid] = //utility.TexLoad(FileToPNG(texturename), true);
			texturemode[tid] = TEXTUREMODE_ADD;
			autounload[tid] = true;
		}
		else
		{
			if (//utility.FileExists(FileToTexturesizePNG(texturename)))
			{
				textureid[tid] = //utility.TexLoad(FileToTexturesizePNG(texturename), true);
				texturemode[tid] = TEXTUREMODE_ADD;
				autounload[tid] = true;
			}
		}
	}
}*/

bool JOEMODEL::Collide(VERTEX origin, VERTEX direction, VERTEX &outtri, bool closest)
{
	int i;
	
	VERTEX curtri[3];
	
	bool hadcollision = false;
	
	int retval = 0;
	
	float t, u, v;
	
	VERTEX temptri;
	
	int colat = -1;
	int numcols = 0;

	if (!closest)
	{
		//cout << modelpath << endl;
		
		for (i = 0; i < NUM_COLLIDE_CACHE; i++)
		{
			//int pos = (collidecachepos - i + NUM_COLLIDE_CACHE) % NUM_COLLIDE_CACHE;
			
			//cout << collidecachepos << ", " << i << ": " << collidecache[i] << endl;
			
			curtri[0].Set(pObject->frames[0].verts[pObject->frames[0].faces[collidecache[i]].vertexIndex[0]].vertex);
			curtri[1].Set(pObject->frames[0].verts[pObject->frames[0].faces[collidecache[i]].vertexIndex[1]].vertex);
			curtri[2].Set(pObject->frames[0].verts[pObject->frames[0].faces[collidecache[i]].vertexIndex[2]].vertex);
			
			retval = 0;
			
			if (retval)
			{
				if (t < 0)
				{
					//noop
					//cout << "noop" << endl;
				}
				else
				{
					outtri = curtri[0].ScaleR(1-u-v) + curtri[1].ScaleR(u) + curtri[2].ScaleR(v);
					//cout << "cached: " << i << endl;
					return true;
				}
			}
			//else cout << "cache miss: " << i << endl;
		}
	}
	
	for (i = 0; i < pObject->info.num_faces; i++)
	{	
		//GetTri(COLLISION_DIVS, i, curtri);
		curtri[0].Set(pObject->frames[0].verts[pObject->frames[0].faces[i].vertexIndex[0]].vertex);
		curtri[1].Set(pObject->frames[0].verts[pObject->frames[0].faces[i].vertexIndex[1]].vertex);
		curtri[2].Set(pObject->frames[0].verts[pObject->frames[0].faces[i].vertexIndex[2]].vertex);
		
		/*cout << i << endl;
		curtri[0].DebugPrint();
		curtri[1].DebugPrint();
		curtri[2].DebugPrint();
		cout << endl;*/
		
		retval = 0;
		
		VERTEX tvert;
		if (retval)
		{
			numcols++;
			if (t < 0)
				retval = 0;
			else
				tvert = curtri[0].ScaleR(1-u-v) + curtri[1].ScaleR(u) + curtri[2].ScaleR(v);
		}
		
		if (retval && !closest)
		{
			outtri = tvert;
			colat = i;
			
			//implicit caching
			/*int n;
			short temp[3];
			for (n = 0; n < 3; n++)
			{
				temp[n] = pObject->frames[0].faces[0].vertexIndex[n];
				pObject->frames[0].faces[0].vertexIndex[n] = pObject->frames[0].faces[i].vertexIndex[n];
				pObject->frames[0].faces[i].vertexIndex[n] = temp[n];
			}*/
			
			//add to cache list
			int n;
			bool incache = false;
			for (n = 0; n < NUM_COLLIDE_CACHE; n++)
			{
				if (collidecache[n] == colat)
					incache = true;
			}
			
			if (!incache)
			{
				collidecache[collidecachepos] = colat;
				collidecachepos = (collidecachepos + 1) % NUM_COLLIDE_CACHE;
			}
			
			//cout << "col: " << colat << endl;
			
			return true;
		}
		else if (retval && closest)
		{
			if ((hadcollision && (origin - tvert).len() < (origin - outtri).len()) || !hadcollision)
			{
				if (hadcollision)
				{
					/*cout << "flip from ";
					outtri.DebugPrint();
					cout << " to ";
					tvert.DebugPrint();
					cout << endl;*/
				}
				
				outtri = tvert;
				colat = i;
			}
			
			hadcollision = true;
		}
	}
	
	if (closest && hadcollision)
	{
		//cout << numcols << ": " << colat << "/" << pObject->info.num_faces << endl;
		return true;
	}
	
	//cout << "nocol" << endl;
	
	outtri = origin;
	return false;
}
