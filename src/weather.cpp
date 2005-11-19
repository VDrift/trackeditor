#include "weather.h"

#define TEMP_MIN_TEMP 65
#define TEMP_MAX_TEMP 100
#define FOG_TEMP_MAX 75
#define DEW_MIN_TEMP 55
#define DEW_MAX_TEMP 90
#define FOG_MAX 0.005f
#define FOG_MIN 0.0005f

void WEATHER::Init()
{
	//set the temperature to vary between 50 and 90 degrees, and change
	// every half a day or every 5 days
	//temperature.Init(50, 90, 0.5, 5.0f);
	
	temperature.Init(TEMP_MIN_TEMP, TEMP_MAX_TEMP, 1.0, 3.0f, 10.0f);
	
	dewpoint.Init(DEW_MIN_TEMP, DEW_MAX_TEMP, 0.2, 1.5f);
	
	globalwind_highfreq.Init(-0.0005f, 0.0005f, 0.0003f, 0.0006f);
	globalwind_lowfreq.Init(-0.002f, 0.002f, 0.01f, 0.04f);
}

void WEATHER::Update(double cur_time)
{
	abs_time = cur_time;
	
	temperature.Update(abs_time);
	dewpoint.Update(abs_time);
	globalwind_lowfreq.Update(abs_time);
	globalwind_highfreq.Update(abs_time);
	
	//cout << temperature.GetVal(abs_time) << ", " << dewpoint.GetVal(abs_time) << ":  "; GetGlobalWind().DebugPrint();
}

void WEATHERVARIABLE::Update(double abs_time)
{
	if (abs_time > start_time + duration)
		CalcNew(abs_time);
}

void WEATHERVARIABLE::CalcNew(double abs_time)
{
	value = next_value;
	start_time = abs_time;
	
	//is this needed?  are we seeding the random generator somewhere else??
	//srand((unsigned int) (abs_time*1000.0));
	
	duration = (rand()/(float)RAND_MAX)*(durmax-durmin)+durmin;
	next_value = (rand()/(float)RAND_MAX)*(valmax-valmin)+valmin;
}

void WEATHERVARIABLE::Init(float minval, float maxval, float mindur, float maxdur, float newnightdip)
{
	valmin = minval;
	valmax = maxval;
	durmin = mindur;
	durmax = maxdur;
	
	night_dip = newnightdip;
	
	CalcNew(-0.1);
	CalcNew(0.0);
}

void WEATHERVARIABLE::Init(float minval, float maxval, float mindur, float maxdur)
{
	valmin = minval;
	valmax = maxval;
	durmin = mindur;
	durmax = maxdur;
	
	night_dip = 0.0f;
	
	CalcNew(-0.1);
	CalcNew(0.0);
}

float WEATHERVARIABLE::GetVal(double abs_time)
{
	float outval;
	double timeindex = abs_time - start_time;
	float timefactor = timeindex / duration;
	
	outval = value*(1.0f-timefactor)+next_value*timefactor;
	
	//night dip stuff
	if (night_dip > 0)
	{
		float night=0;
		float ntime;
		int itime = (int) abs_time;
		ntime = abs_time - itime;
		float nightthresh = 0.2f;
		if (ntime <= 1.0f-nightthresh && ntime >= 0.5f+nightthresh )
			night = 1.0f;
		else if (ntime <= 0.5)
			night = 0.0f;
		else if (ntime > 1.0f-nightthresh)
			night = (1.0f-ntime)/nightthresh;
		else if (ntime > 0.5f && ntime < 0.5f + nightthresh)
			night = (ntime - 0.5f)/nightthresh;
		if (night > 1.0f)
			night = 1.0f;
		outval -= night*night_dip;
	}
	
	return outval;
}

int WEATHER::GetCloudCover()
{
	float ccover, temp, dew;
	temp = temperature.GetVal(abs_time);
	dew = dewpoint.GetVal(abs_time);
	
	//knock temp about 10 degrees at night
	
	if (temp > dew + CLOUD_FORMATION)
		ccover = 0.0f;
	else if (temp < dew)
		ccover = 1.0f;
	else
		ccover = 1.0f - (temp-dew)/CLOUD_FORMATION;
	
	ccover = 1.0f - ccover;
	
	ccover *= 255.0f;
	
	//cout << ccover - 128 << endl;
	
	return (int) (ccover - 128);
	//return -128;
	//return 120;
}

int WEATHER::GetCloudFuzziness()
{
	//eventually, this should depend a bit on wind
	return 50;
}

void WEATHERVERTEX::Update(double abs_time)
{
	int i;
	for (i = 0; i < 3; i++)
		components[i].Update(abs_time);
}

void WEATHERVERTEX::CalcNew(double abs_time)
{
	int i;
	for (i = 0; i < 3; i++)
	{
		components[i].CalcNew(abs_time);
	}
}

void WEATHERVERTEX::Init(float minval, float maxval, float mindur, float maxdur)
{
	int i;
	for (i = 0; i < 3; i++)
	{
		components[i].Init(minval, maxval, mindur, maxdur);
	}
}

VERTEX WEATHERVERTEX::GetVal(double abs_time)
{
	VERTEX outval;
	outval.x = components[0].GetVal(abs_time);
	outval.y = components[1].GetVal(abs_time);
	outval.z = components[2].GetVal(abs_time);
	return outval;
}

VERTEX WEATHER::GetGlobalWind()
{
	return globalwind_lowfreq.GetVal(abs_time)+globalwind_highfreq.GetVal(abs_time);
	//VERTEX zero;
	//return zero;
}

int WEATHER::GetRainDrops()
{
	float dew = dewpoint.GetVal(abs_time);
	float temp = temperature.GetVal(abs_time);
	
	if (temp > dew)
		return 0;
	
	//raindrops per degree of difference between the dew point and temperature
	float rainfactor = 100.0f;
	float maxrain = 500.0f;
	
	float raindrops = (dew-temp)*rainfactor;
	if (raindrops > maxrain)
		raindrops = maxrain;
	
	return (int) raindrops;
	//return 500;
}

float WEATHER::GetFogDensity()
{
	float dew = dewpoint.GetVal(abs_time);
	float temp = temperature.GetVal(abs_time);
	
	float fogfactor;
	
	//fog based on temperature
	if (temp > FOG_TEMP_MAX)
		fogfactor = 0.0f;
	else
		fogfactor = (FOG_TEMP_MAX-temp)/(FOG_TEMP_MAX-TEMP_MIN_TEMP);
	
	//add dewpoint coefficient
	float dewcoeff;
	if (dew > FOG_TEMP_MAX)
		dewcoeff = 0.0f;
	else
		dewcoeff = (FOG_TEMP_MAX-dew)/(FOG_TEMP_MAX-DEW_MIN_TEMP);
	fogfactor *= dewcoeff;
	
	float fogdensity = FOG_MIN*(1.0f-fogfactor)+FOG_MAX*(fogfactor);
	
	//cout << fogdensity << endl;
	
	return fogdensity;
}
