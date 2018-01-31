//========= Copyright Valve Corporation, All rights reserved. ============//

#include "cbase.h"

#include "triggers.h"

#include "entities/triggers/CTriggerTeleportRelative.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS( trigger_teleport_relative, CTriggerTeleportRelative );
BEGIN_DATADESC( CTriggerTeleportRelative )
	DEFINE_KEYFIELD( m_TeleportOffset, FIELD_VECTOR, "teleportoffset" )
END_DATADESC()


void CTriggerTeleportRelative::Spawn( void )
{
	InitTrigger();
}

void CTriggerTeleportRelative::Touch( CBaseEntity *pOther )
{
	if ( !PassesTriggerFilters(pOther) )
	{
		return;
	}

	const Vector finalPos = m_TeleportOffset + WorldSpaceCenter();
	const Vector *momentum = &vec3_origin;

	pOther->Teleport( &finalPos, NULL, momentum );
}
