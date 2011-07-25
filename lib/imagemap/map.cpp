#include "map.h"
#include "map_p.h"

#include "container.h"
#include "containerimage.h"
#include "containersvg.h"

#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QFile>
#include <QtGui/QPixmap>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

// QJson
#include <qjson/parser.h>

// wzLog
#include <lib/framework/frame.h>

namespace Imagemap {

// Declared external.
const int LOG_IM = Logger::instance().addLoggingLevel("imagemap", false);

MapPrivate::MapPrivate() :
    m_errorCode(0),
    m_errorLine(0)
{
}

MapPrivate::~MapPrivate()
{
    m_images.clear();
}

Map::Map(const QString &filename) :
    d(new MapPrivate)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        setError(1, QString("Could not open %1 for reading: %2")
                       .arg(filename)
                       .arg(file.errorString())
        );
        return;
    }

    QJson::Parser parser;
    bool ok = false;
    QVariantMap result = parser.parse(&file, &ok).toMap();
    if (!ok) {
        setError(1, QString("Failed to load imagemap %1, error %2 on line %3")
                    .arg(filename)
                    .arg(parser.errorString())
                    .arg(parser.errorLine()));
        return;
    }

    if (result.isEmpty())
    {
        setError(1, QString("Empty Map %1").arg(filename));
        return;
    }

    QString basepath = QFileInfo(filename).absolutePath();

    QVariantMap imagemaps = result["images"].toMap();
    for (QVariantMap::iterator files = imagemaps.begin();
         files != imagemaps.end(); ++files)
    {
        QString filename = basepath + "/" + files.key() ;
        QVariantMap fileOpts = files.value().toMap();

        // Load the image container.
        Container* container;
        if (filename.endsWith(".svg"))
        {
            container = ContainerSVG::create(this, filename);
        }
        else
        {
            container = ContainerImage::create(this, filename);
        }

        if (errorCode() != 0)
        {
            setError(1, QString("Failed to load image %1: %2")
                            .arg(filename)
                            .arg(errorString()));
            return;
        }

        // Create the list of images.
        if (fileOpts.contains("full"))
        {
            QVariantMap value = fileOpts["full"].toMap();
            if (value.isEmpty())
            {
                wzLog(LOG_IM) << "Image" << filename << "is empty!";
            }
            else
            {
                Image myImage;

                // Add the first found image.
                myImage.type = FixedType;
                myImage.xPosition = 0;
                myImage.yPosition = 0;
                myImage.size = QSize(value.begin().value().toList()[0].toInt(),
                                     value.begin().value().toList()[1].toInt());
                myImage.container = container;

                d->m_images.insert(value.begin().key(), myImage);
            }
        }

        if (fileOpts.contains("fixed"))
        {
            QVariantMap value = fileOpts["fixed"].toMap();
            for (QVariantMap::iterator images = value.begin();
                 images != value.end(); ++images)
            {
                QVariantList imgdefList = images.value().toList();

                Image myImage;
                myImage.type = FixedType;
                myImage.xPosition = imgdefList[0].toInt();
                myImage.yPosition = imgdefList[1].toInt();
                myImage.size = QSize(imgdefList[2].toInt(),
                                     imgdefList[3].toInt());
                myImage.container = container;

                d->m_images.insert(images.key(), myImage);
            }
        }

        if (filename.endsWith(".svg") && fileOpts.contains("named"))
        {
            QVariantMap value = fileOpts["named"].toMap();
            for (QVariantMap::iterator images = value.begin();
                 images != value.end(); ++images)
            {
                Image myImage;
                myImage.type = NamedType;
                myImage.name = images.value().toString();
                myImage.container = container;

                d->m_images.insert(images.key(), myImage);
            }
        }

        if (fileOpts.contains("cache"))
        {
            container->setCache(fileOpts["cache"].toBool());
        }
    } // for files
}

Map::~Map()
{
    delete d;
}

QPixmap Map::pixmap(const QString &name,
                        const AspectRatioMode mode)
{
#ifdef IMAGEMAP_DEBUG
    wzLog(LOG_IM) << "Fetching:" << name
                  << "mode" << mode;
#endif

    if (!d->m_images.contains(name))
    {
        setError(1, QString("Image %1 not found!").arg(name));
        return QPixmap();
    }

    return d->m_images[name].container->pixmap(d->m_images[name], QSize(), mode);
};

QPixmap Map::pixmap(const QString &name,
                        const int factor,
                        const AspectRatioMode mode)
{
#ifdef IMAGEMAP_DEBUG
    wzLog(LOG_IM) << "Fetching:" << name
                  << "factor" << factor
                  << "mode" << mode;
#endif

    if (!d->m_images.contains(name))
    {
        setError(1, QString("Image %1 not found!").arg(name));
        return QPixmap();
    }

    return d->m_images[name].container->pixmap(d->m_images[name], factor, mode);
};

QPixmap Map::pixmap(const QString &name,
                        const QSize &size,
                        const AspectRatioMode mode)
{
#ifdef IMAGEMAP_DEBUG
    wzLog(LOG_IM) << "Fetching:" << name
                  << "size" << size
                  << "mode" << mode;
#endif

    if (!d->m_images.contains(name))
    {
        setError(1, QString("Image %1 not found!").arg(name));
        return QPixmap();
    }
    return d->m_images[name].container->pixmap(d->m_images[name], size, mode);
};

const QStringList Map::imageList()
{
    return QStringList(d->m_images.keys());
}

void Map::setError(int errorCode, QString errorString, int errorLine)
{
#ifdef IMAGEMAP_DEBUG
    wzLog(LOG_ERROR) << "Code" << errorCode
                            << errorString << "at line" << errorLine;
#endif

    d->m_errorCode = errorCode;
    d->m_errorString = errorString;
    d->m_errorLine = errorLine;
}

void Map::freeError()
{
    d->m_errorCode = 0;
    d->m_errorLine = 0;
    d->m_errorString.clear();
}

const QString& Map::errorString()
{
  return d->m_errorString;
}

int Map::errorCode()
{
  return d->m_errorCode;
}

int Map::errorLine()
{
  return d->m_errorLine;
}

} // namespace Imagemap {