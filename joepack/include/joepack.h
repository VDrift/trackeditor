#ifndef _JOEPACK_H
#define _JOEPACK_H

#include <cstdio>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <cmath>
#include <cassert>

using namespace std;

#include <map>

class JOEPACK_FADATA
{
public:
	JOEPACK_FADATA() {offset = 0; length = 0;}
	unsigned int offset;
	unsigned int length;
};

class JOEPACK
{
private:
	map < string, JOEPACK_FADATA> fat;
	FILE * f;
	JOEPACK_FADATA * curfa;
	
public:
	JOEPACK() {f = NULL;curfa = NULL;}
	~JOEPACK() {ClosePack();}
	
	bool LoadPack(string fn);
	void ClosePack();
	
	bool Pack_fopen(string fn);
	void Pack_fclose();
	int Pack_fread(void * buffer, unsigned int size, unsigned int count);
	
	map <string, JOEPACK_FADATA> & GetFAT() {return fat;}
};

#endif
