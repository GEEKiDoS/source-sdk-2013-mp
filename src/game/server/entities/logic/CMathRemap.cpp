//========= Copyright Valve Corporation, All rights reserved. ============//

#include "cbase.h"

#include "entities/logic/CMathRemap.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS(math_remap, CMathRemap);


BEGIN_DATADESC( CMathRemap )

	DEFINE_INPUTFUNC(FIELD_FLOAT, "InValue", InputValue ),

	DEFINE_OUTPUT(m_OutValue, "OutValue"),

	DEFINE_KEYFIELD(m_flInMin, FIELD_FLOAT, "in1"),
	DEFINE_KEYFIELD(m_flInMax, FIELD_FLOAT, "in2"),
	DEFINE_KEYFIELD(m_flOut1, FIELD_FLOAT, "out1"),
	DEFINE_KEYFIELD(m_flOut2, FIELD_FLOAT, "out2"),

	DEFINE_FIELD( m_bEnabled, FIELD_BOOLEAN ),

	DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),

END_DATADESC()




//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CMathRemap::Spawn(void)
{
	//
	// Avoid a divide by zero in ValueChanged.
	//
	if (m_flInMin == m_flInMax)
	{
		m_flInMin = 0;
		m_flInMax = 1;
	}

	//
	// Make sure min and max are set properly relative to one another.
	//
	if (m_flInMin > m_flInMax)
	{
		float flTemp = m_flInMin;
		m_flInMin = m_flInMax;
		m_flInMax = flTemp;
	}

	m_bEnabled = true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CMathRemap::InputEnable( inputdata_t &inputdata )
{
	m_bEnabled = true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CMathRemap::InputDisable( inputdata_t &inputdata )
{
	m_bEnabled = false;
}

//-----------------------------------------------------------------------------
// Purpose: Input handler that is called when the input value changes.
//-----------------------------------------------------------------------------
void CMathRemap::InputValue( inputdata_t &inputdata )
{
	float flValue = inputdata.value.Float();

	//
	// Disallow out-of-range input values to avoid out-of-range output values.
	//
	float flClampValue = clamp(flValue, m_flInMin, m_flInMax);

	if ((flClampValue == flValue) || !FBitSet(m_spawnflags, SF_MATH_REMAP_IGNORE_OUT_OF_RANGE))
	{
		//
		// Remap the input value to the desired output range and update the output.
		//
		float flRemappedValue = m_flOut1 + (((flValue - m_flInMin) * (m_flOut2 - m_flOut1)) / (m_flInMax - m_flInMin));

		if ( FBitSet( m_spawnflags, SF_MATH_REMAP_CLAMP_OUTPUT_TO_RANGE ) )
		{
			flRemappedValue = clamp( flRemappedValue, m_flOut1, m_flOut2 );
		}

		if ( m_bEnabled == true )
		{
			m_OutValue.Set(flRemappedValue, inputdata.pActivator, this);
		}
	}
}
