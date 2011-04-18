#include "joepack.h"
#include "endian_utility.h"

#include <string>
using std::string;

#include <map>
using std::map;

#include <fstream>
using std::ios_base;

#include <cassert>

bool JOEPACK::LoadPack(const string & fn)
{
	ClosePack();
	
	f.open(fn.c_str(), ios_base::in | ios_base::binary);
	
	if (f)
	{
		packpath = fn;
		
		//load header
		char * versioncstr = new char[versionstr.length() + 1];
		f.read(versioncstr, versionstr.length());
		versioncstr[versionstr.length()] = '\0';
		string fversionstr = versioncstr;
		delete [] versioncstr;
		if (fversionstr != versionstr)
		{
			//write out an error?
			return false;
		}
		
		unsigned int numobjs = 0;
		assert(sizeof(unsigned int) == 4);
		f.read((char*)(&numobjs), sizeof(unsigned int));
		numobjs = ENDIAN_SWAP_32(numobjs);
		
		//DPRINT(numobjs << " objects");
		
		unsigned int maxstrlen = 0;
		f.read((char*)(&maxstrlen), sizeof(unsigned int));
		maxstrlen = ENDIAN_SWAP_32(maxstrlen);
		
		//DPRINT(maxstrlen << " max string length");
		
		char * fnch = new char[maxstrlen+1];
		
		//load FAT
		for (unsigned int i = 0; i < numobjs; i++)
		{
			JOEPACK_FADATA fa;
			f.read((char*)(&(fa.offset)), sizeof(unsigned int));
			fa.offset = ENDIAN_SWAP_32(fa.offset);
			f.read((char*)(&(fa.length)), sizeof(unsigned int));
			fa.length = ENDIAN_SWAP_32(fa.length);
			f.read(fnch, maxstrlen);
			fnch[maxstrlen] = '\0';
			string filename = fnch;
			fat[filename] = fa;
			
			//DPRINT(filename << ": offest " << fa.offset << " length " << fa.length);
		}
		
		delete [] fnch;
		return true;
	}
	else
	{
		//write an error?
		return false;
	}
}

void JOEPACK::ClosePack()
{
	Pack_fclose();
	fat.clear();
	if (f.is_open())
	{
		f.close();
	}
	packpath.clear();
	curfa = fat.end();
}

void JOEPACK::Pack_fclose()
{
	curfa = fat.end();
}

std::istream & JOEPACK::Pack_fopen(string fn)
{
	if (fn.find(packpath, 0) < fn.length())
	{
		string newfn = fn.substr(packpath.length()+1);
		fn = newfn;
	}
	
	curfa = fat.find(fn);
	if (curfa == fat.end())
	{
		f.seekg(0, ios_base::end);
	}
	else
	{
		f.seekg(curfa->second.offset);
	}
	return f;
}
