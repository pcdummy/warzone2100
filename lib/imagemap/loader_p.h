#ifndef IMAGEMAP_LOADER_P_H
#define IMAGEMAP_LOADER_P_H

#include "map.h"

#include <QtCore/QMap>
#include <QtCore/QHash>

namespace Imagemap {

    class LoaderPrivate
    {
    public:
        LoaderPrivate();
        ~LoaderPrivate();

        void setError(int errorCode, const QString errorString, int errorLine = 0);

        int m_errorCode;
        int m_errorLine;
        QString m_errorString;

        QMap<QString, Map*> m_maps;
        QHash<QString, Map*> m_images;
    };

}; // namespace Imagemap

#endif // IMAGEMAP_LOADER_P_H
