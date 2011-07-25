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

#ifndef __SRC_LAUNCHER_FILESYSTEM_FILESYSTEM_H__
#define __SRC_LAUNCHER_FILESYSTEM_FILESYSTEM_H__

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QHash>

#include <physfs.h>

enum searchPathMode { mod_clean, mod_campaign, mod_multiplay, mod_override };

extern const int LOG_FS;

namespace FileSystem {

enum GAMEMOD_TYPE
{
    GAMEMOD_GLOBAL,     // Mod for both multiplay and campaign games.
    GAMEMOD_CAMPAIGN,   // Campaign only mod.
    GAMEMOD_MULTIPLAY   // Multiplay only mod.
};

typedef QHash<QString, QString> MOD_LIST;

/**
 * @brief Initalizes Physfs and creates the writedir.
 *
 * @param binpath       Application path - argv[0].
 * @param appSubDir     Subdirectory in platforms Userdir for the Physfs writedir.
 * @param userConfigdir Custom supplied config directory.
 * @param userDataDir   Custom supplied data directory.
 *
 * @return Path to the config dir (empty on failure).
 */
QString init(const QString &binpath, const char* appSubDir, const QString& userConfigDir = QString());

/**
 * @brief Finds the application data and adds it to the search path.
 *
 * @param cmDataDir     User supplied data dir which overwrites ours.
 * @param fallbackDir   ?
 *
 * @return Path to the data dir (empty on failure).
 */
QString scanDataDirs(const QString cmdDataDir = QString(), const QString fallbackDir = QString());

/**
 * @brief Cleanup stuff and deinitalize PHYSFS.
 */
void shutdown();

/**
 * @brief Clean the path, build to mode and (re)load its persistant mods.
 *
 * @param mode      Mode to build to.
 *
 * @return Success/Failure.
 */
bool setSearchPathMode(searchPathMode mode);


/**
 * @brief Loads all maps into the searchpath and sets the search mode to mod_multiplay.
 */
void loadMaps();

/**
 * @brief Appends the given map path to the search path, remove it with unloadMaps().
 *
 * @return Success/Failure.
 */
bool loadMap(const char *path);

/**
 * @brief Unloads all maps from the searchpath and resets the search mode.
 */
void unloadMaps();

/**
 * @brief Checks if the mod exists.
 *
 * @param type  Gametype (global, campaign or multiplay)
 * @param mod   mod name (as example: old-1.10-balance.wz)
 * @param reloadList Force a redetect.
 *
 * @return Mod path or empty string.
 */
QString haveMod(GAMEMOD_TYPE type, const QString& mod, bool reloadList = false);

/**
 * @brief Detects mods and loads them.
 *
 * @param type  Gametype (global, campaign or multiplay)
 * @param mod   mod name (as example: old-1.10-balance.wz)
 * @param reloadList Force a redetect.
 *
 * @return Success/Failure.
 */
bool loadMod(GAMEMOD_TYPE type, const QString& mod, bool reloadList = false);

/**
 * @brief Unloads all mods and reloads persistant mods.
 */
void unloadMods();

/**
 * @brief Returns a list of mods.
 */
const QStringList getLoadedMods();

/**
 * @brief Add a persistant/cmd line mod.
 */
void addPersistantMod(GAMEMOD_TYPE type, const char* mod);

/**
 * @brief Prints the current search path (for debugging).
 */
void printSearchPath();

} // namespace FileSystem {

#endif //#ifndef __SRC_LAUNCHER_FILESYSTEM_FILESYSTEM_H__