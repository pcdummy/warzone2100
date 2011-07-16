/*
    This file is part of Warzone 2100.
    Copyright (C) 1999-2004  Eidos Interactive
    Copyright (C) 2005-2011  Warzone 2100 Project

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

// Self
#include "netlobby.h"

// Qt Core
#include <QtCore/QtEndian>
#include <QtCore/QFile>

// QT Network
#if !defined(NO_SSL)
#  include <QtNetwork/QSslCertificate>
#endif

// QJson
#include <qjson/parser.h>
#include <qjson/serializer.h>

// Framework (for MSVC and i18n and logging)
#include "lib/framework/frame.h"

const int LOG_LOBBY = Logger::instance().addLoggingLevel("lobby", false);

namespace Lobby
{
Client::Client()
{
    // Set defaults
    gameId_ = 0;

    lastError_.code = LOBBY_NO_ERROR;

    isAuthenticated_ = false;
    useSSL_ = false;
    useAuth_ = false;
}

void Client::stop()
{
    wzLog(LOG_NET) << "Lobby client: Stopping";

    // remove the game from the masterserver,
    delGame();
    freeError();

    // and disconnect.
    disconnect();

    delete socket_;
}

RETURN_CODES Client::addGame(char** result, const qint32 port, const qint32 maxPlayers,
                             const char* description, const char* versionstring,
                             const qint32 game_version_major, const qint32 game_version_minor,
                             const bool isPrivate, const char* modlist,
                             const char* mapname, const char* hostplayer)
{
    if (gameId_ != 0)
    {
        // Ignore server errors here.
        if (delGame() != LOBBY_NO_ERROR)
        {
            freeError();
        }
    }

    QVariantMap kwargs;
    kwargs.insert("port", port);
    kwargs.insert("description", description);
    kwargs.insert("currentPlayers", 1);
    kwargs.insert("maxPlayers", maxPlayers);
    kwargs.insert("multiVer", versionstring);
    kwargs.insert("wzVerMajor", game_version_major);
    kwargs.insert("wzVerMinor", game_version_minor);
    kwargs.insert("private", isPrivate);
    kwargs.insert("modlist", modlist);
    kwargs.insert("mapname", mapname);
    kwargs.insert("hostplayer", hostplayer);

    if (call_("addGame", kwargs) != LOBBY_NO_ERROR)
    {
        return lastError_.code;
    }

    QVariantMap resultMap = callResult_.result.toMap();
    if (!resultMap.contains("gameId") || !resultMap.contains("result"))
    {
        freeCallResult_();
        return setError_(INVALID_DATA, _("Received invalid addGame data."));
    }

    gameId_ = resultMap["gameId"].toLongLong();
    // asprintf(result, resultMap["result"].toString().toUtf8().constData());

    freeCallResult_();
    return LOBBY_NO_ERROR;
}

RETURN_CODES Client::delGame()
{
    if (gameId_ == 0)
    {
        return LOBBY_NO_ERROR;
    }

    QVariantMap kwargs;
    kwargs.insert("gameId", gameId_);

    if (call_("delGame", kwargs) != LOBBY_NO_ERROR)
    {
        // Ignore a server side error and unset the local game.
        gameId_ = 0;
        return lastError_.code;
    }

    gameId_ = 0;

    freeCallResult_();
    return LOBBY_NO_ERROR;
}

RETURN_CODES Client::addPlayer(const unsigned int index, const char* name,
                               const char* username, const char* session)
{
    if (gameId_ == 0 || gameId_ == -1)
    {
        return setError_(NO_GAME, _("Create a game first!"));
    }

    QVariantMap kwargs;
    kwargs.insert("gameId", gameId_);
    kwargs.insert("slot", index);
    kwargs.insert("name", name);
    kwargs.insert("username", username);
    kwargs.insert("session", session);

    if (call_("addPlayer", kwargs) != LOBBY_NO_ERROR)
    {
        return lastError_.code;
    }

    freeCallResult_();
    return LOBBY_NO_ERROR;
}

RETURN_CODES Client::delPlayer(const unsigned int index)
{
    if (gameId_ == 0 || gameId_ == -1)
    {
        return setError_(NO_GAME, _("Create a game first!"));
    }

    QVariantMap kwargs;
    kwargs.insert("gameId", gameId_);
    kwargs.insert("slot", index);

    if (call_("delPlayer", kwargs) != LOBBY_NO_ERROR)
    {
        return lastError_.code;
    }

    freeCallResult_();
    return LOBBY_NO_ERROR;
}

RETURN_CODES Client::updatePlayer(const unsigned int index, const char* name)
{
    if (gameId_ == 0 || gameId_ == -1)
    {
        return setError_(NO_GAME, _("Create a game first!"));
    }

    QVariantMap kwargs;
    kwargs.insert("gameId", gameId_);
    kwargs.insert("slot", index);
    kwargs.insert("name", name);

    if (call_("updatePlayer", kwargs) != LOBBY_NO_ERROR)
    {
        return lastError_.code;
    }

    freeCallResult_();
    return LOBBY_NO_ERROR;
}

RETURN_CODES Client::listGames(const int maxGames)
{
    qint32 gameCount = 0;
    GAME game;

    // Clear old games.
    games.clear();

    // Run "list" and retrieve its result
    if (call_("list") != LOBBY_NO_ERROR)
        return lastError_.code;

    QVariantList resultList = callResult_.result.toList();
    for (int i = 0; i < resultList.size() && gameCount < maxGames; ++i)
    {
        // FIXME: should clear "game"" here or initialize a new one each loop.
        QVariantMap rawGame = resultList.at(i).toMap();

        if (rawGame.contains("port"))
        {
            game.port = rawGame["port"].toInt();
        }
        if (rawGame.contains("host"))
        {
            game.host = rawGame["host"].toString().toStdString();
        }
        if (rawGame.contains("description"))
        {
            game.description = rawGame["description"].toString().toStdString();
        }
        if (rawGame.contains("currentPlayers"))
        {
            game.currentPlayers = rawGame["currentPlayers"].toInt();
        }
        if (rawGame.contains("maxPlayers"))
        {
            game.maxPlayers = rawGame["maxPlayers"].toInt();
        }
        if (rawGame.contains("multiVer"))
        {
            game.versionstring = rawGame["multiVer"].toString().toStdString();
        }
        if (rawGame.contains("wzVerMajor"))
        {
            game.game_version_major = rawGame["wzVerMajor"].toInt();
        }
        if (rawGame.contains("wzVerMinor"))
        {
            game.game_version_minor = rawGame["wzVerMinor"].toInt();
        }
        if (rawGame.contains("isPrivate"))
        {
            game.isPrivate = rawGame["isPrivate"].toInt();
        }
        if (rawGame.contains("modlist"))
        {
            game.modlist = rawGame["modlist"].toInt();
        }
        if (rawGame.contains("mapname"))
        {
            game.mapname = rawGame["mapname"].toString().toStdString();
        }
        if (rawGame.contains("hostplayer"))
        {
            game.hostplayer = rawGame["hostplayer"].toString().toStdString();
        }

        games.append(game);
        gameCount++;
    }

    freeCallResult_();
    return LOBBY_NO_ERROR;
}

inline bool Client::isConnected()
{
    return (socket_ && socket_->state() == QAbstractSocket::ConnectedState);
}

Client& Client::addCACertificate(const QString& path)
{
#if defined(NO_SSL)
    wzLog(LOG_NET) << "Cannot add an SSL Certificate as SSL is not compiled in.";
    return *this;
#else
    wzLog(LOG_NET) << QString("Adding the CA certificate %1.").arg(path);

    QFile cafile(path);
    if (!cafile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        wzLog(LOG_ERROR) << QString("Cannot open the CA certificate %1!")
                                    .arg(path);
        return *this;
    }

    QSslCertificate certificate(&cafile, QSsl::Pem);
    if (!certificate.isValid())
    {
        wzLog(LOG_ERROR) << QString("Failed to load the CA certificate %1!")
                                    .arg(path);
        return *this;
    }
    cafile.close();

    wzLog(LOG_NET) << QString("Cert common name: %1")
                        .arg(certificate.subjectInfo(QSslCertificate::CommonName));

    cacerts_.append(certificate);

    return *this;
#endif
}

RETURN_CODES Client::connect()
{
    if (isConnected())
    {
        return LOBBY_NO_ERROR;
    }

    isAuthenticated_ = false;

    callId_ = 0;

#if defined(NO_SSL)
    socket_ = new QTcpSocket();
    socket_->connectToHost(host_, port_);
#else
    socket_ = new QSslSocket();
    // Connect
    if (useSSL_ == false)
    {
        wzLog(LOG_NET) << QString("Connecting to \"[%1]:%2\".")
                            .arg(host_).arg(port_);
        socket_->connectToHost(host_, port_);
    }
    else
    {
        wzLog(LOG_NET) << QString("Connecting to \"[%1]:%2\" with SSL enabled.")
                            .arg(host_).arg(port_);
        socket_->setCaCertificates(cacerts_);
        socket_->connectToHostEncrypted(host_, port_);
    }
#endif

#if defined(NO_SSL)
    if (!socket_->waitForConnected())
    {
#else
    if ((useSSL_ && !socket_->waitForEncrypted())
            || (!useSSL_ && !socket_->waitForConnected()))
    {
#endif
        wzLog(LOG_ERROR) << QString("Cannot connect to lobby \"[%1]:%2\": %3.")
                                    .arg(host_).arg(port_).arg(socket_->errorString());
        return setError_(TRANSPORT_ERROR, "");
    }

    // Build the initial version command.
    uchar buf[sizeof(qint32)] = { "\0" };
    qToBigEndian<qint32>(PROTOCOL, buf);
    QByteArray version;
    version.append("version", sizeof("version"));
    version.append((char *)buf, sizeof(buf));

    // Send Version command
    if (socket_->write(version) == -1)
    {
        wzLog(LOG_ERROR) << QString("Cannot send version string to lobby \"[%1]:%2\": %3.")
                                    .arg(host_).arg(port_).arg(socket_->errorString());
        return setError_(TRANSPORT_ERROR, "");
    }

    // At last try to login
    return login("");
}

RETURN_CODES Client::login(const QString& password)
{
    if (isAuthenticated_ == true)
    {
        return LOBBY_NO_ERROR;
    }
    else if (useAuth_ == false)
    {
        wzLog(LOG_NET) << "Authentication is disabled.";
        return LOBBY_NO_ERROR;
    }

    QVariantMap kwargs;
    kwargs.insert("username", user_);

    if (!token_.isEmpty())
    {
        kwargs.insert("token", token_);
    }
    else if (!password.isEmpty())
    {
        token_.clear();
        kwargs.insert("password", password);
    }
    else
    {
        wzLog(LOG_NET) << "Not trying to login no password/token supplied.";

        // Do not return an error for internal use.
        return LOBBY_NO_ERROR;
    }

    if (call_("login", kwargs) != LOBBY_NO_ERROR)
    {
        // Reset login credentials on a wrong login
        if (lastError_.code == WRONG_LOGIN)
        {
            wzLog(LOG_NET) << "Lobby Login failed!";
            token_.clear();
            session_.clear();
            isAuthenticated_ = false;
        }
        return lastError_.code;
    }

    token_.clear();
    session_.clear();

    QVariantMap resultMap = callResult_.result.toMap();
    if (!resultMap.contains("token") || !resultMap.contains("session"))
    {
        wzLog(LOG_NET) << "Lobby Login failed!";
        freeCallResult_();
        return setError_(INVALID_DATA, _("Received invalid login data."));
    }

    token_ = resultMap["token"].toString();
    session_ = resultMap["session"].toString();

    wzLog(LOG_NET) << QString("Received Session \"%1\"").arg(session_);

    isAuthenticated_ = true;

    freeCallResult_();
    return LOBBY_NO_ERROR;
}

RETURN_CODES Client::logout()
{
    // Remove a maybe hosted game.
    if (delGame() != LOBBY_NO_ERROR)
    {
        freeError();
    }

    // Tell the lobby that we want to logout.
    if (call_("logout") != LOBBY_NO_ERROR)
    {
        return lastError_.code;
    }

    // Clear auth data.
    isAuthenticated_ = false;
    user_.clear();
    token_.clear();
    session_.clear();

    freeCallResult_();
    return LOBBY_NO_ERROR;
}

bool Client::disconnect()
{
    if (!isConnected())
    {
        return true;
    }

    socket_->close();

    // Rest call id.
    callId_ = 0;

    // clear/free up games,
    games.clear();

    // clear auth data,
    session_.clear();
    isAuthenticated_ = false;

    return true;
}

RETURN_CODES Client::call_(const char* command, const QVariantMap& kwargs)
{
    // Connect to the lobby
    if (isConnected() != true && connect() != LOBBY_NO_ERROR)
    {
        return lastError_.code;
    }

    wzLog(LOG_NET) << QString("Calling \"%1\" on the lobby").arg(command);

    callId_ += 1;
    QVariantList callArgs;
    callArgs << command << QVariantMap() << kwargs << callId_;

    QVariantMap call;
    call.insert("call", callArgs);

    // Debug - needs QtCore/QDebug
    //qDebug() << call;

    QJson::Serializer serializer;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out.setByteOrder(QDataStream::BigEndian);
    out << serializer.serialize(call);

    if (socket_->write(block) == -1)
    {
        wzLog(LOG_ERROR) << QString("Failed sending command \"%1\" to the lobby: %2.")
                                    .arg(command).arg(socket_->errorString());
        return setError_(TRANSPORT_ERROR, _("Failed to communicate with the lobby."));
    }

    quint32 blockSize;
    QDataStream in(socket_);
    in.setVersion(QDataStream::Qt_4_0);
    in.setByteOrder(QDataStream::BigEndian);
    while (socket_->bytesAvailable() < sizeof(quint16))
    {
        if (!socket_->waitForReadyRead())
        {
            wzLog(LOG_ERROR) << QString("Failed reading the results size for \"%1\" to the lobby: %2.")
                                        .arg(command).arg(socket_->errorString());
            return setError_(TRANSPORT_ERROR, _("Failed to communicate with the lobby."));
        }

        in >> blockSize;

        while (socket_->bytesAvailable() < blockSize)
        {
            if (!socket_->waitForReadyRead())
            {
                wzLog(LOG_ERROR) << QString("Failed reading the result for \"%1\" to the lobby: %2.")
                                            .arg(command).arg(socket_->errorString());
                return setError_(TRANSPORT_ERROR, _("Failed to communicate with the lobby."));
            }
        }
    }

    // FIXME: Lots copying here.
    char* buffer = new char[blockSize];
    in.readRawData(buffer, blockSize); // needed for "\0".
    QByteArray jsonData(buffer, blockSize);
    delete buffer;

    QJson::Parser parser;
    bool ok;
    QVariantMap result = parser.parse(jsonData, &ok).toMap();
    if (!ok)
    {
        wzLog(LOG_ERROR) << QString("Received an invalid JSON Object, line %1, error: %2")
                                    .arg(parser.errorLine()).arg(parser.errorString());
        return setError_(INVALID_DATA, _("Failed to communicate with the lobby."));
    }

    // Debug - needs QtCore/QDebug
    // qDebug() << result;

    if (!result.contains("reply"))
    {
        wzLog(LOG_ERROR) << "Received an invalid answer, no \"reply\" received.";
        return setError_(INVALID_DATA, _("Failed to communicate with the lobby."));
    }

    QVariantList resultList = result["reply"].toList();
    if (!resultList.size() == 3)
    {
        wzLog(LOG_ERROR) << QString("Received an invalid Answer %1 number of list args instead of 3")
                                    .arg(resultList.size());
        return setError_(INVALID_DATA, _("Failed to communicate with the lobby."));
    }

    if (resultList.at(0).toInt() != LOBBY_NO_ERROR)
    {
        wzLog(LOG_NET) << QString("Received: server error %1: %2")
                            .arg(resultList.at(0).toInt()).arg(resultList.at(1).toString());

        setError_(callResult_.code,
                  _("Got server error: %s"),
                  resultList.at(1).toString().toUtf8().constData());
        return lastError_.code;
    }

    callResult_.code = (RETURN_CODES)resultList.at(0).toInt();
    callResult_.result = resultList.at(1);

    return LOBBY_NO_ERROR;
}

RETURN_CODES Client::setError_(const RETURN_CODES code, const char* message, ...)
{
    char *buff;

    va_list ap;
    va_start(ap, message);
    vasprintf(&buff, message, ap);
    va_end(ap);

    lastError_.code = code;
    lastError_.message = buff;

    free(buff);

    return code;
}

} // namespace Lobby
