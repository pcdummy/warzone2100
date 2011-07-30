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

// QML_DECLARE_TYPE Macro
#include <QtDeclarative/qdeclarative.h>

// Map List
#include <src/Launcher/Map/map.h>

// Configuration
#include <confighandler.h>

// GAMETYPE enum
#include <lconfig.h>

namespace Map
{
	class Map;
}

namespace Frontend {

// Forwarder
class WzQMLView;

class WzHelper : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QObject *config READ getConfig CONSTANT)
	Q_ENUMS(Gametype)
	Q_ENUMS(Techlevel)
public:
	WzHelper(WzQMLView* qmlview = 0);

	virtual ~WzHelper();

	WzHelper(const WzHelper& copy) : QObject() {};

	/**
	 * @brief Helper for the property "config".
	 */
	inline QObject* getConfig() const
	{
		return &config;
	}

	QObject* getMapList() const;

	enum Gametype {
		Campaign = GAMETYPE_CAMPAIGN,
		SinglePlayer = GAMETYPE_SINGLEPLAYER,
		Multiplayer = GAMETYPE_MULTIPLAYER
	};

	enum Techlevel
	{
		Techlevel_NONE = Map::CA_NONE,
		Techlevel_1 = Map::MP_SKIRMISH_T1,
		Techlevel_2 = Map::MP_SKIRMISH_T2,
		Techlevel_3 = Map::MP_SKIRMISH_T3
	};
	Q_INVOKABLE QVariantMap getMapList(int mapType);
	Q_INVOKABLE int setMap(int mapType, QString name);

	/**
	 * @brief Log a message to wzLog.
	 */
	Q_INVOKABLE void logVariant(const QVariant &message);
	Q_INVOKABLE void log(const QString &message);

	/**
	 * @brief Translate the given message using gettext.
	 */
	Q_INVOKABLE QString tr(const QString &message);

	Q_INVOKABLE QString getCurrentResolution();
	Q_INVOKABLE QStringList getAvailableResolutions();
	Q_INVOKABLE void setResolution(const QString &resolution);

	Q_INVOKABLE QString getLanguage();
	Q_INVOKABLE QString setNextLanguage();

private:
	WzQMLView* m_view;
	Map::List* m_maplist;
};

} // namespace Frontend {

Q_DECLARE_METATYPE(Frontend::WzHelper::Gametype)
Q_DECLARE_METATYPE(Frontend::WzHelper::Techlevel)
QML_DECLARE_TYPE(Frontend::WzHelper)

#endif // LAUNCHER_QMLWZHELPER_H
