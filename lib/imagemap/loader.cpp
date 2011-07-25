#include "loader.h"
#include "loader_p.h"

#include <QtCore/QString>
#include <QtCore/QString>
#include <QtGui/QPixmap>

using namespace Imagemap;

LoaderPrivate::LoaderPrivate() :
    m_errorCode(0),
    m_errorLine(0)
{
}

LoaderPrivate::~LoaderPrivate()
{
    m_images.clear();
}

void LoaderPrivate::setError(int errorCode, QString errorString, int errorLine)
{
    m_errorCode = errorCode;
    m_errorString = errorString;
    m_errorLine = errorLine;
}

Loader::Loader() :
    d(new LoaderPrivate)
{}

Loader::~Loader()
{
    delete d;
}

bool Loader::addImagemap(const QString& file)
{
    if (d->m_maps.contains(file)) {
        return false;
    }

    Map* map = new Map(file);
    if (map->errorCode() != 0)
    {
        d->setError(map->errorCode(), map->errorString(), map->errorLine());
        return false;
    }

    d->m_maps.insert(file, map);

    QStringList imageList = map->imageList();
    foreach(QString image, imageList)
    {
        d->m_images.insert(image, map);
    }

    return true;
}

void Loader::removeImagemap(const QString& file)
{
    if (d->m_maps.remove(file) != 0)
    {
        return;
    }

    // Rebuild the imagelist
    d->m_images.clear();
    foreach(Map* map, d->m_maps)
    {
        QStringList imageList = map->imageList();
        foreach(QString image, imageList)
        {
            d->m_images.insert(image, map);
        }
    }

}

QPixmap Loader::pixmap(const QString &name,
                        const AspectRatioMode mode)
{
    if (!d->m_images.contains(name))
    {
        d->setError(1, QString("Image %1 not found!").arg(name));
        return QPixmap();
    }

    return d->m_images[name]->pixmap(name, QSize(), mode);
};

QPixmap Loader::pixmap(const QString &name,
                        const int factor,
                        const AspectRatioMode mode)
{
    if (!d->m_images.contains(name))
    {
        d->setError(1, QString("Image %1 not found!").arg(name));
        return QPixmap();
    }

    return d->m_images[name]->pixmap(name, factor, mode);
};

QPixmap Loader::pixmap(const QString &name,
                        const QSize &size,
                        const AspectRatioMode mode)
{
    if (!d->m_images.contains(name))
    {
        d->setError(1, QString("Image %1 not found!").arg(name));
        return QPixmap();
    }
    return d->m_images[name]->pixmap(name, size, mode);
};

const QStringList Loader::imageList()
{
    return QStringList(d->m_images.keys());
}

void Loader::freeError()
{
    d->m_errorCode = 0;
    d->m_errorLine = 0;
    d->m_errorString.clear();
}

const QString& Loader::errorString()
{
  return d->m_errorString;
}

int Loader::errorCode()
{
  return d->m_errorCode;
}

int Loader::errorLine()
{
  return d->m_errorLine;
}