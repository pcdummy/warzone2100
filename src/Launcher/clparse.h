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
/** @file
 *  All the command line values
 */

#ifndef __LAUNCHER_CLPARSE_H__
#define __LAUNCHER_CLPARSE_H__

// Vars that gets set by the funcs below.
extern bool cmdShowVersion;
extern bool cmdDoHostlaunch;

// parse the commandline
bool ParseCommandLine(int argc, const char** argv);
bool ParseCommandLineEarly(int argc, const char** argv);

#endif // __LAUNCHER_CLPARSE_H__
