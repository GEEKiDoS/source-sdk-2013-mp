//========= Copyright Valve Corporation, All rights reserved. ============//

#include "cbase.h"

#include "globalstate.h"

#include "entities/logic/CEnvGlobal.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Console command to set the state of a global
//-----------------------------------------------------------------------------
void CC_Global_Set( const CCommand &args )
{
	const char *szGlobal = args[1];
	const char *szState = args[2];

	if ( szGlobal == NULL || szState == NULL )
	{
		Msg( "Usage: global_set <globalname> <state>: Sets the state of the given env_global (0 = OFF, 1 = ON, 2 = DEAD).\n" );
		return;
	}

	int nState = atoi( szState );

	int nIndex = GlobalEntity_GetIndex( szGlobal );

	if ( nIndex >= 0 )
	{
		GlobalEntity_SetState( nIndex, ( GLOBALESTATE )nState );
	}
	else
	{
		GlobalEntity_Add( szGlobal, STRING( gpGlobals->mapname ), ( GLOBALESTATE )nState );
	}
}

static ConCommand global_set( "global_set", CC_Global_Set, "global_set <globalname> <state>: Sets the state of the given env_global (0 = OFF, 1 = ON, 2 = DEAD).", FCVAR_CHEAT );

BEGIN_DATADESC( CEnvGlobal )

	DEFINE_KEYFIELD( m_globalstate, FIELD_STRING, "globalstate" ),
	DEFINE_FIELD( m_triggermode, FIELD_INTEGER ),
	DEFINE_KEYFIELD( m_initialstate, FIELD_INTEGER, "initialstate" ),
	DEFINE_KEYFIELD( m_counter, FIELD_INTEGER, "counter" ),

	// Inputs
	DEFINE_INPUTFUNC( FIELD_VOID, "TurnOn",	InputTurnOn ),
	DEFINE_INPUTFUNC( FIELD_VOID, "TurnOff", InputTurnOff ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Remove",	InputRemove ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Toggle",	InputToggle ),

	DEFINE_INPUTFUNC( FIELD_INTEGER, "SetCounter",	InputSetCounter ),
	DEFINE_INPUTFUNC( FIELD_INTEGER, "AddToCounter",	InputAddToCounter ),
	DEFINE_INPUTFUNC( FIELD_VOID, "GetCounter",	InputGetCounter ),
	
	DEFINE_OUTPUT( m_outCounter, "Counter" ),

END_DATADESC()


LINK_ENTITY_TO_CLASS( env_global, CEnvGlobal );


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CEnvGlobal::Spawn( void )
{
	if ( !m_globalstate )
	{
		UTIL_Remove( this );
		return;
	}

#ifdef HL2_EPISODIC
	// if we modify the state of the physics cannon, make sure we precache the ragdoll boogie zap sound
	if ( ( m_globalstate != NULL_STRING ) && ( stricmp( STRING( m_globalstate ), "super_phys_gun" ) == 0 ) )
	{
		PrecacheScriptSound( "RagdollBoogie.Zap" );
	}
#endif

	if ( FBitSet( m_spawnflags, SF_GLOBAL_SET ) )
	{
		if ( !GlobalEntity_IsInTable( m_globalstate ) )
		{
			GlobalEntity_Add( m_globalstate, gpGlobals->mapname, (GLOBALESTATE)m_initialstate );
		}
		
		if ( m_counter != 0 )
		{
			GlobalEntity_SetCounter( m_globalstate, m_counter );
		}
	}
}


//------------------------------------------------------------------------------
// Purpose:
//------------------------------------------------------------------------------
void CEnvGlobal::InputTurnOn( inputdata_t &inputdata )
{
	if ( GlobalEntity_IsInTable( m_globalstate ) )
	{
		GlobalEntity_SetState( m_globalstate, GLOBAL_ON );
	}
	else
	{
		GlobalEntity_Add( m_globalstate, gpGlobals->mapname, GLOBAL_ON );
	}
}


//------------------------------------------------------------------------------
// Purpose:
//------------------------------------------------------------------------------
void CEnvGlobal::InputTurnOff( inputdata_t &inputdata )
{
	if ( GlobalEntity_IsInTable( m_globalstate ) )
	{
		GlobalEntity_SetState( m_globalstate, GLOBAL_OFF );
	}
	else
	{
		GlobalEntity_Add( m_globalstate, gpGlobals->mapname, GLOBAL_OFF );
	}
}


//------------------------------------------------------------------------------
// Purpose:
//------------------------------------------------------------------------------
void CEnvGlobal::InputRemove( inputdata_t &inputdata )
{
	if ( GlobalEntity_IsInTable( m_globalstate ) )
	{
		GlobalEntity_SetState( m_globalstate, GLOBAL_DEAD );
	}
	else
	{
		GlobalEntity_Add( m_globalstate, gpGlobals->mapname, GLOBAL_DEAD );
	}
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CEnvGlobal::InputSetCounter( inputdata_t &inputdata )
{
	if ( !GlobalEntity_IsInTable( m_globalstate ) )
	{
		GlobalEntity_Add( m_globalstate, gpGlobals->mapname, GLOBAL_ON );
	}

	GlobalEntity_SetCounter( m_globalstate, inputdata.value.Int() );
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CEnvGlobal::InputAddToCounter( inputdata_t &inputdata )
{
	if ( !GlobalEntity_IsInTable( m_globalstate ) )
	{
		GlobalEntity_Add( m_globalstate, gpGlobals->mapname, GLOBAL_ON );
	}

	GlobalEntity_AddToCounter( m_globalstate, inputdata.value.Int() );
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CEnvGlobal::InputGetCounter( inputdata_t &inputdata )
{
	if ( !GlobalEntity_IsInTable( m_globalstate ) )
	{
		GlobalEntity_Add( m_globalstate, gpGlobals->mapname, GLOBAL_ON );
	}

	m_outCounter.Set( GlobalEntity_GetCounter( m_globalstate ), inputdata.pActivator, this );
}


//------------------------------------------------------------------------------
// Purpose:
//------------------------------------------------------------------------------
void CEnvGlobal::InputToggle( inputdata_t &inputdata )
{
	GLOBALESTATE oldState = GlobalEntity_GetState( m_globalstate );
	GLOBALESTATE newState;

	if ( oldState == GLOBAL_ON )
	{
		newState = GLOBAL_OFF;
	}
	else if ( oldState == GLOBAL_OFF )
	{
		newState = GLOBAL_ON;
	}
	else
	{
		return;
	}

	if ( GlobalEntity_IsInTable( m_globalstate ) )
	{
		GlobalEntity_SetState( m_globalstate, newState );
	}
	else
	{
		GlobalEntity_Add( m_globalstate, gpGlobals->mapname, newState );
	}
}


//-----------------------------------------------------------------------------
// Purpose: Draw any debug text overlays
// Input  :
// Output : Current text offset from the top
//-----------------------------------------------------------------------------
int CEnvGlobal::DrawDebugTextOverlays(void) 
{
	// Skip AIClass debug overlays
	int text_offset = CBaseEntity::DrawDebugTextOverlays();

	if (m_debugOverlays & OVERLAY_TEXT_BIT) 
	{
		char tempstr[512];
		Q_snprintf(tempstr,sizeof(tempstr),"State: %s",STRING(m_globalstate));
		EntityText(text_offset,tempstr,0);
		text_offset++;

		GLOBALESTATE nState = GlobalEntity_GetState( m_globalstate );

		switch( nState )
		{
		case GLOBAL_OFF:
			Q_strncpy(tempstr,"Value: OFF",sizeof(tempstr));
			break;

		case GLOBAL_ON:
			Q_strncpy(tempstr,"Value: ON",sizeof(tempstr));
			break;

		case GLOBAL_DEAD:
			Q_strncpy(tempstr,"Value: DEAD",sizeof(tempstr));
			break;
		}
		EntityText(text_offset,tempstr,0);
		text_offset++;
	}
	return text_offset;
}
