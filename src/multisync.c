/*
	This file is part of Warzone 2100.
	Copyright (C) 1999-2004  Eidos Interactive
	Copyright (C) 2005-2007  Warzone Resurrection Project

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
/*
 * MultiSync.c
 *
 * synching issues
 * This file handles the constant backstream of net info, checking that recvd info
 * is concurrent with the local world, and correcting as required. Magic happens here.
 *
 * All conflicts due to non-guaranteed messaging are detected/resolved here.
 *
 * Alex Lee, pumpkin Studios, bath.
 */

#include "lib/framework/frame.h"
#include "lib/framework/input.h"
#include "lib/framework/strres.h"

#include "stats.h"
#include "lib/gamelib/gtime.h"
#include "map.h"
#include "objects.h"
#include "display.h"								// for checking if droid in view.
#include "order.h"
#include "action.h"
#include "hci.h"									// for byte packing funcs.
#include "display3ddef.h"							// tile size constants.
#include "console.h"
#include "geometry.h"								// for gettilestructure
#include "mapgrid.h"								// for move droids directly.
#include "lib/netplay/netplay.h"
#include "multiplay.h"
#include "frontend.h"								// for titlemode
#include "multistat.h"
#include "power.h"									// for power checks

// ////////////////////////////////////////////////////////////////////////////
// function definitions

static BOOL sendStructureCheck	(void);							//Structure
static void packageCheck		(UDWORD i, NETMSG *pMsg, DROID *pD);
static BOOL sendDroidCheck		(void);							//droids

static void highLevelDroidUpdate(DROID *psDroid,
								 UDWORD x,
								 UDWORD y,
								 UDWORD state,
								 UDWORD order,
								 BASE_OBJECT *psTarget,
								 UDWORD	numKills);


static void onscreenUpdate		(DROID *pDroid,UDWORD dam,		// the droid and its damage
								 UDWORD x, UDWORD y,			// the ideal position
								 float fx,float fy,				// the ideal fractional position
								 UWORD dir,					// direction it should facing
								 DROID_ORDER order);			// what it should be doing

static void offscreenUpdate		(DROID *pDroid,UDWORD dam,
								 UDWORD x, UDWORD y,
								 float fx,float fy,
								 UWORD dir,
								 DROID_ORDER order);

// ////////////////////////////////////////////////////////////////////////////
// Defined numeric values
#define AV_PING_FREQUENCY	45000					// how often to update average pingtimes. in approx millisecs.
#define PING_FREQUENCY		12000					// how often to update pingtimes. in approx millisecs.
#define STRUCT_FREQUENCY	700						// how often (ms) to send a structure check.
#define DROID_FREQUENCY		1000					// how ofter (ms) to send droid checks
#define POWER_FREQUENCY		10000					// how often to send power levels
#define SCORE_FREQUENCY		25000					// how often to update global score.

#define SYNC_PANIC			40000					// maximum time before doing a dirty fix.

static UDWORD				PingSend[MAX_PLAYERS];	//stores the time the ping was called.

// ////////////////////////////////////////////////////////////////////////////
// test traffic level.
static BOOL okToSend(void)
{
	//update checks	& go no further if any exceeded.
	if((NETgetRecentBytesSent() + NETgetRecentBytesRecvd()) >= game.bytesPerSec)
	{
		return FALSE;
	}

	if( NETgetRecentPacketsSent() >= game.packetsPerSec )
	{
		return FALSE;
	}

	return TRUE;
}

// ////////////////////////////////////////////////////////////////////////////
// Droid checking info. keep position and damage in sync.
BOOL sendCheck(void)
{
	UDWORD i;

	NETgetBytesSent();			// update stats.
	NETgetBytesRecvd();
	NETgetPacketsSent();
	NETgetPacketsRecvd();

	// dont send checks till all players are present.
	for(i=0;i<MAX_PLAYERS;i++)
	{
		if(isHumanPlayer(i) && ingame.JoiningInProgress[i])
		{
			return TRUE;
		}
	}
	// send Checks. note each send has it's own send criteria, so might not send anything.
	if(okToSend())
	{
		sendPing();
	}
	if(okToSend())
	{
		sendStructureCheck();
	}
	if(okToSend())
	{
		sendPowerCheck(FALSE);
	}
	if(okToSend())
	{
		sendScoreCheck();
	}
	if(okToSend())
	{
		sendDroidCheck();
	}
	return TRUE;
}

// ////////////////////////////////////////////////////////////////////////////
// pick a droid to send, NULL otherwise.
static DROID* pickADroid(void)
{
	DROID			*pD=NULL;						// current droid we're checking
	UDWORD			i;
	static UDWORD	droidnum=0;						// how far down the playerlist to go.
	static UDWORD	player=0;						// current player we're checking
	static UDWORD	maxtrys=0;

	if ( !myResponsibility(player) )				// dont send stuff that's not our problem.
	{
		player ++;									// next player next time.
		player = player%MAX_PLAYERS;
		droidnum =0;

		if(maxtrys<MAX_PLAYERS)
		{
			maxtrys++;
			return pickADroid();
		}
		else
		{
			maxtrys =0;
			return NULL;
		}
	}

	pD = apsDroidLists[player];						// get the droid to send to everyone
	for(i=0;												// CRITERIA FOR PICKADROID
			(i<droidnum)									//not yet done.
			&& (pD != NULL)									//still here.
		;i++)//
	{
		pD= pD->psNext;
	}

	if (pD == NULL)								// droid is no longer there or list end.
	{
		player ++;										// next player next time.
		player = player%MAX_PLAYERS;
		droidnum=0;

		if(maxtrys<MAX_PLAYERS)
		{
			maxtrys++;
			return pickADroid();
		}
		else
		{
			maxtrys =0;
			return NULL;
		}
	}

	droidnum++;
	maxtrys = 0;

	return pD;
}

// ///////////////////////////////////////////////////////////////////////////
// send a droid info packet.
static BOOL sendDroidCheck(void)
{
	DROID			*pD;
	NETMSG			msg;
	UDWORD			i=0,count=0;
	static UDWORD	lastSent=0;					// last time a struct was sent.
	UDWORD			toSend = 6;
	if(lastSent > gameTime)lastSent= 0;
	if((gameTime-lastSent) < DROID_FREQUENCY)	// only send a struct send if not done recently.
	{
		return TRUE;
	}

	lastSent = gameTime;
	msg.size = 0;

	while(count < toSend)	// send x droids.
	{

		pD = pickADroid();

		if(pD)
		{
			packageCheck(i,&msg,pD);
		}

		i = msg.size;
		count = count +1;
	}

	msg.type = NET_CHECK_DROID;
	NETbcast(&msg,FALSE);

	return TRUE;
}

// ////////////////////////////////////////////////////////////////////////////
// Send a Single Droid Check message
static void packageCheck(UDWORD i, NETMSG *pMsg, DROID *pD)
{
//	UDWORD packtemp;
	UWORD numkills;		//
	uint16_t direction = pD->direction * 32;	// preserve some precision

	pMsg->body[			i+0] =		(char)pD->player;
	pMsg->body[			i+1] =		(char)pD->order;		// order being executed
	NetAdd2( pMsg,		i+2,		pD->id);				// droid id
	NetAdd2( pMsg,		i+6,		pD->secondaryOrder );
	NetAdd2( pMsg,		i+10,		pD->body);				// damage points
	NetAdd2( pMsg,		i+14,		direction);			// direction

	if (pD->order == DORDER_ATTACK || pD->order == DORDER_MOVE || pD->order == DORDER_RTB || pD->order == DORDER_RTR)
	{
		NetAdd2(pMsg,	i+16,		pD->sMove.fx);			//fraction move pos
		NetAdd2(pMsg,	i+20,		pD->sMove.fy);
	}
	else
	{
		NetAdd2(pMsg,	i+16,		pD->pos.x);					//non fractional move pos
		NetAdd2(pMsg,	i+20,		pD->pos.y);
	}

	if (pD->order == DORDER_ATTACK)
	{
		NetAdd2(pMsg,	i+24,		pD->psTarget->id);		// target id
	}
	else if(pD->order == DORDER_MOVE)
	{
		NetAdd2(pMsg,	i+24,		pD->orderX);
		NetAdd2(pMsg,	i+26,		pD->orderY);
	}
	numkills = pD->experience;
	NetAdd2(pMsg,		i+28,		numkills);	// droid kills

	pMsg->size =(UWORD)( pMsg->size + 30);

	return ;
}


// ////////////////////////////////////////////////////////////////////////////
// receive a check and update the local world state accordingly
BOOL recvDroidCheck(NETMSG *m)
{
	float			fx=0,fy=0;
	UDWORD			ref,player,x = 0,y = 0,bod,target=0;//,dir;
	UWORD			dir,numkills;
	DROID_ORDER		ord;
	BOOL			onscreen;
	DROID			*pD;
	BASE_OBJECT		*psTarget=NULL;
	UDWORD			i,state, tx=0,ty=0;

	i = 0;
	while(i < m->size)
	{
		// obtain information about remote droid.
		player = m->body[i+0];
		ord = (DROID_ORDER)m->body[i+1];								// droid id.
		NetGet(m,					i+2,ref);
		NetGet(m,					i+6,state);
		NetGet(m,					i+10,bod);							// Damage update.
		NetGet(m,					i+14,dir);

		if (ord == DORDER_ATTACK || ord == DORDER_MOVE || ord == DORDER_RTB    || ord == DORDER_RTR)	// detailed position info mode
		{
			NetGet(m,				i+16,fx);
			NetGet(m,				i+20,fy);
		}
		else
		{
			NetGet(m,				i+16,x);				// dont pack since could be sending fractional vals anyway.
			NetGet(m,				i+20,y);
		}

		if (ord == DORDER_ATTACK)
		{
			NetGet(m,				i+24,target);
		}
		else if(ord == DORDER_MOVE)
		{
			NetGet(m,				i+24,tx);
			NetGet(m,				i+26,ty);
		}
		NetGet(m,					i+28,numkills);

		i = i + 30;

		//////////////////////////////////////

		if ( !(IdToDroid(ref,player,&pD)) )				// find the droid in question
		{
			NETlogEntry("Recvd Unknown droid info. val=player",0,player);
			debug( LOG_NEVER, "Received Checking Info for an unknown (As yet) droid player:%d ref:%d\n", player, ref );
			return TRUE;								//Recvd checking info for an unknown droid
		}

		if(target)
		{
			psTarget = IdToPointer(target,ANYPLAYER);	// get the target in question.
		}

		//////////////////////////////////////
		// decide how to sync it.
		if( DrawnInLastFrame(pD->sDisplay.frameNumber)
			&& (pD->sDisplay.screenX < pie_GetVideoBufferWidth())
			&& (pD->sDisplay.screenY < pie_GetVideoBufferHeight()) )  // check for onscreen
		{
			if(pD->visible[selectedPlayer])
			{
				onscreen = TRUE;						// onscreen and visible
			}
			else
			{
				onscreen = FALSE;						// onscreen, but not visible.
			}
		}else{
			onscreen = FALSE;							// not onscreen.
		}


		if(ingame.PingTimes[player] > PING_LIMIT)		// if it's a big ping then don't do a smooth move.
		{
			onscreen = FALSE;
		}

//		if( pD->lastSync > gameTime)pD->lastSync =0;
//		if(  (gameTime - pD->lastSync) > SYNC_PANIC )	// if it's been a while then jump it.
//		{
//			onscreen = FALSE;
//		}


		//////////////////////////////////////
		/// now do the update.

		if (   onscreen
		    || vtolDroid(pD))
		{
			onscreenUpdate(pD,bod,x,y,fx,fy,dir,ord);
		}
		else
		{
			offscreenUpdate(pD,bod,x,y,fx,fy,dir,ord);
		}

		//////////////////////////////////////
		// now make note of how accurate the world model is for this droid.	// if droid is close then remember.
		if(    abs(x-pD->pos.x)<(TILE_UNITS*2)
			|| abs(y- pD->pos.y)<(TILE_UNITS*2))
		{
			pD->lastSync	= gameTime;								// note we did a reasonable job.
		}


		//////////////////////////////////////
		// now update the higher level stuff
		if(!vtolDroid(pD))
		{
			highLevelDroidUpdate(pD,x,y,state,  ord, psTarget,numkills);
		}

		// done this droid, move on to next.

	}

	return TRUE;
}

// ////////////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////////////////////////
// higher order droid updating. Works mainly at the order level. comes after the main sync.
static void highLevelDroidUpdate(DROID *psDroid,UDWORD x, UDWORD y,
								 //UDWORD state, UDWORD order,UDWORD orderX,UDWORD orderY,
								 UDWORD state, UDWORD order,
								 BASE_OBJECT *psTarget,UDWORD numKills)
{
	// update kill rating.
	psDroid->experience = (UWORD)numKills;

	// remote droid is attacking, not here tho!
	if(order == DORDER_ATTACK && psDroid->order != DORDER_ATTACK && psTarget)
	{
		turnOffMultiMsg(TRUE);
		orderDroidObj(psDroid, DORDER_ATTACK, psTarget);
		turnOffMultiMsg(FALSE);
	}

	// secondary orders.
	if(psDroid->secondaryOrder != state)
	{
		psDroid->secondaryOrder = state;
	}

	// see how well the sync worked, optionally update.
	// offscreen updates will make this ok each time.
	if(psDroid->order == DORDER_NONE && order == DORDER_NONE)
	{
		if(  (abs(x- psDroid->pos.x)>(TILE_UNITS*2))		// if more than 2 tiles wrong.
		   ||(abs(y- psDroid->pos.y)>(TILE_UNITS*2)) )
		{
			turnOffMultiMsg(TRUE);
			orderDroidLoc(psDroid, DORDER_MOVE,x,y);
			turnOffMultiMsg(FALSE);
		}
	}

}

// ////////////////////////////////////////////////////////////////////////////
// droid on screen needs modifying
static void onscreenUpdate(DROID *psDroid,
						   UDWORD dam,
						   UDWORD x,
						   UDWORD y,
						   float fx,
						   float fy,
						   UWORD dir,
						   DROID_ORDER order)
{

	BASE_OBJECT *psClickedOn;
	BOOL		bMouseOver = FALSE;

	psClickedOn = mouseTarget();
	if( psClickedOn != NULL && psClickedOn->type == OBJ_DROID)
	{
		if(psClickedOn->id == psDroid->id && mouseDown(MOUSE_RMB))
		{
			bMouseOver = TRUE;						// override, so you dont see the updates.
		}
	}

	if(!bMouseOver)
	{
		psDroid->body = dam;						// update damage
	}

//	if(psDroid->order == DORDER_NONE || (psDroid->order == DORDER_GUARD && psDroid->action == DACTION_NONE) )
//	{
//		psDroid->direction	 = dir  %360;				//update rotation
//	}

	return;
}

// ////////////////////////////////////////////////////////////////////////////
// droid offscreen needs modyfying.
static void offscreenUpdate(DROID *psDroid,
							UDWORD dam,
							UDWORD x,
							UDWORD y,
							float fx,
							float fy,
							UWORD dir,
							DROID_ORDER order)
{
	UDWORD				oldx,oldy;
	PROPULSION_STATS	*psPropStats;
 	SDWORD			xdiff,ydiff, distSq;

	// stage one, update the droids position & info, LOW LEVEL STUFF.
	if(	   order == DORDER_ATTACK
		|| order == DORDER_MOVE
		|| order ==	DORDER_RTB
		|| order == DORDER_RTR)	// move order
	{

		// calculate difference between remote and local
		xdiff = psDroid->pos.x - (UWORD)fx;
	    ydiff = psDroid->pos.y - (UWORD)fy;
	    distSq = (xdiff*xdiff) + (ydiff*ydiff);

		// if more than  2 squares, jump it.
		if(distSq > (2*TILE_UNITS)*(2*TILE_UNITS) )
		{
			if( ((UDWORD)fx != 0) && ((UDWORD)fy != 0) )
			{
				oldx = psDroid->pos.x;
				oldy = psDroid->pos.y;

				psDroid->sMove.fx = fx;							//update x
				psDroid->sMove.fy = fy;							//update y

				psDroid->pos.x		 = (UWORD) fx;					//update move progress
				psDroid->pos.y		 = (UWORD) fy;
				gridMoveObject((BASE_OBJECT *)psDroid, (SDWORD)oldx,(SDWORD)oldy);

				psDroid->direction = dir % 360;		// update rotation

				// reroute the droid.
				turnOffMultiMsg(TRUE);
				moveDroidTo(psDroid, psDroid->sMove.DestinationX,psDroid->sMove.DestinationY);
				turnOffMultiMsg(FALSE);

			}
		}
	}
	else
	{
		oldx = psDroid->pos.x;
		oldy = psDroid->pos.y;
		psDroid->pos.x		 = (UWORD)x;						//update x
		psDroid->pos.y		 = (UWORD)y;						//update y
		gridMoveObject((BASE_OBJECT *)psDroid, (SDWORD)oldx,(SDWORD)oldy);
		psDroid->direction = dir % 360;				// update rotation
	}

	psDroid->body		= dam;								// update damage

	// stop droid if remote droid has stopped.
	if(		(order == DORDER_NONE		   || order == DORDER_GUARD)
		&& !(psDroid->order == DORDER_NONE || psDroid->order == DORDER_GUARD )
	   )
	{
		turnOffMultiMsg(TRUE);
		moveStopDroid(psDroid);
		turnOffMultiMsg(FALSE);
	}


	// snap droid(if on ground)  to terrain level at x,y.
	psPropStats = asPropulsionStats + psDroid->asBits[COMP_PROPULSION].nStat;
	ASSERT( psPropStats != NULL, "offscreenUpdate: invalid propulsion stats pointer" );
	if(	psPropStats->propulsionType != LIFT )		// if not airborne.
	{
		psDroid->pos.z = map_Height(psDroid->pos.x, psDroid->pos.y);
	}
	return;
}





// ////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////
// Structure Checking, to ensure smoke and stuff is consistent across machines.
// this func is recursive!
static  STRUCTURE *pickAStructure(void)
{
	static UDWORD	player=0;					// player currently checking.
	static UDWORD	snum=0;						// structure index for this player.
	STRUCTURE		*pS=NULL;
	static UDWORD	maxtrys = 0;				// don't loop forever if failing/.
	UDWORD			i;

	if ( !myResponsibility(player) )			// dont send stuff that's not our problem.
	{
		player ++;								// next player next time.
		player = player%MAX_PLAYERS;
		snum =0;

		if(maxtrys<MAX_PLAYERS)
		{
			maxtrys ++;
			return pickAStructure();
		}
		else
		{
			maxtrys = 0;
			return NULL;
		}
	}

	pS = apsStructLists[player];				// find the strucutre
	for(i=0; ((i<snum) && (pS != NULL)) ;i++)
	{
		pS= pS->psNext;
	}


	if (pS == NULL)								// last structure or no structures at all
	{
		player ++;								// go onto the next player
		player = player%MAX_PLAYERS;
		snum=0;

		if(maxtrys<MAX_PLAYERS)
		{
			maxtrys ++;
			return pickAStructure();
		}
		else
		{
			maxtrys = 0;
			return NULL;
		}
	}
	snum ++;										// next structure next time

	maxtrys = 0;
	return pS;
}

// ////////////////////////////////////////////////////////////////////////
// Send structure information.
static BOOL sendStructureCheck(void)
{
	static UDWORD	lastSent = 0;	// Last time a struct was sent
	STRUCTURE		*pS;
    uint8_t			capacity;

	if (lastSent > gameTime)
	{
		lastSent = 0;
	}
	
	if ((gameTime - lastSent) < STRUCT_FREQUENCY)	// Only send a struct send if not done recently
	{
		return TRUE;
	}
	
	lastSent = gameTime;


	pS = pickAStructure();
	// Only send info about complete buildings
	if (pS && (pS->status == SS_BUILT))
	{
		NETbeginEncode(NET_CHECK_STRUCT, NET_ALL_PLAYERS);
			NETuint8_t(&pS->player);
			NETuint32_t(&pS->id);
			NETuint16_t(&pS->body);
			NETuint32_t(&pS->pStructureType->ref);
			NETuint16_t(&pS->pos.x);
			NETuint16_t(&pS->pos.y);
			NETuint16_t(&pS->pos.z);
			NETfloat(&pS->direction);
			
			switch (pS->pStructureType->type)
			{
				
				case REF_RESEARCH:
					capacity = ((RESEARCH_FACILITY *) pS->pFunctionality)->capacity;
					NETuint8_t(&capacity);
					break;
				case REF_FACTORY:
				case REF_VTOL_FACTORY:
					capacity = ((FACTORY *) pS->pFunctionality)->capacity;
					NETuint8_t(&capacity);
					break;
				case REF_POWER_GEN:
					capacity = ((POWER_GEN *) pS->pFunctionality)->capacity;
					NETuint8_t(&capacity);
				default:
					break;
			}

		NETend();
	}
	
	return TRUE;
}

// receive checking info about a structure and update local world state
BOOL recvStructureCheck()
{
	STRUCTURE		*pS;
	STRUCTURE_STATS	*psStats;
	BOOL			hasCapacity = TRUE;
	int				i, j;
	float			direction;
	uint8_t			player, ourCapacity, actualCapacity;
	uint16_t		body;
	uint16_t		x, y, z;
	uint32_t		ref, type;
	
	NETbeginDecode();
		NETuint8_t(&player);
		NETuint32_t(&ref);
		NETuint16_t(&body);
		NETuint32_t(&type);
		NETuint16_t(&x);
		NETuint16_t(&y);
		NETuint16_t(&z);
		NETfloat(&direction);
		
		// If the structure exists our job is easy
		pS = IdToStruct(ref, player);
		if (!pS)
		{
			pS->body = body;
			pS->direction = direction;
		}
		// Structure was not found - build it
		else
		{
			NETlogEntry("scheck:structure check failed, adding struct. val=type", 0, type - REF_STRUCTURE_START);
	
			for (i = 0; i < numStructureStats && asStructureStats[i].ref != type; i++);
			psStats = &asStructureStats[i];
	
			// Check for similar buildings, to avoid overlaps
			if (TILE_HAS_STRUCTURE(mapTile(map_coord(x), map_coord(y))))
			{
				NETlogEntry("scheck:Tile has structure val=player", 0, player);
	
				pS = getTileStructure(map_coord(x), map_coord(y));

				// If correct type && player then complete & modify
				if (pS
				 && pS->pStructureType->type == type
				 && pS->player == player)
				{
					pS->direction = direction;
					pS->id = ref;
					
					if (pS->status != SS_BUILT)
					{
						pS->status = SS_BUILT;
						buildingComplete(pS);
					}
					
					NETlogEntry("scheck: fixed?", 0, player);
				}
				// Wall becoming a cornerwall
				else if (pS->pStructureType->type == REF_WALL)
				{
					if (psStats->type == REF_WALLCORNER)
					{
						NETlogEntry("scheck: fixed wall->cornerwall", 0, 0);
						removeStruct(pS, TRUE);
	
						powerCalc(FALSE);
						pS = buildStructure((STRUCTURE_STATS * )psStats, x, y, player, TRUE);
						powerCalc(TRUE);
	
						if (pS)
						{
							pS->id = ref;
						}
						else
						{
							NETlogEntry("scheck: failed to upgrade wall!", 0, player);
							return FALSE;
						}
					}
				}
				else
				{
					NETlogEntry("scheck:Tile did not have correct type or player val=player",0,player);
					return FALSE;
			    }
			}
			// Nothing exists there so lets get building!
			else
			{
				NETlogEntry("scheck: didn't find structure at all, building it",0,0);
	
				powerCalc(FALSE);
				pS = buildStructure((STRUCTURE_STATS *) psStats, x, y, player, TRUE);
				powerCalc(TRUE);
			}
		}
	
		if (pS)
		{
			// Check its finished
			if (pS->status != SS_BUILT)
			{
				pS->direction = direction;
				pS->id = ref;
				pS->status = SS_BUILT;
				buildingComplete(pS);
			}
			
			// If the structure has a capacity
			switch (pS->pStructureType->type)
			{
				case REF_RESEARCH:
					ourCapacity = ((RESEARCH_FACILITY *) pS->pFunctionality)->capacity;
					j = researchModuleStat;
					break;
				case REF_FACTORY:
				case REF_VTOL_FACTORY:
					ourCapacity = ((FACTORY *) pS->pFunctionality)->capacity;
					j = factoryModuleStat;
					break;
				case REF_POWER_GEN:
					ourCapacity = ((POWER_GEN *) pS->pFunctionality)->capacity;
					j = powerModuleStat;
					break;
				default:
					hasCapacity = FALSE;
					break;
			}
			
			// So long as the struct has a capacity fetch it from the packet
			if (hasCapacity)
			{
				NETuint8_t(&actualCapacity);
				
				// If our capacity is different upgrade ourself
				for (; ourCapacity < actualCapacity; ourCapacity++)
				{
					buildStructure(&asStructureStats[j], pS->pos.x, pS->pos.y, pS->player, FALSE);
					
					// Check it is finished
					if (pS && pS->status != SS_BUILT)
					{
						pS->id = ref;
						pS->status = SS_BUILT;
						buildingComplete(pS);
					}
				}
			}
		}
	
	NETend();
	return TRUE;
}


// ////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////
// Power Checking. Send a power level check every now and again.
BOOL sendPowerCheck(BOOL now)
{
	static UDWORD	lastsent = 0;
	uint8_t			player = selectedPlayer;
	uint32_t		power = asPower[player]->currentPower;

	if (!now)
	{
		if (lastsent > gameTime)
		{
			lastsent = 0;
		}
		
		// Only send if not done recently
		if (gameTime - lastsent < POWER_FREQUENCY)
		{
			return TRUE;
		}
	}

	lastsent = gameTime;

	NETbeginEncode(NET_CHECK_POWER, NET_ALL_PLAYERS);
		NETuint8_t(&player);
		NETuint32_t(&power);
	return NETend();
}

BOOL recvPowerCheck()
{
	uint8_t		player;
	uint32_t	power;

	NETbeginDecode();
		NETuint8_t(&player);
		NETuint32_t(&power);
	NETend();
	
	setPower(player, power);
	return TRUE;
}

// ////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////
// Score
BOOL sendScoreCheck(void)
{
	static UDWORD	lastsent = 0;
	uint8_t			i;
	BOOL			isData = FALSE;
	PLAYERSTATS		stats;

	if (lastsent > gameTime)
	{
		lastsent= 0;
	}
	
	if (gameTime - lastsent < SCORE_FREQUENCY)
	{
		return TRUE;
	}
	
	lastsent = gameTime;

	// Update local score
	stats = getMultiStats(selectedPlayer, TRUE);

	// Add recently scored points
	stats.recentKills += stats.killsToAdd;
	stats.totalKills  += stats.killsToAdd;
	stats.recentScore += stats.scoreToAdd;
	stats.totalScore  += stats.scoreToAdd;
	
	// Zero them now added
	stats.killsToAdd = stats.scoreToAdd = 0;

	// Store local version
	setMultiStats(player2dpid[selectedPlayer], stats, TRUE);
	
	// Send score to the ether
	setMultiStats(player2dpid[selectedPlayer], stats, FALSE);

	// Broadcast any changes in other players, but not in FRONTEND!!!
	if (titleMode != MULTIOPTION && titleMode != MULTILIMIT)
	{
		NETbeginEncode(NET_SCORESUBMIT, NET_ALL_PLAYERS);
		
		for (i = 0; i < MAX_PLAYERS; i++)
		{
			if (isHumanPlayer(i) && i != selectedPlayer)
			{
				stats = getMultiStats(i, TRUE);
				
				if (stats.killsToAdd || stats.scoreToAdd  )
				{
					NETuint8_t(&i);
					
					NETuint32_t(&stats.killsToAdd);
					NETuint32_t(&stats.scoreToAdd);
					
					isData = TRUE;
				}
			}
		}
		
		// If we added any data to the packet
		if (isData)
		{
			// Terminate the message with ANYPLAYER
			uint8_t player = ANYPLAYER;
			NETuint8_t(&player);
			
			// Send the message
			NETend();
		}
	}

	// Get global versions of scores
	for (i = 0; i < MAX_PLAYERS; i++)
	{
		if (isHumanPlayer(i))
		{
			setMultiStats(player2dpid[i], getMultiStats(i, FALSE), TRUE);
		}
	}
	
	return TRUE;
}



BOOL recvScoreSubmission()
{
	uint8_t		player;
	uint32_t	kills, score;
	PLAYERSTATS	stats;

	NETbeginDecode();
	
	for (NETuint8_t(&player); player != ANYPLAYER; NETuint8_t(&player))
	{
		if (player == selectedPlayer)
		{
			NETuint32_t(&kills);
			NETuint32_t(&score);
			
			stats = getMultiStats(player, TRUE);
			stats.killsToAdd += kills;
			stats.scoreToAdd += score;
			
			break;
		}
	}
	
	return TRUE;
}

// ////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////
// Pings

UDWORD averagePing(void)
{
	UDWORD i,count,total;

	count =0;
	total =0;

	for(i=0;i<MAX_PLAYERS;i++)
	{
		if(isHumanPlayer(i))
		{
			total += ingame.PingTimes[i];
			count ++;
		}
	}
	return total/count;
}

BOOL sendPing(void)
{
	BOOL			new = TRUE;
	uint8_t			player = selectedPlayer;
	int				i;
	static UDWORD	lastPing = 0;	// Last time we sent a ping
	static UDWORD	lastav = 0;		// Last time we updated average

	// Only ping every so often
	if (lastPing > gameTime)
	{
		lastPing = 0;
	}
	
	if (gameTime - lastPing < PING_FREQUENCY)
	{
		return TRUE;
	}
	
	lastPing = gameTime;


	// If host, also update the average ping stat for joiners
	if (NetPlay.bHost)
	{
		if (lastav > gameTime)
		{
			lastav = 0;
		}
		
		if (gameTime - lastav > AV_PING_FREQUENCY)
		{
			NETsetGameFlags(2, averagePing());
			lastav = gameTime;
		}
	}

	/*
	 * Before we send the ping, if any player failed to respond to the last one
	 * we should re-enumerate the players.
	 */

	for (i = 0; i < MAX_PLAYERS; i++)
	{
		if (isHumanPlayer(i)
		 && PingSend[i]
		 && ingame.PingTimes[i]
		 && i != selectedPlayer)
		{
			ingame.PingTimes[i] = PING_LIMIT;
		}
		else if (!isHumanPlayer(i)
		      && PingSend[i]
		      && ingame.PingTimes[i]
		      && i != selectedPlayer)
		{
			ingame.PingTimes[i] = 0;
		}
	}

	NETbeginEncode(NET_PING, NET_ALL_PLAYERS);
		NETuint8_t(&player);
		NETbool(&new);
	NETend();

	// Note when we sent the ping
	for (i = 0; i < MAX_PLAYERS; i++)
	{
		PingSend[i] = gameTime2;
	}
	
	return TRUE;
}

// accept and process incoming ping messages.
BOOL recvPing()
{
	BOOL	new;
	uint8_t	sender, us = selectedPlayer;
	
	NETbeginDecode();
		NETuint8_t(&sender);
		NETbool(&new);
	NETend();

	// If this is a new ping, respond to it
	if (new)
	{
		NETbeginEncode(NET_PING, player2dpid[sender]);
			// We are responding to a new ping
			new = FALSE;
			
			NETuint8_t(&us);
			NETbool(&new);
		NETend();
	}
	// They are responding to one of our pings
	else
	{
		// Work out how long it took them to respond
		ingame.PingTimes[sender] = (gameTime2 = PingSend[sender]) / 2;
		
		// Note that we have received it
		PingSend[sender] = 0;
	}
	
	return TRUE;
}
