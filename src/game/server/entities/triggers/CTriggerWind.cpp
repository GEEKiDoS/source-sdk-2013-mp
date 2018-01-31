//========= Copyright Valve Corporation, All rights reserved. ============//

#include "cbase.h"

#include "physics_saverestore.h"
#include "triggers.h"

#include "entities/triggers/CPhysicsWind.h"
#include "entities/triggers/CTriggerWind.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS( trigger_wind, CTriggerWind );

BEGIN_DATADESC( CTriggerWind )

	DEFINE_FIELD( m_nSpeedCurrent, FIELD_INTEGER),
	DEFINE_FIELD( m_nSpeedTarget,	FIELD_INTEGER),
	DEFINE_FIELD( m_nDirBase,		FIELD_INTEGER),
	DEFINE_FIELD( m_nDirCurrent,	FIELD_INTEGER),
	DEFINE_FIELD( m_nDirTarget,	FIELD_INTEGER),
	DEFINE_FIELD( m_bSwitch,		FIELD_BOOLEAN),

	DEFINE_FIELD( m_nSpeedBase,		FIELD_INTEGER ),
	DEFINE_KEYFIELD( m_nSpeedNoise,	FIELD_INTEGER, "SpeedNoise"),
	DEFINE_KEYFIELD( m_nDirNoise,	FIELD_INTEGER, "DirectionNoise"),
	DEFINE_KEYFIELD( m_nHoldBase,	FIELD_INTEGER, "HoldTime"),
	DEFINE_KEYFIELD( m_nHoldNoise,	FIELD_INTEGER, "HoldNoise"),

	DEFINE_PHYSPTR( m_pWindController ),
	DEFINE_EMBEDDED( m_WindCallback ),

	DEFINE_FUNCTION( WindThink ),

	DEFINE_INPUTFUNC( FIELD_INTEGER, "SetSpeed", InputSetSpeed ),

END_DATADESC()


//------------------------------------------------------------------------------
// Purpose:
//------------------------------------------------------------------------------
void CTriggerWind::Spawn( void )
{
	m_bSwitch = true;
	m_nDirBase = GetLocalAngles().y;

	BaseClass::Spawn();

	m_nSpeedCurrent = m_nSpeedBase;
	m_nDirCurrent = m_nDirBase;

	SetContextThink( &CTriggerWind::WindThink, gpGlobals->curtime, WIND_THINK_CONTEXT );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTriggerWind::KeyValue( const char *szKeyName, const char *szValue )
{
	// Done here to avoid collision with CBaseEntity's speed key
	if ( FStrEq(szKeyName, "Speed") )
	{
		m_nSpeedBase = atoi( szValue );
	}
	else
		return BaseClass::KeyValue( szKeyName, szValue );

	return true;
}

//------------------------------------------------------------------------------
// Create VPhysics
//------------------------------------------------------------------------------
bool CTriggerWind::CreateVPhysics()
{
	BaseClass::CreateVPhysics();

	m_pWindController = physenv->CreateMotionController( &m_WindCallback );
	return true;
}

//------------------------------------------------------------------------------
// Cleanup
//------------------------------------------------------------------------------
void CTriggerWind::UpdateOnRemove()
{
	if ( m_pWindController )
	{
		physenv->DestroyMotionController( m_pWindController );
		m_pWindController = NULL;
	}

	BaseClass::UpdateOnRemove();
}

//------------------------------------------------------------------------------
// Purpose:
//------------------------------------------------------------------------------
void CTriggerWind::OnRestore()
{
	BaseClass::OnRestore();
	if ( m_pWindController )
	{
		m_pWindController->SetEventHandler( &m_WindCallback );
	}
}


//------------------------------------------------------------------------------
// Purpose:
//------------------------------------------------------------------------------
void CTriggerWind::StartTouch(CBaseEntity *pOther)
{
	if ( !PassesTriggerFilters(pOther) )
		return;
	if ( pOther->IsPlayer() )
		return;

	IPhysicsObject *pPhys = pOther->VPhysicsGetObject();
	if ( pPhys)
	{
		m_pWindController->AttachObject( pPhys, false );
		pPhys->Wake();
	}
}

//------------------------------------------------------------------------------
// Purpose:
//------------------------------------------------------------------------------
void CTriggerWind::EndTouch(CBaseEntity *pOther)
{
	if ( !PassesTriggerFilters(pOther) )
		return;
	if ( pOther->IsPlayer() )
		return;

	IPhysicsObject *pPhys = pOther->VPhysicsGetObject();
	if ( pPhys && m_pWindController )
	{
		m_pWindController->DetachObject( pPhys );
	}
}

//------------------------------------------------------------------------------
// Purpose:
//------------------------------------------------------------------------------
void CTriggerWind::InputEnable( inputdata_t &inputdata )
{
	BaseClass::InputEnable( inputdata );
	SetContextThink( &CTriggerWind::WindThink, gpGlobals->curtime + 0.1f, WIND_THINK_CONTEXT );
}

//------------------------------------------------------------------------------
// Purpose:
//------------------------------------------------------------------------------
void CTriggerWind::WindThink( void )
{
	// By default...
	SetContextThink( &CTriggerWind::WindThink, gpGlobals->curtime + 0.1, WIND_THINK_CONTEXT );

	// Is it time to change the wind?
	if (m_bSwitch)
	{
		m_bSwitch = false;

		// Set new target direction and speed
		m_nSpeedTarget = m_nSpeedBase + random->RandomInt( -m_nSpeedNoise, m_nSpeedNoise );
		m_nDirTarget = UTIL_AngleMod( m_nDirBase + random->RandomInt(-m_nDirNoise, m_nDirNoise) );
	}
	else
	{
		bool bDone = true;
		// either ramp up, or sleep till change
		if (abs(m_nSpeedTarget - m_nSpeedCurrent) > MAX_WIND_CHANGE)
		{
			m_nSpeedCurrent += (m_nSpeedTarget > m_nSpeedCurrent) ? MAX_WIND_CHANGE : -MAX_WIND_CHANGE;
			bDone = false;
		}

		if (abs(m_nDirTarget - m_nDirCurrent) > MAX_WIND_CHANGE)
		{

			m_nDirCurrent = UTIL_ApproachAngle( m_nDirTarget, m_nDirCurrent, MAX_WIND_CHANGE );
			bDone = false;
		}
		
		if (bDone)
		{
			m_nSpeedCurrent = m_nSpeedTarget;
			SetContextThink( &CTriggerWind::WindThink, m_nHoldBase + random->RandomFloat(-m_nHoldNoise,m_nHoldNoise), WIND_THINK_CONTEXT );
			m_bSwitch = true;
		}
	}

	// If we're starting to blow, where we weren't before, wake up all our objects
	if ( m_nSpeedCurrent )
	{
		m_pWindController->WakeObjects();
	}

	// store the wind data in the controller callback
	m_WindCallback.m_nWindYaw = m_nDirCurrent;
	if ( m_bDisabled )
	{
		m_WindCallback.m_flWindSpeed = 0;
	}
	else
	{
		m_WindCallback.m_flWindSpeed = m_nSpeedCurrent;
	}
}


//------------------------------------------------------------------------------
// Purpose:
//------------------------------------------------------------------------------
void CTriggerWind::InputSetSpeed( inputdata_t &inputdata )
{
	// Set new speed and mark to switch
	m_nSpeedBase = inputdata.value.Int();
	m_bSwitch = true;
}


//-----------------------------------------------------------------------------
// Purpose: Draw any debug text overlays
// Output : Current text offset from the top
//-----------------------------------------------------------------------------
int CTriggerWind::DrawDebugTextOverlays(void) 
{
	int text_offset = BaseClass::DrawDebugTextOverlays();

	if (m_debugOverlays & OVERLAY_TEXT_BIT) 
	{
		// --------------
		// Print Target
		// --------------
		char tempstr[255];
		Q_snprintf(tempstr,sizeof(tempstr),"Dir: %i (%i)",m_nDirCurrent,m_nDirTarget);
		EntityText(text_offset,tempstr,0);
		text_offset++;

		Q_snprintf(tempstr,sizeof(tempstr),"Speed: %i (%i)",m_nSpeedCurrent,m_nSpeedTarget);
		EntityText(text_offset,tempstr,0);
		text_offset++;
	}
	return text_offset;
}
