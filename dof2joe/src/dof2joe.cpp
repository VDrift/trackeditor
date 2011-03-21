#include <string>
#include <iostream>
#include <cstdio>

#include <list>
#include <vector>
#include <map>
#include <cstring>

using namespace std;

#include "model.h"

unsigned int indent = 0;
class MATERIAL
{
public:
	string name;
	string texture;
};
vector <MATERIAL> textures;
vector <JOEMODEL> models;

map <string,string> shaders;

map <string,bool> alltextures;

bool verbose = false;
bool verboseprint = true;
bool verboseerror = true;
bool batch = false;

#define VERBOSEPRINT(str) {if (verbose) {for (unsigned int i = 0; i < indent; i++) cout << "\t"; cout << str << endl;}}
#define INDENTPRINT(str) {if (verboseprint) {for (unsigned int i = 0; i < indent; i++) cout << "\t"; cout << str << endl;}}
#define ERRORPRINT(str) {if (verboseerror) {for (unsigned int i = 0; i < indent; i++) cout << "\t"; cerr << "Error: " << str << endl;}}

string StringToLower(string strToConvert)
{//change each element of the string to lower case
   for(unsigned int i=0;i<strToConvert.length();i++)
   {
      strToConvert[i] = tolower(strToConvert[i]);
   }
   return strToConvert;//return the converted string
}

bool ReadString(FILE * f, unsigned int btoread, string & outstr)
{
	unsigned int bread;
	
	char buffer[btoread+1];
	
	bread = fread (buffer, 1, btoread, f);
	
	buffer[bread] = '\0';
	
	if (bread != btoread)
	{
		return false;
	}
	
	outstr = buffer;
	
	return true;
}

bool ReadInt(FILE * f, int & out)
{
	unsigned int bread;
	unsigned int btoread = 4;
	
	bread = fread (&out, 1, sizeof(int), f);
	
	if (bread != btoread)
	{
		return false;
	}
	
	return true;
}

bool ReadShort(FILE * f, short & out)
{
	unsigned int bread;
	unsigned int btoread = 2;
	
	bread = fread (&out, 1, sizeof(short), f);
	
	if (bread != btoread)
	{
		return false;
	}
	
	return true;
}

bool ReadRString(FILE * f, string & instr)
{
	short slen = 0;
	bool suc = ReadShort(f, slen);
	//cout << endl << slen << endl;
	if (!suc)
		return false;
	suc = ReadString(f, slen, instr);
	if (!suc)
		return false;
	
	return true;
}

bool ReadMAT0(FILE * f, int chunklen, int matid)
{
	string instr;
	int inint;
	bool suc = true;
	
	textures.resize(matid+1);
	
	/*suc = ReadString(f, chunklen, instr);
	if (!suc)
	{
		for (unsigned int i = 0; i < indent; i++)
			cout << "\t";
		cout << "read error" << endl;
		return false;
	}*/
	
	suc = ReadString(f, 4, instr);
	while (suc && instr != "MEND")
	{
		for (unsigned int i = 0; i < indent; i++)
			cout << "\t";
		cout << instr;
		if (instr != "MEND")
		{
			suc = ReadInt(f, inint);
			
			cout << ": len " << inint << endl;
			
			if (instr == "MTEX")
			{
				suc = ReadInt(f, inint);
				if (inint > 0)
				{
					for (unsigned int i = 0; i < indent; i++)
						cout << "\t";
					cout << inint << " texture(s)" << endl;
				}
				indent++;
				for (unsigned int t = 0; t < (unsigned int) inint; t++)
				{
					if (ReadRString(f, instr))
					{
						for (unsigned int i = 0; i < indent; i++)
							cout << "\t";
						cout << instr << endl;
						if (textures[matid].texture.empty())
						{
							textures[matid].texture = instr;
							if (batch)
								alltextures[instr] = true;
							INDENTPRINT("Using texture: " << textures[matid].texture << " for material " << matid);
						}
						else
						{
							INDENTPRINT("Already have texture: " << textures[matid].texture << " for material " << matid);
						}
					}
					else
					{
						suc = false;
						for (unsigned int i = 0; i < indent; i++)
							cout << "\t";
						cout << "read error" << endl;
					}
				}
				indent--;
			}
			else if (instr == "MUVW")
			{
				struct
				{
					float uvwUoffset;
					float uvwVoffset;
					float uvwUtiling;
					float uvwVtiling;
					float uvwAngle;
					float uvwBlur;
					float uvwBlurOffset;
				} uvwinfo;
				
				int bread = fread (&uvwinfo, 1, sizeof(uvwinfo), f);
				if (bread != sizeof(uvwinfo))
				{
					suc = false;
				}
				
				indent++;
				INDENTPRINT(uvwinfo.uvwUoffset << "," << uvwinfo.uvwVoffset);
				INDENTPRINT(uvwinfo.uvwUtiling << "," << uvwinfo.uvwVtiling);
				indent--;
			}
			else if (instr == "MHDR")
			{
				string inname;
				suc = ReadRString(f, inname);
				suc = ReadRString(f, instr);
				INDENTPRINT("Name: " << inname << "," << instr);
				textures[matid].name = inname;
				
			}
			else if (instr == "MSUB")
			{
				suc = ReadInt(f, inint);
				if (inint > 0)
				{
					for (unsigned int i = 0; i < indent; i++)
						cout << "\t";
					cout << inint << " sub-material(s)" << endl;
				}
				indent++;
				for (unsigned int t = 0; t < (unsigned int) inint; t++)
				{
					int sublen = 0;
					suc = ReadString(f, 4, instr);
					if (suc && ReadInt(f, sublen))
					{
						INDENTPRINT(instr << ": len " << sublen);
						ReadMAT0(f, sublen, matid);
					}
					else
					{
						suc = false;
					}
				}
				indent--;
			}
			else
			{
				for (unsigned int i = 0; i < indent+1; i++)
					cout << "\t";
				cout << "skipping chunk" << endl;;
				suc = ReadString(f, inint, instr);
			}
		}
		
		suc = ReadString(f, 4, instr);
	}
	
	if (!suc)
	{
		for (unsigned int i = 0; i < indent; i++)
			cout << "\t";
		cout << "read error" << endl;
	}
	
	return true;
}

bool ReadGOB1(FILE * f, int chunklen, int objid)
{
	string instr;
	int inint;
	bool suc = true;
	
	//models.resize(objid+1);
	//models[objid].pObject = NULL;
	
	suc = ReadString(f, 4, instr);
	while (suc && instr != "GEND")
	{
		for (unsigned int i = 0; i < indent; i++)
			cout << "\t";
		cout << instr;
		if (instr != "GEND")
		{
			suc = ReadInt(f, inint);
			
			cout << ": len " << inint << endl;
			
			if (instr == "GHDR")
			{
				struct
				{
					int flags;
					int paintFlags;
					int materialRef;
				} ghdr;
				
				int bread = fread (&ghdr, 1, sizeof(ghdr), f);
				if (bread != sizeof(ghdr))
				{
					suc = false;
				}
				
				indent++;
				INDENTPRINT("Material index " << ghdr.materialRef);
				models[objid].textureid[0] = ghdr.materialRef;
				indent--;
			}
			else if (instr == "BRST")
			{
				int bursts;
				
				suc = ReadInt(f, bursts);
				
				if (suc)
				{
					indent++;
					INDENTPRINT("Bursts: " << bursts);
					
					for (unsigned int b = 0; b < (unsigned int) bursts; b++)
					{
						int bin;
						suc = ReadInt(f, bin);
						INDENTPRINT("Start: " << bin);
					}
					for (unsigned int b = 0; b < (unsigned int) bursts; b++)
					{
						int bin;
						suc = ReadInt(f, bin);
						INDENTPRINT("Count: " << bin);
					}
					for (unsigned int b = 0; b < (unsigned int) bursts; b++)
					{
						int bin;
						suc = ReadInt(f, bin);
						INDENTPRINT("MtlID: " << bin);
					}
					for (unsigned int b = 0; b < (unsigned int) bursts; b++)
					{
						int bin;
						suc = ReadInt(f, bin);
						INDENTPRINT("VperP: " << bin);
					}
					indent--;
				}
				
				/*struct
				{
					int burstStart[bursts]; //(start in vertex[] for the first burst vertex)
					int burstCount[bursts]; //(number of floats in vertex[] to use for this burst)
					int burstMtlID[bursts]; //(material selection for this burst)
					int burstVperP[bursts]; //(should always be 3 for all bursts)
				} brst;
				
				int bread = fread (&brst, 1, sizeof(brst), f);
				if (bread != sizeof(brst))
				{
					suc = false;
				}
				
				indent++;
				INDENTPRINT("Bursts: " << brst.bursts);
				INDENTPRINT("BurstStart: " << brst.burstStart[0]);
				INDENTPRINT("BurstCount: " << brst.burstCount[0]);
				INDENTPRINT("BurstMtlID: " << brst.burstMtlID[0]);
				INDENTPRINT("BurstVperP: " << brst.burstVperP[0]);
				indent--;*/
			}
			else if (instr == "INDI")
			{
				indent++;
				
				int indices;
				suc = ReadInt(f, indices);
				if (suc)
				{
					if (indices > 32000)
					{
						INDENTPRINT("Warning: too many faces for JOE format: " << indices);
					}
					
					short index[indices];
					INDENTPRINT("Faces: " << indices/3.0);
					int bread = fread (index, 1, indices*sizeof(short), f);
					if ((unsigned int) bread != sizeof(short)*indices)
					{
						suc = false;
					}
					else
					{
						if (models[objid].pObject != NULL)
						{	
							INDENTPRINT("Error in file: double INDI declaration");
						}
						
						//create the object
						models[objid].pObject = new JOEObject;
						
						//set headers
						models[objid].pObject->info.magic = 0;
						models[objid].pObject->info.version = 3;
						models[objid].pObject->info.num_faces = indices/3;
						models[objid].pObject->info.num_frames = 1;
						models[objid].pObject->frames = new JOEFrame [1];
						
						models[objid].pObject->frames[0].num_verts = 0;
						models[objid].pObject->frames[0].num_texcoords = 0;
						models[objid].pObject->frames[0].num_normals = 0;
						
						models[objid].pObject->frames[0].faces = new JOEFace [models[objid].pObject->info.num_faces];
						
						for (unsigned int idx = 0; idx < (unsigned int) models[objid].pObject->info.num_faces; idx++)
						{
							VERBOSEPRINT(index[idx*3+0] << "," << index[idx*3+1] << "," << index[idx*3+2]);
							for (unsigned int n = 0; n < 3; n++)
							{
								models[objid].pObject->frames[0].faces[idx].vertexIndex[n] = index[idx*3+n];
								models[objid].pObject->frames[0].faces[idx].normalIndex[n] = index[idx*3+n];
								models[objid].pObject->frames[0].faces[idx].textureIndex[n] = index[idx*3+n];
							}
						}
					}
				}
				
				indent--;
			}
			else if (instr == "VERT")
			{
				indent++;
				
				int num;
				suc = ReadInt(f, num);
				if (suc)
				{
					float array[num*3];
					INDENTPRINT("Num: " << num);
					int bread = fread (array, 1, num*3*sizeof(float), f);
					if ((unsigned int) bread != num*3*sizeof(float))
					{
						suc = false;
					}
					else
					{
						models[objid].pObject->frames[0].num_verts = num;
						models[objid].pObject->frames[0].num_normals = num;
						models[objid].pObject->frames[0].num_texcoords = num;
						
						models[objid].pObject->frames[0].verts = new JOEVertex [models[objid].pObject->frames[0].num_verts];
						models[objid].pObject->frames[0].normals = new JOEVertex [models[objid].pObject->frames[0].num_verts];
						models[objid].pObject->frames[0].texcoords = new JOETexCoord [models[objid].pObject->frames[0].num_verts];
						
						models[objid].goodnorms = false;
						
						for (unsigned int idx = 0; idx < (unsigned int) models[objid].pObject->frames[0].num_verts; idx++)
						{
							/*for (unsigned int n = 0; n < 3; n++)
							{
								models[objid].pObject->frames[0].verts[idx].vertex[n] = array[idx*3+n];
							}*/
							
							/*pObject->frames[i].verts[v].vertex[0] = temp.y;
							pObject->frames[i].verts[v].vertex[1] = temp.z;
							pObject->frames[i].verts[v].vertex[2] = -temp.x;*/
							
							VERBOSEPRINT(array[idx*3+0] << "," << array[idx*3+1] << "," << array[idx*3+2]);
							
							models[objid].pObject->frames[0].verts[idx].vertex[0] = array[idx*3+0];
							models[objid].pObject->frames[0].verts[idx].vertex[1] = -array[idx*3+2];
							models[objid].pObject->frames[0].verts[idx].vertex[2] = array[idx*3+1];
							
							//assign defaults for tex coords
							models[objid].pObject->frames[0].texcoords[idx].u = 0;
							models[objid].pObject->frames[0].texcoords[idx].v = 0;
							
							//assign defaults for normals
							models[objid].pObject->frames[0].normals[idx].vertex[0] = 0;
							models[objid].pObject->frames[0].normals[idx].vertex[1] = 1;
							models[objid].pObject->frames[0].normals[idx].vertex[2] = 0;
						}
					}
				}
				
				indent--;
			}
			else if (instr == "NORM")
			{
				indent++;
				
				int num;
				suc = ReadInt(f, num);
				if (suc)
				{
					float array[num*3];
					INDENTPRINT("Num: " << num);
					int bread = fread (array, 1, num*3*sizeof(float), f);
					if ((unsigned int) bread != num*3*sizeof(float))
					{
						suc = false;
					}
					else
					{
						models[objid].goodnorms = true;
						
						for (unsigned int idx = 0; idx < (unsigned int) models[objid].pObject->frames[0].num_normals; idx++)
						{
							for (unsigned int n = 0; n < 3; n++)
							{
								models[objid].pObject->frames[0].normals[idx].vertex[n] = array[idx*3+n];
							}
						}
					}
				}
				
				indent--;
			}
			else if (instr == "VCOL")
			{
				indent++;
				
				int num;
				suc = ReadInt(f, num);
				if (suc)
				{
					float array[num*3];
					INDENTPRINT("Num: " << num);
					int bread = fread (array, 1, num*3*sizeof(float), f);
					if ((unsigned int) bread != num*3*sizeof(float))
					{
						suc = false;
					}
					else
					{
						for (unsigned int idx = 0; idx < (unsigned int) num; idx++)
						{
							//INDENTPRINT(array[idx*3+0] << "," << array[idx*3+1] << "," << array[idx*3+2]);
						}
					}
				}
				
				indent--;
			}
			else if (instr == "TVER")
			{
				indent++;
				
				int num;
				suc = ReadInt(f, num);
				if (suc)
				{
					float array[num*2];
					INDENTPRINT("Num: " << num);
					int bread = fread (array, 1, num*2*sizeof(float), f);
					if ((unsigned int) bread != num*2*sizeof(float))
					{
						suc = false;
					}
					else
					{
						for (unsigned int idx = 0; idx < (unsigned int) models[objid].pObject->frames[0].num_texcoords; idx++)
						{
							models[objid].pObject->frames[0].texcoords[idx].u = array[idx*2+0];
							models[objid].pObject->frames[0].texcoords[idx].v = 1.0-array[idx*2+1];
						}
					}
				}
				
				indent--;
			}
			else
			{
				for (unsigned int i = 0; i < indent+1; i++)
					cout << "\t";
				cout << "skipping chunk" << endl;;
				suc = ReadString(f, inint, instr);
			}
		}
		
		if (!suc)
		{
			for (unsigned int i = 0; i < indent; i++)
				cout << "\t";
			cout << "read error" << endl;
		}
		
		suc = ReadString(f, 4, instr);
	}
	
	if (!suc)
	{
		for (unsigned int i = 0; i < indent; i++)
			cout << "\t";
		cout << "read error" << endl;
	}
	
	return true;
}

bool ReadGEOB(FILE * f, int chunklen)
{
	string instr;
	int inint;
	bool suc = true;
	
	suc = ReadInt(f, inint);
	if (!suc)
	{
		for (unsigned int i = 0; i < indent; i++)
			cout << "\t";
		cout << "read error" << endl;
		return false;
	}
	
	for (unsigned int i = 0; i < indent; i++)
		cout << "\t";
	cout << inint << " object(s)" << endl;
	
	unsigned int numobj = inint;

	models.resize(numobj);
	
	for (unsigned int o = 0; o < numobj; o++)
	{
		suc = ReadString(f, 4, instr);
		if (suc && ReadInt(f, inint))
		{
			for (unsigned int i = 0; i < indent; i++)
				cout << "\t";
			cout << instr << ": len " << inint << endl;
			
			indent++;
			
			//suc = ReadString(f, inint, instr);
			suc = ReadGOB1(f, inint, o);
			
			if (!suc)
			{
				for (unsigned int i = 0; i < indent; i++)
					cout << "\t";
				cout << "read error" << endl;
				indent--;
				return false;
			}
			
			indent--;
		}
		else
		{
			for (unsigned int i = 0; i < indent; i++)
				cout << "\t";
			cout << "read error" << endl;
			return false;
		}
	}
	
	return true;
}

bool ReadMATS(FILE * f, int chunklen)
{
	string instr;
	int inint;
	bool suc = true;
	
	suc = ReadInt(f, inint);
	if (!suc)
	{
		for (unsigned int i = 0; i < indent; i++)
			cout << "\t";
		cout << "read error" << endl;
		return false;
	}
	
	for (unsigned int i = 0; i < indent; i++)
		cout << "\t";
	cout << inint << " material(s)" << endl;
	
	unsigned int nummat = inint;

	for (unsigned int m = 0; m < nummat; m++)
	{
		suc = ReadString(f, 4, instr);
		if (suc && ReadInt(f, inint))
		{
			for (unsigned int i = 0; i < indent; i++)
				cout << "\t";
			cout << instr << ": len " << inint << endl;
			
			indent++;
			
			suc = ReadMAT0(f, inint, m);
			if (!suc)
			{
				for (unsigned int i = 0; i < indent; i++)
					cout << "\t";
				cout << "read error" << endl;
				indent--;
				return false;
			}
			
			indent--;
		}
		else
		{
			for (unsigned int i = 0; i < indent; i++)
				cout << "\t";
			cout << "read error" << endl;
			return false;
		}
	}
	
	return true;
}

bool LoadDOF(string filename)
{
	indent = 0;
	
	FILE * f = fopen (filename.c_str(), "rb");
	if (f == NULL)
	{
		cout << "Can't open file \"" << filename << "\"" << endl;
		return false;
	}
	
	cout << "Loading \"" << filename << "\"" << endl;
	
	string instr;
	
	unsigned int filepos = 0;
	
	if (ReadString(f, 4, instr))
	{
		filepos += 4;
		
		if (instr == "DOF1")
		{
			int dofsize = 0;
			if (ReadInt(f, dofsize))
			{
				cout << "DOF1: len " << dofsize << endl;
				indent++;
				
				filepos += 4;
				
				//start the file reading engine
				int chunklen;
				bool readsuccess = ReadString(f, 4, instr);
				if (!(readsuccess && ReadInt(f, chunklen)))
				{
					readsuccess = false;
				}
				else
					filepos += 8;
				while (filepos < (unsigned int) dofsize && readsuccess && instr != "EDOF")
				{
					for (unsigned int i = 0; i < indent; i++)
						cout << "\t";
					cout << instr << ": len " << chunklen << endl;
					
					indent++;
					
					if (instr == "MATS")
					{
						readsuccess = ReadMATS(f, chunklen);
						filepos += chunklen;
					}
					else if (instr == "GEOB")
					{
						readsuccess = ReadGEOB(f, chunklen);
						filepos += chunklen;
					}
					else
					{
						for (unsigned int i = 0; i < indent; i++)
							cout << "\t";
						cout << "skipping chunk" << endl;
						ReadString(f, chunklen, instr);
						filepos += chunklen;
					}
					
					indent--;
					
					readsuccess = ReadString(f, 4, instr);
					if (!(readsuccess && ReadInt(f, chunklen)))
					{
						readsuccess = false;
					}
					else
						filepos += 8;
				}
				
				if (!readsuccess && instr != "EDOF")
				{
					for (unsigned int i = 0; i < indent; i++)
						cout << "\t";
					cout << "EOF" << endl;
					fclose(f);
					return false;
				}
				
				if (instr == "EDOF")
				{
					for (unsigned int i = 0; i < indent; i++)
						cout << "\t";
					cout << "Successfully got to end of DOF." << endl;
					fclose(f);
					return true;
				}
			}
			else {cout << "EOF" << endl;fclose(f);return false;}
		}
		else {cout << "not a DOF file" << endl;fclose(f);return false;}
	}
	else {cout << "EOF" << endl;fclose(f);return false;}
	
	fclose(f);
	
	return true;
}

void GenerateNormals(JOEMODEL & obj)
{
	//for now, I use a naive one normal per face 
	// normal generation approach (flat shading)
	
	if (obj.pObject != NULL)
	{
		for (int idx = 0; idx < obj.pObject->info.num_frames; idx++)
		{
			//clear out old normals
			delete [] obj.pObject->frames[idx].normals;
			
			//generate new memory
			obj.pObject->frames[idx].num_normals = obj.pObject->info.num_faces;
			obj.pObject->frames[idx].normals = new JOEVertex [obj.pObject->info.num_faces];
			
			for (int f = 0; f < obj.pObject->info.num_faces; f++)
			{
				//get verts
				VERTEX vert[3];
				for (int v = 0; v < 3; v++)
				{
					int vnum = obj.pObject->frames[idx].faces[f].vertexIndex[v];
					vert[v].Set(obj.pObject->frames[idx].verts[vnum].vertex);
					
					//flip axes around
					float tval = vert[v].x;
					vert[v].x = vert[v].y;
					vert[v].y = vert[v].z;
					vert[v].z = -tval;
				}
				
				//generate normal
				VERTEX normal;
				normal = (vert[2] - vert[0]).cross(vert[1] - vert[0]);
				normal = normal.normalize();
				
				//normal.DebugPrint();
				
				//set the normal
				obj.pObject->frames[idx].normals[f].vertex[0] = normal.x;
				obj.pObject->frames[idx].normals[f].vertex[1] = normal.y;
				obj.pObject->frames[idx].normals[f].vertex[2] = normal.z;
				
				//set all normal indices to correspond to the new normal index
				for (int v = 0; v < 3; v++)
					obj.pObject->frames[idx].faces[f].normalIndex[v] = f;
			}
		}
	}
}

void WriteObject(string filename, JOEMODEL & obj)
{
	if (obj.goodnorms == false)
	{
		GenerateNormals(obj);
	}
	
	FILE * f = fopen (filename.c_str(), "wb");
	
	if (f == NULL)
	{
		ERRORPRINT("Can't open \"" << filename << "\" for writing");
	}
	
	if (obj.pObject != NULL)
	{
		fwrite(&(obj.pObject->info), sizeof(JOEHeader), 1, f);
		
		for (int idx = 0; idx < obj.pObject->info.num_frames; idx++)
		{
			fwrite(obj.pObject->frames[idx].faces, sizeof(JOEFace), obj.pObject->info.num_faces, f);
			
			fwrite(&(obj.pObject->frames[idx].num_verts), sizeof(int), 1, f);
			fwrite(&(obj.pObject->frames[idx].num_texcoords), sizeof(int), 1, f);
			fwrite(&(obj.pObject->frames[idx].num_normals), sizeof(int), 1, f);
			
			for (int n = 0; n < obj.pObject->frames[idx].num_verts; n++)
			{
				VERBOSEPRINT(obj.pObject->frames[idx].verts[n].vertex[0] << "," << obj.pObject->frames[idx].verts[n].vertex[1] << "," << obj.pObject->frames[idx].verts[n].vertex[2]);
			}
			
			fwrite((obj.pObject->frames[idx].verts), sizeof(JOEVertex), obj.pObject->frames[idx].num_verts, f);
			fwrite((obj.pObject->frames[idx].normals), sizeof(JOEVertex), obj.pObject->frames[idx].num_normals, f);
			fwrite((obj.pObject->frames[idx].texcoords), sizeof(JOETexCoord), obj.pObject->frames[idx].num_texcoords, f);
		}
	}
	else
	{
		ERRORPRINT("Object is NULL");
	}
	
	fclose(f);
}

void ConvertTexture(string texture_filename, string outpath, string texname)
{
	char buffer[1024];/*
	if( strcmp(CONVERT,"mogrify") == 0 )
	{
		sprintf(buffer, "convert -format png %s %s/%s.png",
			texture_filename.c_str(), outpath.c_str(), texname.c_str());
	}
	else*/
	{
		sprintf(buffer, "nconvert -out png -o %s/%s.png %s",
			outpath.c_str(), texname.c_str(), texture_filename.c_str());
	}

	indent++;
	INDENTPRINT("Running \"" << buffer << "\"" << endl);
	system(buffer);
	indent--;
}

void WriteObjects(ofstream & lf, string filename, string outpath, string inpath)
{
	bool lcaseinput = true;
	
	cout << endl;
	
	char buffer[1024];
	
	for (unsigned int o = 0; o < models.size(); o++)
	{
		filename = StringToLower(filename);
		string texname = StringToLower(textures[models[o].textureid[0]].texture);
		if (texname.empty())
		{
			map<string,string>::iterator cur = shaders.find(textures[models[o].textureid[0]].name);
			if (cur != shaders.end())
			{
				//texname = shaders[textures[models[o].textureid[0]].name];
				texname = cur->second;
			}
		}

		if (texname.empty())
		{
			INDENTPRINT("Warning: Object " << o << " has no texture");
		}
		else if (!batch)
		{
			INDENTPRINT("Object " << o << ": Texture \"" << texname << "\"");

			string texture_filename = inpath + "/";
			texture_filename += lcaseinput ? texname : textures[models[o].textureid[0]].texture;
			ConvertTexture(texture_filename, outpath, texname);
		}
		
		indent++;
		sprintf(buffer, "%s/%s-%02d.joe", outpath.c_str(), filename.c_str(), o);
		
		if (outpath != "." && lf.good())
		{
			lf << endl << "#added by dof2joe" << endl;
			char buffer[1024];
			sprintf(buffer, "%s-%02d.joe", filename.c_str(), o);
			lf << buffer << endl;
			sprintf(buffer, "%s.png", texname.c_str());
			lf << buffer << endl;
			/* default parameter settings
			mipmap = 1
			nolighting = 0
			skybox = 0
			transparent_blend = 0
			bump_wavelength = 1
			bump_amplitude = 0
			driveable = false
			collideable = false
			friction_notread = 0
			friction_tread = 0
			rolling_resistance = 0
			rolling_drag = 0
			isashadow = 0
			clamptexture = 0
			surface = 0*/
			lf << "1\n0\n0\n0\n1\n0\n0\n0\n0\n0\n0\n0\n0\n0\n0" << endl;
		}
		
		WriteObject(buffer, models[o]);
		indent--;
	}
}

bool ShaderDent(ifstream & s, string desired, string undesired, bool allowundesired)
{
	string instr;
	int dent = 1;
	while (dent > 0 && !s.eof())
	{
		s >> instr;
		while (instr != desired && instr != undesired && !s.eof())
			s >> instr;
		if (instr == desired)
			dent--;
		if (instr == undesired)
		{
			if (allowundesired)
				dent++;
			else
				dent--;
		}
	}
	
	if (s.eof())
		return false;
	
	if (!allowundesired && instr == undesired)
		return false;
	
	else return true;
}

bool NextShaderIndent(ifstream & s)
{
	return ShaderDent(s, "{", "}", false);
}

bool NextShaderOutdent(ifstream & s)
{
	return ShaderDent(s, "}", "{", true);
}

string GetRemainder(string prefix, string str)
{
	string output;
	unsigned int startpos = prefix.length();
	if (startpos < str.length())
		output = str.substr(prefix.length());
	return output;
}

bool ProcessShader(ifstream & s, ostream & log, bool dolog)
{
	if (s.eof())
		return false;
	
	string rawhandle,handle;
	string rawmapcandidate,rawmap,map;
	s >> rawhandle;
	
	if (!NextShaderIndent(s))
	{
		if (dolog) log << "Error finding next indent" << endl;
		return false;
	}
	if (!NextShaderIndent(s))
	{
		if (dolog) log << "Error finding next indent" << endl;
		return false;
	}
	
	s >> rawmapcandidate;
	while (rawmapcandidate != "}" && !(rawmapcandidate.find("map") < rawmapcandidate.length()) && !s.eof())
		s >> rawmapcandidate;
	if (rawmapcandidate != "}")
	{
		rawmap = rawmapcandidate;
		
		handle = GetRemainder("shader_", rawhandle);
		map = GetRemainder("map=", rawmap);
		
		if (!handle.empty() && !map.empty())
		{
			if (dolog) log << "Handle: " << rawhandle << "," << handle << endl;
			if (dolog) log << "Map: " << rawmap << "," << map << endl;
			shaders[handle]=map;
			if (batch)
				alltextures[map] = true;
		}
		else
		{
			if (dolog) log << "Error:  malformed handle or map " << rawhandle << "," << rawmap << endl;
		}
		
		if (!NextShaderOutdent(s))
		{
			if (dolog) log << "Error finding next indent" << endl;
			return false;
		}
	}
	else
	{
		if (dolog) log << "Error: Couldn't find map" << endl;
	}

	if (!NextShaderOutdent(s))
	{
		if (dolog) log << "Error finding next indent" << endl;
		return false;
	}
	
	return true;
}

bool LoadShaders(string path)
{
	shaders.clear();
	string shaderfile = path+"/track.shd";
	ifstream s;
	bool dolog = true;
	s.open(shaderfile.c_str());
	if (s)
	{
		if (dolog)
		{
			ofstream log("shader.log");
			while (ProcessShader(s,log,dolog));
		}
		else
			while (ProcessShader(s,cout,dolog));
		return true;
	}
	else
	{
		//if (dolog) log << "No shaders found: " << shaderfile << endl;
	}
	
	return false;
}

void ConvertAllTextures(string inpath, string outpath)
{
	for (map<string,bool>::iterator i = alltextures.begin(); i != alltextures.end(); i++)
	{
		ConvertTexture(inpath+"/"+i->first, outpath, i->first);
	}
}

std::streampos fileSize(const char* filePath )
{
	std::streampos fsize = 0;
	std::ifstream file(filePath, ios::in | ios::binary);
	if (file.good())
	{
		fsize = file.tellg();
		file.seekg(0, std::ios::end);
		fsize = file.tellg() - fsize;
	}
	return fsize;
}

int main(int argc, char * argv[])
{
	if (argc <= 1)
	{
		cout << "Usage: dof2joe [-p output object path] file1.dof file2.dof ..." << endl;
		return 0;
	}
	
	vector <string> args;
	args.resize(argc);
	for (int i = 0; i < argc; i++)
	{
		args[i] = argv[i];
	}
	
	int curarg = 1;
	
	string outpath = ".";
	
	if (args[curarg] == "-p")
	{
		curarg++;
		outpath = args[curarg];
		curarg++;
		batch = true;
	}
	else
	{
		batch = false;
	}
	
	alltextures.clear();
	
	bool haveshaders = false;
	string batchpath;
	
	ofstream lf;
	string listfile = outpath + "/list.txt";
	if (curarg < argc && fileSize(listfile.c_str()) == 0)
	{
		// write parameter count
		lf.open(listfile.c_str());
		lf << "17" << endl;
		if (!lf.good())
		{
			cout << "Failed to open: " << listfile << std::endl;
			return 0;
		}
	}
	
	for (int i = curarg; i < argc; i++)
	{
		indent = 0;
		textures.clear();
		models.clear();
		
		LoadDOF(args[i]);
		
		string filename = args[i];
		
		string shortfn;
		string inpath;
		
		shortfn = filename;
		unsigned int loc = shortfn.rfind("/",shortfn.length()-1);
		if (loc + 1 < filename.length())
		{
			shortfn = shortfn.substr(loc+1);
		}
		
		inpath = ".";
		if (loc + 1 < filename.length())
		{
			inpath = filename.substr(0,loc);
		}
		
		if (batch && !haveshaders)
		{
			if (LoadShaders(inpath))
				cout << "Found shaders" << endl;
			haveshaders = true;
		}
		else if (!batch)
		{
			if (LoadShaders(inpath))
				cout << "Found shaders" << endl;
		}
		
		if (batch && batchpath.empty()) batchpath = inpath;
		
		WriteObjects(lf, shortfn, outpath, inpath);
	}
	
	if (batch)
	{
		//ConvertAllTextures(batchpath, outpath);
	}
	
	return 0;
}
