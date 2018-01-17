//========= Copyright Valve Corporation, All rights reserved. ============//

#include "cbase.h"

#include "entities/logic/CMathColorBlend.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS(math_colorblend, CMathColorBlend);


BEGIN_DATADESC( CMathColorBlend )

	DEFINE_INPUTFUNC(FIELD_FLOAT, "InValue", InputValue ),

	DEFINE_OUTPUT(m_OutValue, "OutColor"),

	DEFINE_KEYFIELD(m_flInMin, FIELD_FLOAT, "inmin"),
	DEFINE_KEYFIELD(m_flInMax, FIELD_FLOAT, "inmax"),
	DEFINE_KEYFIELD(m_OutColor1, FIELD_COLOR32, "colormin"),
	DEFINE_KEYFIELD(m_OutColor2, FIELD_COLOR32, "colormax"),

END_DATADESC()




//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CMathColorBlend::Spawn(void)
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
}


//-----------------------------------------------------------------------------
// Purpose: Input handler that is called when the input value changes.
//-----------------------------------------------------------------------------
void CMathColorBlend::InputValue( inputdata_t &inputdata )
{
	float flValue = inputdata.value.Float();

	//
	// Disallow out-of-range input values to avoid out-of-range output values.
	//
	float flClampValue = clamp(flValue, m_flInMin, m_flInMax);
	if ((flClampValue == flValue) || !FBitSet(m_spawnflags, SF_COLOR_BLEND_IGNORE_OUT_OF_RANGE))
	{
		//
		// Remap the input value to the desired output color and update the output.
		//
		color32 Color;
		Color.r = m_OutColor1.r + (((flClampValue - m_flInMin) * (m_OutColor2.r - m_OutColor1.r)) / (m_flInMax - m_flInMin));
		Color.g = m_OutColor1.g + (((flClampValue - m_flInMin) * (m_OutColor2.g - m_OutColor1.g)) / (m_flInMax - m_flInMin));
		Color.b = m_OutColor1.b + (((flClampValue - m_flInMin) * (m_OutColor2.b - m_OutColor1.b)) / (m_flInMax - m_flInMin));
		Color.a = m_OutColor1.a + (((flClampValue - m_flInMin) * (m_OutColor2.a - m_OutColor1.a)) / (m_flInMax - m_flInMin));

		m_OutValue.Set(Color, inputdata.pActivator, this);
	}
}
