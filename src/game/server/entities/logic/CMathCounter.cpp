//========= Copyright Valve Corporation, All rights reserved. ============//

#include "cbase.h"

#include "entities/logic/CMathCounter.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS(math_counter, CMathCounter);


BEGIN_DATADESC( CMathCounter )

	DEFINE_FIELD(m_bHitMax, FIELD_BOOLEAN),
	DEFINE_FIELD(m_bHitMin, FIELD_BOOLEAN),

	// Keys
	DEFINE_KEYFIELD(m_flMin, FIELD_FLOAT, "min"),
	DEFINE_KEYFIELD(m_flMax, FIELD_FLOAT, "max"),

	DEFINE_KEYFIELD(m_bDisabled, FIELD_BOOLEAN, "StartDisabled" ),

	// Inputs
	DEFINE_INPUTFUNC(FIELD_FLOAT, "Add", InputAdd),
	DEFINE_INPUTFUNC(FIELD_FLOAT, "Divide", InputDivide),
	DEFINE_INPUTFUNC(FIELD_FLOAT, "Multiply", InputMultiply),
	DEFINE_INPUTFUNC(FIELD_FLOAT, "SetValue", InputSetValue),
	DEFINE_INPUTFUNC(FIELD_FLOAT, "SetValueNoFire", InputSetValueNoFire),
	DEFINE_INPUTFUNC(FIELD_FLOAT, "Subtract", InputSubtract),
	DEFINE_INPUTFUNC(FIELD_FLOAT, "SetHitMax", InputSetHitMax),
	DEFINE_INPUTFUNC(FIELD_FLOAT, "SetHitMin", InputSetHitMin),
	DEFINE_INPUTFUNC(FIELD_VOID, "GetValue", InputGetValue),
	DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),

	// Outputs
	DEFINE_OUTPUT(m_OutValue, "OutValue"),
	DEFINE_OUTPUT(m_OnHitMin, "OnHitMin"),
	DEFINE_OUTPUT(m_OnHitMax, "OnHitMax"),
	DEFINE_OUTPUT(m_OnGetValue, "OnGetValue"),

END_DATADESC()




//-----------------------------------------------------------------------------
// Purpose: Handles key values from the BSP before spawn is called.
//-----------------------------------------------------------------------------
bool CMathCounter::KeyValue(const char *szKeyName, const char *szValue)
{
	//
	// Set the initial value of the counter.
	//
	if (!stricmp(szKeyName, "startvalue"))
	{
		m_OutValue.Init(atoi(szValue));
		return(true);
	}

	return(BaseClass::KeyValue(szKeyName, szValue));
}


//-----------------------------------------------------------------------------
// Purpose: Called before spawning, after key values have been set.
//-----------------------------------------------------------------------------
void CMathCounter::Spawn( void )
{
	//
	// Make sure max and min are ordered properly or clamp won't work.
	//
	if (m_flMin > m_flMax)
	{
		float flTemp = m_flMax;
		m_flMax = m_flMin;
		m_flMin = flTemp;
	}

	//
	// Clamp initial value to within the valid range.
	//
	if ((m_flMin != 0) || (m_flMax != 0))
	{
		float flStartValue = clamp(m_OutValue.Get(), m_flMin, m_flMax);
		m_OutValue.Init(flStartValue);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Draw any debug text overlays
// Input  :
// Output : Current text offset from the top
//-----------------------------------------------------------------------------
int CMathCounter::DrawDebugTextOverlays( void ) 
{
	int text_offset = BaseClass::DrawDebugTextOverlays();

	if (m_debugOverlays & OVERLAY_TEXT_BIT) 
	{
		char tempstr[512];

		Q_snprintf(tempstr,sizeof(tempstr),"    min value: %f", m_flMin);
		EntityText(text_offset,tempstr,0);
		text_offset++;

		Q_snprintf(tempstr,sizeof(tempstr),"    max value: %f", m_flMax);
		EntityText(text_offset,tempstr,0);
		text_offset++;

		Q_snprintf(tempstr,sizeof(tempstr),"current value: %f", m_OutValue.Get());
		EntityText(text_offset,tempstr,0);
		text_offset++;

		if( m_bDisabled )
		{	
			Q_snprintf(tempstr,sizeof(tempstr),"*DISABLED*");		
		}
		else
		{
			Q_snprintf(tempstr,sizeof(tempstr),"Enabled.");
		}
		EntityText(text_offset,tempstr,0);
		text_offset++;

	}
	return text_offset;
}

//-----------------------------------------------------------------------------
// Change min/max
//-----------------------------------------------------------------------------
void CMathCounter::InputSetHitMax( inputdata_t &inputdata )
{
	m_flMax = inputdata.value.Float();
	if ( m_flMax < m_flMin )
	{
		m_flMin = m_flMax;
	}
	UpdateOutValue( inputdata.pActivator, m_OutValue.Get() );
}

void CMathCounter::InputSetHitMin( inputdata_t &inputdata )
{
	m_flMin = inputdata.value.Float();
	if ( m_flMax < m_flMin )
	{
		m_flMax = m_flMin;
	}
	UpdateOutValue( inputdata.pActivator, m_OutValue.Get() );
}

	
//-----------------------------------------------------------------------------
// Purpose: Input handler for adding to the accumulator value.
// Input  : Float value to add.
//-----------------------------------------------------------------------------
void CMathCounter::InputAdd( inputdata_t &inputdata )
{
	if( m_bDisabled )
	{
		DevMsg("Math Counter %s ignoring ADD because it is disabled\n", GetDebugName() );
		return;
	}

	float fNewValue = m_OutValue.Get() + inputdata.value.Float();
	UpdateOutValue( inputdata.pActivator, fNewValue );
}


//-----------------------------------------------------------------------------
// Purpose: Input handler for multiplying the current value.
// Input  : Float value to multiply the value by.
//-----------------------------------------------------------------------------
void CMathCounter::InputDivide( inputdata_t &inputdata )
{
	if( m_bDisabled )
	{
		DevMsg("Math Counter %s ignoring DIVIDE because it is disabled\n", GetDebugName() );
		return;
	}

	if (inputdata.value.Float() != 0)
	{
		float fNewValue = m_OutValue.Get() / inputdata.value.Float();
		UpdateOutValue( inputdata.pActivator, fNewValue );
	}
	else
	{
		DevMsg( 1, "LEVEL DESIGN ERROR: Divide by zero in math_value\n" );
		UpdateOutValue( inputdata.pActivator, m_OutValue.Get() );
	}
}


//-----------------------------------------------------------------------------
// Purpose: Input handler for multiplying the current value.
// Input  : Float value to multiply the value by.
//-----------------------------------------------------------------------------
void CMathCounter::InputMultiply( inputdata_t &inputdata )
{
	if( m_bDisabled )
	{
		DevMsg("Math Counter %s ignoring MULTIPLY because it is disabled\n", GetDebugName() );
		return;
	}

	float fNewValue = m_OutValue.Get() * inputdata.value.Float();
	UpdateOutValue( inputdata.pActivator, fNewValue );
}


//-----------------------------------------------------------------------------
// Purpose: Input handler for updating the value.
// Input  : Float value to set.
//-----------------------------------------------------------------------------
void CMathCounter::InputSetValue( inputdata_t &inputdata )
{
	if( m_bDisabled )
	{
		DevMsg("Math Counter %s ignoring SETVALUE because it is disabled\n", GetDebugName() );
		return;
	}

	UpdateOutValue( inputdata.pActivator, inputdata.value.Float() );
}


//-----------------------------------------------------------------------------
// Purpose: Input handler for updating the value.
// Input  : Float value to set.
//-----------------------------------------------------------------------------
void CMathCounter::InputSetValueNoFire( inputdata_t &inputdata )
{
	if( m_bDisabled )
	{
		DevMsg("Math Counter %s ignoring SETVALUENOFIRE because it is disabled\n", GetDebugName() );
		return;
	}

	float flNewValue = inputdata.value.Float();
	if (( m_flMin != 0 ) || (m_flMax != 0 ))
	{
		flNewValue = clamp(flNewValue, m_flMin, m_flMax);
	}

	m_OutValue.Init( flNewValue );
}


//-----------------------------------------------------------------------------
// Purpose: Input handler for subtracting from the current value.
// Input  : Float value to subtract.
//-----------------------------------------------------------------------------
void CMathCounter::InputSubtract( inputdata_t &inputdata )
{
	if( m_bDisabled )
	{
		DevMsg("Math Counter %s ignoring SUBTRACT because it is disabled\n", GetDebugName() );
		return;
	}

	float fNewValue = m_OutValue.Get() - inputdata.value.Float();
	UpdateOutValue( inputdata.pActivator, fNewValue );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CMathCounter::InputGetValue( inputdata_t &inputdata )
{
	float flOutValue = m_OutValue.Get();
	m_OnGetValue.Set( flOutValue, inputdata.pActivator, inputdata.pCaller );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CMathCounter::InputEnable( inputdata_t &inputdata )
{
	m_bDisabled = false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CMathCounter::InputDisable( inputdata_t &inputdata )
{
	m_bDisabled = true;
}

//-----------------------------------------------------------------------------
// Purpose: Sets the value to the new value, clamping and firing the output value.
// Input  : fNewValue - Value to set.
//-----------------------------------------------------------------------------
void CMathCounter::UpdateOutValue(CBaseEntity *pActivator, float fNewValue)
{
	if ((m_flMin != 0) || (m_flMax != 0))
	{
		//
		// Fire an output any time we reach or exceed our maximum value.
		//
		if ( fNewValue >= m_flMax )
		{
			if ( !m_bHitMax )
			{
				m_bHitMax = true;
				m_OnHitMax.FireOutput( pActivator, this );
			}
		}
		else
		{
			m_bHitMax = false;
		}

		//
		// Fire an output any time we reach or go below our minimum value.
		//
		if ( fNewValue <= m_flMin )
		{
			if ( !m_bHitMin )
			{
				m_bHitMin = true;
				m_OnHitMin.FireOutput( pActivator, this );
			}
		}
		else
		{
			m_bHitMin = false;
		}

		fNewValue = clamp(fNewValue, m_flMin, m_flMax);
	}

	m_OutValue.Set(fNewValue, pActivator, this);
}
