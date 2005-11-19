#ifndef _SETTINGS_H
#define _SETTINGS_H

extern "C"
{
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
}

#include <vector>
#include <string>
#include <iostream>
#include <stdlib.h>

#ifdef __APPLE__
       #include "config_mac.h"
#endif

#include "configfile.h"

using namespace std;

class SETTINGS
{
public:
	SETTINGS();
	~SETTINGS();

	void SaveSettings(CONFIGFILE &c);
	void Init();

	bool Get( string var_name, float & out_var );
	bool Get( string var_name, float * out_var );
	bool Get( string var_name, int & out_var );
	bool Get( string var_name, string & out_var );
	bool Get( string var_name, bool & out_var );

	bool Set( string var_name, float in_var );
	bool Set( string var_name, float * in_var );
	bool Set( string var_name, int in_var );
	bool Set( string var_name, string in_var );
	bool Set( string var_name, bool in_var );

	string GetSettingsDir();
	string GetHomeDir();
	string GetDataDir();
	string GetDataDir( string filename );
	string GetGameVersion() { return VERSION; }
	string GetConfigFileVersion();
	string GetFullDataPath( string filename );
	void SetDataDir( string new_data_dir );
	void SetDefaultDataDir( string new_data_dir );
	vector<string> LoadCarPartsSettings();
	void SaveCarPartsSettings( vector<string> &p_list );
	string GetCarPartName( string part_path );

	int GetDisplayX();
	int GetDisplayY();
	bool GetFullscreenEnabled();
	int GetDisplayDepth();
	string GetTreeDetail();
	string GetTerrainDetail();
	string GetTexSize();
	bool GetHUDEnabled();
	bool GetFPSEnabled();
	bool GetMPH();
	float GetViewDistance();
	string GetCarName();
	int GetCarPaint();
	int GetCameraMode();
	int GetGameMode();
	string GetTrack();
	bool GetIsHosting();
	string GetServerIP();
	int GetServerPort();
	int GetNumShots();
	float GetSoundVolume();
	bool GetMouseEnabled();
	float GetMouseXSens();
	float GetMouseYDead();
	float GetMouseYSens();
	bool GetAutoClutch();

	void SetDisplayX( int x );
	void SetDisplayY( int y );
	void SetFullscreenEnabled( bool f );
	void SetDisplayDepth( int depth );
	void SetTreeDetail( string detail );
	void SetTerrainDetail( string detail );
	void SetTexSize( string texsize );
	void SetHUDEnabled( bool hud );
	void SetFPSEnabled( bool fps );
	void SetMPH( bool enabled );
	void SetViewDistance( float distance ); 
	void SetCarName( string name );
	void SetCarPaint( int paint );
	void SetCameraMode( int cam );
	void SetGameMode( int mode );
	void SetTrack( string track );
	void SetIsHosting( bool hosting );
	void SetServerIP( string ip );
	void SetServerPort( int port );
	void SetNumShots( int num );
	void SetSoundVolume( float vol );
	void SetMouseEnabled( bool enabled );
	void SetMouseXSens( float xsens );
	void SetMouseYDead( float ydead );
	void SetMouseYSens( float ysens );
	void SetAutoClutch( bool enabled );

private:

	CONFIGFILE configfile;
	CONFIGFILE default_configfile;
	CONFIGFILE car_parts;

	vector<string> parts_list;

	void ParseSettings(CONFIGFILE &c);
	bool FileExists(string path);
	//bool DirExists(string path);
	void CreateDir(string path);
	void FindHomeDir();

	vector<string> float_settings;
	vector<string> vec_settings;
	vector<string> int_settings;
	vector<string> bool_settings;
	vector<string> string_settings;

	string settings_path;
	string data_directory;
	string settings_directory;
	string home_directory;
	string config_version;

	bool spew;

	int display_x;
	int display_y;
	int display_depth;
	int display_full;
	string display_tree;
	string display_terrain;
	string display_texsize;
	int display_hud;
	int display_fps;
	int display_mph;
	float display_distance;

	string game_car;
	int game_paint;
	int game_camera;
	int game_mode;
	string game_track;
	int net_host;
	string net_ip;
	int net_port;
	int game_shots;
	float sound_volume;
	int sound_mute;
	int mouse_enabled;
	float mouse_xsens;
	float mouse_ydead;
	float mouse_ysens;
	int control_autoclutch;
};

#endif /* _SETTINGS_H */
