#ifndef IMAGEMAP_CONTAINERIMAGE_H
#define IMAGEMAP_CONTAINERIMAGE_H

#include "container.h"

namespace Imagemap {

    class ContainerImage : public Container
    {
    public:
        static ContainerImage* create(Map *map, const QString &filename)
        {
            return new ContainerImage(map, filename);
        };

        QPixmap pixmap(const Image &image,
                       const QSize &size,
                       const AspectRatioMode &mode = KeepAspectRatio);

    protected:
        ContainerImage(Map *map, const QString &filename);

        QPixmap createScaledContainer_(const QVector2D &factors);
    };

}; // namespace Imagemap {

#endif // #ifndef IMAGEMAP_CONTAINERIMAGE_H