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
#include <src/Launcher/wzqmlview.h>

// Get platform defines before checking for them.
// Qt headers MUST come before platform specific stuff!
// compiler fixes + logging + i18n
#include <lib/framework/frame.h>

// Subdirectory in users home/documents
// TODO: Let ./configure handle this
#if defined(Q_OS_WIN)
# define WZ_WRITEDIR "Warzone 2100 master"
#elif defined(Q_OS_MAC)
# define WZ_WRITEDIR "Warzone 2100 master"
#else
# define WZ_WRITEDIR ".warzone2100-master"
#endif

/* Always use fallbacks on Windows */
#if defined(WZ_OS_WIN)
#  undef WZ_DATADIR
#endif

#if !defined(WZ_DATADIR)
#  define WZ_DATADIR "data"
#endif

int main(int argc, char *argv[])
{
    QString tmpstr;
    
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

    // Setup debugging cmd options.
    if (!ParseCommandLineEarly(utfargc, utfargv))
    {
        return EXIT_FAILURE;
    }
    
    if (cmdShowVersion)
    {
        printf("%s", "Warzone 2100 launcher\n");
        // printf("Warzone 2100 - %s\n", version_getFormattedVersionString());
        return EXIT_SUCCESS;
    }    
        
    tmpstr = FileSystem::init(args.at(0), WZ_WRITEDIR, cmdConfigdir);
    if (tmpstr.isEmpty())
    {
        return EXIT_FAILURE;
    }
    config.set("configDir", QVariant(tmpstr));      
    
    tmpstr = FileSystem::scanDataDirs(cmdDatadir, WZ_DATADIR);
    if (tmpstr.isEmpty())
    {
        return EXIT_FAILURE;
    }
    config.set("dataDir", QVariant(tmpstr));         
    
    // Its important this line is before ParseCommandLine as the user
    // can override values by the cmd.
    config.loadConfig("wz::config");
       
    // Now do the rest.
    if (!ParseCommandLine(utfargc, utfargv))
    {
        return EXIT_FAILURE;
    }   
   
    tmpstr = config.get("loadModGlobal").toString();
    if (!tmpstr.isEmpty())
    {
        tmpstr = FileSystem::haveMod(FileSystem::GAMEMOD_GLOBAL, tmpstr);
        if (tmpstr.isEmpty())
        {
            wzLog(LOG_ERROR) << QString("The global mod \"%1\" does not exists.")
                                    .arg(tmpstr);
            config.set("loadModGlobal", QVariant(""));
        }
        else
        {
            wzLog(LOG_INFO) << QString("Global mod \"%1\" enabled.")
                                    .arg(tmpstr);
            config.set("loadModGlobal", QVariant(tmpstr));
        }
    }

    tmpstr = config.get("loadModSP").toString();
    if (!tmpstr.isEmpty())
    {
        tmpstr = FileSystem::haveMod(FileSystem::GAMEMOD_CAMPAIGN, tmpstr);
        if (tmpstr.isEmpty())
        {
            wzLog(LOG_ERROR) << QString("The campaign mod \"%1\" does not exists.")
                                    .arg(tmpstr);
            config.set("loadModSP", QVariant(""));
        }
        else
        {
            wzLog(LOG_INFO) << QString("Campaign mod \"%1\" enabled.")
                                    .arg(tmpstr);
            config.set("loadModGlobal", QVariant(tmpstr));
        }
    }

    tmpstr = config.get("loadModMP").toString();
    if (!tmpstr.isEmpty())
    {
        tmpstr = FileSystem::haveMod(FileSystem::GAMEMOD_MULTIPLAY, tmpstr);
        if (tmpstr.isEmpty())
        {
            wzLog(LOG_ERROR) << QString("The multiplayer mod \"%1\" does not exists.")
                                    .arg(tmpstr);
            config.set("loadModMP", QVariant(""));
        }
        else
        {
            wzLog(LOG_INFO) << QString("Multiplayer mod \"%1\" enabled.")
                                    .arg(tmpstr);
            config.set("loadModMP", QVariant(tmpstr));
        }
    }
    
    tmpstr.clear();

    /*** Initialize translations ***/
    initI18n();
    wzLog(LOG_WZ) << QString("Using language: %1").arg(getLanguage());

    // wzLog(LOG_WZ) << QString("Warzone 2100 - %1").arg(version_getFormattedVersionString());

    // Now run the frontend
    WzQMLView view;
    view.run();

    app.exec();

    config.storeConfig("wz::config", CONFCONTEXT_USER);
}