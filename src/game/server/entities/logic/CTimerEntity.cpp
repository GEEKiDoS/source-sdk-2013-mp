//========= Copyright Valve Corporation, All rights reserved. ============//

#include "cbase.h"

#include "entities/logic/CTimerEntity.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS( logic_timer, CTimerEntity );


BEGIN_DATADESC( CTimerEntity )

	// Keys
	DEFINE_KEYFIELD( m_iDisabled, FIELD_INTEGER, "StartDisabled" ),
	DEFINE_KEYFIELD( m_flRefireTime, FIELD_FLOAT, "RefireTime" ),

	DEFINE_FIELD( m_bUpDownState, FIELD_BOOLEAN ),

	// Inputs
	DEFINE_INPUTFUNC( FIELD_FLOAT, "RefireTime", InputRefireTime ),
	DEFINE_INPUTFUNC( FIELD_VOID, "FireTimer", InputFireTimer ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Toggle", InputToggle ),
	DEFINE_INPUTFUNC( FIELD_FLOAT, "AddToTimer", InputAddToTimer ),
	DEFINE_INPUTFUNC( FIELD_VOID, "ResetTimer", InputResetTimer ),
	DEFINE_INPUTFUNC( FIELD_FLOAT, "SubtractFromTimer", InputSubtractFromTimer ),

	DEFINE_INPUT( m_iUseRandomTime, FIELD_INTEGER, "UseRandomTime" ),
	DEFINE_INPUT( m_flLowerRandomBound, FIELD_FLOAT, "LowerRandomBound" ),
	DEFINE_INPUT( m_flUpperRandomBound, FIELD_FLOAT, "UpperRandomBound" ),


	// Outputs
	DEFINE_OUTPUT( m_OnTimer, "OnTimer" ),
	DEFINE_OUTPUT( m_OnTimerHigh, "OnTimerHigh" ),
	DEFINE_OUTPUT( m_OnTimerLow, "OnTimerLow" ),

END_DATADESC()



//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTimerEntity::Spawn( void )
{
	if (!m_iUseRandomTime && (m_flRefireTime < LOGIC_TIMER_MIN_INTERVAL))
	{
		m_flRefireTime = LOGIC_TIMER_MIN_INTERVAL;
	}

	if ( !m_iDisabled && (m_flRefireTime > 0 || m_iUseRandomTime) )
	{
		Enable();
	}
	else
	{
		Disable();
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTimerEntity::Think( void )
{
	FireTimer();
}


//-----------------------------------------------------------------------------
// Purpose: Sets the time the timerentity will next fire
//-----------------------------------------------------------------------------
void CTimerEntity::ResetTimer( void )
{
	if ( m_iDisabled )
		return;

	if ( m_iUseRandomTime )
	{
		m_flRefireTime = random->RandomFloat( m_flLowerRandomBound, m_flUpperRandomBound );
	}

	SetNextThink( gpGlobals->curtime + m_flRefireTime );
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTimerEntity::Enable( void )
{
	m_iDisabled = FALSE;
	ResetTimer();
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTimerEntity::Disable( void )
{
	m_iDisabled = TRUE;
	SetNextThink( TICK_NEVER_THINK );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTimerEntity::Toggle( void )
{
	if ( m_iDisabled )
	{
		Enable();
	}
	else
	{
		Disable();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTimerEntity::FireTimer( void )
{
	if ( !m_iDisabled )
	{
		//
		// Up/down timers alternate between two outputs.
		//
		if (m_spawnflags & SF_TIMER_UPDOWN)
		{
			if (m_bUpDownState)
			{
				m_OnTimerHigh.FireOutput( this, this );
			}
			else
			{
				m_OnTimerLow.FireOutput( this, this );
			}

			m_bUpDownState = !m_bUpDownState;
		}
		//
		// Regular timers only fire a single output.
		//
		else
		{
			m_OnTimer.FireOutput( this, this );
		}

		ResetTimer();
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTimerEntity::InputEnable( inputdata_t &inputdata )
{
	Enable();
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTimerEntity::InputDisable( inputdata_t &inputdata )
{
	Disable();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTimerEntity::InputToggle( inputdata_t &inputdata )
{
	Toggle();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTimerEntity::InputFireTimer( inputdata_t &inputdata )
{
	FireTimer();
}


//-----------------------------------------------------------------------------
// Purpose: Changes the time interval between timer fires
//			Resets the next firing to be time + newRefireTime
// Input  : Float refire frequency in seconds.
//-----------------------------------------------------------------------------
void CTimerEntity::InputRefireTime( inputdata_t &inputdata )
{
	float flRefireInterval = inputdata.value.Float();

	if ( flRefireInterval < LOGIC_TIMER_MIN_INTERVAL)
	{
		flRefireInterval = LOGIC_TIMER_MIN_INTERVAL;
	}

	if (m_flRefireTime != flRefireInterval )
	{
		m_flRefireTime = flRefireInterval;
		ResetTimer();
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CTimerEntity::InputResetTimer( inputdata_t &inputdata )
{
	// don't reset the timer if it isn't enabled
	if ( m_iDisabled )
		return;

	ResetTimer();
}


//-----------------------------------------------------------------------------
// Purpose: Adds to the time interval if the timer is enabled
// Input  : Float time to add in seconds
//-----------------------------------------------------------------------------
void CTimerEntity::InputAddToTimer( inputdata_t &inputdata )
{
	// don't add time if the timer isn't enabled
	if ( m_iDisabled )
		return;
	
	// Add time to timer
 	float flNextThink = GetNextThink();	
	SetNextThink( flNextThink += inputdata.value.Float() );
}

//-----------------------------------------------------------------------------
// Purpose: Subtract from the time interval if the timer is enabled
// Input  : Float time to subtract in seconds
//-----------------------------------------------------------------------------
void CTimerEntity::InputSubtractFromTimer( inputdata_t &inputdata )
{
	// don't add time if the timer isn't enabled
	if ( m_iDisabled )
		return;

	// Subtract time from the timer but don't let the timer go negative
	float flNextThink = GetNextThink();
	if ( ( flNextThink - gpGlobals->curtime ) <= inputdata.value.Float() )
	{
		SetNextThink( gpGlobals->curtime );
	}
	else
	{
		SetNextThink( flNextThink -= inputdata.value.Float() );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Draw any debug text overlays
// Output : Current text offset from the top
//-----------------------------------------------------------------------------
int CTimerEntity::DrawDebugTextOverlays( void ) 
{
	int text_offset = BaseClass::DrawDebugTextOverlays();

	if (m_debugOverlays & OVERLAY_TEXT_BIT) 
	{
		char tempstr[512];

		// print refire time
		Q_snprintf(tempstr,sizeof(tempstr),"refire interval: %.2f sec", m_flRefireTime);
		EntityText(text_offset,tempstr,0);
		text_offset++;

		// print seconds to next fire
		if ( !m_iDisabled )
		{
			float flNextThink = GetNextThink();
			Q_snprintf( tempstr, sizeof( tempstr ), "      firing in: %.2f sec", flNextThink - gpGlobals->curtime );
			EntityText( text_offset, tempstr, 0);
			text_offset++;
		}
	}
	return text_offset;
}
