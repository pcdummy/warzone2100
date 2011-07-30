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
 * @file qmlimageprovider.h
 * Provides an interface for QML to fetch pixmaps out of the previously loaded
 * imagemaps.
 *
 * @see: http://doc.qt.nokia.com/latest/qdeclarativeimageprovider.html
 * @see: lib/Imagemap
 */

#ifndef LAUNCHER_QMLIMAGEPROVIDER_H
#define LAUNCHER_QMLIMAGEPROVIDER_H

#include <QtCore/QHash>
#include <QtDeclarative/QDeclarativeImageProvider>

namespace Imagemap {
	class Map;
}
class QPixmap;

namespace Frontend {

class ImagemapProvider : public QDeclarativeImageProvider
{
public:
	ImagemapProvider()
		: QDeclarativeImageProvider(QDeclarativeImageProvider::Pixmap)
	{
	}

	QPixmap requestPixmap(const QString &id,
						  QSize *size,
						  const QSize &requestedSize);

private:
	QHash<QString, Imagemap::Map*> m_maps;
	QString m_basepath;
};

} // namespace Frontend {

#endif // #ifndef LAUNCHER_QMLIMAGEPROVIDER_H
