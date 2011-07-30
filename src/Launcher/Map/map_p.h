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
 *  Internal "shared" defintions for map.cpp and the lexer.
 */

#ifndef CORE_MAP_MAP_P_H
#define CORE_MAP_MAP_P_H

#include "lib/framework/frame.h"
#include "lib/framework/lexer_input.h"

// return values from the lexer, they need to be in the same order
// as in level_lexer.l
enum _token_type
{
	LTK_LEVEL = 0x100,		// level key word
	LTK_PLAYERS,			// players key word
	LTK_TYPE,				// type key word
	LTK_DATA,				// data key word
	LTK_GAME,				// game key word
	LTK_CAMPAIGN,			// campaign key word
	LTK_CAMSTART,			// camstart key word
	LTK_CAMCHANGE,			// camchange key word
	LTK_DATASET,			// dataset key word
	LTK_EXPAND,				// expand key word
	LTK_EXPAND_LIMBO,		// expand Limbo key word
	LTK_BETWEEN,			// between key word
	LTK_MKEEP,				// miss_keep key word
	LTK_MKEEP_LIMBO,		// miss_keep Limbo key word
	LTK_MCLEAR,				// miss_clear key word
	LTK_IDENT,				// an identifier
	LTK_STRING,				// a quoted string
	LTK_INTEGER,			// a number
};

extern int lev_get_lineno(void);
extern char* lev_get_text(void);
extern int lev_lex(void);
extern void lev_set_extra(YY_EXTRA_TYPE user_defined);
extern int lev_lex_destroy(void);

extern char *lev_text;

// return values from the lexer
extern uint16_t levVal;

// error report function for the level parser
extern void lev_error(const char* msg);

#endif // CORE_MAP_MAP_P_H
