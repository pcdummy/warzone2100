#ifndef IMAGEMAP_LOADER_H
#define IMAGEMAP_LOADER_H

#include "map.h"

namespace Imagemap {

    class LoaderPrivate;
    class IMAGEMAPSHARED_EXPORT Loader
    {
    public:
        static Loader& instance()
        {
            static Loader sInstance;
            return sInstance;
        };

        bool addImagemap(const QString &file);
        void removeImagemap(const QString &file);

        QPixmap pixmap(const QString &name,
                       const AspectRatioMode mode = KeepAspectRatio);
        QPixmap pixmap(const QString &name,
                       const int factor,
                       const AspectRatioMode mode = KeepAspectRatio);
        QPixmap pixmap(const QString &name,
                       const QSize &size,
                       const AspectRatioMode mode = KeepAspectRatio);

        const QStringList imageList();

        void freeError();
        const QString& errorString();
        int errorCode();
        int errorLine();

    private:
        Loader();
        Loader(const Loader&);
        Loader& operator=(const Loader&);
        ~Loader();

        LoaderPrivate* d;
    };

} // namespace Imagemap

#endif // IMAGEMAP_LOADER_H