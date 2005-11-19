/***************************************************************************
 *            configfile.h
 *
 *  Sun Oct  2 18:34:24 2005
 *  Copyright  2005  Joe Venzon
 *  joe@venzon.net
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#ifndef _CONFIGFILE_H
#define _CONFIGFILE_H

#include <stdio.h>			// Header File For Standard Input/Output
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <cassert>

using namespace std;

//see the user's guide at the bottom of the file

class CONFIGVARIABLE
{
public:
	CONFIGVARIABLE();	

	string section;
	string name;

	string GetFullName();

	string val_s;
	int val_i;
	float val_f;
	float val_v[3];
	bool val_b;

	CONFIGVARIABLE * next;

	void Set(string newval);

	void DebugPrint();

	string strLTrim(string instr);
	string strRTrim(string instr);
	string strTrim(string instr);
	string strLCase(string instr);

	bool written;
};

class CONFIGFILE
{
private:
	string filename;
	CONFIGVARIABLE * vars;
	void Add(CONFIGVARIABLE * newvar);
	string Trim(string instr);
	void ProcessLine(string & cursection, string linestr);
	string Strip(string instr, char stripchar);
	string LCase(string instr);
	bool SUPPRESS_ERROR;

public:
	CONFIGFILE();
	CONFIGFILE(string fname);
	~CONFIGFILE();

	void Load(string fname);
	void Clear();

	//returns true if the param was found
	bool ClearParam(string param);

	//returns true if the param was found
	bool GetParam(string param, string & outvar);
	bool GetParam(string param, int & outvar);
	bool GetParam(string param, float & outvar);
	bool GetParam(string param, float * outvar); //for float[3]
	bool GetParam(string param, bool & outvar);

	//always returns true at the moment
	bool SetParam(string param, string invar);
	bool SetParam(string param, int invar);
	bool SetParam(string param, float invar);
	bool SetParam(string param, float * invar);
	bool SetParam(string param, bool invar);

	void DebugPrint();

	bool Write();
	bool Write(bool with_brackets);
	bool Write(bool with_brackets, string save_as);
	
	void SuppressError(bool newse) {SUPPRESS_ERROR = newse;}
	
	CONFIGVARIABLE * GetHead() {return vars;}
};

#endif /* _CONFIGFILE_H */

/* USER GUIDE

Paste the file included below somewhere, then run this code:

	CONFIGFILE testconfig("/home/joe/.vdrift/test.cfg");
	testconfig.DebugPrint();
	string tstr = "notfound";
	cout << "!!! test vectors: " << endl;
	testconfig.GetParam("variable outside of", tstr);
	cout << tstr << endl;
	tstr = "notfound";
	testconfig.GetParam(".variable outside of", tstr);
	cout << tstr << endl;
	float vec[3];
	testconfig.GetParam("what about.even vectors", vec);
	cout << vec[0] << "," << vec[1] << "," << vec[2] << endl;

your output should be the debug print of all variables, then:

	!!! test vectors: 
	a section
	a section
	2.1,0.9,0


*/

/* EXAMPLE (not a good example -- tons of errors -- to demonstrate robustness)

#comment on the FIRST LINE??

		variable outside of=a section  

test section numero UNO
look at me = 23.4

i'm so great=   BANANA
#break!
[ section    dos??]
 why won't you = breeeak #trying to break it

what about ] # this malformed thing???
nope works = fine.
even vectors = 2.1,0.9,GAMMA
this is a duplicate = 0
this is a duplicate = 1
random = intermediary
this is a duplicate = 2


*/
