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

// Self
#include <src/Launcher/Filesystem/filesystem.h>

// "wz::" handler
#include <src/Launcher/Filesystem/qphysfsenginehandler.h>

// QT
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QCoreApplication>

// For getPlatformUserDir
// TODO: Needs testing.
#if defined(Q_OS_WIN)
    #include <shlobj.h> /* For SHGetFolderPath */
#elif defined(Q_OS_MAC)
    #include <CoreServices/CoreServices.h>
#endif // WZ_OS_WIN

// Logger
#include <lib/framework/frame.h>

// PHYSFS_APPEND, PHYSFS_PREPEND
#include <lib/framework/physfs_ext.h>

const int LOG_FS = Logger::instance().addLoggingLevel("fs", false);

namespace FileSystem {

static QMap<unsigned int, QString> searchPathRegistry;

// These contain the mod name as key and its path as value.
static MOD_LIST mods_global;
static MOD_LIST mods_campaign;
static MOD_LIST mods_multiplay;
static MOD_LIST mods_loaded;

static searchPathMode currentSearchMode = mod_clean;
// To remember the last used searchpath when detecting maps.
static searchPathMode lastSearchMode = mod_clean;

// Registers the "wz::" Filesystem handler.
QPhysfsEngineHandler qPhysfsEngine("wz::");

// Priority range for mods in searchPathRegistry.
const int PRIORITY_MOD_MIN = 100;
const int MAX_MODS = 99;
// Internal Map Priority to add/remove a map from the searchPathRegistry.
const int PRIORITY_MAP = 200;
// Game data starts a this priority.
const int PRIORITY_DATA = 300;

// Static Functions declarations.
static void getPlatformUserDir(QString& result, const char* appSubDir);
static void addSubdirs(const QString& basedir, const char* subdir, const bool appendToPath);
static void removeSubdirs(const QString& basedir, const char* subdir);
static void registerSearchPath(QString path, unsigned int priority);
static bool rebuildSearchPath( searchPathMode mode, bool force = false);

static void findMods(MOD_LIST& modList, const char* subdir);
static void findAvailableMods(bool forceReload = false);

bool init(const QString &binpath, const char* appSubDir, const QString &cmdUserConfigdir)
{
    QString tmpdir;

    PHYSFS_Version compiled;
    PHYSFS_Version linked;

    PHYSFS_init(binpath.toUtf8().constData());

    PHYSFS_VERSION(&compiled);
    PHYSFS_getLinkedVersion(&linked);

     wzLog(LOG_FS) << QString("Compiled against PhysFS version: %1.%2.%3")
         .arg(compiled.major).arg(linked.minor).arg(linked.patch);
     wzLog(LOG_FS) << QString("Linked against PhysFS version: %1.%2.%3")
         .arg(linked.major).arg(linked.minor).arg(linked.patch);
    if (linked.major < 2)
    {
        wzLog(LOG_FATAL) << "At least version 2 of PhysicsFS required!";
        return false;
    }

    PHYSFS_permitSymbolicLinks(1);

    if (cmdUserConfigdir.isEmpty())
    {
        getPlatformUserDir(tmpdir, appSubDir);
        if (!QFile::exists(tmpdir) && !QDir().mkdir(tmpdir))
        {
            wzLog(LOG_FATAL) << QString("Error creating user directory \"%1\"").arg(tmpdir);
            return false;
        }
    }
    else
    {
        tmpdir = cmdUserConfigdir;
        if (!tmpdir.endsWith(PHYSFS_getDirSeparator()))
        {
            tmpdir.append(PHYSFS_getDirSeparator());
        }

        wzLog(LOG_FS) << QString("Using custom configuration directory: %1").arg(tmpdir);

        if (!QFile::exists(tmpdir) && !QDir().mkdir(tmpdir))
        {
            wzLog(LOG_FATAL) << QString("Error creating custom user directory \"%1\"").arg(tmpdir);
            return false;
        }
    }

    if (!PHYSFS_setWriteDir(tmpdir.toUtf8().constData()))
    {
        wzLog(LOG_FATAL) << QString("Error setting write directory to \"%1\": %2")
            .arg(tmpdir).arg(PHYSFS_getLastError());
        return false;
    }
    
    PHYSFS_addToSearchPath(PHYSFS_getWriteDir(), PHYSFS_PREPEND);
    
    return true;
}

void exit()
{
    unloadMaps();
    unloadMods();

    PHYSFS_deinit();
}

void setSearchPathMode(searchPathMode mode)
{
    unloadMaps();
    unloadMods();

    rebuildSearchPath(mode);
}

bool scanDataDirs(const QString &cmdDataDir, const QString &fallbackDir)
{
    QString basedir(PHYSFS_getBaseDir());
    QString separator(PHYSFS_getDirSeparator());
    if (basedir.lastIndexOf(separator) != -1)
    {
        // Trim ending '/', which getBaseDir always provides
        basedir.chop(separator.length());

        if (basedir.lastIndexOf(separator) != -1)
        {
            // Now remove the last directory
            basedir.resize(basedir.lastIndexOf(separator));
        }
    }

#ifdef Q_OS_MAC
    // version-independent location for video files
    registerSearchPath("/Library/Application Support/Warzone 2100/", 1);
#endif

    // Commandline supplied datadir
    if (!cmdDataDir.isEmpty())
    {
        registerSearchPath(cmdDataDir, 2);
    }

    // User's home dir
    registerSearchPath(PHYSFS_getWriteDir(), 3);

    // Data in source tree
    registerSearchPath(basedir + "/data/", PRIORITY_DATA);
    rebuildSearchPath(mod_multiplay, true);

    if ( !PHYSFS_exists("gamedesc.lev") )
    {
        // Relocation for AutoPackage
        registerSearchPath(basedir + "/share/warzone2100/", PRIORITY_DATA);
        rebuildSearchPath(mod_multiplay, true);

        if ( !PHYSFS_exists("gamedesc.lev") )
        {
            // Program dir
            registerSearchPath(PHYSFS_getBaseDir(), PRIORITY_DATA);
            rebuildSearchPath(mod_multiplay, true);

            if (!PHYSFS_exists("gamedesc.lev"))
            {
                // Guessed fallback default datadir on Unix
                registerSearchPath(fallbackDir, PRIORITY_DATA);
                rebuildSearchPath(mod_multiplay, true);
            }
        }
    }

#ifdef Q_OS_MAC
    if ( !PHYSFS_exists("gamedesc.lev") )
    {
        CFURLRef resourceURL = CFBundleCopyResourcesDirectoryURL(CFBundleGetMainBundle());
        char resourcePath[PATH_MAX];
        if (CFURLGetFileSystemRepresentation( resourceURL, true,
                                               (UInt8 *) resourcePath,
                                               PATH_MAX))
        {
            chdir(resourcePath);
            registerSearchPath("data", PRIORITY_DATA);
            rebuildSearchPath(mod_multiplay, true);
        }
        else
        {
            wzLog(LOG_FATAL) << "Could not change to resources directory.";
        }

        if (resourceURL != NULL)
        {
            CFRelease(resourceURL);
        }
    }
#endif

    /** Debugging and sanity checks **/
    printSearchPath();

    if (PHYSFS_exists("gamedesc.lev"))
    {
        wzLog(LOG_FS) << QString("gamedesc.lev found at %1").arg(PHYSFS_getRealDir("gamedesc.lev"));
    }
    else
    {
        wzLog(LOG_FATAL) << "Could not find game data. Aborting.";
        return false;
    }

    return true;
}

static void getPlatformUserDir(QString& result, const char* appSubDir)
{
#ifdef Q_OS_WIN
    wchar_t tmpWStr[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW( NULL, CSIDL_PERSONAL|CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, tmpWStr)))
    {
        result = QString::fromUtf16(tmpWStr);
    }
    else
#elifdef Q_OS_MAC
    FSRef fsref;
    OSErr error = FSFindFolder(kUserDomain, kApplicationSupportFolderType, false, &fsref);
    if (!error)
    {
        error = FSRefMakePath(&fsref, (UInt8 *) tmpstr, size);
    }
    if (!error)
    {
        result = QString(fsref);
    }
    else
#endif
    if (PHYSFS_getUserDir())
    {
        result = QString(PHYSFS_getUserDir());
    }
    else
    {
        result = QCoreApplication::instance()->applicationDirPath();
    }

    result.append(appSubDir).append(PHYSFS_getDirSeparator());
}

void printSearchPath()
{
    char ** i, ** searchPath;

    wzLog(LOG_FS) << QString("Search paths:");
    searchPath = PHYSFS_getSearchPath();
    for (i = searchPath; *i != NULL; i++)
    {
        wzLog(LOG_FS) << QString("    [%1]").arg(*i);
    }
    PHYSFS_freeList(searchPath);
}

/**
 * @brief Tries to mount a list of directories, found in /basedir/subdir/<list>.
 *
 * @param basedir Base directory
 * @param subdir A subdirectory of basedir
 * @param appendToPath Whether to append or prepend
 */
static void addSubdirs(const QString& basedir, const char* subdir, const bool appendToPath)
{
    //char buf[256];
    char **subdirlist, **i;

    subdirlist = PHYSFS_enumerateFiles(subdir);

    for (i = subdirlist; *i != NULL; ++i)
    {
#ifdef DEBUG
        wzLog(LOG_NEVER) << QString("Examining subdir: [%1]").arg(*i);
#endif // DEBUG

        if (*i[0] != '.')
        {
            QString tmpstr = QString("%1%2%3%4").arg(basedir).arg(subdir).arg(PHYSFS_getDirSeparator()).arg(*i);
#ifdef DEBUG
            wzLog(LOG_NEVER) << QString("Adding [%1] to search path").arg(tmpstr);
#endif // DEBUG

            PHYSFS_addToSearchPath(tmpstr.toUtf8().constData(), appendToPath);
        }
    }
    PHYSFS_freeList(subdirlist);
}

static void removeSubdirs(const QString& basedir, const char* subdir)
{
    char ** subdirlist = PHYSFS_enumerateFiles(subdir);
    char ** i = subdirlist;

    while (*i != NULL)
    {
        QString tmpstr = QString("%1%2%3%4").arg(basedir).arg(subdir).arg(PHYSFS_getDirSeparator()).arg(*i);

        #ifdef DEBUG
            wzLog(LOG_NEVER) << QString("Removing [%1] from search path").arg(tmpstr);
        #endif

        PHYSFS_removeFromSearchPath(tmpstr.toUtf8().constData());
        i++;
    }
    PHYSFS_freeList(subdirlist);
}

/**
 * @brief Rebuilds the PHYSFS searchPath with mode specific subdirs
 *
 * Priority:
 * maps > mods > base > base.wz
 *
 * @param mode      The Mode to rebuild to either mod_campaign or mod_multiplay.
 * @param force     Ignoring the internal state, force a rebuild.
 *
 * @return Success/Failure.
 */
static bool rebuildSearchPath( searchPathMode mode, bool force)
{
    QString tmpstr;

    if (mode != currentSearchMode || force)
    {
        if (mode != mod_clean)
        {
            rebuildSearchPath(mod_clean, false);
        }

        currentSearchMode = mode;

        switch ( mode )
        {
        case mod_clean:
            wzLog(LOG_FS) << QString("Cleaning up");

            foreach(QString path, searchPathRegistry)
            {
                #ifdef DEBUG
                    wzLog(LOG_FS) << QString("Removing [%1] from search path").arg(path);
                #endif

                // Remove maps and mods
                removeSubdirs(path, "maps");
                removeSubdirs(path, "mods/music");
                removeSubdirs(path, "mods/global");
                removeSubdirs(path, "mods/campaign");
                removeSubdirs(path, "mods/multiplay");
                removeSubdirs(path, "mods/autoload");

                // Remove multiplay patches
                tmpstr = path;
                tmpstr += "mp";
                PHYSFS_removeFromSearchPath(tmpstr.toUtf8().constData());
                tmpstr = path;
                tmpstr += "mp.wz";
                PHYSFS_removeFromSearchPath(tmpstr.toUtf8().constData());

                // Remove plain dir
                PHYSFS_removeFromSearchPath(path.toUtf8().constData());

                // Remove base files
                tmpstr = path;
                tmpstr += "base";
                PHYSFS_removeFromSearchPath(tmpstr.toUtf8().constData());
                tmpstr = path;
                tmpstr += "base.wz";
                PHYSFS_removeFromSearchPath(tmpstr.toUtf8().constData());

                // remove video search path as well
                tmpstr = path;
                tmpstr += "sequences.wz";
                PHYSFS_removeFromSearchPath(tmpstr.toUtf8().constData());
            } // foreach(QString path, searchPathRegistry)
            break;

        case mod_campaign:
        case mod_multiplay:
            if (mode == mod_campaign)
            {
                wzLog(LOG_FS) << "*** Switching to campaign mods ***";
            }
            else
            {
                wzLog(LOG_FS) << "*** Switching to multiplay mods ***";
            }

            // Find and add sequences.wz first
            foreach(QString path, searchPathRegistry)
            {
                tmpstr = path;
                tmpstr += "sequences.wz";
                PHYSFS_addToSearchPath(tmpstr.toUtf8().constData(), PHYSFS_APPEND);
            }

            foreach(QString path, searchPathRegistry)
            {
                #ifdef DEBUG
                    wzLog(LOG_FS) << QString("Adding [%1] to search path").arg(path);
                #endif

                // Add global and campaign mods
                PHYSFS_addToSearchPath(path.toUtf8().constData(), PHYSFS_APPEND);
                addSubdirs(path, "mods/music", PHYSFS_APPEND);
                addSubdirs(path, "mods/autoload", PHYSFS_APPEND);
                if (!PHYSFS_removeFromSearchPath(path.toUtf8().constData()))
                {
                    wzLog(LOG_FS) << QString("Failed to remove path %1 again").arg(path);
                }

                if (mode == mod_multiplay)
                {
                    tmpstr = path;
                    tmpstr += "mp";
                    if (PHYSFS_mount(tmpstr.toUtf8().constData(), NULL, PHYSFS_APPEND) == 0)
                    {
                        tmpstr = path;
                        tmpstr += "mp.wz";
                        PHYSFS_mount(tmpstr.toUtf8().constData(), NULL, PHYSFS_APPEND);
                    }
                }

                // Add base files
                tmpstr = path;
                tmpstr += "base";
                if (PHYSFS_mount(tmpstr.toUtf8().constData(), NULL, PHYSFS_APPEND) == 0)
                {
                    tmpstr = path;
                    tmpstr += "base.wz";
                    PHYSFS_mount(tmpstr.toUtf8().constData(), NULL, PHYSFS_APPEND);
                }
            }
            break;

        default:
            wzLog(LOG_ERROR) << QString("Can't switch to unknown mods %1").arg(mode);
            return false;
        }
    }

	// User's home dir must be first so we allways see what we write
	PHYSFS_removeFromSearchPath(PHYSFS_getWriteDir());
	PHYSFS_addToSearchPath(PHYSFS_getWriteDir(), PHYSFS_PREPEND);

    return true;
}

/*!
 * Register searchPath above the path with next lower priority
 * For information about what can be a search path, refer to PhysFS documentation
 */
static void registerSearchPath(QString path, unsigned int priority)
{
    if (!path.endsWith(PHYSFS_getDirSeparator()))
    {
        path += PHYSFS_getDirSeparator();
    }

    wzLog(LOG_FS) << QString("Registering %1 at priority %2").arg(path).arg(priority);

    // Just insert if the registry is empty.
    if (searchPathRegistry.isEmpty())
    {
        searchPathRegistry.insert(priority, path);
        return;
    }

    // Insert if the priority does not exists.
    if (!searchPathRegistry.contains(priority))
    {
        searchPathRegistry.insert(priority, path);
        return;
    }

    // Increase the priority if it exists
    priority++;
    while (searchPathRegistry.contains(priority))
    {
        priority++;
    }
    // And finally insert it.
    searchPathRegistry.insert(priority, path);
    return;
}

void loadMaps()
{
    // Remember the last search mode.
    lastSearchMode = currentSearchMode;

    rebuildSearchPath(mod_multiplay);

    // Adds all map subdiretories to the the physfs search path.
    foreach(QString path, searchPathRegistry)
    {
        PHYSFS_mount(path.toUtf8().constData(), NULL, PHYSFS_APPEND);
        addSubdirs(path, "maps", PHYSFS_APPEND);
        if (QString(PHYSFS_getWriteDir()) != path)
        {
            if (!PHYSFS_removeFromSearchPath(path.toUtf8().constData()))
            {
                wzLog(LOG_FS) << QString("Failed to remove path %1 again").arg(path);
            }
        }
    }

	// User's home dir must be first so we allways see what we write
	PHYSFS_removeFromSearchPath(PHYSFS_getWriteDir());
	PHYSFS_addToSearchPath(PHYSFS_getWriteDir(), PHYSFS_PREPEND);
}

bool loadMap(const char *path)
{
    if (PHYSFS_mount(path, NULL, PHYSFS_APPEND) != 0)
    {
        return false;
    }
    return true;
}

bool unLoadMap(const char *path)
{
    return PHYSFS_removeFromSearchPath(path) == 0;
}


void unloadMaps()
{
    // Remove all map subdiretories from the the physfs search path.
    foreach(QString path, searchPathRegistry)
    {
        PHYSFS_mount(path.toUtf8().constData(), NULL, PHYSFS_APPEND);
        removeSubdirs(path, "maps");
        if (!PHYSFS_removeFromSearchPath(path.toUtf8().constData()))
        {
            wzLog(LOG_FS) << QString("Failed to remove path %1 again").arg(path);
        }
    }

    rebuildSearchPath(lastSearchMode);
}

static void findMods(MOD_LIST& modList, const char* subdir)
{
    char **subdirlist, **i;
    QStringList fullpath;
    QStringList modpath;

    subdirlist = PHYSFS_enumerateFiles(subdir);
    
    for (i = subdirlist; *i != NULL; ++i)
    {
        if (*i[0] != '.')
        {
            if (!QString(*i).endsWith(".wz")) 
            {
                continue;
            }
            
            modpath.clear();
            modpath << subdir
                    << PHYSFS_getDirSeparator()
                    << *i;
            
            fullpath.clear();
            fullpath << PHYSFS_getRealDir(modpath.join("").toUtf8().constData())
                     << modpath;

            wzLog(LOG_FS) << QString("Found %1: %2.")
                                 .arg(subdir)
                                 .arg(*i);
            modList.insert(*i, fullpath.join(""));
        }
    }
}

static void findAvailableMods(bool forceReload)
{
    static bool modListLoaded = false;

    if (modListLoaded && !forceReload)
    {
        return;
    }

    lastSearchMode = currentSearchMode;
    rebuildSearchPath(mod_multiplay);

    foreach(QString path, searchPathRegistry)
    {
        PHYSFS_mount(path.toUtf8().constData(), NULL, PHYSFS_APPEND);
    }

    mods_global.clear();
    findMods(mods_global, "mods/global");

    mods_campaign.clear();
    findMods(mods_campaign, "mods/campaign");

    mods_multiplay.clear();
    findMods(mods_multiplay, "mods/multiplay");

    foreach(QString path, searchPathRegistry)
    {
		if (!PHYSFS_removeFromSearchPath(path.toUtf8().constData()))
		{
			wzLog(LOG_FS) << QString("Failed to remove path %1 again").arg(path);
		}
    }

    rebuildSearchPath(lastSearchMode);

    modListLoaded = true;
}

bool loadMod(GAMEMOD_TYPE type, const QString& mod, bool reloadList)
{
    findAvailableMods(reloadList);

    MOD_LIST list;
    QString typeString;

    switch(type)
    {
        case GAMEMOD_GLOBAL:
            list = mods_global;
            typeString = "global";
        break;
        case GAMEMOD_CAMPAIGN:
            list = mods_campaign;
            typeString = "campaign";
        break;
        case GAMEMOD_MULTIPLAY:
            list = mods_multiplay;
            typeString = "multiplay";
        break;
    }

    if (!list.contains(mod) ||
         PHYSFS_mount(list.value(mod).toUtf8().constData(), NULL, PHYSFS_APPEND) == 0)
    {
        return false;
    }

    mods_loaded.insert(mod, list.value(mod));
    return true;
}

void unloadMods()
{
    foreach(QString path, mods_loaded)
    {
        if (!PHYSFS_removeFromSearchPath(path.toUtf8().constData()))
        {
            wzLog(LOG_FS) << QString("Failed to remove mod %1 again").arg(path);
        }
    }
    mods_loaded.clear();
}

const QStringList getLoadedMods()
{
    return mods_loaded.keys();
}

} // namespace FileSystem {