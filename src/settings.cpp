#include "settings.h"
#include "globals.h"

using namespace std;

#define NUM_FILES_CHK 7
#define NUM_SUBDIRS_CHK 5

SETTINGS::SETTINGS()
{
	// Enable/disable spewing debug output
	spew = true;	

	// Get user's home directory
	FindHomeDir();

	// Figure out where the settings are to be
	settings_directory = "settings";
#ifndef _WIN32
	settings_path = home_directory + settings_directory;
#else
	if(home_directory == "data/")
	{
	    settings_path = home_directory + "settings";
	}
	else
	{
	    settings_path = home_directory + settings_directory;    
	}
#endif

	data_directory = "data";

	// Read saved settings
	default_configfile.Load((data_directory + "/settings/VDrift.config").c_str());

	Init();

	configfile.Load((data_directory + "/settings/VDrift.config").c_str());

	ParseSettings(configfile);

	data_directory = "data";

/*
	if(!DirExists(data_directory))
	{
		cout << endl;
		cout << "WARNING!! Could not find the VDrift data directory. This is currently set to:" << endl;
		cout << data_directory << endl;;
		cout << "You may change this by editing " + settings_path + "/VDrift.config or by running vdrift thus:" << endl;
		cout << "vdrift -datadir /path/to/vdrift/data" << endl;
		cout << "Continuing to run, probably won't work..." << endl << endl;
	}
*/
}

SETTINGS::~SETTINGS()
{
	// Save current settings
	SaveSettings(configfile);
}

void SETTINGS::Init()
{
	//const string files[NUM_FILES_CHK] = {"controls", "carsettings/CS", "carsettings/XS", "carsettings/GT", "carsettings/XSR", "carsettings/XM"};
	//const string subdirs[NUM_SUBDIRS_CHK] = {"replays/", "screenshots/", "logs/", "carsettings/", "userdata/"};
 
	// Check for settings dir and create if non-existant (I think that a directory cannot be opened as a file)
/*	if(!FileExists(settings_path + "/VDrift.config"))
	{
		// Whine
		cout << "Can't find the settings directory at \"" + settings_path + "\". Making a new one..." << endl;
		// Make it
		CreateDir(settings_path);

		// Copy file
		default_configfile.Write( true, settings_path + "/VDrift.config" );


#ifndef _WIN32
		//string copy_command = "cp -rvf " + data_directory + "/settings/* " + settings_path;
#else
		//string copy_command = "xcopy /E /Y \"" + data_directory + "/settings\" \"" + settings_path + "\""; 
#endif
		//system(copy_command.c_str());
	}

	// Check for subdirs
	for( int i = 0; i < NUM_SUBDIRS_CHK; i++ )
	{
		if(!DirExists(settings_path + "/" + subdirs[i]))
		{
			cout << "Missing " + settings_path + "/" + subdirs[i] + " directory, creating." << endl;
			CreateDir(settings_path + "/" + subdirs[i]);
//		}
	}

	// Check for files
	for( int i = 0; i < NUM_FILES_CHK; i++ )
	{
		if( !FileExists( settings_path + "/" + files[i] ) )
		{
			cout << "Missing " + settings_path + "/" + files[i] + " file, copying." << endl;

			CONFIGFILE c;
			c.Load( data_directory + "/settings/" + files[i] );
			c.Write( true, settings_path + "/" + files[i] );
			c.Clear();

#ifndef _WIN32
			//string copy_command = "cp -vf " + data_directory + "/settings/" + files[i] + " " + settings_path + "/" + files[i];
#else
			//string copy_command = "copy /Y \""+ data_directory + "/settings/" + files[i] + "\" \"" + settings_path + "/" + files[i] + "\"";
#endif
			//system(copy_command.c_str());
		}
		else
		{
			cout << "Found config file " + settings_path + "/" + files[i] + "." << endl;
		}
	}
	*/
	// define VDrift.config's settings' valid types
	// TODO: load these from a file instead of hard-coding them here
	// floats
	
	// vectors (float,float,float) - there are none in VDrift.config
	// ints - these are settings that are used as integers, not 1/0 (these are under bool)
	int_settings.push_back("display.width");
	int_settings.push_back("display.height");
	int_settings.push_back("display.depth");
	// bools - 1/0 values
	bool_settings.push_back("display.fullscreen");
	bool_settings.push_back("display.show_fps");
	// strings
	string_settings.push_back("display.texture_size");
}

void SETTINGS::CreateDir(string path)
{
#ifndef _WIN32
	// use mode 755
	mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#else
	// Quick & Dirty
	string winpath = "md \""+path+"\"";
	//system(winpath.c_str());
	mkdir(path.c_str());
#endif
}

void SETTINGS::FindHomeDir()
{
	/* Figure out the user's home directory - POSIX only... */
	char *homedir;
#ifndef _WIN32
	homedir = getenv("HOME");
	if (homedir == NULL)
	{
		homedir = getenv("USER");
		if (homedir == NULL)
		{
			homedir = getenv("USERNAME");
			if (homedir == NULL)
			{
				fprintf( stderr, "Could not find user's home directory!\n" );
				//Quit( 1 );
			}
		}
		home_directory = "/home/";
	}
#else
	homedir = getenv("USERPROFILE");
	if(homedir == NULL)
	{
	    homedir = "data"; // WIN 9x/Me
	}
#endif
	home_directory += homedir;
	home_directory += "/";
}

/*
bool SETTINGS::FileExists(string path)
{
	int result;
	struct stat dir_stat;
	result = stat(path.c_str(), &dir_stat);
	return ( result == 0 );
}

bool SETTINGS::DirExists(string path)
{
	int result;
	struct stat dir_stat;
	result = stat(path.c_str(), &dir_stat);
	return  S_ISDIR(dir_stat.st_mode);
}
*/

bool SETTINGS::FileExists(string path)
{
	bool result;
	ifstream testfile(path.c_str());
	result = !testfile.fail();
	return result;
	
	/*return utility.FileExists(path);*/
}

void SETTINGS::SaveSettings(CONFIGFILE &c)
{
	// set the current parameters in the config file

/*	All this has got to go !
*/
	c.SetParam("main.data_dir", data_directory);
	c.SetParam("main.version", config_version);

	c.SetParam("display.width", display_x);
	c.SetParam("display.height", display_y);
	c.SetParam("display.depth", display_depth);
	c.SetParam("display.fullscreen", display_full);
	c.SetParam("display.tree_detail", display_tree);
	c.SetParam("display.terrain_detail", display_terrain);
	c.SetParam("display.show_hud", display_hud);
	c.SetParam("display.show_fps", display_fps);
	c.SetParam("display.mph", display_mph);
	c.SetParam("display.texture_size", display_texsize);
	c.SetParam("display.view_distance", display_distance);

	c.SetParam("game.selected_car", game_car);
	c.SetParam("game.car_paint", game_paint);
	c.SetParam("game.camera_mode", game_camera);
	c.SetParam("game.game_mode", game_mode);
	c.SetParam("game.track", game_track);

	c.SetParam("network.host_game", net_host);
	c.SetParam("network.server_ip", net_ip);
	c.SetParam("network.server_port", net_port);

	c.SetParam("sound.volume", sound_volume);

	c.SetParam("mouse.enabled", mouse_enabled);
	c.SetParam("mouse.xsens", mouse_xsens);
	c.SetParam("mouse.ydead", mouse_ydead);
	c.SetParam("mouse.ysens", mouse_ysens);

	c.SetParam("control.autoclutch", control_autoclutch);
/*	End of stuff that needs to go
*/
	// save the config file
	c.Write(true);
}

void SETTINGS::ParseSettings(CONFIGFILE &c)
{

/*	All this has got to go
*/

#ifndef __APPLE__
	if(!c.GetParam("main.data_dir", data_directory))
	{
#ifndef _WIN32
		data_directory = "/usr/share/games/vdrift";
#else
		data_directory = "data";
#endif
		c.SetParam("main.data_dir", data_directory);
	}
#endif
	if(!c.GetParam("main.version", config_version))
	{
		config_version = "pre-2005-11-03";
		c.SetParam("main.version", config_version);
	}

	if(!c.GetParam("display.width", display_x))
	{
		display_x = 800;
		c.SetParam("display.width", display_x);
	}
	if(!c.GetParam("display.height", display_y))
	{
		display_y = 600;
		c.SetParam("display.height", display_y);
	}
	if(!c.GetParam("display.depth", display_depth))
	{
		display_depth = 16;
		c.SetParam("display.depth", display_depth);
	}
	if(!c.GetParam("display.fullscreen", display_full))
	{
		display_full = 0;
		c.SetParam("display.fullscreen", display_full);
	}
	if(!c.GetParam("display.tree_detail", display_tree))
	{
		display_tree = "Low";
		c.SetParam("display.tree_detail", display_tree);
	}
	if(!c.GetParam("display.terrain_detail", display_terrain))
	{
		display_terrain = "Low";
		c.SetParam("display.terrain_detail", display_terrain);
	}
	if(!c.GetParam("display.show_hud", display_hud))
	{
		display_hud = 1;
		c.SetParam("display.show_hud", display_hud);
	}
	if(!c.GetParam("display.show_fps", display_fps))
	{
		display_fps = 0;
		c.SetParam("display.show_fps", display_fps);
	}
	if(!c.GetParam("display.mph", display_mph))
	{
		display_mph = 1;
		c.SetParam("display.mph", display_mph);
	}
	if(!c.GetParam("display.texture_size", display_texsize))
	{
		display_texsize = "medium";
		c.SetParam("display.texture_size", display_texsize);
	}
	if(!c.GetParam("display.view_distance", display_distance))
	{
		display_distance = 10000.0;
		c.SetParam("display.view_distance", display_distance);
	}

	if(!c.GetParam("game.selected_car", game_car))
	{
		game_car = "XS";
		c.SetParam("game.selected_car", game_car);
	}
	if(!c.GetParam("game.car_paint", game_paint))
	{
		game_paint = 0;
		c.SetParam("game.car_paint", game_paint);
	}
	if(!c.GetParam("game.camera_mode", game_camera))
	{
		game_camera = 1;
		c.SetParam("game.camera_mode", game_camera);
	}
	if(!c.GetParam("game.game_mode", game_mode))
	{
		game_mode = 0;
		c.SetParam("game.game_mode", game_mode);
	}
	if(!c.GetParam("game.track", game_track))
	{
		game_track = "Nurburgring";
		c.SetParam("game.track", game_track);
	}

	if(!c.GetParam("network.host_game", net_host))
	{
		net_host = 0;
		c.SetParam("network.host_game", net_host);
	}
	if(!c.GetParam("network.server_ip", net_ip))
	{
		net_ip = "127.0.0.1";
		c.SetParam("network.server_ip", net_ip);
	}
	if(!c.GetParam("network.server_port", net_port))
	{
		net_port = 1234;
		c.SetParam("network.server_port", net_port);
	}

	if(!c.GetParam("sound.volume", sound_volume))
	{
		sound_volume = 0.8f;
		c.SetParam("sound.volume", sound_volume);
	}

	if(!c.GetParam("mouse.enabled", mouse_enabled))
	{
		mouse_enabled = 0;
		c.SetParam("mouse.enabled", mouse_enabled);
	}
	if(!c.GetParam("mouse.xsens", mouse_xsens))
	{
		mouse_xsens = 1.0f;
		c.SetParam("mouse.xsens", mouse_xsens);
	}
	if(!c.GetParam("mouse.ydead", mouse_ydead))
	{
		mouse_ydead = 0.2f;
		c.SetParam("mouse.ydead", mouse_ydead);
	}
	if(!c.GetParam("mouse.ysens", mouse_ysens))
	{
		mouse_ysens = 1.0f;
		c.SetParam("mouse.ysens", mouse_ysens);
	}

	if(!c.GetParam("control.autoclutch", control_autoclutch))
	{
		control_autoclutch = 1;
		c.SetParam("control.autoclutch", control_autoclutch);
	}

/*	End of stuff that needs to go
*/
}

vector<string> SETTINGS::LoadCarPartsSettings()
{
	string filename = settings.GetSettingsDir() + "/carsettings/" + game_car;
	char partid[256];
	int i = 0;
	string part;
	parts_list.clear();
	car_parts.Clear();

	car_parts.Load( filename.c_str() );
	car_parts.SuppressError( true );
	sprintf( partid, "parts.part-%02d", i );
	
	while( car_parts.GetParam( partid, part ) )
	{
		//cout << "part loaded: " << part;
		parts_list.push_back( part );
		i++;
		sprintf( partid, "parts.part-%02d", i );
	}
	return parts_list;
}

void SETTINGS::SaveCarPartsSettings(vector<string> &p_list)
{
	char partid[256];
	for(int i = 0; i < 100; i++)
	{
		sprintf( partid, "parts.part-%02d", i );
		car_parts.ClearParam(partid);
	}
	for(unsigned int i = 0; i < p_list.size(); i++)
	{
		sprintf( partid, "parts.part-%02d", i );
		car_parts.SetParam( partid, p_list[i] );
	}
	car_parts.Write();
}

string SETTINGS::GetCarPartName(string part_path)
{
	string filename = settings.GetFullDataPath("carparts/" + part_path);
	CONFIGFILE part_file;
	string result;
	part_file.Load(filename.c_str());
	part_file.GetParam(".partname", result);
	return result;
}

string SETTINGS::GetDataDir()
{
	return data_directory;
}

string SETTINGS::GetDataDir(string filename)
{
		return data_directory;
}

string SETTINGS::GetFullDataPath(string filename)
{
	return (GetDataDir(filename) + "/" + filename);
}

string SETTINGS::GetHomeDir()
{
	return home_directory;
}

string SETTINGS::GetSettingsDir()
{
	//return settings_path;
	return data_directory + "/settings";
}

void SETTINGS::SetDataDir(string new_data_dir)
{
	data_directory = new_data_dir;
}

void SETTINGS::SetDefaultDataDir(string new_data_dir)
{
	CONFIGFILE default_config((new_data_dir + "/settings/VDrift.config").c_str());
	ParseSettings(default_config);
	data_directory = new_data_dir;
	SaveSettings(default_config);
}

bool SETTINGS::Get( string val_name, float & out_var )
{
	if( spew ) cout << "Getting float value " << val_name << "...";
	for( string::size_type i = 0; i < float_settings.size(); i++ )
	{
		if( val_name == float_settings[i] )
		{
			if( spew ) cout << "this is a float value...";
			if( !configfile.GetParam( val_name, out_var ) )
			{
				if( spew ) cout << "can't find value in local config, fetching default...";
				default_configfile.GetParam( val_name, out_var );
				configfile.SetParam( val_name, out_var );
			}
			if( spew ) cout << "found value: [" << out_var << "]. done." << endl;
			return true;
		}

	}
	if( spew ) cout << "this is not a float value." << endl;
	return false;
}

bool SETTINGS::Get( string val_name, float * out_var )
{
	if( spew ) cout << "Getting vector value " << val_name << "...";
	for( string::size_type i = 0; i < vec_settings.size(); i++ )
	{
		if( val_name == vec_settings[i] )
		{
			if( spew ) cout << "this is a vector value...";
			if( !configfile.GetParam( val_name, out_var ) )
			{
				if( spew ) cout << "can't find value in local config, fetching default...";
				default_configfile.GetParam( val_name, out_var );
				configfile.SetParam( val_name, out_var );
			}
			if( spew ) cout << "found value: [" << out_var << "]. done." << endl;
			return true;
		}
	}
	if( spew ) cout << "this is not a vector value." << endl;
	return false;
}

bool SETTINGS::Get( string val_name, int & out_var )
{
	if( spew ) cout << "Getting int value " << val_name << "...";
	for( string::size_type i = 0; i < int_settings.size(); i++ )
	{
		if( val_name == int_settings[i] )
		{
			if( spew ) cout << "this is a int value...";
			if( !configfile.GetParam( val_name, out_var ) )
			{
				if( spew ) cout << "can't find value in local config, fetching default...";
				default_configfile.GetParam( val_name, out_var );
				configfile.SetParam( val_name, out_var );
			}
			if( spew ) cout << "found value: [" << out_var << "]. done." << endl;
			return true;
		}
	}
	if( spew ) cout << "this is not an int value." << endl;
	return false;
}

bool SETTINGS::Get( string val_name, string & out_var )
{
	if( spew ) cout << "Getting string value " << val_name << "...";
	for( string::size_type i = 0; i < string_settings.size(); i++ )
	{
		if( val_name == string_settings[i] )
		{
			if( spew ) cout << "this is a string value...";
			if( !configfile.GetParam( val_name, out_var ) )
			{
				if( spew ) cout << "can't find value in local config, fetching default...";
				default_configfile.GetParam( val_name, out_var );
				configfile.SetParam( val_name, out_var );
			}
			if( spew ) cout << "found value: [" << out_var << "]. done." << endl;
			return true;
		}
	}
	if( spew ) cout << "this is not a string value." << endl;
	return false;
}

bool SETTINGS::Get( string val_name, bool & out_var )
{
	if( spew ) cout << "Getting bool value " << val_name << "...";
	int val;
	for( string::size_type i = 0; i < string_settings.size(); i++ )
	{
		if( val_name == string_settings[i] )
		{
			if( spew ) cout << "this is a bool value...";
			if( !configfile.GetParam( val_name, val ) )
			{
				if( spew ) cout << "can't find value in local config, fetching default...";
				default_configfile.GetParam( val_name, val );
				configfile.SetParam( val_name, val );
			}
			out_var = val == 1 ? true : false;
			if( spew ) cout << "found value: [" << out_var << "]. done." << endl;
			return true;
		}
	}
	if( spew ) cout << "this is not a bool value." << endl;
	return false;
}

bool SETTINGS::Set( string val_name, float in_var )
{
	if( spew ) cout << "Setting float value " << val_name << "...";
	for( string::size_type i = 0; i < float_settings.size(); i++ )
	{
		if( val_name == float_settings[i] )
		{
			if( spew ) cout << "this is a float value...settings to [" << in_var << "]...done.";
			return configfile.SetParam( val_name, in_var );
		}
	}
	if( spew ) cout << "this is not a float value." << endl;
	return false;
}

bool SETTINGS::Set( string val_name, float * in_var )
{
	if( spew ) cout << "Setting vector value " << val_name << "...";
	for( string::size_type i = 0; i < vec_settings.size(); i++ )
	{
		if( val_name == vec_settings[i] )
		{
			if( spew ) cout << "this is a vector value...settings to [" << in_var << "]...done.";
			return configfile.SetParam( val_name, in_var );
		}
	}
	if( spew ) cout << "this is not a vector value." << endl;
	return false;
}

bool SETTINGS::Set( string val_name, int in_var )
{
	if( spew ) cout << "Setting int value " << val_name << "...";
	for( string::size_type i = 0; i < int_settings.size(); i++ )
	{
		if( val_name == int_settings[i] )
		{
			if( spew ) cout << "this is an int value...settings to [" << in_var << "]...done.";
			return configfile.SetParam( val_name, in_var );
		}
	}
	if( spew ) cout << "this is not an float value." << endl;
	return false;
}

bool SETTINGS::Set( string val_name, string in_var )
{
	if( spew ) cout << "Setting string value " << val_name << "...";
	for( string::size_type i = 0; i < string_settings.size(); i++ )
	{
		if( val_name == string_settings[i] )
		{
			if( spew ) cout << "this is a string value...settings to [" << in_var << "]...done.";
			return configfile.SetParam( val_name, in_var );
		}
	}
	if( spew ) cout << "this is not a string value." << endl;
	return false;
}

bool SETTINGS::Set( string val_name, bool in_var )
{
	int val;
	if( spew ) cout << "Setting bool value " << val_name << "...";
	for( string::size_type i = 0; i < string_settings.size(); i++ )
	{
		if( val_name == string_settings[i] )
		{
			val = in_var ? 1 : 0;
			if( spew ) cout << "this is a bool value...settings to [" << val << "]...done.";
			return configfile.SetParam( val_name, val );
		}
	}
	if( spew ) cout << "this is not a bool value." << endl;
	return false;
}



/* All this has got to go
*/


int SETTINGS::GetDisplayX()
{
	return display_x;
}

int SETTINGS::GetDisplayY()
{
	return display_y;
}

int SETTINGS::GetDisplayDepth()
{
	return display_depth;
}

bool SETTINGS::GetFullscreenEnabled()
{
	return ( display_full == 1 );
}

string SETTINGS::GetTreeDetail()
{
	return display_tree;
}

string SETTINGS::GetTerrainDetail()
{
	return display_terrain;
}

string SETTINGS::GetTexSize()
{
	return display_texsize;
}	

bool SETTINGS::GetHUDEnabled()
{
	return ( display_hud == 1 );
}

bool SETTINGS::GetFPSEnabled()
{
	return ( display_fps == 1 );
}

bool SETTINGS::GetMPH()
{
	return ( display_mph == 1 );
}

float SETTINGS::GetViewDistance()
{
	return display_distance;
}

string SETTINGS::GetCarName()
{
	return game_car;
}

int SETTINGS::GetCarPaint()
{
	return game_paint;
}

int SETTINGS::GetCameraMode()
{
	return game_camera;
}

int SETTINGS::GetGameMode()
{
	return game_mode;
}

string SETTINGS::GetTrack()
{
	return game_track;
}

bool SETTINGS::GetIsHosting()
{
	return ( net_host == 1 );
}

string SETTINGS::GetServerIP()
{
	return net_ip;
}

int SETTINGS::GetServerPort()
{
	return net_port;
}

int SETTINGS::GetNumShots()
{
	return game_shots;
}

float SETTINGS::GetSoundVolume()
{
	return sound_volume;
}

bool SETTINGS::GetMouseEnabled()
{
	return ( mouse_enabled == 1 );
}

float SETTINGS::GetMouseXSens()
{
	return mouse_xsens;
}

float SETTINGS::GetMouseYDead()
{
	return mouse_ydead;
}

float SETTINGS::GetMouseYSens()
{
	return mouse_ysens;
}

bool SETTINGS::GetAutoClutch()
{
	return control_autoclutch == 1 ? true : false;
}

void SETTINGS::SetDisplayX( int x )
{
	display_x = x;
}

void SETTINGS::SetDisplayY( int y )
{
	display_y = y;
}

void SETTINGS::SetFullscreenEnabled( bool f )
{
	if( f ) display_full = 1;
	else display_full = 0;
}

void SETTINGS::SetDisplayDepth( int depth )
{
	display_depth = depth;
}

void SETTINGS::SetTreeDetail( string detail )
{
	display_tree = detail;
}

void SETTINGS::SetTerrainDetail( string detail )
{
	display_terrain = detail;
}

void SETTINGS::SetTexSize( string texsize )
{
	display_texsize = texsize;
}

void SETTINGS::SetHUDEnabled( bool hud )
{
	if( hud ) display_hud = 1;
	else display_hud = 0;
}

void SETTINGS::SetFPSEnabled( bool fps )
{
	if( fps ) display_fps = 1;
	else display_fps = 0;
}

void SETTINGS::SetMPH( bool enabled )
{
	display_mph = enabled ? 1 : 0;
}

void SETTINGS::SetViewDistance( float distance )
{
	display_distance = distance;
}

void SETTINGS::SetCarName( string name )
{
	game_car = name;
}

void SETTINGS::SetCarPaint( int paint )
{
	game_paint = paint;
}

void SETTINGS::SetCameraMode( int cam )
{
	game_camera = cam;
}

void SETTINGS::SetGameMode( int mode )
{
	game_mode = mode;
}

void SETTINGS::SetTrack( string track )
{
	game_track = track;
}

void SETTINGS::SetIsHosting( bool hosting )
{
	if( hosting ) net_host = 1;
	else net_host = 0;
}

void SETTINGS::SetServerIP( string ip )
{
	net_ip = ip;
}

void SETTINGS::SetServerPort( int port )
{
	net_port = port;
}

void SETTINGS::SetNumShots( int num )
{
	game_shots = num;
}

void SETTINGS::SetSoundVolume( float vol )
{
	sound_volume = vol;
}

void SETTINGS::SetMouseEnabled( bool enabled )
{
	if( enabled ) mouse_enabled = 1;
	else mouse_enabled = 0;
}

void SETTINGS::SetMouseXSens( float xsens )
{
	mouse_xsens = xsens;
}

void SETTINGS::SetMouseYDead( float ydead )
{
	mouse_ydead = ydead;
}

void SETTINGS::SetMouseYSens( float ysens )
{
	mouse_ysens = ysens;
}

void SETTINGS::SetAutoClutch( bool enabled )
{
	control_autoclutch = enabled ? 1 : 0;
}


/* End of stuff that needs to go
*/
