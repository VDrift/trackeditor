/***************************************************************************
 *            controls.cc
 *
 *  Sat Jun  4 08:18:38 2005
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
 
#include "controls.h"

void CONTROL::Dump()
{
	cout << "Name: " << controlname << endl;
	cout << "Type: " << (int) controltype << " = ";
	if (controltype == Joy)
	{
		cout << "Joy" << endl;
		cout << "Joy number: " << joynum << endl;
		cout << "Input type: ";
		if (joytype == Button)
		{
			cout << "Button" << endl;
			cout << "Button number: " << joybutton << endl;
			cout << "One time: " << onetime << endl;
		}
		else if (joytype == Axis)
		{
			cout << "Axis" << endl;
			cout << "Axis number: " << joyaxis << endl;
			cout << "Positive axis: " << joyaxisplus << endl;
		}
		else
			cout << "Unknown" << endl;
	}
	else if (controltype == Key)
	{
		cout << "Key" << endl;
		cout << "Keycode: " << keycode << endl;
		cout << "One time: " << onetime << endl;
	}
	else
		cout << "Unknown" << endl;
}


void GAMECONTROLS::LoadControls()
{
	LoadCalibration();
	
	string fname;
	fname = settings.GetSettingsDir() + "/controls";
	ifstream controlfile(fname.c_str());
	
	if (!controlfile)
		error_log << "Couldn't open control file: " << fname << endl;
	
	//first pass to get the number of lines
	string input;
	input = utility.sGetLine(controlfile);
	numcontrols = 0;
	while (input != "!!!END OF FILE!!!")
	{
		input = utility.sGetLine(controlfile);
		if(input.find("joysticktype") == string::npos && input.find("!!!END OF FILE!!!") == string::npos)
			numcontrols++;
	}
	
	if (controls != NULL)
		delete [] controls;
	
	controls = new CONTROL [numcontrols];
	
	controlfile.close();
	controlfile.open(fname.c_str());
	controlfile.clear();
	
	//second pass to actually read the file
	
	int i;
	for (i = 0; i < numcontrols; i++)
	{
		input = utility.sGetParam(controlfile);
		if (input == "joysticktype")
		{
			joysticktype = utility.sGetParam(controlfile);
			//input = utility.sGetParam(controlfile);
			compensation = utility.sGetParam(controlfile);
			deadzone = utility.sGetParam(controlfile);
			if (joysticktype != "wheel" && joysticktype != "joystick")
			{
				cout << "invalid joystick type in " + settings.GetSettingsDir() + "/controls: " << joysticktype << endl;
				joysticktype = "joystick";
			}
			if (compensation != "off" && compensation != "low" && compensation != "med" && compensation != "high" && compensation != "900to200")
			{
				cout << "invalid joystick touchiness compensation in " + settings.GetSettingsDir() + "/controls: " << compensation << endl;
				compensation = "off";
			}
			if (deadzone != "off" && deadzone != "low" && deadzone != "med" && deadzone != "high")
			{
				cout << "invalid joystick deadzone in " + settings.GetSettingsDir() + "/controls: " << deadzone << endl;
				deadzone = "off";
			}
			i--;
		}
		else
		{
			controls[i].SetName(input);
			//controls[i].SetName(utility.sGetParam(controlfile));
			controls[i].SetType(utility.sGetParam(controlfile));
			if (controls[i].GetType() == Joy)
			{
				controls[i].SetJoyNum(utility.iGetParam(controlfile));
				controls[i].SetJoyType(utility.sGetParam(controlfile));
				if (controls[i].GetJoyType() == Button)
				{
					controls[i].SetJoyButton(utility.iGetParam(controlfile));
					controls[i].SetOneTime(utility.bGetParam(controlfile));
					
					//if (controls[i].GetOneTime())
						controls[i].SetJoyPushDown(utility.bGetParam(controlfile));
				}
				else if (controls[i].GetJoyType() == Axis)
				{
					controls[i].SetJoyAxis(utility.iGetParam(controlfile));
					controls[i].SetJoyAxisPlus(utility.bGetParam(controlfile));
				}
				else
				{
					ControlParseError(i, controlfile);
					i = numcontrols; //exit loop
				}
			}
			else if (controls[i].GetType() == Key)
			{
				controls[i].SetKeyCode(keyman.GetKey(utility.sGetParam(controlfile)));
				controls[i].SetOneTime(utility.bGetParam(controlfile));
				controls[i].SetKeyPushDown(utility.bGetParam(controlfile));
			}
			else
			{
				ControlParseError(i, controlfile);
				i = numcontrols; //exit loop
			}
		}
	}
	
	//for debugging
	/*for (i = 0; i < numcontrols; i++)
	{
		controls[i].Dump();
		cout << endl;
	}*/
}

void GAMECONTROLS::ControlParseError(int line, ifstream & ffrom)
{
	error_log << "Error parsing control number " << line << endl;
	ffrom.unget();
	ffrom.unget();
	ffrom.unget();
	ffrom.unget();
	ffrom.unget();
	string errorword;
	ffrom >> errorword;
	ffrom.seekg(0, ios_base::beg);
	string errorline;
	int i;
	for (i = 0; i <= line; i++)
		errorline = utility.sGetLine(ffrom);
	error_log << "The line was: " << errorline << endl;
	//error_log << "The error is in the vicinity of: " << errorword << endl;
}

GAMECONTROLS::GAMECONTROLS()
{
	controls = NULL;
	numcontrols = 0;
	
	error_log.open((settings.GetSettingsDir() + "/logs/controls.log").c_str());
	
	joysticktype = "joystick";
	
	int i;
	for (i = 0; i < MAX_JOYSTICKS; i++)
	{
		int m;
		for (m = 0; m < MAX_AXES; m++)
		{
			calibration[i][m][0] = -1;
			calibration[i][m][1] = 1;
		}
	}
}

extern bool verbose_output;
GAMECONTROLS::~GAMECONTROLS()
{
	if (verbose_output)
		cout << "gamecontrols deinit" << endl;
	
	if (controls != NULL)
		delete [] controls;
	
	error_log.close();
}

CONTROL * GAMECONTROLS::GetControls()
{
	return controls;
}

int GAMECONTROLS::GetNumControls()
{
	return numcontrols;
}

void GAMECONTROLS::SetControls(CONTROL * newcontrols, int newnum)
{
	delete [] controls;
	controls = newcontrols;
	numcontrols = newnum;
}

void GAMECONTROLS::WriteControlFile()
{
	string fname;
	fname = settings.GetSettingsDir() + "/controls";
	ofstream controlfile(fname.c_str());
	
	if (!controlfile)
		error_log << "Couldn't open control file for writing: " << fname << endl;
	
	controlfile << "#Generated by VDrift, feel free to edit manually" << endl << endl;

	controlfile << "# joystick type:  wheel or joystick, touchiness compensation" << endl;	
	controlfile << "joysticktype " << joysticktype << " " << compensation << " " << deadzone << endl << endl;
	
	controlfile << "# for a joystick button:" << endl << "# [function name] joy [joystick number] button [button number] [true for not held down, false for held down] [true to trigger when button is pressed, false to trigger when button is released]" << endl << endl;
	
	controlfile << "# for a joystick axis:" << endl << "# [function name] joy [joystick number] axis [axis number] [true for positive axis, false for negative axis]" << endl << endl;
	
	controlfile << "# for a keyboard key:" << endl << "# [fuction name] key [key name as defined in lists/keys] [true for keys that aren't held down, false for keys that are]  [true to trigger when key is pressed, false to trigger when key is released]" << endl << endl;
	
	int i;
	for (i = 0; i < numcontrols; i++)
	{
		controlfile << controls[i].GetName() << " ";
		if (controls[i].GetType() == Joy)
		{
			controlfile << "joy ";
			controlfile << controls[i].GetJoyNum() << " ";
			
			if (controls[i].GetJoyType() == Axis)
			{
				controlfile << "axis ";
				controlfile << controls[i].GetJoyAxis() << " ";
				if (controls[i].GetJoyAxisPlus())
					controlfile << "true";
				else
					controlfile << "false";
			}
			else if (controls[i].GetJoyType() == Button)
			{
				controlfile << "button ";
				controlfile << controls[i].GetJoyButton() << " ";
				if (controls[i].GetOneTime())
					controlfile << "true ";
				else
					controlfile << "false ";
				if (controls[i].GetJoyPushDown())
					controlfile << "true";
				else
					controlfile << "false";
			}
		}
		else if (controls[i].GetType() == Key)
		{
			controlfile << "key ";
			controlfile << keyman.GetKeyName(controls[i].GetKeyCode()) << " ";
			if (controls[i].GetOneTime())
				controlfile << "true ";
			else
				controlfile << "false ";
			if (controls[i].GetKeyPushDown())
				controlfile << "true";
			else
				controlfile << "false";
		}
		controlfile << endl;
		/*input = utility.sGetParam(controlfile);
		if (input == "joysticktype")
		{
			joysticktype = utility.sGetParam(controlfile);
		}
		else
		{
			controls[i].SetName(input);
			//controls[i].SetName(utility.sGetParam(controlfile));
			controls[i].SetType(utility.sGetParam(controlfile));
			if (controls[i].GetType() == Joy)
			{
				controls[i].SetJoyNum(utility.iGetParam(controlfile));
				controls[i].SetJoyType(utility.sGetParam(controlfile));
				if (controls[i].GetJoyType() == Button)
				{
					controls[i].SetJoyButton(utility.iGetParam(controlfile));
					controls[i].SetOneTime(utility.bGetParam(controlfile));
					
					if (controls[i].GetOneTime())
						controls[i].SetJoyPushDown(utility.bGetParam(controlfile));
				}
				else if (controls[i].GetJoyType() == Axis)
				{
					controls[i].SetJoyAxis(utility.iGetParam(controlfile));
					controls[i].SetJoyAxisPlus(utility.bGetParam(controlfile));
				}
				else
				{
					ControlParseError(i, controlfile);
					i = numcontrols; //exit loop
				}
			}
			else if (controls[i].GetType() == Key)
			{
				controls[i].SetKeyCode(keyman.GetKey(utility.sGetParam(controlfile)));
				controls[i].SetOneTime(utility.bGetParam(controlfile));
			}
			else
			{
				ControlParseError(i, controlfile);
				i = numcontrols; //exit loop
			}
		}*/
	}
	
	controlfile.close();
}

void GAMECONTROLS::LoadCalibration()
{
	ifstream cf;
	cf.open((settings.GetSettingsDir() + "/calibration").c_str());
	if (cf)
	{
		int i, m;
		for (i = 0; i < MAX_JOYSTICKS; i++)
		{
			for (m = 0; m < MAX_AXES; m++)
			{
				calibration[i][m][0] = utility.fGetParam(cf);
				calibration[i][m][1] = utility.fGetParam(cf);
				
				if (calibration[i][m][0] > -0.1)
					calibration[i][m][0] = -0.1;
				if (calibration[i][m][1] < 0.1)
					calibration[i][m][1] = 0.1;
			}
		}
	
		cf.close();
	}
}

void GAMECONTROLS::WriteCalibration()
{
	ofstream cf;
	cf.open((settings.GetSettingsDir() + "/calibration").c_str());
	if (cf)
	{
		cf << "# VDrift joystick calibration file.\n#Each line is an axis with the format [min] [max]" << endl << endl;
		int i, m;
		for (i = 0; i < MAX_JOYSTICKS; i++)
		{
			for (m = 0; m < MAX_AXES; m++)
				cf << calibration[i][m][0] << " " << calibration[i][m][1] << endl;
		}
		
		cf.close();
	}
	else
		cout << "Error writing calibration file to " + settings.GetSettingsDir() + "/calibration" << endl;
}

float GAMECONTROLS::GetCalibration(int joynum, int joyaxis, bool max)
{
	float c;
	
	if (joynum >= MAX_JOYSTICKS || joyaxis >= MAX_AXES)
	{
		if (max)
			return 1;
		else
			return -1;
	}

	if (max)
		c = calibration[joynum][joyaxis][1];
	else
		c = calibration[joynum][joyaxis][0];
	
	return c;
}

void GAMECONTROLS::InitJoy()
{
	int j;
	for (j = 0; j < SDL_NumJoysticks(); j++)
	{
		js[j] = SDL_JoystickOpen(j);
	}
}

void GAMECONTROLS::DeinitJoy()
{
	int j;
	for (j = 0; j < SDL_NumJoysticks(); j++)
		SDL_JoystickClose(js[j]);
}

void GAMECONTROLS::MinimizeCalibration()
{
	int i;
	for (i = 0; i < MAX_JOYSTICKS; i++)
	{
		int m;
		for (m = 0; m < MAX_AXES; m++)
		{
			calibration[i][m][0] = -0.1;
			calibration[i][m][1] = 0.1;
		}
	}
}

void GAMECONTROLS::SetCalibrationPoint(int joynum, int joyaxis, float val)
{
	if (val < calibration[joynum][joyaxis][0])
		calibration[joynum][joyaxis][0] = val;
	if (val > calibration[joynum][joyaxis][1])
		calibration[joynum][joyaxis][1] = val;
}
