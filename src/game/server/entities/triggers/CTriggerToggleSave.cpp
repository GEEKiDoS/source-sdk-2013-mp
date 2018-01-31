//========= Copyright Valve Corporation, All rights reserved. ============//

#include "cbase.h"

#include "triggers.h"

#include "entities/triggers/CTriggerToggleSave.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

BEGIN_DATADESC( CTriggerToggleSave )
	DEFINE_KEYFIELD( m_bDisabled, FIELD_BOOLEAN, "StartDisabled" ),

	DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( trigger_togglesave, CTriggerToggleSave );


//-----------------------------------------------------------------------------
// Purpose: Called when spawning, after keyvalues have been set.
//-----------------------------------------------------------------------------
void CTriggerToggleSave::Spawn( void )
{
	if ( g_pGameRules->IsDeathmatch() )
	{
		UTIL_Remove( this );
		return;
	}

	InitTrigger();
}


//-----------------------------------------------------------------------------
// Purpose: Performs the autosave when the player touches us.
// Input  : pOther - 
//-----------------------------------------------------------------------------
void CTriggerToggleSave::Touch( CBaseEntity *pOther )
{
	if( m_bDisabled )
		return;

	// Only save on clients
	if ( !pOther->IsPlayer() )
		return;
    
	// Can be re-enabled
	m_bDisabled = true;

	engine->ServerCommand( "autosave\n" );
}
