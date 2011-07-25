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
/**
 * @file configuration.h
 *
 * Configuration handlers.
 */
#ifndef __SRC_LAUNCHER_CONFIGHANDLER_H__
#define __SRC_LAUNCHER_CONFIGHANDLER_H__

#include <QtCore/QString>
#include <QtCore/QSettings>
#include <QtCore/QVariantMap>

enum CONFOPTION_TYPES
{
    CONFTYPE_ERROR,
    CONFTYPE_INT,
    CONFTYPE_BOOL,
    CONFTYPE_STRING,
    CONFTYPE_ENUM // int bounds checked
};

class ConfigHandlerPrivate;
class ConfigHandler
{
public:
    /**
     * @brief Constructor initializes defaults.
     */
    ConfigHandler();

    /**
     * @brief Destructor cleans this up especialy: m_userConfig and m_engineConfig.
     */
    ~ConfigHandler();

    /**
     * @brief Reads and validates the users configuration.
     *
     * Reads the users ini configuration from the given path,
     * e.g.: ~/.warzone2100-3.0/config
     *
     * And validates it.
     *
     * @param filename      Users ini config path.
     *
     * @return Success/Failure
     */
    bool loadUserConfig(const QString &filename);

    /**
     * @brief Stores the users configuration to the given path.
     *
     * @param filename      Users ini config path.
     *
     * @return Success/Failure
     */
    bool storeUserConfig(const QString &filename);

    bool loadEngineConfig(const QString &filename);
    bool storeEngineConfig(const QString &filename);

    bool add(const QString key, CONFOPTION_TYPES type, bool storeUserConf, QVariant defaultValue, qint32 maxValue = 0);

    bool set(const QString key, QVariant value, bool store = true);
    const QVariant& get(const QString key);

private:
    ConfigHandlerPrivate* m_d;
};

// Global config holder.
extern ConfigHandler config;

#endif // #ifndef __SRC_LAUNCHER_CONFIGHANDLER_H__
