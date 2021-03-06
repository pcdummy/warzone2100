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
%{

#include "lib/framework/frame.h"
#include "lib/gamelib/parser.h"
#include "lib/sound/audio.h"
#include "lib/gamelib/anim.h"
#include "lib/framework/lexer_input.h"

static int		g_iCurAnimID = 0;
static Vector3i vecPos, vecRot, vecScale;

extern int audp_lex(void);
extern int audp_lex_destroy(void);
extern int audp_get_lineno(void);
extern char* audp_get_text(void);
extern void audp_set_extra(YY_EXTRA_TYPE user_defined);

void yyerror(const char* fmt);

%}

%name-prefix="audp_"

%union {
	float		fval;
	long		ival;
	bool            bval;
	char*		sval;
}

	/* value tokens */
%token <fval> FLOAT_T
%token <ival> INTEGER
%token <sval> QTEXT /* Text with double quotes surrounding it */

%destructor {
#ifndef WZ_OS_WIN
	// Force type checking by the compiler
	char * const s = $$;

	if (s)
		free(s);
#endif
} QTEXT

	/* keywords */
%token ONESHOT
%token LOOP
%token AUDIO
%token ANIM3DFRAMES
%token ANIM3DTRANS
%token ANIM3DFILE
%token AUDIO_MODULE
%token ANIM_MODULE
%token ANIMOBJECT

	/* rule types */
%type <bval> looping

%%

data_file:				module_file | anim_file
						;

module_file:			module_file data_list |
						data_list
						;

data_list:				audio_module | anim_module
						;

audio_header:			AUDIO_MODULE '{'
						;

audio_module:			audio_header audio_list '}' |
						audio_header  '}'
						;

audio_list:				audio_list audio_track |
						audio_track
						;
	/*
	 * unsigned int audio_SetTrackVals(const char* fileName, bool loop, unsigned int volume, unsigned int audibleRadius)
	 */

audio_track:			AUDIO QTEXT looping INTEGER INTEGER
				{
					audio_SetTrackVals($2, $3, $4, $5);
					free($2);
				}
				;

looping:			LOOP
				{ $$ = true; }
				| ONESHOT
				{ $$ = false; }
				;

anim_module_header:		ANIM_MODULE '{'
						{
						}
						;

anim_module:			anim_module_header anim_file_list   '}' |
						anim_module_header anim_config_list '}'
						/* NULL */
						;

anim_config_list:		anim_config_list anim_config |
						anim_config
						/* NULL */
						;

anim_file_list:			anim_file_list anim_file |
						anim_file
						/* NULL */
						;

anim_file:				anim_trans | anim_frames
						;

anim_config:			QTEXT INTEGER
						{
							g_iCurAnimID = $2;
							anim_SetVals( $1, $2 );
							free($1);
						}
						;

	/*
	 * anim_Create3D( char szAniFileName[], char szPieFileName[], UWORD uwFrames,
	 *					UWORD uwFrameRate, UWORD uwNumObj, char cType, UWORD uwID )
	 */

anim_trans:				ANIM3DTRANS QTEXT INTEGER INTEGER INTEGER
						{
							anim_Create3D( $2, $3, $4, $5, ANIM_3D_TRANS, g_iCurAnimID );
							free($2);
						}
						'{' anim_obj_list '}'
						{
							g_iCurAnimID++;
						}
						;

anim_frames:			ANIM3DFRAMES QTEXT INTEGER INTEGER
						{
							anim_Create3D( $2, $3, $4, 1, ANIM_3D_FRAMES, g_iCurAnimID );
							free($2);
						}
						'{'
						{
							anim_BeginScript();
						}
						anim_script '}'
						{
							anim_EndScript();
							g_iCurAnimID++;
						}
						;

anim_obj_list:			anim_obj anim_obj_list |
						anim_obj
						;

anim_obj:				ANIMOBJECT INTEGER QTEXT '{'
						{
							anim_BeginScript();
							free($3);
						}
						anim_script '}'
						{
							anim_EndScript();
						}
						;

anim_script:			anim_script anim_state |
						anim_state
						;

anim_state:				INTEGER INTEGER INTEGER INTEGER INTEGER INTEGER INTEGER INTEGER INTEGER INTEGER
						{
							vecPos.x   = $2;
							vecPos.y   = $3;
							vecPos.z   = $4;
							vecRot.x   = $5;
							vecRot.y   = $6;
							vecRot.z   = $7;
							vecScale.x = $8;
							vecScale.y = $9;
							vecScale.z = $10;
							anim_AddFrameToAnim( $1, vecPos, vecRot, vecScale );
						}
						;

%%

/* A simple error reporting routine */
void yyerror(const char* msg)
{
	debug(LOG_ERROR, "RES file parse error:\n%s at line %d\nToken: %d, Text: '%s'\n", msg, audp_get_lineno(), audp_char, audp_get_text());
}

/** Read an audio properties file
 */
bool ParseResourceFile(PHYSFS_file* fileHandle)
{
	bool retval;
	lexerinput_t input;
	input.type = LEXINPUT_PHYSFS;
	input.input.physfsfile = fileHandle;

	audp_set_extra(&input);

	retval = (audp_parse() == 0);
	audp_lex_destroy();

	return retval;
}
