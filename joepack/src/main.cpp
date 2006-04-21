#include <string>
#include <iostream>
#include <cstdio>

#include <list>
#include <vector>

using namespace std;

#include "joepack.h"

const bool verbose = false;

#define DPRINT(text) {if (verbose) cout << text << endl;}

#define JPKVERSION "JPK01.00"

#define CHUNKSIZE 1024

void PrintUsage()
{
	cout << "Usage: joepack -c output.jpk file1.joe file2.joe ..." << endl;
	cout << "Usage: joepack -x input.jpk" << endl;
	cout << "-c\tCreate jpk file" << endl;
	cout << "-x\tExtract jpk file" << endl;
}

unsigned int FileLength(string fn)
{
	unsigned int flen = 0;
	unsigned int numread = CHUNKSIZE;
	
	FILE * f = fopen(fn.c_str(), "rb");
	
	if (f)
	{
		char buffer[CHUNKSIZE+16];
		
		while (numread == CHUNKSIZE)
		{
			numread = fread(buffer, 1, 1024, f);
			flen += numread;
		}
		
		DPRINT(fn << ": length " << flen);
		
		fclose(f);
	}
	else
	{
		cerr << "Can't open file " << fn << endl;
	}
	
	return flen;
}

bool WriteFileToPack(FILE * f, string fn)
{
	unsigned int flen = 0;
	unsigned int numread = CHUNKSIZE;
	
	FILE * inf = fopen(fn.c_str(), "rb");
	
	if (inf)
	{
		char buffer[CHUNKSIZE+16];
		
		while (numread == CHUNKSIZE)
		{
			numread = fread(buffer, 1, CHUNKSIZE, inf);
			fwrite(buffer, 1, numread, f);
			flen += numread;
		}
		
		DPRINT(fn << ": added " << flen << " bytes");
		
		fclose(inf);
		
		return true;
	}
	else
	{
		cerr << "Can't open file " << fn << endl;
		return false;
	}
}

void Unpack(string fn, JOEPACK & pack)
{
	if (pack.Pack_fopen(fn))
	{
		FILE * f = fopen(fn.c_str(), "wb");
		
		if (!f)
		{
			cerr << "Can't open " << fn << " for writing." << endl;
		}
		else
		{
			char buffer[CHUNKSIZE+16];
			unsigned int numread = CHUNKSIZE;
			
			while (numread == CHUNKSIZE)
			{
				numread = pack.Pack_fread(buffer, 1, CHUNKSIZE);
				fwrite(buffer, 1, numread, f);
				//DPRINT("Unpack: numread = " << numread);
				//flen += numread;
			}
			
			fclose(f);
		}
		
		pack.Pack_fclose();
	}
	else
	{
		cerr << "Pack has an internal consistency error." << endl;
	}
}

int main(int argc, char * argv[])
{
	if (argc <= 2)
	{
		PrintUsage();
		return 0;
	}
	
	vector <string> args;
	args.resize(argc);
	for (unsigned int i = 0; i < (unsigned int) argc; i++)
	{
		args[i] = argv[i];
	}
	
	string jpkfile = args[2];
	
	unsigned int curarg = 3;
	
	if (args[1] == "-c" && argc >= 3)
	{
		FILE * f = fopen (jpkfile.c_str(), "wb");
		
		//write out the header
		string versionstr = JPKVERSION;
		fwrite(versionstr.c_str(), 1, versionstr.length(), f);
		
		unsigned int numobjs = argc - curarg;
		fwrite(&numobjs, sizeof(unsigned int), 1, f);
		
		unsigned int maxstrlen = 0;
		
		for (unsigned int i = curarg; i < (unsigned int) argc; i++)
		{
			if (args[i].length() > maxstrlen)
				maxstrlen = args[i].length();
		}
		
		fwrite(&maxstrlen, sizeof(unsigned int), 1, f);
		
		char cstrfilename[maxstrlen+1];
		
		unsigned int datastart = versionstr.length() + sizeof(unsigned int) * 2
				+ numobjs*(maxstrlen+(sizeof(unsigned int)*2));
		
		DPRINT("data starts at " << datastart);
		
		unsigned int curpos = datastart;
		
		//write out the FAT
		for (unsigned int i = curarg; i < (unsigned int) argc; i++)
		{
			strcpy(cstrfilename, args[i].c_str());
			cstrfilename[maxstrlen] = '\0';
			cstrfilename[args[i].length()] = '\0';
			
			//string objname = ShortName(args[i]);
			string objname = args[i];
			cout << "Reading " << objname << "..." << endl;
			//unsigned int namelen = objname.length();
			unsigned int filelen = FileLength(objname);
			fwrite(&curpos, 1, sizeof(unsigned int), f);
			fwrite(&filelen, 1, sizeof(unsigned int), f);
			fwrite(cstrfilename, 1, maxstrlen, f);
			
			DPRINT("putting " << cstrfilename << " at " << curpos << " with length " << filelen);
			
			curpos += filelen;
		}
		
		for (unsigned int i = curarg; i < (unsigned int) argc; i++)
		{
			string objname = args[i];
			
			DPRINT(objname << " ftell: " << ftell(f));
			
			WriteFileToPack(f, objname);
		}
		
		DPRINT(maxstrlen << " is maximum filename length");
		
		cout << "Done. " << argc - 3 << " files put in " << jpkfile << endl;
		
		DPRINT("Output file size: " << curpos);
		
		fclose(f);
	}
	else if (args[1] == "-x")
	{
		JOEPACK pack;
		if (!pack.LoadPack(args[2]))
		{
			cerr << "Error loading joepack " << args[2] << endl;
		}
		else
		{
			//pack.Pack_fopen("example/auto14.dof-00.joe");
			/*pack.Pack_fopen("../../howtobuild.txt");
			char test[1025];
			int retval = pack.Pack_fread(test, 1, 1024);
			test[retval] = '\0';
			DPRINT(retval << ": " << test);*/
			
			map <string, JOEPACK_FADATA> & fat = pack.GetFAT();
			
			for (map <string, JOEPACK_FADATA>::iterator i = fat.begin(); i != fat.end(); i++)
			{
				cout << "unpacking " << i->first << "..." << endl;
				Unpack(i->first, pack);
			}
			
			cout << "Done." << endl;
		}
	}
	else
	{
		//cout << "Expected -c or -x" << endl << endl;
		PrintUsage();
		return 0;
	}
	
	return 0;
}
