// Self
#include <src/Launcher/Map/levparser.h>

// Qt Core
#include <QtCore/QHash>
#include <QtCore/QRegExp>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

// For LOG_LMAP
#include <src/Launcher/Map/map.h>

#include <lib/framework/frame.h> // Logging

namespace Map {

enum LEVEL_IDENT_TYPE
{
    IDENT_TYPE_DATASET = 1,
    IDENT_TYPE_LEVEL,
};

enum LEVEL_TYPES
{
    LEVEL_PLAYERS = 1,
    LEVEL_TYPE,
    LEVEL_DATASET,
    LEVEL_GAME,
    LEVEL_DATA
};

static QHash<QString, int> levelIdentTokens;
static QHash<QString, int> levelTypesTokens;
static QString datasetDataKey = "data";

static QRegExp levelRx("([a-z]+)\\s*\"?([\\w-_/.]+)\"?");

static inline QStringList readLevLine(QIODevice* io)
{
    char buff[1024];

    io->readLine(buff, sizeof(buff));
    QString line = QString::fromUtf8(buff);
    //Skip empty lines and comments
    if (line.isEmpty() ||
        line.at(0) == QChar('\n') ||
        line.at(0) == QChar('/') ||
        line.at(1) == QChar('*'))
    {
        return QStringList();
    }

    int pos = levelRx.indexIn(line);
    if (pos == -1)
    {
        return QStringList();
    }

    return levelRx.capturedTexts();
}

static qint64 parseDataset(QIODevice* io, QStringList& dataset, qint64 line)
{
    for (;;)
    {
        QStringList data = readLevLine(io);
        line++;

        if (data.isEmpty())
        {
            break;
        }

        if (data.at(1) == datasetDataKey)
        {
            dataset.append(data.at(2));
        }
        else
        {
            wzLog(LOG_ERROR) << QString("Line %1, unknown dataset token: %2")
                                            .arg(line).arg(data.at(1));
            return -1;
        }
    }

    return line;
}

static qint64 parseLevel(QIODevice *io, levelData &level, qint64 line)
{
    for (;;)
    {
        QStringList data = readLevLine(io);
        line++;

        if (data.isEmpty())
        {
            break;
        }

        int tokenId = levelTypesTokens.value(data.at(1));
        switch(static_cast<LEVEL_TYPES>(tokenId))
        {
            case LEVEL_PLAYERS:
                level.players = data.at(2).toInt();
            break;
            case LEVEL_TYPE:
                level.type = data.at(2).toInt();
            break;
            case LEVEL_DATASET:
                level.dataset = data.at(2);
            break;
            case LEVEL_GAME:
                level.game = data.at(2);
            break;
            case LEVEL_DATA:
                level.dataFiles.append(data.at(2));
            break;
            default:
                wzLog(LOG_ERROR) << QString("Line %1, unknown level token: %2")
                                                    .arg(line).arg(data.at(1));
        }
    }

    return line;
}

// FIXME: This should be static somewhere.
LevParser::LevParser()
{
    // Needs to be in exact the same order as the enum LEVEL_IDENT_TYPE
    if (levelIdentTokens.isEmpty())
    {
        levelIdentTokens.insert("campaign", 1);
        levelIdentTokens.insert("level", 2);
    }

    // Needs to be in exact the same order as the enum LEVEL_TYPE
    if (levelTypesTokens.isEmpty())
    {
        levelTypesTokens.insert("players", 1);
        levelTypesTokens.insert("type", 2);
        levelTypesTokens.insert("dataset", 3);
        levelTypesTokens.insert("game", 4);
        levelTypesTokens.insert("data", 5);
    }
}

bool LevParser::parse(QString &filename, const char* path)
{
    qint64 line = 0;

    QFile file(filename);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        // Failed to open file
        wzLog(LOG_ERROR) << QString("Failed to open %1")
                                        .arg(filename);
        return false;
    }

    wzLog(LOG_LMAP) << QString("Parsing %1").arg(filename);

    while (!file.atEnd())
    {
        QStringList data = readLevLine(&file);
        line++;

        if (data.isEmpty())
        {
            continue;
        }

        int tokenId = levelIdentTokens.value(data.at(1));
        if (tokenId == 0)
        {
            // Invalid Token received.
            continue;
        }

        QStringList dataset;
        levelData level;
        switch(static_cast<LEVEL_IDENT_TYPE>(tokenId))
        {
            case IDENT_TYPE_DATASET:
                line = parseDataset(&file, dataset, line);
                if (line == -1) {
                    return false;
                }
                datasets.insert(data.at(2), dataset);
            break;

            case IDENT_TYPE_LEVEL:
                line = parseLevel(&file, level, line);
                if (line == -1) {
                    return false;
                }
                level.path = path;
                level.file = QFileInfo(filename).fileName();

                maps.insert(data.at(2), level);
            break;
            default:
                qDebug() << QString("Line %1, unknown token: %2")
                                .arg(line).arg(data.at(1));
                return false;
        }
    }

    return true;
}

} // namespace Map {
