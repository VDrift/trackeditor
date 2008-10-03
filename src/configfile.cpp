/***************************************************************************
 *            configfile.cc
 *
 *  Sun Oct  2 18:34:08 2005
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
 
#include "configfile.h"
#include <cstdlib>

CONFIGFILE::CONFIGFILE()
{
	vars = NULL;
	filename = "";
	SUPPRESS_ERROR = false;
}

CONFIGFILE::CONFIGFILE(string fname)
{
	vars = NULL;
	
	SUPPRESS_ERROR = false;
	
	Load(fname);
}

CONFIGFILE::~CONFIGFILE()
{
	Clear();
}

bool CONFIGFILE::GetParam(string param, int & outvar)
{
	CONFIGVARIABLE * cur = vars;
	bool found = false;
	while (cur != NULL)
	{
		//if the var section/name matches the param name, set outvar
		
		string pname = param;
		string psec = "";
		string::size_type ppos;
		ppos = param.find(".", 0);
		if (ppos < /*(int)*/ param.length())
		{
			psec = param.substr(0, ppos);
			ppos++;
			pname = param.substr(ppos, param.length() - ppos);
		}
		
		if (pname == cur->name && psec == cur->section)
		{
			found = true;
			outvar = cur->val_i;
		}
		
		cur = cur->next;
	}
	
	if (!found && !SUPPRESS_ERROR)
	{
		cout << "CONFIGFILE::GetParam():  " << param << " not found in " << filename << endl;
	}
	
	return found;
}

bool CONFIGFILE::GetParam(string param, bool & outvar)
{
	CONFIGVARIABLE * cur = vars;
	bool found = false;
	while (cur != NULL)
	{
		//if the var section/name matches the param name, set outvar
		
		string pname = param;
		string psec = "";
		string::size_type ppos;
		ppos = param.find(".", 0);
		if (ppos < /*(int)*/ param.length())
		{
			psec = param.substr(0, ppos);
			ppos++;
			pname = param.substr(ppos, param.length() - ppos);
		}
		
		if (pname == cur->name && psec == cur->section)
		{
			found = true;
			outvar = cur->val_b;
		}
		
		cur = cur->next;
	}
	
	if (!found && !SUPPRESS_ERROR)
	{
		cout << "CONFIGFILE::GetParam():  " << param << " not found in " << filename << endl;
	}
	
	return found;
}

bool CONFIGFILE::GetParam(string param, float & outvar)
{
	CONFIGVARIABLE * cur = vars;
	bool found = false;
	while (cur != NULL)
	{
		//if the var section/name matches the param name, set outvar
		
		string pname = param;
		string psec = "";
		string::size_type ppos;
		ppos = param.find(".", 0);
		if (ppos < /*(int)*/ param.length())
		{
			psec = param.substr(0, ppos);
			ppos++;
			pname = param.substr(ppos, param.length() - ppos);
		}
		
		if (pname == cur->name && psec == cur->section)
		{
			found = true;
			outvar = cur->val_f;
		}
		
		cur = cur->next;
	}
	
	if (!found && !SUPPRESS_ERROR)
	{
		cout << "CONFIGFILE::GetParam():  " << param << " not found in " << filename << endl;
	}
	
	return found;
}

bool CONFIGFILE::GetParam(string param, float * outvar)
{
	CONFIGVARIABLE * cur = vars;
	bool found = false;
	while (cur != NULL)
	{
		//if the var section/name matches the param name, set outvar
		
		string pname = param;
		string psec = "";
		string::size_type ppos;
		ppos = param.find(".", 0);
		if (ppos < /*(int)*/ param.length())
		{
			psec = param.substr(0, ppos);
			ppos++;
			pname = param.substr(ppos, param.length() - ppos);
		}
		
		if (pname == cur->name && psec == cur->section)
		{
			found = true;
			
			int i;
			for (i = 0; i < 3; i++)
				outvar[i] = cur->val_v[i];
		}
		
		cur = cur->next;
	}
	
	if (!found && !SUPPRESS_ERROR)
	{
		cout << "CONFIGFILE::GetParam():  " << param << " not found in " << filename << endl;
	}
	
	return found;
}

bool CONFIGFILE::GetParam(string param, string & outvar)
{
	CONFIGVARIABLE * cur = vars;
	bool found = false;
	while (cur != NULL)
	{
		//if the var section/name matches the param name, set outvar
		
		string pname = param;
		string psec = "";
		string::size_type ppos;
		ppos = param.find(".", 0);
		if (ppos < /*(int)*/ param.length())
		{
			psec = param.substr(0, ppos);
			ppos++;
			pname = param.substr(ppos, param.length() - ppos);
		}
		
		if (pname == cur->name && psec == cur->section)
		{
			found = true;
			outvar = cur->val_s;
		}
		
		cur = cur->next;
	}
	
	if (!found && !SUPPRESS_ERROR)
	{
		cout << "CONFIGFILE::GetParam():  " << param << " not found in " << filename << endl;
	}
	
	return found;
}

void CONFIGFILE::Clear()
{
	CONFIGVARIABLE * cur = vars;
	while (cur != NULL)
	{
		CONFIGVARIABLE * todel = cur;
		cur = cur->next;
		
		delete todel;
	}
	
	vars = NULL;
}

void CONFIGFILE::Add(CONFIGVARIABLE * newvar)
{
	//check for dups
	CONFIGVARIABLE * cur = vars;
	bool isdup = false;
	while (cur != NULL)
	{
		if (cur->section == newvar->section && cur->name == newvar->name)
		{
			isdup = true;
			cur->Set(newvar->val_s);
		}
		
		cur = cur->next;
	}
	
	//go ahead and insert
	if (!isdup)
	{
		/*CONFIGVARIABLE * oldfirst = vars;
		
		vars = newvar;
		vars->next = oldfirst;*/

		cur = vars;
		CONFIGVARIABLE * last = vars;
		while (cur != NULL)
		{
			last = cur;
			cur = cur->next;
		}
		if (vars == NULL)
		{
			vars = newvar;
		}
		else
			last->next = newvar;
		newvar->next = cur;
	}
	else
		delete newvar;
}

CONFIGVARIABLE::CONFIGVARIABLE()
{
	val_s = "";
	val_i = 0;
	val_f = 0;
	val_b = false;
	int i;
	for (i = 0; i < 3; i++)
		val_v[i] = 0;
	
	next = NULL;
}

string CONFIGVARIABLE::GetFullName()
{
	string outstr = "";
	
	if (section != "")
		outstr = outstr + section + ".";
	outstr = outstr + name;
	
	return outstr;
}

void CONFIGVARIABLE::Set(string newval)
{
	newval = strTrim(newval);
	
	val_i = atoi(newval.c_str());
	val_f = atof(newval.c_str());
	val_s = newval;
	
	val_b = false;
	if (val_i == 0)
		val_b = false;
	if (val_i == 1)
		val_b = true;
	if (strLCase(newval) == "true")
		val_b = true;
	if (strLCase(newval) == "false")
		val_b = false;
	if (strLCase(newval) == "on")
		val_b = true;
	if (strLCase(newval) == "off")
		val_b = false;
	
	//now process as vector information
	int pos = 0;
	int arraypos = 0;
	string::size_type nextpos = newval.find(",", pos);
	string frag;
	
	while (nextpos < /*(int)*/ newval.length() && arraypos < 3)
	{
		frag = newval.substr(pos, nextpos - pos);
		val_v[arraypos] = atof(frag.c_str());
		
		pos = nextpos+1;
		arraypos++;
		nextpos = newval.find(",", pos);
	}
	
	//don't forget the very last one
	if (arraypos < 3)
	{
		frag = newval.substr(pos, newval.length() - pos);
		val_v[arraypos] = atof(frag.c_str());
	}
}

void CONFIGVARIABLE::DebugPrint()
{
	if (section != "")
		cout << section << ".";
	cout << name << endl;
	cout << "string: " << val_s << endl;
	cout << "int: " << val_i << endl;
	cout << "float: " << val_f << endl;
	cout << "vector: (" << val_v[0] << "," << val_v[1] << "," << val_v[2] << ")" << endl;
	cout << "bool: " << val_b << endl;
	
	cout << endl;
}

string CONFIGVARIABLE::strLTrim(string instr)
{
	string tempstr = "";
	
	string::size_type inpos = 0;
	
	if (inpos < instr.length())
	{		
		while (inpos < /*(int)*/ instr.length() && (instr.c_str()[inpos] == ' ' || instr.c_str()[inpos] == '\t'))
			inpos++;
		
		while (inpos < /*(int)*/ instr.length())
		{
			tempstr = tempstr + instr.substr(inpos, 1);
			inpos++;
		}
	}
	
	return tempstr;
}

string CONFIGVARIABLE::strRTrim(string instr)
{
	string tempstr = "";
	
	string::size_type inpos = instr.length()-1;
	
	if (inpos < instr.length())
	{	
		while (inpos >= 0 && (instr.c_str()[inpos] == ' ' || instr.c_str()[inpos] == '\t'))
			inpos--;
		
		while (inpos >= 0 && inpos < /*(int)*/ instr.length())
		{
			tempstr = instr.substr(inpos, 1) + tempstr;
			inpos--;
		}
	}
	
	return tempstr;
}

string CONFIGVARIABLE::strTrim(string instr)
{
	return strLTrim(strRTrim(instr));
}

void CONFIGFILE::Load(string fname)
{
	filename = fname;
	
	string cursection = "";
	
	//work string
	string ws;
	const int MAXIMUMCHAR = 1024;
	char trashchar[MAXIMUMCHAR];
	
	ifstream f;
	f.open(fname.c_str());
	
	if (!f && !SUPPRESS_ERROR)
	{
		cout << "CONFIGFILE.Load: Couldn't find file:  " << fname << endl;
	}
		
	while (f && !f.eof())
	{
		f.getline(trashchar, MAXIMUMCHAR, '\n');
		ProcessLine(cursection, trashchar);
	}
	
	f.close();
	
	//DebugPrint();
}

string CONFIGFILE::Trim(string instr)
{
	CONFIGVARIABLE trimmer;
	string outstr = trimmer.strTrim(instr);
	return outstr;
}

void CONFIGFILE::ProcessLine(string & cursection, string linestr)
{
	linestr = Trim(linestr);
	linestr = Strip(linestr, '\r');
	linestr = Strip(linestr, '\n');
	
	//remove comments
	string::size_type commentpos = linestr.find("#", 0);
	if (commentpos < /*(int)*/ linestr.length())
	{
		linestr = linestr.substr(0, commentpos);
	}
	
	linestr = Trim(linestr);
	
	//only continue if not a blank line or comment-only line
	if (linestr.length() > 0)
	{
		if (linestr.find("=", 0) < linestr.length())
		{
			//find the name part
			string::size_type equalpos = linestr.find("=", 0);
			string name = linestr.substr(0, equalpos);
			equalpos++;
			string val = linestr.substr(equalpos, linestr.length() - equalpos);
			name = Trim(name);
			val = Trim(val);
			
			//only continue if valid
			if (name.length() > 0 && val.length() > 0)
			{
				CONFIGVARIABLE * newvar;
				newvar = new CONFIGVARIABLE;
				newvar->section = cursection;
				newvar->name = name;
				newvar->Set(val);
				
				Add(newvar);
			}
		}
		else
		{
			//section header
			linestr = Strip(linestr, '[');
			linestr = Strip(linestr, ']');
			linestr = Trim(linestr);
			cursection = linestr;
		}
	}
}

string CONFIGFILE::Strip(string instr, char stripchar)
{
	string::size_type pos = 0;
	string outstr = "";
	
	while (pos < /*(int)*/ instr.length())
	{
		if (instr.c_str()[pos] != stripchar)
			outstr = outstr + instr.substr(pos, 1);
		
		pos++;
	}
	
	return outstr;
}

void CONFIGFILE::DebugPrint()
{
	CONFIGVARIABLE * cur = vars;
	cout << "*** " << filename << " ***" << endl << endl;
	while (cur != NULL)
	{
		cur->DebugPrint();
		
		cur = cur->next;
	}
}

string CONFIGVARIABLE::strLCase(string instr)
{
	char tc[2];
	tc[1] = '\0';
	string outstr = "";
	
	string::size_type pos = 0;
	while (pos < /*(int)*/ instr.length())
	{
		if (instr.c_str()[pos] <= 90 && instr.c_str()[pos] >= 65)
		{
			tc[0] = instr.c_str()[pos] + 32;
			string tstr = tc;
			outstr = outstr + tc;
		}
		else
			outstr = outstr + instr.substr(pos, 1);
		
		pos++;
	}
	
	return outstr;
}

string CONFIGFILE::LCase(string instr)
{
	CONFIGVARIABLE lcaser;
	string outstr = lcaser.strLCase(instr);
	return outstr;
}

bool CONFIGFILE::SetParam(string param, int invar)
{
	char tc[256];
	
	sprintf(tc, "%i", invar);
	
	string tstr = tc;
	
	return SetParam(param, tstr);
}

bool CONFIGFILE::SetParam(string param, bool invar)
{
	//char tc[256];
	
	//sprintf(tc, "%i", invar);
	
	string tstr = "off";
	
	if (invar)
		tstr = "on";
	
	return SetParam(param, tstr);
}

bool CONFIGFILE::SetParam(string param, float invar)
{
	char tc[256];
	
	sprintf(tc, "%f", invar);
	
	string tstr = tc;
	
	return SetParam(param, tstr);
}

bool CONFIGFILE::SetParam(string param, float * invar)
{
	char tc[256];
	
	sprintf(tc, "%f,%f,%f", invar[0], invar[1], invar[2]);
	
	string tstr = tc;
	
	return SetParam(param, tstr);
}

bool CONFIGFILE::SetParam(string param, string invar)
{
	CONFIGVARIABLE * newvar;
	newvar = new CONFIGVARIABLE;
	
	newvar->name = param;
	newvar->section = "";
	string::size_type ppos;
	ppos = param.find(".", 0);
	if (ppos < /*(int)*/ param.length())
	{
		newvar->section = param.substr(0, ppos);
		ppos++;
		newvar->name = param.substr(ppos, param.length() - ppos);
	}
	
	newvar->Set(invar);
	
	Add(newvar);
	
	return true;
}

bool CONFIGFILE::Write(bool with_brackets)
{
	return Write(with_brackets, filename);
}

bool CONFIGFILE::Write(bool with_brackets, string save_as)
{
	ofstream f;
	f.open(save_as.c_str());
	
	if (f)
	{
		//clear out written flags
		CONFIGVARIABLE * cur = vars;
		while (cur != NULL)
		{
			cur->written = false;
			cur = cur->next;
		}
		
		//write non-section variables first
		bool nosection = false;
		cur = vars;
		while (cur != NULL)
		{
			if (cur->section == "")
			{
				f << cur->name << " = " << cur->val_s << endl;
				
				nosection = true;
				
				cur->written = true;
			}
			
			cur = cur->next;
		}
		
		if (nosection)
			f << endl;
		
		//write variables by section
		cur = vars;
		while (cur != NULL)
		{
			if (!cur->written)
			{
				if (with_brackets)
					f << "[ " << cur->section << " ]" << endl;
				else
					f << cur->section << endl;
				
				CONFIGVARIABLE * sub = vars;
				while (sub != NULL)
				{
					if (!sub->written && cur->section == sub->section)
					{
						f << sub->name << " = " << sub->val_s << endl;
						
						sub->written = true;
					}
					
					sub = sub->next;
				}
				
				f << endl;
				
				cur->written = true;
			}
			
			cur = cur->next;
		}
		
		f.close();
		return true;
	}
	else
		return false;
}

bool CONFIGFILE::Write()
{
	Write(true);
	return true;
}

bool CONFIGFILE::ClearParam(string param)
{
	/*CONFIGVARIABLE * cur = vars;
	while (cur != NULL)
	{
		CONFIGVARIABLE * todel = cur;
		cur = cur->next;
		
		delete todel;
	}
	
	vars = NULL;*/
	
	CONFIGVARIABLE * cur = vars;
	CONFIGVARIABLE * prev = vars;
	bool found = false;
	while (cur != NULL)
	{
		//if the var section/name matches the param name, set outvar
		
		string pname = param;
		string psec = "";
		string::size_type ppos;
		ppos = param.find(".", 0);
		if (ppos < /*(int)*/ param.length())
		{
			psec = param.substr(0, ppos);
			ppos++;
			pname = param.substr(ppos, param.length() - ppos);
		}
		
		if (pname == cur->name && psec == cur->section)
		{
			found = true;

			CONFIGVARIABLE * todel = cur;
			if (cur == vars)
			{
				vars = cur->next;
				prev = vars;
			}
			else
			{
				cur = cur->next;
				prev->next = cur->next;
			}
			
			delete todel;
		}
		else
		{		
			prev = cur;
			cur = cur->next;
		}
	}

	/*	
	if (!found && !SUPPRESS_ERROR)
	{
		cout << "CONFIGFILE::GetParam():  " << param << " not found in " << filename << endl;
	}*/
	
	return found;
}
