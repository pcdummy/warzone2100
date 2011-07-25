#ifndef IMAGEMAP_MAP_P_H
#define IMAGEMAP_MAP_P_H

#include <QtCore/QHash>

namespace Imagemap {

    struct Image;

    class MapPrivate
    {
    public:
        MapPrivate();
        ~MapPrivate();

        int m_errorCode;
        int m_errorLine;
        QString m_errorString;

        QHash<QString, Image> m_images;
    };

}; // namespace Imagemap

#endif // IMAGEMAP_MAP_P_H
