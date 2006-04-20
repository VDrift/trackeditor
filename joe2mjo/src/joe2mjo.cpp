#include <string>
#include <iostream>
#include <cstdio>

#include <list>
#include <vector>

using namespace std;

#include "model.h"

const bool verbose = true;

#define DPRINT(text) {if (verbose) cout << text << endl;}

#define MJOVERSION "MJO1"

void AppendObject(FILE * f, JOEMODEL & obj)
{
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

			}
			
			fwrite((obj.pObject->frames[idx].verts), sizeof(JOEVertex), obj.pObject->frames[idx].num_verts, f);
			fwrite((obj.pObject->frames[idx].normals), sizeof(JOEVertex), obj.pObject->frames[idx].num_normals, f);
			fwrite((obj.pObject->frames[idx].texcoords), sizeof(JOETexCoord), obj.pObject->frames[idx].num_texcoords, f);
		}
	}
	else
	{
		DPRINT("Object is NULL");
	}
}

void WriteObject(string filename, JOEMODEL & obj)
{
	FILE * f = fopen (filename.c_str(), "wb");
	
	if (f == NULL)
	{
		DPRINT("Can't open \"" << filename << "\" for writing");
	}
	
	AppendObject(f, obj);
	
	fclose(f);
}

string ShortName(string s)
{
	char fn[1024];
	strcpy(fn, s.c_str());
	char * found = strtok(fn, "/\\");
	
	string lastfound = "";
	
	while (found != NULL)
	{
		//cout << found << endl;
		
		lastfound = found;
		found = strtok(NULL, "/\\");
	}
	
	//cout << "lastfound: " << lastfound << endl;
	return lastfound;
}

int main(int argc, char * argv[])
{
	if (argc <= 2)
	{
		cout << "Usage: joe2mjo -c output.mjo file1.joe file2.joe ..." << endl;
		cout << "Usage: joe2mjo -x input.mjo" << endl;
		cout << "-c\tCreate mjo file" << endl;
		cout << "-x\tExtract mjo file" << endl;
		return 0;
	}
	
	vector <string> args;
	args.resize(argc);
	for (unsigned int i = 0; i < (unsigned int) argc; i++)
	{
		args[i] = argv[i];
	}
	
	string mjofile = args[2];
	
	unsigned int curarg = 3;
	
	if (args[1] == "-c" && argc >= 3)
	{
		
		FILE * f = fopen (mjofile.c_str(), "wb");
		
		string versionstr = MJOVERSION;
		fwrite(versionstr.c_str(), 1, 4, f);
		
		unsigned int numobjs = argc - curarg;
		fwrite(&numobjs, sizeof(unsigned int), 1, f);
		
		for (unsigned int i = curarg; i < (unsigned int) argc; i++)
		{
			//DPRINT(args[i]);
			
			JOEMODEL inmodel;
			
			string objname = ShortName(args[i]);
			cout << "Reading " << objname << "..." << endl;
			inmodel.Load(args[i]);
			unsigned int namelen = objname.length();
			fwrite(&namelen, sizeof(unsigned int), 1, f);
			fwrite(objname.c_str(), 1, namelen, f);
			
			AppendObject(f, inmodel);
			
			/*indent = 0;
			textures.clear();
			models.clear();
			
			LoadDOF(args[i]);
			
			string filename = args[i];
			
			string shortfn = filename;
			unsigned int loc = shortfn.rfind("/",shortfn.length()-1);
			if (loc + 1 < filename.length())
			{
				shortfn = shortfn.substr(loc+1);
			}
			
			string inpath = ".";
			if (loc + 1 < filename.length())
			{
				inpath = filename.substr(0,loc);
			}
			
			WriteObjects(shortfn, outpath, inpath);*/
		}
		
		cout << "Done. " << argc - 3 << " files put in " << mjofile << endl;
		
		fclose(f);
	}
	else if (args[1] == "-x")
	{
		FILE * f = fopen (mjofile.c_str(), "rb");
		
		if (f == NULL)
		{
			cerr << "Can't open " << mjofile << endl;
			return 0;
		}
		
		char versionstr[5];
		fread(versionstr, 1, 4, f);
		versionstr[4] = '\0';
		string vstr = versionstr;
		if (vstr == MJOVERSION)
		{
			DPRINT(vstr);
			
			unsigned int numobjs;
			fread(&numobjs, sizeof(unsigned int), 1, f);
			
			for (int i = 0; i < numobjs; i++)
			{
				JOEMODEL inmodel;
				
				string objname;
				char objname_cstr[1024];
				unsigned int namelen = 0;
				fread(&namelen, sizeof(unsigned int), 1, f);
				fread(objname_cstr, 1, namelen, f);
				objname_cstr[namelen] = '\0';
				objname = objname_cstr;
				
				inmodel.LoadFromHandle(f);
				
				cout << "Writing " << objname << "..." << endl;
				//char temp[256];
				//sprintf(temp, "%d", i);
				WriteObject(objname, inmodel);
				//WriteObject(temp, inmodel);
			}
			
			cout << "Done. " << numobjs << " files written from " << mjofile << endl;
		}
		else
		{
			cerr << "Expected version " << MJOVERSION << " got version " << vstr << endl;
		}
		
		fclose(f);
	}
	else
	{
		cout << "Expected -c or -x" << endl << endl;
		cout << "Usage: joe2mjo -c output.mjo file1.joe file2.joe ..." << endl;
		cout << "Usage: joe2mjo -x input.mjo" << endl;
		cout << "-c\tCreate mjo file" << endl;
		cout << "-x\tExtract mjo file" << endl;
		return 0;
	}
	
	return 0;
}
