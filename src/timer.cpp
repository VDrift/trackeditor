/***************************************************************************
 *            timer.cpp
 *
 *  Wed Jun  1 21:31:45 2005
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
 
#include "timer.h"

TIMER::TIMER()
{
	time = 0.0;
	pretime = 0.0;
	lastlap = 0.0;
	bestlap = 0.0;
	
	ifstream blf;
	blf.open((settings.GetSettingsDir() + "/bestlap.txt").c_str());
	if (blf)
	{
		blf >> bestlap;
		blf.close();
	}
	
	loaded = false;
}

TIMER::~TIMER()
{
	if (loaded)
		glDeleteTextures(1, &timerbox);
}

void TIMER::Lap()
{	
	if (time > 0.5)
	{
		lastlap = time;
	
		if (lastlap < bestlap || bestlap == 0.0)
		{
			bestlap = lastlap;
			ofstream blf;
			blf.open((settings.GetSettingsDir() + "/bestlap.txt").c_str());
			blf << bestlap << endl;
			blf.close();
		}
	}
	
	time = 0.0;
}

float TIMER::GetTimer()
{
	return time;
}

void TIMER::TickTimer(float timefactor, float fps)
{
	if (state.GetGameState() == STATE_PLAYING)
		time += timefactor / fps;
	else if (state.GetGameState() == STATE_STAGING)
	{
		pretime += timefactor / fps;
		
		if (pretime > STAGING_TIME)
		{
			time = 0.0;
			pretime = 0.0;
			state.SetGameState(STATE_PLAYING);
		}
	}
}

void TIMER::Draw()
{
	float boxx, boxy;
	
	boxx = 0.01;
	boxy = 0.01;
	
	utility.Draw2D(boxx, boxy, 0.5+boxx, 0.5+boxy, timerbox);
	
	char tempchar[256];
	
	float secs;
	int min;
	
	float r,g,b;
	r=g=b=0.0;
	
	float cx, cy;
//	float vertspacing = 0.03;
	cx = 0.03+boxx;
	//cy = 0.005;
	cy = 0.0214+boxy;
	if (state.GetGameState() == STATE_STAGING)
	{
		SecsToSecsMin(STAGING_TIME - pretime, secs, min);
		sprintf(tempchar, "%02d:%06.3f", min, secs);
		font.Print(cx, cy, tempchar, 0, 6, 1,0,0);
	}
	else
	{
		SecsToSecsMin(time, secs, min);
		sprintf(tempchar, "%02d:%06.3f", min, secs);
		font.Print(cx, cy, tempchar, 0, 6, r,g,b);
	}
	//cy += vertspacing;
	
	{
		//cx = 0.195+boxx;
		cx = 0.193+boxx;
		SecsToSecsMin(lastlap, secs, min);
		if (lastlap != 0.0)
			sprintf(tempchar, "%02d:%06.3f", min, secs);
		else
			sprintf(tempchar, "--:--.---");
		font.Print(cx, cy, tempchar, 0, 6, r,g,b);
		//cy += vertspacing;
	}
	
	{
		cx = 0.35+boxx;
		SecsToSecsMin(bestlap, secs, min);
		if (bestlap != 0.0)
			sprintf(tempchar, "%02d:%06.3f", min, secs);
		else
			sprintf(tempchar, "--:--.---");
		font.Print(cx, cy, tempchar, 0, 6, r,g,b);
		//cy += vertspacing;
	}
	
	/*if (state.GetGameState() == STATE_STAGING)
	{
		cx = 0.03+boxx;
		cy = 0.0214+boxy;
		cy += 0.1;
		SecsToSecsMin(pretime, secs, min);
		sprintf(tempchar, "%02d:%06.3f", min, secs);
		font.Print(cx, cy, tempchar, 0, 6, 1,0,0);
	}*/
}

void TIMER::SecsToSecsMin(double t, float &secs, int &min)
{
	min = (int) t / 60;
	secs = t - min*60;
}

void TIMER::ResetBest()
{
	bestlap = 0.0;
	//bestlap = lastlap;
	ofstream blf;
	blf.open((settings.GetSettingsDir() + "bestlap.txt").c_str());
	blf << bestlap << endl;
	blf.close();
}

void TIMER::Reset()
{
	time = 0.0;
	pretime = 0.0;
}

void TIMER::Load()
{
	if (loaded)
		glDeleteTextures(1, &timerbox);
	
	timerbox = utility.TexLoad("hud/timerbox.png", GL_RGBA, false);
	
	loaded = true;
}
