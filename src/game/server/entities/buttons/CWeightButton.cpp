//========= Copyright Valve Corporation, All rights reserved. ============//

#include "cbase.h"

#include "entities/buttons/CWeightButton.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS( func_weight_button, CWeightButton );

BEGIN_DATADESC( CWeightButton )

	DEFINE_KEYFIELD( m_fStressToActivate, FIELD_FLOAT, "WeightToActivate" ),
	DEFINE_FIELD( m_bHasBeenPressed, FIELD_BOOLEAN ),

	DEFINE_OUTPUT( m_OnPressed, "OnPressed" ),
	DEFINE_OUTPUT( m_OnReleased, "OnReleased" ),

	DEFINE_THINKFUNC( TriggerThink ),

END_DATADESC()


void CWeightButton::Spawn()
{
	BaseClass::Spawn();

	// Convert movedir from angles to a vector
	SetMoveType( MOVETYPE_VPHYSICS );
	SetSolid( SOLID_VPHYSICS );
	SetModel( STRING( GetModelName() ) );
	CreateVPhysics();
	SetThink( &CWeightButton::TriggerThink );
	SetNextThink( gpGlobals->curtime + TICK_INTERVAL );
	m_bHasBeenPressed = false;
}

//-----------------------------------------------------------------------------
// Purpose: Create VPhysics collision for this entity
//-----------------------------------------------------------------------------
bool CWeightButton::CreateVPhysics()
{
	VPhysicsInitShadow( false, false );
	return true;
}


//-----------------------------------------------------------------------------
// Purpose: Every second, check total stress and fire an output if we have reached 
//			our threshold. If the stress is relieved below our threshold, fire a different output.
//-----------------------------------------------------------------------------
void CWeightButton::TriggerThink( void )
{
	vphysics_objectstress_t vpobj_StressOut;
	IPhysicsObject* pMyPhysics = VPhysicsGetObject();

	if ( !pMyPhysics )
	{
		SetNextThink( TICK_NEVER_THINK );
		return;
	}

	float fStress = CalculateObjectStress( pMyPhysics, this, &vpobj_StressOut );

//	fStress = vpobj_StressOut.receivedStress;

	if ( fStress > m_fStressToActivate && !m_bHasBeenPressed )
	{
		m_OnPressed.FireOutput( this, this );
		m_bHasBeenPressed = true;
	}
	else if ( fStress < m_fStressToActivate && m_bHasBeenPressed )
	{
		m_OnReleased.FireOutput( this, this );
		m_bHasBeenPressed = false;
	}

	// think every tick
	SetNextThink( gpGlobals->curtime + TICK_INTERVAL );
}
