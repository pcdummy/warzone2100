#ifndef IMAGEMAP_MAP_H
#define IMAGEMAP_MAP_H

#include "imagemap_export.h"

class QSize;
class QPixmap;
class QIODevice;
class QStringList;

namespace Imagemap {

    extern const int LOG_IM;

    enum AspectRatioMode {
        IgnoreAspectRatio,
        KeepAspectRatio,
        ResizeToWith,
        ResizeToHeight,
    };

    class MapPrivate;
    class IMAGEMAPSHARED_EXPORT Map
    {
    public:
        Map(const QString &file);
        ~Map();

        QPixmap pixmap(const QString &name,
                       const AspectRatioMode mode = KeepAspectRatio);
        QPixmap pixmap(const QString &name,
                       const int factor,
                       const AspectRatioMode mode = KeepAspectRatio);
        QPixmap pixmap(const QString &name,
                       const QSize &size,
                       const AspectRatioMode mode = KeepAspectRatio);

        const QStringList imageList();

        void setError(int errorCode, const QString errorString, int errorLine = 0);
        void freeError();

        const QString& errorString();
        int errorCode();
        int errorLine();

    private:
        MapPrivate* d;
    };

} // namespace Imagemap

#endif // IMAGEMAP_MAP_H