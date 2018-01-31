//========= Copyright Valve Corporation, All rights reserved. ============//

#include "cbase.h"

#include "triggers.h"

#include "entities/triggers/CTriggerTeleport.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS( trigger_teleport, CTriggerTeleport );

BEGIN_DATADESC( CTriggerTeleport )

	DEFINE_KEYFIELD( m_iLandmark, FIELD_STRING, "landmark" ),

END_DATADESC()

void CTriggerTeleport::Spawn( void )
{
	InitTrigger();
}

//-----------------------------------------------------------------------------
// Purpose: Teleports the entity that touched us to the location of our target,
//			setting the toucher's angles to our target's angles if they are a
//			player.
//
//			If a landmark was specified, the toucher is offset from the target
//			by their initial offset from the landmark and their angles are
//			left alone.
//
// Input  : pOther - The entity that touched us.
//-----------------------------------------------------------------------------
void CTriggerTeleport::Touch( CBaseEntity *pOther )
{
	CBaseEntity	*pentTarget = NULL;

	if (!PassesTriggerFilters(pOther))
	{
		return;
	}

	// The activator and caller are the same
	pentTarget = gEntList.FindEntityByName( pentTarget, m_target, NULL, pOther, pOther );
	if (!pentTarget)
	{
	   return;
	}
	
	//
	// If a landmark was specified, offset the player relative to the landmark.
	//
	CBaseEntity	*pentLandmark = NULL;
	Vector vecLandmarkOffset(0, 0, 0);
	if (m_iLandmark != NULL_STRING)
	{
		// The activator and caller are the same
		pentLandmark = gEntList.FindEntityByName(pentLandmark, m_iLandmark, NULL, pOther, pOther );
		if (pentLandmark)
		{
			vecLandmarkOffset = pOther->GetAbsOrigin() - pentLandmark->GetAbsOrigin();
		}
	}

	pOther->SetGroundEntity( NULL );
	
	Vector tmp = pentTarget->GetAbsOrigin();

	if (!pentLandmark && pOther->IsPlayer())
	{
		// make origin adjustments in case the teleportee is a player. (origin in center, not at feet)
		tmp.z -= pOther->WorldAlignMins().z;
	}

	//
	// Only modify the toucher's angles and zero their velocity if no landmark was specified.
	//
	const QAngle *pAngles = NULL;
	Vector *pVelocity = NULL;

#ifdef HL1_DLL
	Vector vecZero(0,0,0);		
#endif

	if (!pentLandmark && !HasSpawnFlags(SF_TELEPORT_PRESERVE_ANGLES) )
	{
		pAngles = &pentTarget->GetAbsAngles();

#ifdef HL1_DLL
		pVelocity = &vecZero;
#else
		pVelocity = NULL;	//BUGBUG - This does not set the player's velocity to zero!!!
#endif
	}

	tmp += vecLandmarkOffset;
	pOther->Teleport( &tmp, pAngles, pVelocity );
}


LINK_ENTITY_TO_CLASS( info_teleport_destination, CPointEntity );
