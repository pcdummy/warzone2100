#ifndef IMAGEMAP_CONTAINER_H
#define IMAGEMAP_CONTAINER_H

#include "map.h"
#include "map_p.h"

#include <QtGui/QVector2D>
#include <QtCore/QHash>
#include <QtGui/QPixmap>

uint qHash(const QSize &size);

namespace Imagemap {

    class Container
    {
    public:
        virtual ~Container();

        QPixmap pixmap(const Image &image,
                       const int factor,
                       const AspectRatioMode &mode = KeepAspectRatio);
        virtual QPixmap pixmap(const Image &image,
                               const QSize &size,
                               const AspectRatioMode &mode = KeepAspectRatio) = 0;

        inline void setCache(bool cache) { m_nocache = !cache; }

    protected:
        Container(Map *map, const QString& filename);

        QVector2D calculateFactors_(const QSize &from, const QSize &to,
                                    const AspectRatioMode &mode);

        // Reference to the map.
        Map* m_map;

        // The filepath of the container.
        QString m_filename;

        // The full Size of the underlaying container image.
        QSize m_size;

        // Cache of resized container images.
        QHash<QSize, QPixmap> m_cache;

        // Cache disabled when true.
        bool m_nocache;
    };

    enum ImageTypes {
        InvalidType,
        FixedType,
        NamedType
    };

    struct Image {
        ImageTypes type;
        QString name;
        int xPosition;
        int yPosition;
        QSize size;
        Container* container;
    };

}; // namespace Imagemap {

#endif // IMAGEMAP_CONTAINER_H
