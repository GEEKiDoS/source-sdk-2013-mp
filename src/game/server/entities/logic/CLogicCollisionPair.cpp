//========= Copyright Valve Corporation, All rights reserved. ============//

#include "cbase.h"

#include "entities/logic/CLogicCollisionPair.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// Finds the named physics object.  If no name, returns the world
// If a name is specified and an object not found - errors are reported
IPhysicsObject *FindPhysicsObjectByNameOrWorld( string_t name, CBaseEntity *pErrorEntity )
{
	if ( !name )
		return g_PhysWorldObject;

	IPhysicsObject *pPhysics = FindPhysicsObjectByName( name.ToCStr(), pErrorEntity );
	if ( !pPhysics )
	{
		DevWarning("%s: can't find %s\n", pErrorEntity->GetClassname(), name.ToCStr());
	}
	return pPhysics;
}

BEGIN_DATADESC( CLogicCollisionPair )
	DEFINE_KEYFIELD( m_nameAttach1, FIELD_STRING, "attach1" ),
	DEFINE_KEYFIELD( m_nameAttach2, FIELD_STRING, "attach2" ),
	DEFINE_KEYFIELD( m_disabled, FIELD_BOOLEAN, "startdisabled" ),
	DEFINE_FIELD( m_succeeded, FIELD_BOOLEAN ),

	// Inputs
	DEFINE_INPUTFUNC( FIELD_VOID, "DisableCollisions", InputDisableCollisions ),
	DEFINE_INPUTFUNC( FIELD_VOID, "EnableCollisions", InputEnableCollisions ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( logic_collision_pair, CLogicCollisionPair );
