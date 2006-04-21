#include "joepack.h"

#define JPKVERSION "JPK01.00"

#define VERBOSE false

#define DPRINT(text) {if (VERBOSE) cout << text << endl;}

bool JOEPACK::LoadPack(string fn)
{
	ClosePack();
	
	f = fopen(fn.c_str(), "rb");
	
	if (f)
	{
		//load header
		string versionstr = JPKVERSION;
		char versioncstr[versionstr.length()+1];
		fread(versioncstr, 1, versionstr.length(), f);
		versioncstr[versionstr.length()] = '\0';
		string fversionstr = versioncstr;
		if (versioncstr != versionstr)
		{
			//write out an error?
			return false;
		}
		
		unsigned int numobjs = 0;
		fread(&numobjs, sizeof(unsigned int), 1, f);
		
		DPRINT(numobjs << " objects");
		
		unsigned int maxstrlen = 0;
		fread(&maxstrlen, sizeof(unsigned int), 1, f);
		
		DPRINT(maxstrlen << " max string length");
		
		char fnch[maxstrlen+1];
		
		//load FAT
		for (unsigned int i = 0; i < numobjs; i++)
		{
			JOEPACK_FADATA fa;
			fread(&(fa.offset), 1, sizeof(unsigned int), f);
			fread(&(fa.length), 1, sizeof(unsigned int), f);
			fread(fnch, 1, maxstrlen, f);
			fnch[maxstrlen] = '\0';
			string filename = fnch;
			fat[filename] = fa;
			
			DPRINT(filename << ": offest " << fa.offset << " length " << fa.length);
		}
		
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
	if (f != NULL)
		fclose(f);
}

void JOEPACK::Pack_fclose()
{
	curfa = NULL;
}

bool JOEPACK::Pack_fopen(string fn)
{
	DPRINT("Opening " << fn << " by seeking to " << fat[fn].offset);
	
	if (fat[fn].offset == 0)
	{
		curfa = NULL;
		//write an error?
		return false;
	}
	else
	{
		curfa = &(fat[fn]);
		fseek(f, curfa->offset, SEEK_SET);
		return true;
	}
}

int JOEPACK::Pack_fread(void * buffer, unsigned int size, unsigned int count)
{
	if (curfa != NULL)
	{
		unsigned int abspos = ftell(f);
		assert(abspos >= curfa->offset);
		unsigned int relpos = abspos - curfa->offset;
		//DPRINT("relpos: " << relpos);
		assert(curfa->length >= relpos);
		unsigned int fileleft = curfa->length - relpos;
		unsigned int requestedread = size*count;
		
		if (requestedread > fileleft)
		{
			//overflow
			requestedread = fileleft;
		}
		
		DPRINT("JOEPACK fread: " << abspos << "," << relpos << "," << fileleft << "," << requestedread);
		
		return fread(buffer, 1, requestedread, f);
	}
	else
	{
		//write error?
		return 0;
	}
}
