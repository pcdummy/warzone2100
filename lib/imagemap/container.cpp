#include "container.h"

// QT
#include <QtCore/QSize>
#include <QtCore/QFile>
#include <QtGui/QPixmap>
#include <QtGui/QVector2D>

uint qHash(const QSize &size)
{
    return (size.width() & 0x0000ffff) | (size.height() << 16);
}

using namespace Imagemap;

Container::Container(Map *map, const QString& filename) :
    m_map(map),
    m_filename(filename),
    m_nocache(false)
{
}

Container::~Container()
{
    m_cache.clear();
}

QPixmap Container::pixmap(const Image &image,
                          int factor,
                          const AspectRatioMode &mode)
{
    QSize size = image.size;
    size *= factor;
    return pixmap(image, size, mode);
}

QVector2D Container::calculateFactors_(const QSize &from, const QSize &to,
                                    const AspectRatioMode &mode)
{
    // Calculate resize factors
    qreal factorX = 1;
    qreal factorY = 1;
    if (!from.isEmpty() && from != to)
    {
        factorX = (qreal)from.width() / (qreal)to.width();
        factorY = (qreal)from.height() / (qreal)to.height();

        switch(mode)
        {
        case KeepAspectRatio:
            factorX = factorY = qMin(factorX, factorY);
            break;
        case ResizeToWith:
            factorY = factorX;
            break;
        case ResizeToHeight:
            factorX = factorY;
            break;
        case IgnoreAspectRatio:
            break;
        }
    }

    return QVector2D(factorX, factorY);
}
