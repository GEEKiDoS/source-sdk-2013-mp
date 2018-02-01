//========= Copyright Valve Corporation, All rights reserved. ============//

#include "cbase.h"

#include "triggers.h"

#include "entities/triggers/CTriggerCDAudio.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS( trigger_cdaudio, CTriggerCDAudio );

//-----------------------------------------------------------------------------
// Purpose: Changes tracks or stops CD when player touches
// Input  : pOther - The entity that touched us.
//-----------------------------------------------------------------------------
void CTriggerCDAudio::Touch ( CBaseEntity *pOther )
{
	if ( !pOther->IsPlayer() )
	{
		return;
	}

	PlayTrack();
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerCDAudio::Spawn( void )
{
	BaseClass::Spawn();
	InitTrigger();
}


void CTriggerCDAudio::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	PlayTrack();
}


//-----------------------------------------------------------------------------
// Purpose: Issues a client command to play a given CD track. Called from
//			trigger_cdaudio and target_cdaudio.
// Input  : iTrack - Track number to play.
//-----------------------------------------------------------------------------
static void PlayCDTrack( int iTrack )
{
	edict_t *pClient;
	
	// manually find the single player. 
	pClient = engine->PEntityOfEntIndex( 1 );

	Assert(gpGlobals->maxClients == 1);
	
	// Can't play if the client is not connected!
	if ( !pClient )
		return;

	// UNDONE: Move this to engine sound
	if ( iTrack < -1 || iTrack > 30 )
	{
		Warning( "TriggerCDAudio - Track %d out of range\n", iTrack );
		return;
	}

	if ( iTrack == -1 )
	{
		engine->ClientCommand ( pClient, "cd pause\n");
	}
	else
	{
		engine->ClientCommand ( pClient, "cd play %3d\n", iTrack);
	}
}


// only plays for ONE client, so only use in single play!
void CTriggerCDAudio::PlayTrack( void )
{
	PlayCDTrack( (int)m_iHealth );
	
	SetTouch( NULL );
	UTIL_Remove( this );
}
