//========= Copyright Valve Corporation, All rights reserved. ============//

#include "cbase.h"

#include "triggers.h"

#include "entities/triggers/CTriggerGravity.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS( trigger_gravity, CTriggerGravity );

BEGIN_DATADESC( CTriggerGravity )

	// Function Pointers
	DEFINE_FUNCTION(GravityTouch),

END_DATADESC()

void CTriggerGravity::Spawn( void )
{
	BaseClass::Spawn();
	InitTrigger();
	SetTouch( &CTriggerGravity::GravityTouch );
}

void CTriggerGravity::GravityTouch( CBaseEntity *pOther )
{
	// Only save on clients
	if ( !pOther->IsPlayer() )
		return;

	pOther->SetGravity( GetGravity() );
}
