/*
    This file is part of Warzone 2100.
    Copyright (C) 2011  Warzone 2100 Project

    Warzone 2100 is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Warzone 2100 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Warzone 2100; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/
#ifndef __SRC_LAUNCHER_FILESYSTEM_QPHYSFSENGINEHANDLER_H__
#define __SRC_LAUNCHER_FILESYSTEM_QPHYSFSENGINEHANDLER_H__

#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtCore/QAbstractFileEngine>
#include <physfs.h>

#include <src/Launcher/Filesystem/filesystem.h>

// For logging
#include <lib/framework/frame.h>

namespace FileSystem {

class QPhysfsEngine : public QAbstractFileEngine
{
private:
    PHYSFS_file     *fp;
    FileFlags       flags;
    QString         name;

    void realSetFileName(const QString &file)
    {
        name = file;
        
        // Show potential until actually opened
        flags = QAbstractFileEngine::ReadOtherPerm |
                QAbstractFileEngine::WriteOwnerPerm |
                QAbstractFileEngine::ReadOwnerPerm |
                QAbstractFileEngine::ReadUserPerm |
                QAbstractFileEngine::WriteUserPerm;

        if (PHYSFS_exists(name.toUtf8().constData()))
        {
            flags |= QAbstractFileEngine::ExistsFlag;
        }

        if (PHYSFS_isDirectory(name.toUtf8().constData()))
        {
            flags |= QAbstractFileEngine::DirectoryType;
        }

        // No translator for symlinks -> realname so disabled.
        // if (PHYSFS_isSymbolicLink(name.toUtf8().constData())) flags |= QAbstractFileEngine::LinkType;
        if (!flags & QAbstractFileEngine::DirectoryType)
        {
            flags |= QAbstractFileEngine::FileType;
        }
    }

public:
    QPhysfsEngine(QString filename) :
        fp(NULL),
        flags(0),
        name(filename)
    {
        realSetFileName(filename);
    }

    ~QPhysfsEngine()
    {
        if (fp) PHYSFS_close(fp);
    }

    bool caseSensitive() const
    {
        return true;
    }

    bool close()
    {
        if (fp)
        {
            int retval = PHYSFS_close(fp);
            fp = NULL;
            return retval != 0;
        }
        else
        {
            return true;
        }
    }

    QFile::FileError error() const
    {
        return QFile::NoError;
    }

    QString errorString() const
    {
        return QString(PHYSFS_getLastError());
    }

    FileFlags fileFlags(FileFlags type = FileInfoAll) const
    {
        return type & flags;
    }

    QDateTime fileTime(FileTime time) const
    {
        if (time == QAbstractFileEngine::ModificationTime)
        {
            return QDateTime::fromTime_t(PHYSFS_getLastModTime(name.toUtf8().constData()));
        }
        else
        {
            return QDateTime();
        }
    }

    bool flush()
    {
        return PHYSFS_flush(fp) != 0;
    }

    bool isRelativePath() const
    {
        return true;    // in physfs, all paths are relative
    }

    bool isSequential() const
    {
        return true;
    }

    bool mkdir(const QString & dirName, bool createParentDirectories) const { Q_UNUSED(createParentDirectories); return PHYSFS_mkdir(dirName.toUtf8().constData()) != 0; }
    qint64 pos() const
    {
        return PHYSFS_tell(fp);
    }

    qint64 read(char *data, qint64 maxlen)
    {
        return PHYSFS_read(fp, data, 1, maxlen);
    }

    bool remove() { return PHYSFS_delete(name.toUtf8().constData()) != 0; }
    bool rmdir(const QString & dirName, bool recurseParentDirectories) const { Q_UNUSED(recurseParentDirectories); return PHYSFS_delete(name.toUtf8().constData()) != 0; }
    bool seek(qint64 offset)
    {
        return PHYSFS_seek(fp, offset) != 0;
    }

    bool extension(QAbstractFileEngine::Extension extension, const QAbstractFileEngine::ExtensionOption* option, QAbstractFileEngine::ExtensionReturn* output)
    {
        Q_UNUSED(option);
        Q_UNUSED(output);

        if (extension == AtEndExtension)
        {
            return PHYSFS_eof(fp) != 0;
        }

        return false;
    }

    bool supportsExtension(Extension extension) const
    {
        return extension == AtEndExtension;
    }

    qint64 write(const char *data, qint64 len) { return PHYSFS_write(fp, data, 1, len); }

    qint64 size() const
    {
        if (!fp)
        {
            if (!PHYSFS_exists(name.toUtf8().constData())) return 0;
            PHYSFS_file *tmp;
            tmp = PHYSFS_openRead(name.toUtf8().constData());
            if (!tmp)
            {
                wzLog(LOG_ERROR) << QString("Failed to open %1 for size info: %2")
                                    .arg(name).arg(PHYSFS_getLastError());
                return -1;
            }
            int retval = PHYSFS_fileLength(tmp);
            PHYSFS_close(tmp);
            return retval;
        }
        return PHYSFS_fileLength(fp);
    }

    void setFileName(const QString& file)
    {
        realSetFileName(file);
    }

    bool open(QIODevice::OpenMode mode)
    {
        // close();
        if (mode & QIODevice::WriteOnly)
        {
            flags = QAbstractFileEngine::WriteOwnerPerm | QAbstractFileEngine::WriteUserPerm | QAbstractFileEngine::FileType;
            fp = PHYSFS_openWrite(name.toUtf8().constData());       // will truncate
        }
        else if (mode & QIODevice::ReadOnly)
        {
            flags = QAbstractFileEngine::ReadOwnerPerm | QAbstractFileEngine::ReadUserPerm | QAbstractFileEngine::FileType | QAbstractFileEngine::ReadOtherPerm;
            fp = PHYSFS_openRead(name.toUtf8().constData());
        }
        else if (mode & QIODevice::Append)
        {
            flags = QAbstractFileEngine::WriteOwnerPerm | QAbstractFileEngine::WriteUserPerm | QAbstractFileEngine::FileType;
            fp = PHYSFS_openAppend(name.toUtf8().constData());
        }
        else
        {
            wzLog(LOG_ERROR) << "Bad file open mode: " << mode;
        }

#ifdef DEBUG
        if (!fp)
        {
            wzLog(LOG_ERROR) << "Failed to open" << name
                             << "with mode:" << mode
                             << "error:" << PHYSFS_getLastError();
        }
#endif

        return fp != NULL;
    }

    QString fileName(FileName file = DefaultName) const
    {
        switch(file)
        {
            case DefaultName:
            case AbsoluteName:
            case CanonicalName:
                return "wz::" + name;
            break;

            case BaseName:
                return name.right(name.lastIndexOf("/"));
            break;

            case PathName:
            case AbsolutePathName:
            case CanonicalPathName:
                return "wz::" + name.left(name.lastIndexOf("/"));
            break;

            default:
                return "";
        }

//         if (file == QAbstractFileEngine::AbsolutePathName)
//         {
//             return PHYSFS_getWriteDir(); // hack for QSettings
//         }

    }
};

class QPhysfsEngineHandler : public QAbstractFileEngineHandler
{
public:
    QPhysfsEngineHandler(const char* prefix)
    {
        m_prefix = prefix;
    }

    inline QAbstractFileEngine *create(const QString &fileName) const
    {
        if (fileName.toLower().startsWith(m_prefix))
        {
            QString newPath = fileName;
            return new QPhysfsEngine(newPath.remove(0, 4));
        }
        return NULL;
    }

private:
    QString m_prefix;
};

} // namespace FileSystem {

#endif // #ifndef __SRC_LAUNCHER_FILESYSTEM_QPHYSFSENGINEHANDLER_H__