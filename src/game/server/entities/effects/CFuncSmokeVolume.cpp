//========= Copyright Valve Corporation, All rights reserved. ============//

#include "cbase.h"

#include "baseparticleentity.h"

#include "entities/effects/CFuncSmokeVolume.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

BEGIN_DATADESC( CFuncSmokeVolume )

	// Save/restore Keyvalue fields
	DEFINE_KEYFIELD( m_Color1, FIELD_COLOR32, "Color1" ),
	DEFINE_KEYFIELD( m_Color2, FIELD_COLOR32, "Color2" ),
//	DEFINE_ARRAY( m_MaterialName, FIELD_STRING, 255 ),
	DEFINE_KEYFIELD( m_String_tMaterialName, FIELD_STRING, "Material" ),
	DEFINE_KEYFIELD( m_ParticleDrawWidth, FIELD_FLOAT, "ParticleDrawWidth" ),
	DEFINE_KEYFIELD( m_ParticleSpacingDistance, FIELD_FLOAT, "ParticleSpacingDistance" ),
	DEFINE_KEYFIELD( m_DensityRampSpeed, FIELD_FLOAT, "DensityRampSpeed" ),
	DEFINE_KEYFIELD( m_RotationSpeed, FIELD_FLOAT, "RotationSpeed" ),
	DEFINE_KEYFIELD( m_MovementSpeed, FIELD_FLOAT, "MovementSpeed" ),
	DEFINE_KEYFIELD( m_Density, FIELD_FLOAT, "Density" ),
	// inputs
	DEFINE_INPUT( m_RotationSpeed, FIELD_FLOAT, "SetRotationSpeed"),
	DEFINE_INPUT( m_MovementSpeed, FIELD_FLOAT, "SetMovementSpeed"),
	DEFINE_INPUT( m_Density, FIELD_FLOAT, "SetDensity"),

END_DATADESC()




IMPLEMENT_SERVERCLASS_ST( CFuncSmokeVolume, DT_FuncSmokeVolume )
	SendPropInt( SENDINFO( m_Color1 ), 32, SPROP_UNSIGNED, SendProxy_Color32ToInt ),
	SendPropInt( SENDINFO( m_Color2 ), 32, SPROP_UNSIGNED, SendProxy_Color32ToInt ),
	SendPropString( SENDINFO( m_MaterialName ) ),
	SendPropFloat( SENDINFO( m_ParticleDrawWidth ), 0, SPROP_NOSCALE ),
	SendPropFloat( SENDINFO( m_ParticleSpacingDistance ), 0, SPROP_NOSCALE ),
	SendPropFloat( SENDINFO( m_DensityRampSpeed ), 0, SPROP_NOSCALE ),
	SendPropFloat( SENDINFO( m_RotationSpeed ), 0, SPROP_NOSCALE ),
	SendPropFloat( SENDINFO( m_MovementSpeed ), 0, SPROP_NOSCALE ),
	SendPropFloat( SENDINFO( m_Density ), 0, SPROP_NOSCALE ),
	SendPropInt( SENDINFO(m_spawnflags), 8, SPROP_UNSIGNED )
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( func_smokevolume, CFuncSmokeVolume );

CFuncSmokeVolume::CFuncSmokeVolume()
{
	m_Density = 1.0f;
}

void CFuncSmokeVolume::SetDensity( float density )
{
	m_Density = density;
}

void CFuncSmokeVolume::Spawn()
{
	memset( m_MaterialName.GetForModify(), 0, sizeof( m_MaterialName ) );

	// Bind to our bmodel.
	SetModel( STRING( GetModelName() ) );

	BaseClass::Spawn();
}

void CFuncSmokeVolume::Activate( void )
{
	BaseClass::Activate();
	Q_strncpy( m_MaterialName.GetForModify(), STRING( m_String_tMaterialName ), 255 );
}
