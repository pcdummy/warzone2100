// See QString docs for them.
// TODO: Move to a better place.
#define QT_USE_FAST_CONCATENATION
#define QT_USE_FAST_OPERATOR_PLUS

// Qt Gui
#include <QtGui/QApplication>

// Qt Core
#include <QtCore/QTextCodec>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

#include <src/Launcher/lconfig.h>
#include <src/Launcher/Filesystem/filesystem.h>
#include <src/Launcher/clparse.h>
#include <src/Launcher/Frontend/wzqmlview.h>

// Get platform defines before checking for them.
// Qt headers MUST come before platform specific stuff!
// compiler fixes + logging + i18n
#include <lib/framework/frame.h>

// setupExceptionHandler
#include <lib/exceptionhandler/exceptionhandler.h>

// Subdirectory in users home/documents
// TODO: Let ./configure handle this
#if defined(Q_OS_WIN)
# define WZ_WRITEDIR "Warzone 2100 master"
#elif defined(Q_OS_MAC)
# define WZ_WRITEDIR "Warzone 2100 master"
#else
# define WZ_WRITEDIR ".warzone2100-master"
#endif

#include "src/Launcher/Map/map.h"

/* Always use fallbacks on Windows */
#if defined(WZ_OS_WIN)
#  undef WZ_DATADIR
#endif

#if !defined(WZ_DATADIR)
#  define WZ_DATADIR "data"
#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QStringList args = app.arguments();

    // make Qt treat all C strings in Warzone as UTF-8
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    // Sets a basic debugger up.
    debug_init();

    // Initializes default values for the configuration.
    config_init();

    int utfargc = args.size();
    const char* utfargv[utfargc];
    for (int i = 0; i < utfargc; i++)
    {
        // TODO: Free this memory?
        utfargv[i] = strdup(args.at(i).toUtf8().constData()); 
    }

    setupExceptionHandler(utfargc, utfargv, "Warzone 2100 - Launcher 0.0.0");

    // Setup debugging cmd options.
    if (!ParseCommandLineEarly(utfargc, utfargv))
    {
        return EXIT_FAILURE;
    }
    
    if (cmdShowVersion)
    {
        printf("%s", "Warzone 2100 - Launcher 0.0.0\n");
        // printf("Warzone 2100 - %s\n", version_getFormattedVersionString());
        return EXIT_SUCCESS;
    }    
        
    if (!FileSystem::init(args.at(0), WZ_WRITEDIR, config.get("configDir").toString()))
    {
        return EXIT_FAILURE;
    }
    
    if (!FileSystem::scanDataDirs(config.get("dataDir").toString(), WZ_DATADIR))
    {
        return EXIT_FAILURE;
    }
    
    // Its important this line is before ParseCommandLine as the user
    // can override values by the cmd.
    config.loadConfig("wz::config");
       
    // Now do the rest.
    if (!ParseCommandLine(utfargc, utfargv))
    {
        return EXIT_FAILURE;
    }   

    QStringList tmpStringlist;
    tmpStringlist = config.get("loadModGlobal").toStringList();
    if (!tmpStringlist.isEmpty())
    {
        foreach(QString mod, tmpStringlist)
        {
            if (!FileSystem::loadMod(FileSystem::GAMEMOD_GLOBAL, mod))
            {
                wzLog(LOG_ERROR) << QString("The global mod \"%1\" does not exists.")
                                        .arg(mod);
            }
            else
            {
                wzLog(LOG_INFO) << QString("Global mod \"%1\" enabled.")
                                        .arg(mod);
            }
        }
    }

    tmpStringlist = config.get("loadModSP").toStringList();
    if (!tmpStringlist.isEmpty())
    {
        foreach(QString mod, tmpStringlist)
        {
            if (!FileSystem::loadMod(FileSystem::GAMEMOD_CAMPAIGN, mod))
            {
                wzLog(LOG_ERROR) << QString("The campaign mod \"%1\" does not exists.")
                                        .arg(mod);
            }
            else
            {
                wzLog(LOG_INFO) << QString("campaign mod \"%1\" enabled.")
                                        .arg(mod);
            }
        }
    }

    tmpStringlist = config.get("loadModMP").toStringList();
    if (!tmpStringlist.isEmpty())
    {
        foreach(QString mod, tmpStringlist)
        {
            if (!FileSystem::loadMod(FileSystem::GAMEMOD_MULTIPLAY, mod))
            {
                wzLog(LOG_ERROR) << QString("The multiplayer mod \"%1\" does not exists.")
                                        .arg(mod);
            }
            else
            {
                wzLog(LOG_INFO) << QString("Multiplayer mod \"%1\" enabled.")
                                        .arg(mod);
            }
        }
    }
    tmpStringlist.clear();

    FileSystem::printSearchPath();

    /*** Initialize translations ***/
    initI18n();
    wzLog(LOG_MAIN) << QString("Using language: %1").arg(getLanguage());

    // wzLog(LOG_WZ) << QString("Warzone 2100 - %1").arg(version_getFormattedVersionString());

    if (config.get("doCrash").toBool() ||
        config.get("doSelftest").toBool())
    {
        wzLog(LOG_INFO) << "Would run engine crashtest/selftest here.";
        return EXIT_FAILURE;
    }
    else if (!config.get("saveGame").toString().isEmpty() ||
             !config.get("game").toString().isEmpty())
    {
        config.set("gameType", GAMETYPE_SINGLEPLAYER);
        wzLog(LOG_INFO) << "Would open a singleplayer game here.";
        return EXIT_FAILURE;
    }

    // Now run the frontend
    Frontend::WzQMLView view;
    if (cmdDoHostlaunch)
    {
        config.set("gameType", GAMETYPE_MULTIPLAYER);
        view.run("screens/hostGameScreen.qml");
    }
    else
    {
        view.run();
    }
    
    app.exec();

    config.storeConfig("wz::config", CONFCONTEXT_USER);

    FileSystem::exit();
    debug_exit();
}