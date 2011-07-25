#include "containersvg.h"

// QT
#include <QtCore/QFile>

// SVG
#include <QtGui/QPainter>
#include <QtSvg/QSvgRenderer>

// WzLog
#include <lib/framework/frame.h>

using namespace Imagemap;

ContainerSVG::ContainerSVG(Map *map, const QString &filename) :
    Container(map, filename)
{
    if (!QFile::exists(m_filename))
    {
        m_map->setError(1, "File does not exists.");
    }

    m_renderer = new QSvgRenderer(m_filename);
    m_size = m_renderer->defaultSize();

#ifdef IMAGEMAP_DEBUG
    wzLog(LOG_IM) << "Loaded container:" << m_filename;
#endif
}

ContainerSVG::~ContainerSVG()
{
    delete m_renderer;
}

QPixmap ContainerSVG::pixmap(const Image &image,
                         const QSize &size,
                         const AspectRatioMode &mode)
{
#ifdef IMAGEMAP_DEBUG
    wzLog(LOG_IM) << "\tContainer:" << m_filename;
    wzLog(LOG_IM) << "\tMode:" << mode;
    wzLog(LOG_IM) << "\tType:" << image.type;
#endif

    if (image.type == FixedType)
    {
        QVector2D factors = calculateFactors_(size, image.size, mode);

#ifdef IMAGEMAP_DEBUG
        wzLog(LOG_IM) << "\tFactors:" << factors;
        wzLog(LOG_IM) << "\tSize:" << qRound(image.size.width() * factors.x()) <<
                                qRound(image.size.height() * factors.y());
        wzLog(LOG_IM) << "\tPosition:" << qRound(image.xPosition * factors.x()) <<
                                    qRound(image.yPosition * factors.y());
#endif

        // Creates a full scaled copy of the container image with the factors.
        QPixmap full = createScaledContainer_(factors);

        // Extracts the image.
        QPixmap result = full.copy(qRound(image.xPosition * factors.x()),
                                qRound(image.yPosition * factors.y()),
                                qRound(image.size.width() * factors.x()),
                                qRound(image.size.height() * factors.y()));

        return result;
    }
    else
    {
        if (!m_renderer->elementExists(image.name))
        {
            m_map->setError(1, QString("Image \"%1\" not found in container \"%2\"")
                                .arg(image.name).arg(m_filename));
            return QPixmap();
        }

        QRectF bounds = m_renderer->boundsOnElement(image.name);
        QVector2D factors = calculateFactors_(size, bounds.size().toSize(), mode);

#ifdef IMAGEMAP_DEBUG
        wzLog(LOG_IM) << "\tSVG ID:" << image.name;
        wzLog(LOG_IM) << "\tSVG Size:" << bounds.size();
        wzLog(LOG_IM) << "\tFactors:" << factors;
        wzLog(LOG_IM) << "\tSize:" << qRound(image.size.width() * factors.x()) <<
                                      qRound(image.size.height() * factors.y());
        wzLog(LOG_IM) << "\tPosition:" << qRound(image.xPosition * factors.x()) <<
                                          qRound(image.yPosition * factors.y());
#endif

        QSize mySize(qRound(bounds.width() * factors.x()),
                    qRound(bounds.height() * factors.y()));

        QPixmap result(mySize);
        result.fill(Qt::transparent);
        QPainter pixPainter(&result);
        pixPainter.setRenderHints(QPainter::Antialiasing |
                                    QPainter::SmoothPixmapTransform, true);
        m_renderer->render(&pixPainter, image.name);
        pixPainter.end();

        return result;
    }
}

QPixmap ContainerSVG::createScaledContainer_(const QVector2D &factors)
{
    if (factors.x() == 1 && factors.y() == 1 && m_cache.contains(m_size))
    {
        return m_cache[m_size];
    }

    QSize mySize(qRound(m_size.width() * factors.x()),
                 qRound(m_size.height() * factors.y()));

    if (!m_nocache && m_cache.contains(mySize))
    {
        return m_cache[mySize];
    }

    QPixmap pix(mySize);
    pix.fill(Qt::transparent);
    QPainter pixPainter(&pix);
    pixPainter.setRenderHints(QPainter::Antialiasing |
                              QPainter::SmoothPixmapTransform, true);
    m_renderer->render(&pixPainter);
    pixPainter.end();

    if (!m_nocache)
    {
        m_cache.insert(mySize, pix);
    }

#ifdef IMAGEMAP_DEBUG
    wzLog(LOG_IM) << "\tGenerated:" << m_filename << pix.size();
#endif

    return pix;
}