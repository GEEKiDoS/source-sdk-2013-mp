//========= Copyright Valve Corporation, All rights reserved. ============//

#include "cbase.h"

#include "entities/triggers/CPhysicsWind.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

BEGIN_SIMPLE_DATADESC( CPhysicsWind )

	DEFINE_FIELD( m_nWindYaw,		FIELD_INTEGER ),
	DEFINE_FIELD( m_flWindSpeed,	FIELD_FLOAT ),

END_DATADESC()
