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
 *
 * TODO: I am not sure if this should be a class. - Fast.
 */

// Self
#include "wzqmlview.h"

// qtgame
#include "lib/qtgame/qtgame.h"

// Imagemap
#include "lib/imagemap/loader.h"

// Configuration
#include "confighandler.h"

// Image map to qml proxy.
#include "qmlimagemapprovider.h"

// wzHelper - QML Helper
#include "qmlwzhelper.h"

// Qt Gui for the quit signal.
#include <QtGui/QApplication>

// Qt Declarative
#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative/QDeclarativeContext>

// framework - logging
#include "lib/framework/frame.h"

class WzQMLViewPrivate
{
public:
    WzQMLViewPrivate()
    {

    };

    ~WzQMLViewPrivate()
    {
        // TODO: This segfaults.
/*        delete wzhelper;
        delete improvider;
        delete viewPort;*/
    }
    
    QtGameWidget* viewPort;
    QMLImagemapProvider* improvider;
    QMLWzHelper* wzhelper;
    QStringList resolutions;
};

WzQMLView::WzQMLView()
{
    m_d = new WzQMLViewPrivate;
}

WzQMLView::~WzQMLView()
{
    delete m_d;
}

void WzQMLView::run()
{
    wzLog(LOG_WZ) << "Setting up the viewport.";

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

    m_d->viewPort = new QtGameWidget(QSize(config.get("width").toInt(),
                                        config.get("height").toInt()), format);
    if (!m_d->viewPort->context()->isValid())
    {
        wzLog(LOG_FATAL) << "Failed to create an OpenGL context.\n"
                            << "This probably means that your graphics drivers are out of date.\n"
                            << "Try updating them!";
    }

    m_d->improvider = new QMLImagemapProvider();
    m_d->wzhelper = new QMLWzHelper(this);


    setViewport(m_d->viewPort);
    setResizeMode(QDeclarativeView::SizeRootObjectToView);

    wzLog(LOG_WZ) << "Loading the imagemaps.";
    // Add data/frontend/images/imagemap.js to the imagemap loader
    if (!Imagemap::Loader::instance().addImagemap("wz::frontend/images/imagemap.js"))
    {
        wzLog(LOG_FATAL) << "Failed to load Imagemap: frontend/images/imagemap.js!";
        qFatal("%s", qPrintable(Imagemap::Loader::instance().errorString()));
    }

    engine()->addImageProvider("imagemap", m_d->improvider);
    rootContext()->setContextProperty("wz", m_d->wzhelper);

    setSource(QUrl("wz::frontend/main.qml"));    

    // Enable VSync
    m_d->viewPort->setSwapInterval(true);

    int w, h;
    // Setup Fullscreen/window mode.
    w = config.get("width").toInt();
    h = config.get("height").toInt();
    
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

    connect(engine(), SIGNAL(quit()), QApplication::instance(), SLOT(quit()));
}

const QStringList& WzQMLView::availableResolutions() const
{
    if (!m_d->resolutions.isEmpty())
    {
        return m_d->resolutions;
    }
    
    QList<QSize> resolutions(m_d->viewPort->availableResolutions());

    foreach(QSize resolution, resolutions)
    {
        m_d->resolutions.append(QString("%1 x %2")
                            .arg(resolution.width())
                            .arg(resolution.height()));
    }

    return m_d->resolutions;
}
