#ifndef IMAGEMAP_CONTAINERSVG_H
#define IMAGEMAP_CONTAINERSVG_H

#include "container.h"

class QSvgRenderer;

namespace Imagemap {

    class ContainerSVG : public Container
    {
    public:
        static ContainerSVG* create(Map *map, const QString &filename)
        {
            return new ContainerSVG(map, filename);
        };

        QPixmap pixmap(const Image &image,
                       const QSize &size,
                       const AspectRatioMode &mode = KeepAspectRatio);

    protected:
        ContainerSVG(Map *map, const QString &filename);
        ~ContainerSVG();

        QPixmap createScaledContainer_(const QVector2D &factors);

        QSvgRenderer* m_renderer;

        // Cache for renderer images.
        QHash<Image*, QPixmap> m_imagecache;
    };

}; // namespace Imagemap {

#endif // #ifndef IMAGEMAP_CONTAINERSVG_H