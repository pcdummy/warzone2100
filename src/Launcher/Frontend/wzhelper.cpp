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
#include "wzhelper.h"

// Qt Core
#include "QtCore/QVariant"
#include "QtCore/QString"

// Configuration
#include "confighandler.h"

// LOG_FRONTEND and availableResolutions
#include "wzqmlview.h"

// framework - Logger and i18n
#include <lib/framework/frame.h>

namespace Frontend {

WzHelper::WzHelper(WzQMLView* qmlview):
		QObject(qmlview),
        m_view(qmlview),
        m_maplist(new Map::List)
{
}

WzHelper::~WzHelper()
{
	delete m_maplist;
}
	
void WzHelper::logVariant(const QVariant &message)
{
    wzLog(LOG_FRONTEND) << message;
}

void WzHelper::log(const QString &message)
{
    wzLog(LOG_FRONTEND) << message;
}

QString WzHelper::tr(const QString& text)
{
    return QString::fromUtf8(_(text.toUtf8().constData()));
}

QVariantMap WzHelper::getMapList(int mapType)
{
	if (mapType < 0)
	{
		wzLog(LOG_FRONTEND) << "Invalid mapType:" << mapType;
		return QVariantMap();
	}
	
	QList<Map::Map*> list = m_maplist->getList();
	QVariantMap result;

	wzLog(LOG_FRONTEND) << "Finding maps for type:" << mapType;
	for (int i = 0; i < list.size(); ++i)
	{
		Map::Map* map = list.at(i);
		if (map->supportsType(mapType))
		{
			wzLog(LOG_FRONTEND) << "\t" << map->getName() << map->supportedTypes();
			result.insert(map->getName(), map->getMaxPlayers());
		}
	}

	return result;
}

int WzHelper::setMap(int mapType, QString name)
{
	if (mapType < 0)
	{
		wzLog(LOG_FRONTEND) << "Invalid mapType:" << mapType;
		return 0;
	}
	return m_maplist->setMap(mapType, name);
}

QString WzHelper::getCurrentResolution()
{
    return QString("%1 x %2").arg(config.get("width").toInt())
                             .arg(config.get("height").toInt());
}

QStringList WzHelper::getAvailableResolutions()
{
    if (!m_view)
    {
        return QStringList();
    }
    
    return m_view->getAvailableResolutions();
}

void WzHelper::setResolution(const QString& resolution)
{
    QStringList res = resolution.split(" x ");
    if (res.size() != 2)
    {
        wzLog(LOG_FRONTEND) << "Invalid resolution" << resolution << "received.";
        return;
    }

    config.set("width", res.at(0));
    config.set("height", res.at(1));
}

QString WzHelper::getLanguage()
{
    return getLanguageName();
}

QString WzHelper::setNextLanguage()
{
    ::setNextLanguage();
    config.set("language", ::getLanguage());
    return getLanguageName();
}

} // namespace Frontend {
