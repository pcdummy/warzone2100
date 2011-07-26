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
 * @file qmlwzhelper.cpp
 * Class with QML <> C++ Helper functions.
 */

#ifndef LAUNCHER_QMLWZHELPER_H
#define LAUNCHER_QMLWZHELPER_H

// Qt Core
#include <QtCore/QObject>
#include <QtCore/QVariantMap>
#include <QtCore/QString>
#include <QtCore/QStringList>

// QML_DECLARE_TYPE Macro
#include <QtDeclarative/qdeclarative.h>

// Configuration
#include <confighandler.h>

// GAMETYPE enum
#include <lconfig.h>

namespace Frontend {

// Forwarder
class WzQMLView;
    
class WzHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject* config READ getConfig CONSTANT)
public:
    WzHelper(WzQMLView* qmlview = 0) :
        m_view(qmlview) {}

    virtual ~WzHelper() {};

    WzHelper(const WzHelper& copy) : QObject() {};

    /**
     * @brief Helper for the property "config".
     */
    QObject* getConfig() const
    {
        return &config;
    }

    Q_ENUMS(Gametype)
    enum Gametype {
        Campaign = GAMETYPE_CAMPAIGN,
        SinglePlayer = GAMETYPE_SINGLEPLAYER,
        Multiplayer = GAMETYPE_MULTIPLAYER
    };
    
    /**
     * @brief Log a message to wzLog.
     */
    Q_INVOKABLE void logVariant(const QVariant &message);
    Q_INVOKABLE void log(const QString &message);

    /**
     * @brief Translate the given message using gettext.
     */
    Q_INVOKABLE QString tr(const QString &message);

    /**
     * @brief Calls Map::getMapList(techlevel) and returns its result.
     *
     * @see Map::getMapList() for the resulting format.
     *
     * TODO: Should use ENUM's here
     */
    Q_INVOKABLE QVariantMap getMapList(int techlevel);

    /**
     * @brief Sets the map in the configuration.
     *
     * @param techlevel     Tech level 1-3
     * @param name          Shortened map name.
     *
     * @return The maps maxplayers or 0 on not found.
     */
    Q_INVOKABLE int setMap(int techlevel, const QString &name);

    Q_INVOKABLE QString getCurrentResolution();
    Q_INVOKABLE QStringList getAvailableResolutions();
    Q_INVOKABLE void setResolution(const QString &resolution);

    Q_INVOKABLE QString getLanguage();
    Q_INVOKABLE QString setNextLanguage();
private:
    WzQMLView* m_view;
};

} // namespace Frontend {

Q_DECLARE_METATYPE(Frontend::WzHelper::Gametype)
QML_DECLARE_TYPE(Frontend::WzHelper)

#endif // LAUNCHER_QMLWZHELPER_H