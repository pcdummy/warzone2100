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

#include "imageprovider.h"

#include <lib/imagemap/loader.h>

#include <QtGui/QPixmap>

#include "lib/framework/frame.h"

namespace Frontend {
    
QPixmap ImagemapProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    QStringList path = id.split(':');

    QPixmap image;
    if (path.count() == 2 && path[1].toLower() == "width")
    {       
        image = Imagemap::Loader::instance().pixmap(path[1], requestedSize, Imagemap::ResizeToWith);
    }
    else if (path.count() == 2 && path[1].toLower() == "height")
    {
        image = Imagemap::Loader::instance().pixmap(path[0], requestedSize, Imagemap::ResizeToHeight);
    }
    else
    {
        image = Imagemap::Loader::instance().pixmap(path[0], requestedSize);
    }
    *size = image.size();

    return image;
}

} // namespace Frontend {