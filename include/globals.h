/***************************************************************************
 *            globals.h
 *
 *  Sun Sep 26 13:38:17 2004
 *  Copyright  2004  Joe Venzon
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
 
//this file contains all of the global modules defined in main.cpp

//it allows them to be used in other modules without having to explicitly pass
// them.  this means modules can be added or subtracted from projects by simply
// not using them and none of the functions or function calls need to be altered

//when using other modules in a module, make sure to include the other modules'
// include files as well as this globals.h file

#ifdef _FONT_H
extern FONT font;
#endif

#ifdef _UTILITY_H
extern UTILITY utility;
#endif

#ifdef _CAMERA_H
extern CAMERA cam;
#endif

#ifdef _BACKDROP_H
extern BACKDROP backdrop;
#endif

#ifdef _WEATHER_H
extern WEATHER weathersystem;
#endif

#ifdef _VAMOSWORLD_H
extern VAMOSWORLD world;
#endif

#ifdef _KEYMAN_H
extern KEYMAN keyman;
#endif

#ifdef _MESSAGEQ_H
extern MESSAGEQ mq1;
#endif

#ifdef _TERRAIN_H
extern TERRAIN terrain;
#endif

#ifdef _PARTICLE_H
extern PARTICLE particle;
#endif

#ifdef _SOUND_H
extern SOUNDMANAGER sound;
#endif

#ifdef _TIMER_H
extern TIMER timer;
#endif

#ifdef _CONTROLS_H
extern GAMECONTROLS gamecontrols;
#endif

#ifdef _MENU_H
extern MENU menu;
#endif

#ifdef _REPLAY_H
extern REPLAY replay;
#endif

#ifdef _MOUSE_H
extern MOUSE mouse;
#endif

#ifdef _GAMESTATE_H
extern GAMESTATE state;
#endif

#ifdef _OBJECTS_H
extern OBJECTS objects;
#endif

#ifdef _TREES_H
extern TREES trees;
#endif

#ifdef _NET_H
extern NET net;
#endif

#ifdef _MULTIPLAY_H
extern MULTIPLAY multiplay;
#endif

#ifdef _SETTINGS_H
extern SETTINGS settings;
#endif
