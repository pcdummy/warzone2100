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
#ifndef CORE_MAP_MAP_H
#define CORE_MAP_MAP_H

// Qt Core
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QStringList>

#include "lib/framework/frame.h"

extern const int LOG_LMAP;

namespace Map {

enum Type
{
	CA_COMPLETE,		///< all data required for a stand alone level
	CA_CAMPAIGN,		///< the data set for a campaign (no map data)
	CA_CAMSTART,		///< mapdata for the start of a campaign
	CA_CAMCHANGE,		///< data for changing between levels
	CA_EXPAND,			///< extra data for expanding a campaign map
	CA_BETWEEN,			///< pause between missions
	CA_MKEEP,			///< off map mission (extra map data)
	CA_MCLEAR,			///< off map mission (extra map data)
	CA_EXPAND_LIMBO,	///< expand campaign map using droids held in apsLimboDroids
	CA_MKEEP_LIMBO,		///< off map saving any droids (selectedPlayer) at end into apsLimboDroids
	CA_NONE,			///< flags when not got a mission to go back to or
						///< when already on one.
	MP_MAP,				///< A type higher this is a multiplayer map
	MP_CAMPAIGN_T1,		///<
	MP_TEAMPLAY,		///< Do not use.
	MP_SKIRMISH_T1,
	MP_CAMPAIGN_T2,
	MP_CAMPAIGN_T3,
	MP_UNKNOWN,			///< Do not use.
	MP_SKIRMISH_T2,
	MP_SKIRMISH_T3
};

class MapPrivate;
class Map
{
public:
	Map(QString &name, uint8_t type);

	virtual ~Map();

	qint8 getType() const;
	const QString getName() const;

	void setPath(QString path);
	const QString getPath() const;

	void setMaxPlayers(quint8 maxPlayers);
	quint8 getMaxPlayers() const;

	void setDataset(quint8 type, QStringList &dataset);
	const QStringList getDataset(quint8 type) const;

	void addDatafile(quint8 type, QString file);
	QStringList getDatafiles(quint8 type) const;

	QList<quint8> supportedTypes() const;
	bool supportsType(quint8 type) const;

private:
	MapPrivate* d;
};

class ListPrivate;
class List
{
public:
	List();
	virtual ~List();

	/**
	 * @brief Sets the map in the configuration.
	 *
	 * @param mapType		Type of map, see the enumartion Type
	 * 						or use a custome one.
	 * @param name		  Shortened map name.
	 *
	 * @return The maps maxplayers or 0 on not found.
	 */
	quint8 setMap(qint8 mapType, const QString &name) const;

	/**
	 * @brief Tries to find all map in the physfs searchpath and parses them.
	 *
	 * This loads addon.lev first,
	 * then it loads all zipped maps and parses all found .lev files.
	 *
	 * @param forceRefresh	Force a refresh of the internal cache.
	*/
	const QList<Map*> getList(bool forcerefresh = false) const;

private:
	ListPrivate* d;
};

}; // namespace Map {

#endif // #ifndef CORE_MAP_MAP_H
