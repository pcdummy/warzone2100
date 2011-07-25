#ifndef CORE_MAP_LEVPARSER_H
#define CORE_MAP_LEVPARSER_H

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QList>
#include <QtCore/QHash>
#include <QtCore/QMap>

class QIODevice;

namespace Map {

struct levelData
{
    QString path;
    QString file;
    int players;
    int type;
    QString dataset;
    QString game;
    QStringList dataFiles;
};

class LevParser
{
public:
    LevParser();

    /**
     * @brief Parses a .lev file.
     *
     * Parses a .lev file and adds its "campaign" entries to datasets
     * and the levels to map.
     *
     * The Path will be added to each Map.
     *
     * @param file      .lev filepath for QFile.
     * @param path      Path for Physfs (for mounting later)
     * @return          State.
     */
    bool parse(QString &file, const char* path);

    QHash<QString, QStringList> datasets;
    QMap<QString, levelData> maps;
};

} // namespace Parser {

#endif // CORE_MAP_LEVPARSER_H