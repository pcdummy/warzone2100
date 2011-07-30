/*
	This file is part of Warzone 2100.
	Copyright (C) 2011  Warzone 2100 Project

	Warzone 2100 is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	Warzone 2100 is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Warzone 2100; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

// self
#include <src/Launcher/lconfig.h>

#include <QtCore/QStringList>

// For init_config() - FSAA_MAX and FMV_MAX
#include <src/warzoneconfig.h>
// For init_config() - SCANLINES_BLACK
#include "lib/sequence/sequence.h"
// For init_config() - NO_ALLIANCES, ALLIANCES_TEAMS
#include "src/ai.h"
// For init_config() - CAMP_CLEAN, CAMP_WALLS, LEV_MED and LEV_HI
#include "src/multiplay.h"
// For init_config() - DL_NORMAL, DL_KILLER
#include "src/difficulty.h"
// For init_config() - Map::MP_SKIRMISH_T1
#include "src/Launcher/Map/map.h"

void config_init()
{
	//Userconfig
	config.add("colour",				CONFTYPE_ENUM,			true,	0,		7);
	config.add("fullscreen",			CONFTYPE_BOOL,			true,	false);
	config.add("FSAA",					CONFTYPE_ENUM,			true,	0,		FSAA_MAX);
	config.add("trapCursor",			CONFTYPE_BOOL,			true,	false);
	config.add("vsync",					CONFTYPE_BOOL,			true,	true);
	config.add("width",					CONFTYPE_INT,			true,	640);
	config.add("height",				CONFTYPE_INT,			true,	480);
	config.add("FMVmode",				CONFTYPE_ENUM,			true,	0,		FMV_MAX);
	config.add("scanlines",				CONFTYPE_ENUM,			true,	0,		SCANLINES_BLACK);
	config.add("pauseOnFocus",			CONFTYPE_BOOL,			false,	false);

	config.add("MiddleClickRotate",		CONFTYPE_BOOL,			true,	true);
	config.add("PauseOnFocusLoss",		CONFTYPE_BOOL,			true,	true);
	config.add("RightClickOrders",		CONFTYPE_BOOL,			true,	true);
	config.add("UPnP",					CONFTYPE_BOOL,			true,	true);
	config.add("bpp",					CONFTYPE_INT,			true,	32);//Think 24/32

	config.add("voicevol",				CONFTYPE_ENUM,			true,	100,	100+1);
	config.add("cdvol",					CONFTYPE_ENUM,			true,	50,	100+1);
	config.add("fxvol",					CONFTYPE_ENUM,			true,	100,	100+1);
	config.add("sound",					CONFTYPE_BOOL,			true,	true);
	config.add("music_enabled",			CONFTYPE_BOOL,			true,	true);

	config.add("difficulty",			CONFTYPE_ENUM,			true,	DL_NORMAL,DL_KILLER+1);


	//Hostgamescreen
	config.add("playerName",			CONFTYPE_STRING,		true,	_("Player"));
	config.add("gameName",				CONFTYPE_STRING,		true,	_("MyGame"));
	config.add("mapName",				CONFTYPE_STRING,		true,	"Rush");//Displayname.
	config.add("techlevel",				CONFTYPE_INT,			true,	Map::MP_SKIRMISH_T1);
	config.add("scavengers",			CONFTYPE_BOOL,			true,	false);
	config.add("visfog",				CONFTYPE_BOOL,			true,	false);
	config.add("fog",					CONFTYPE_BOOL,			true,	false);	//Not sure whats that good for - Fast
	config.add("alliance",				CONFTYPE_INT,			true,	NO_ALLIANCES,	ALLIANCES_TEAMS+1);
	config.add("power",					CONFTYPE_ENUM,			true,	LEV_MED,		LEV_HI+1);
	config.add("base",					CONFTYPE_INT,			true,	CAMP_CLEAN,		CAMP_WALLS+1);


	config.add("gameserver_port",		CONFTYPE_INT,			true,	2100);
	config.add("language",				CONFTYPE_STRING,		true,	"");
	config.add("mouseflip",				CONFTYPE_BOOL,			true,	true);
	config.add("nomousewarp",			CONFTYPE_BOOL,			true,	true);
	config.add("radarObjectMode",		CONFTYPE_BOOL,			true,	true);
	config.add("radarTerrainMode",		CONFTYPE_BOOL,			true,	true);
	config.add("rotateRadar",			CONFTYPE_BOOL,			true,	true);
	config.add("scroll",				CONFTYPE_BOOL,			true,	true);
	config.add("shadows",				CONFTYPE_BOOL,			true,	true);
	config.add("showFPS",				CONFTYPE_BOOL,			true,	true);
	config.add("subtitles",				CONFTYPE_BOOL,			true,	true);
	config.add("textureSize",			CONFTYPE_INT,			true,	2048);
	config.add("shake",					CONFTYPE_BOOL,			true,	true);

	config.add("lobby_host",			CONFTYPE_STRING,		true,	"lobby.wz2100.net");
	config.add("lobby_port",			CONFTYPE_INT,			true,	9994);
	config.add("lobby_ssl",				CONFTYPE_BOOL,			true,	true);
	config.add("lobby_token",			CONFTYPE_STRING,		true,	"");
	config.add("lobby_user",			CONFTYPE_STRING,		true,	"");

	//Datadirectories
	config.add("configDir",				CONFTYPE_STRING,		false,	"");
	config.add("dataDir",				CONFTYPE_STRING,		false,	"");

	//Commandlinestuff
	config.add("customLogfile",			CONFTYPE_STRING,		false,	"");
	config.add("drawShadows",			CONFTYPE_BOOL,			false,	true);
	config.add("doCrash",				CONFTYPE_BOOL,			false,	false);
	config.add("doSelftest",			CONFTYPE_BOOL,			false,	false);
	config.add("noAssert",				CONFTYPE_BOOL,			false,	false);

	config.add("saveGame",				CONFTYPE_STRING,		false,	"");
	config.add("game",					CONFTYPE_STRING,		false,	"");

	config.add("loadModGlobal",			CONFTYPE_STRINGLIST,	false,	QStringList());
	config.add("loadModSP",				CONFTYPE_STRINGLIST,	false,	QStringList());
	config.add("loadModMP",				CONFTYPE_STRINGLIST,	false,	QStringList());
	config.add("connectIP",				CONFTYPE_STRING,		false,	"");

	//QML/enginevars
	config.add("gameType",				CONFTYPE_ENUM,			false,	GAMETYPE_CAMPAIGN,GAMETYPE_MAX);
	config.add("isHost",				CONFTYPE_BOOL,			false,	false);
	config.add("playerIndex",			CONFTYPE_INT,			false,	0);
	config.add("fullMapName",			CONFTYPE_STRING,		false,	"");//a.e.:Sk-Rush-T1
	config.add("mapPath",				CONFTYPE_STRING,		false,	"");//a.e.:~/.warzone2100-master/maps/8c-Squared.wz
}
