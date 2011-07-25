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
#include "qmlwzhelper.h"

// Qt Core
#include "QtCore/QVariant"
#include "QtCore/QString"

// Map list
#include <src/Launcher/Map/map.h>

// Configuration
#include "confighandler.h"

// framework - Logger and i18n
#include <lib/framework/frame.h>

static const int LOG_QML = Logger::instance().addLoggingLevel("qml", false);

Q_INVOKABLE void QMLWzHelper::logVariant(const QVariant &message)
{
    wzLog(LOG_QML) << message;
}

Q_INVOKABLE void QMLWzHelper::log(const QString &message)
{
    wzLog(LOG_QML) << message;
}

Q_INVOKABLE QString QMLWzHelper::tr(const QString& text)
{
    return QString::fromUtf8(_(text.toUtf8().constData()));
}

Q_INVOKABLE QVariantMap QMLWzHelper::getMapList(int techlevel)
{
    switch (techlevel)
    {
        case 1:
            return Map::getList(Map::GAMETYPE_SKIRMISH_T1);
        break;
        case 2:
            return Map::getList(Map::GAMETYPE_SKIRMISH_T2);
        break;
        case 3:
            return Map::getList(Map::GAMETYPE_SKIRMISH_T3);
        break;
    }

    return Map::getList(Map::GAMETYPE_SKIRMISH_T1);
}

Q_INVOKABLE int QMLWzHelper::setMap(int techlevel, const QString &name)
{
    QVariantMap entry = getMapList(techlevel).value(name).toMap();

    if (entry.isEmpty())
    {
        return 0;
    }
    
    config.set("mapName", name);
    config.set("techlevel", techlevel);
    config.set("fullMapName", entry["name"]);
    config.set("mapPath", entry["path"]);

    return entry["players"].toInt();
}

/**
 * This would normaly call one of the thousands setters Warzone has.
 */
Q_INVOKABLE void QMLWzHelper::setConfigValue(const QString& name, const QVariant& value)
{
    // TODO: May check if successfull.
    config.set(name, value);
}

/**
 * This would normaly call one of the thousands getters Warzone has.
 */
Q_INVOKABLE QVariant QMLWzHelper::getConfigValue(const QString &name)
{
    return config.get(name);
}

Q_INVOKABLE QString QMLWzHelper::getCurrentResolution()
{
    return QString("%1 x %2").arg(config.get("width").toInt())
                             .arg(config.get("height").toInt());
}

Q_INVOKABLE QStringList QMLWzHelper::getAvailableResolutions()
{
    return m_view->availableResolutions();
}

Q_INVOKABLE void QMLWzHelper::setResolution(const QString& resolution)
{
    QStringList res = resolution.split(" x ");
    if (res.size() != 2)
    {
        wzLog(LOG_QML) << "Invalid resolution" << resolution << "received.";
        return;
    }

    config.set("width", res.at(0));
    config.set("height", res.at(1));
}

Q_INVOKABLE QString QMLWzHelper::getLanguage()
{
    return getLanguageName();
}

Q_INVOKABLE QString QMLWzHelper::setNextLanguage()
{
    ::setNextLanguage();
    config.set("language", ::getLanguage());
    return getLanguageName();
}

