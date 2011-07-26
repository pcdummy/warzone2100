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
 * @file configuration.cpp
 *
 * Configuration handler.
 */

// Self
#include <src/Launcher/confighandler.h>

// Qt Core
#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariantMap>
#include <QtCore/QFile>

//Logger
#include <lib/framework/frame.h>

static const int LOG_CONFIG = Logger::instance().addLoggingLevel("config", false);

// Improve me.
static const QVariant emptyQVariant;

struct CONFIGOPTION
{
    CONFOPTION_TYPES type;
    bool storeUserConf;
    QVariant defaultValue;
    qint32 maxValue;
};

class ConfigHandlerPrivate
{
public:
    bool add(const QString &key, CONFOPTION_TYPES type, bool storeUserConf, QVariant &defaultValue, qint32 maxValue = 0)
    {
        if (options.contains(key))
        {
            return false;
        }

        CONFIGOPTION option = {type, storeUserConf, defaultValue, maxValue};
        options.insert(key, option);

        engineConfig.insert(key, defaultValue);
        if (storeUserConf)
        {
            userConfig.insert(key, defaultValue);
        }

        return true;
    }

    QHash<QString, CONFIGOPTION> options;
    QVariantMap engineConfig;
    QVariantMap userConfig;
};

ConfigHandler::ConfigHandler()
{
    m_d = new ConfigHandlerPrivate();
}

ConfigHandler::~ConfigHandler()
{
    delete m_d;
}

bool ConfigHandler::loadConfig(const QString& filename)
{
    wzLog(LOG_CONFIG) << "Loading config:" << filename;
    
    QSettings configfile(filename, QSettings::IniFormat);
    
    foreach(QString key, configfile.childKeys())
    {
        set(key, configfile.value(key));
    }
    
    return true;
}

bool ConfigHandler::storeConfig(const QString& filename, CONFIGCONTEXT context)
{
    QVariantMap config;
    
    switch (context)
    {
        case CONFCONTEXT_USER:
            wzLog(LOG_CONFIG) << "Saving user config:" << filename;
            config = m_d->userConfig;
        break;

        case CONFCONTEXT_ENGINE:
            wzLog(LOG_CONFIG) << "Saving engine config:" << filename;
            config = m_d->engineConfig;
        break;
    }

    QSettings configfile(filename, QSettings::IniFormat);
    
    for (QVariantMap::iterator it = config.begin();
         it != config.end(); ++it)
    {
        configfile.setValue(it.key(), it.value());
    }

    return true;
}

bool ConfigHandler::add(const QString key, CONFOPTION_TYPES type, bool storeUserConf, QVariant defaultValue, qint32 maxValue)
{
    return m_d->add(key, type, storeUserConf, defaultValue, maxValue);
}

Q_INVOKABLE bool ConfigHandler::set(const QString key, QVariant value, bool store)
{
    CONFIGOPTION confItem = m_d->options[key];

    // Type check
    switch(confItem.type)
    {
        case CONFTYPE_ERROR:
            wzLog(LOG_ERROR) << "Unknow config key: " << key;
            return false;
        break;
        
        case CONFTYPE_INT:
            if (!value.convert(QVariant::Int))
            {
                wzLog(LOG_ERROR) << key << "must be a int, not:" << value;
                return false;
            }
        break;

        case CONFTYPE_BOOL:
            if (!value.convert(QVariant::Bool))
            {
                wzLog(LOG_ERROR) << key << "must be a bool, not:" << value;
                return false;
            }
        break;

        case CONFTYPE_ENUM: // Also checks bounds
            if (!value.convert(QVariant::Int) || value.toInt() > confItem.maxValue)
            {
                wzLog(LOG_ERROR) << key << "must be a int, not:" << value;
                return false;
            }
        break;

        case CONFTYPE_STRING:
            if (!value.convert(QVariant::String))
            {
                wzLog(LOG_ERROR) << key << "must be a string, not:" << value;
                return false;
            }
        break;

        case CONFTYPE_STRINGLIST:
            if (!value.convert(QVariant::StringList))
            {
                wzLog(LOG_ERROR) << key << "must be a stringlist, not:" << value;
                return false;
            }
        break;
    }

    m_d->engineConfig.insert(key, value);

    if (store && confItem.storeUserConf)
    {
        // Store in user conf.
        m_d->userConfig.insert(key, value);
        
#ifdef DEBUG
        wzLog(LOG_CONFIG) << "Setting: " << key << "=" << value << "with userconfig";
#endif
    }
#ifdef DEBUG
    else
    {
        wzLog(LOG_CONFIG) << "Setting: " << key << "=" << value << "w/o userconfig";
    }
#endif


    return true;
}

Q_INVOKABLE QVariant ConfigHandler::get(const QString key)
{
    CONFIGOPTION confItem = m_d->options[key];

    if (confItem.type == CONFTYPE_ERROR)
    {
        wzLog(LOG_ERROR) << "Unknow config key: " << key;
        return emptyQVariant;
    }
    
    return m_d->engineConfig[key];
}

// Declared "extern" in configuration.h
ConfigHandler config;
