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
 * @file wzqmlview.h
 *
 * The launchers Declarative View,
 */
#ifndef __SRC_LAUNCHER_WZQMLVIEW_H__
#define __SRC_LAUNCHER_WZQMLVIEW_H__

// Qt Declarative
#include <QtDeclarative/QDeclarativeView>

extern const int LOG_FRONTEND;

namespace Frontend {

// Forwarder
class WzHelper;

class WzQMLViewPrivate;
class WzQMLView : public QDeclarativeView
{
	Q_OBJECT

public:
	explicit WzQMLView(QWidget* parent = 0);
	virtual ~WzQMLView();

	void run(const QString mainScreen = QString(), const QString mainMenu = QString());

	const QStringList& getAvailableResolutions() const;

private:
	WzQMLViewPrivate* d;
	WzHelper* wzhelper;
};

} // namespace Frontend {

#endif // #ifndef __SRC_LAUNCHER_WZQMLVIEW_H__
