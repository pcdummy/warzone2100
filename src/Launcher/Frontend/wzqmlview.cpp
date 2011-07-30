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
/**
 * @file wzqmlview.h
 *
 * The launchers Declarative View,
 */

// Self
#include "wzqmlview.h"

// qtgame
#include <lib/qtgame/qtgame.h>

// Imagemap
#include <lib/imagemap/loader.h>

// Image map to qml proxy.
#include "imageprovider.h"

// WzHelper - QML<->C++ Helper, gets registered as WZ
#include "wzhelper.h"

// Configuration
#include <confighandler.h>

// Qt Gui for the quit signal.
#include <QtGui/QApplication>

// Qt Declarative

#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative/QDeclarativeContext>

#include <QtGui/QGraphicsView>

// framework - logging
#include "lib/framework/frame.h"

const int LOG_FRONTEND = Logger::instance().addLoggingLevel("frontend", false);

namespace Frontend {

class WzQMLViewPrivate
{
public:
    WzQMLViewPrivate() :
        viewPort(0), improvider(0) {}

    void init();

    QtGameWidget* viewPort;
    ImagemapProvider* improvider;
    QStringList resolutions;
};


WzQMLView::WzQMLView(QWidget *parent)
    : QDeclarativeView(parent)
{
    d = new WzQMLViewPrivate();
    wzhelper = new WzHelper(this);
    d->init();
}

WzQMLView::~WzQMLView()
{
    delete wzhelper;
    delete d;
}

void WzQMLViewPrivate::init()
{
    wzLog(LOG_FRONTEND) << "Setting up the viewport.";

    // Workaround for incorrect text rendering on nany platforms.
    QGL::setPreferredPaintEngine(QPaintEngine::OpenGL);

    QGLFormat format;
    format.setDoubleBuffer(true);
    format.setAlpha(true);

    if (config.get("FSAA").toInt())
    {
        format.setSampleBuffers(true);
        format.setSamples(config.get("FSAA").toInt());
    }

    viewPort = new QtGameWidget(QSize(config.get("width").toInt(),
                                        config.get("height").toInt()), format);
    
    // Enable VSync
    viewPort->setSwapInterval(true);
    
    if (!viewPort->context()->isValid())
    {
        wzLog(LOG_FATAL) << "Failed to create an OpenGL context.\n"
                            << "This probably means that your graphics drivers are out of date.\n"
                            << "Try updating them!";
    }

    wzLog(LOG_FRONTEND) << "Loading the imagemaps.";
    // Add data/frontend/images/imagemap.js to the imagemap loader
    if (!Imagemap::Loader::instance().addImagemap("wz::frontend/images/imagemap.js"))
    {
        wzLog(LOG_FATAL) << "Failed to load Imagemap: frontend/images/imagemap.js!";
        qFatal("%s", qPrintable(Imagemap::Loader::instance().errorString()));
    }

    qmlRegisterType<WzHelper>("Warzone",1,0,"Wz");

    improvider = new ImagemapProvider();
}

void WzQMLView::run(const QString loadScreen, const QString loadMenu)
{
    wzLog(LOG_FRONTEND) << "set viewport";
    setViewport(d->viewPort);
    setResizeMode(QDeclarativeView::SizeRootObjectToView);

    wzLog(LOG_FRONTEND) << "call engine";
    engine()->addImageProvider("imagemap", d->improvider);

    rootContext()->setContextProperty("wz", wzhelper);

    wzLog(LOG_FRONTEND) << "execute";
    setSource(QUrl("wz::frontend/main.qml"));

    QObject::connect(engine(), SIGNAL(quit()), QApplication::instance(), SLOT(quit()));
    
    if (!loadScreen.isEmpty())
    {
        rootContext()->setContextProperty("gLoadScreen", loadScreen);
    }
    else
    {
        rootContext()->setContextProperty("gLoadScreen", "screens/menuScreen.qml");
    }

    if (!loadMenu.isEmpty())
    {
        rootContext()->setContextProperty("gLoadMenu", loadMenu);
    }
    else
    {
        rootContext()->setContextProperty("gLoadMenu", "menu/main.qml");
    }
    
    int w, h;
    // Setup Fullscreen/window mode.
    w = config.get("width").toInt();
    h = config.get("height").toInt();

    wzLog(LOG_FRONTEND) << "show, size:" << w << "x" << h;
    
    if (config.get("fullscreen").toBool())
    {
        resize(w, h);
        showFullScreen();
    }
    else
    {
        show();
        setMinimumSize(w, h);
        setMaximumSize(w, h);
    }
}

const QStringList& WzQMLView::getAvailableResolutions() const
{
    if (!d->resolutions.isEmpty())
    {
        return d->resolutions;
    }
    
    QList<QSize> res(d->viewPort->availableResolutions());

    foreach(QSize resSize, res)
    {
        d->resolutions.append(QString("%1 x %2")
                            .arg(resSize.width())
                            .arg(resSize.height()));
    }

    return d->resolutions;
}

} // namespace Frontend