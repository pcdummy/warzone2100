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
#include "src/Launcher/Map/map.h"
#include "map_p.h"

#include <QtCore/QString>

#include "src/Launcher/Filesystem/filesystem.h"

#include "src/Launcher/confighandler.h"

// Compiler fixes and logging.
#include <lib/framework/frame.h>

#include "lib/framework/file.h" // loadFile

// Global
uint16_t levVal;

// error report function for the level parser
void lev_error(const char* msg)
{
	debug(LOG_ERROR, "Level File parse error: `%s` at line `%d` text `%s`", msg, lev_get_lineno(), lev_get_text());
}

const int LOG_LMAP = Logger::instance().addLoggingLevel("map", false);

namespace Map {

/**
 * @brief Removes the prepending "Sk-" and leading "-T1" from a mapname.
 *
 * @param in	Mapname to shorten.
 *
 * @return	  result.
 */
static inline QString shortenMapname(const char *in)
{
	QString out(in);

	 // Remove "Sk-"
	if (out.startsWith("Sk-"))
	{
		out.remove(0, 3);
	}

	 // Remove -T1|-T2|-T3
	 if (out.right(3).left(2) == "-T")
	 {
		 out.chop(3);
	 }

	 return out;
}

class MapPrivate
{
public:
	QString name;							///< Display name, a.e.: "Rush"
	QString path;							///< Internal path, a.e.: "multiplay/maps/4c-rush.gam"
	quint8 type;							///< Map Type, see the enum Type, always MP_MAP for multiplayer maps
	quint8 maxPlayers;						///< Maximum number of players
	QMap<quint8, QStringList> datasets; 	///< Datasets for this Map/Level
	QMap<quint8, QStringList> datafiles;	///< Own data files.
};

Map::Map(QString& name, quint8 type) :
	d((new MapPrivate))
{
	d->name = name;
	d->type = type;
}

Map::~Map()
{
	delete d;
}

const QString Map::getName() const
{
	return d->name;
}

qint8 Map::getType() const
{
	return d->type;
}

void Map::setPath(QString path)
{
	d->path = path;
}

const QString Map::getPath() const
{
	return d->path;
}

void Map::setMaxPlayers(quint8 maxPlayers)
{
	d->maxPlayers = maxPlayers;
}

quint8 Map::getMaxPlayers() const
{
	return d->maxPlayers;
}

void Map::setDataset(quint8 type, QStringList &dataset)
{
	d->datasets.insert(type, dataset);
}

const QStringList Map::getDataset(quint8 type) const
{
	return d->datasets[type];
}

void Map::addDatafile(quint8 type, QString file)
{
	d->datafiles[type].append(file);
}

QStringList Map::getDatafiles(quint8 type) const
{
	return d->datafiles[type];
}

QList<quint8> Map::supportedTypes() const
{
	return d->datasets.keys();
}

bool Map::supportsType(quint8 type) const
{
	return d->datasets.contains(type);
}

class ListPrivate
{
public:
	void buildList(bool forcerefresh = false);
	bool parseLev(const char* filename);

	QHash<QString, QStringList> datasets;
	QHash<QString, Map*> maps;
};

void ListPrivate::buildList(bool forcerefresh)
{
	char **filelist, **file;
	ssize_t length;

	if (forcerefresh)
	{
		maps.clear();
	}
	else if (!maps.isEmpty())
	{
		return;
	}

	ASSERT(parseLev("addon.lev"), "Failed to parse addon.lev!");

	// Load all maps into the searchpath.
	FileSystem::loadMaps();

	filelist = PHYSFS_enumerateFiles("");
	for (file = filelist; *file != NULL; ++file)
	{
		length = strlen(*file);

		if ((length > 10 && strcmp(*file+(length-10), ".addon.lev") == 0) ||
			(length > 13 && strcmp(*file+(length-13), ".xplayers.lev") == 0))
		{
			// No need to print errors
			parseLev(*file);
		}
	}
	PHYSFS_freeList(filelist);

	// Unload maps
	FileSystem::unloadMaps();

	// We have all datasets assigned, we don't need them anymore.
	datasets.clear();

	return;
}

bool ListPrivate::parseLev(const char *filename)
{
	lexerinput_t input;
	int token;

	Map *map;
	quint8 type = CA_NONE;
	QString name;
	QStringList *dataset;

	char *buffer;
	uint32_t size;
	if (!loadFile(filename, &buffer, &size))
	{
		wzLog(LOG_ERROR) << "Failed to load" << filename;
		return false;
	}

#ifdef DEBUG
	wzLog(LOG_LMAP) << "Parsing:" << filename;
#endif

	input.type = LEXINPUT_BUFFER;
	input.input.buffer.begin = buffer;
	input.input.buffer.end = &buffer[size];

	lev_set_extra(&input);

	for (token = lev_lex(); token != 0; token = lev_lex())
	{
		switch (token)
		{
		case LTK_LEVEL:
		case LTK_CAMPAIGN:
		case LTK_CAMSTART:
		case LTK_CAMCHANGE:
		case LTK_EXPAND:
		case LTK_BETWEEN:
		case LTK_MKEEP:
		case LTK_MCLEAR:
		case LTK_EXPAND_LIMBO:
		case LTK_MKEEP_LIMBO:
			// set the dataset type
			switch (token)
			{
			case LTK_LEVEL:
				type = CA_COMPLETE;
				break;
			case LTK_CAMPAIGN:
				type = CA_CAMPAIGN;
				break;
			case LTK_CAMSTART:
				type = CA_CAMSTART;
				break;
			case LTK_BETWEEN:
				type = CA_BETWEEN;
				break;
			case LTK_MKEEP:
				type = CA_MKEEP;
				break;
			case LTK_CAMCHANGE:
				type = CA_CAMCHANGE;
				break;
			case LTK_EXPAND:
				type = CA_EXPAND;
				break;
			case LTK_MCLEAR:
				type = CA_MCLEAR;
				break;
			case LTK_EXPAND_LIMBO:
				type = CA_EXPAND_LIMBO;
				break;
			case LTK_MKEEP_LIMBO:
				type = CA_MKEEP_LIMBO;
				break;
			default:
				ASSERT( false,"eh?" );
				break;
			}

			token = lev_lex();
			if (token == LTK_STRING || token == LTK_IDENT)
			{
				name = shortenMapname(lev_text);
				if (type == CA_CAMPAIGN)
				{
					datasets.insert(name, QStringList());
					dataset = &datasets[name];
					// wzLog(LOG_LMAP) << "New dataset:"  << name;
				}
				else
				{

 					if (maps.contains(name.toLower()))
 					{
 						map = maps[name.toLower()];
 						// wzLog(LOG_LMAP) << "Found map:"  << name
 						//				<< "line:" << lev_get_lineno();
 					}
 					else
 					{
						map = new Map(name, type);
						maps.insert(name.toLower(), map);
#ifdef DEBUG
						wzLog(LOG_LMAP) << "New map:"  << name
										<< "line:" << lev_get_lineno();
#endif
					}
				}
			}
			else
			{
				lev_error("Syntax error (1)");
				return false;
			}

			break;
		case LTK_PLAYERS:
			token = lev_lex();
			if (token == LTK_INTEGER &&
				(type == CA_COMPLETE || type >= MP_MAP))
			{
				map->setMaxPlayers(static_cast<quint8>(levVal));
			}
			else
			{
				lev_error("Syntax Error (2)");
				return false;
			}
			break;
		case LTK_TYPE:
			token = lev_lex();
			if (token == LTK_INTEGER &&
				type == CA_COMPLETE)
			{
				if (levVal < MP_MAP)
				{
					lev_error("invalid type number");
					return false;
				}

				type = static_cast<quint8>(levVal);
			}
			else
			{
				lev_error("Syntax Error (3)");
				return false;
			}
			break;
		case LTK_DATASET:
			token = lev_lex();
			if (token == LTK_IDENT && type != CA_COMPLETE)
			{
 				if (!datasets.contains(lev_text))
 				{
 					lev_error("Unknown dataset");
 					return false;
 				}
 				map->setDataset(type, datasets[lev_text]);
			}
			else
			{
				lev_error("Syntax Error (4)");
				return false;
			}
			break;
		case LTK_DATA:
			token = lev_lex();
			if (token == LTK_STRING)
			{
				if (type == CA_CAMPAIGN)
				{
					dataset->append(lev_text);
				}
				else
				{
					map->addDatafile(type, QString(lev_text));
				}
			}
			else
			{
				lev_error("Syntax Error (5)");
				return false;
			}
			break;
		case LTK_GAME:
			token = lev_lex();
			if (token == LTK_STRING &&
				type != CA_CAMPAIGN)
			{
				map->setPath(QString(lev_text));
			}
			else
			{
				lev_error("Syntax Error (6)");
				return false;
			}
			break;
		default:
			lev_error("Unexpected token");
			break;
		}
	}

	lev_lex_destroy();

	return true;
}

List::List() :
	d((new ListPrivate))
{
}

List::~List()
{
	delete d;
}

quint8 List::setMap(qint8 mapType, const QString &name) const
{
	d->buildList();
	if (!d->maps.contains(name.toLower()))
	{
		wzLog(LOG_LMAP) << "Unknown map" << name;
		return 0;
	}

	Map* map = d->maps[name.toLower()];
	if (map->getType() == CA_NONE || !map->supportsType(mapType))
	{
		wzLog(LOG_LMAP) << QString("Map %1 does not support type: %2")
								.arg(name).arg(mapType);
		return 0;
	}

	config.set("mapName", map->getName());
	config.set("techlevel", mapType);

	return map->getMaxPlayers();
}

const QList<Map*> List::getList(bool forcerefresh) const
{
	d->buildList(forcerefresh);

	return d->maps.values();
}

} // namespace Map {
