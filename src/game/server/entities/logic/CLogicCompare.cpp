//========= Copyright Valve Corporation, All rights reserved. ============//

#include "cbase.h"

#include "entities/logic/CLogicCompare.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS(logic_compare, CLogicCompare);


BEGIN_DATADESC( CLogicCompare )

	// Keys
	DEFINE_KEYFIELD(m_flCompareValue, FIELD_FLOAT, "CompareValue"),
	DEFINE_KEYFIELD(m_flInValue, FIELD_FLOAT, "InitialValue"),

	// Inputs
	DEFINE_INPUTFUNC(FIELD_FLOAT, "SetValue", InputSetValue),
	DEFINE_INPUTFUNC(FIELD_FLOAT, "SetValueCompare", InputSetValueCompare),
	DEFINE_INPUTFUNC(FIELD_FLOAT, "SetCompareValue", InputSetCompareValue),
	DEFINE_INPUTFUNC(FIELD_VOID, "Compare", InputCompare),

	// Outputs
	DEFINE_OUTPUT(m_OnEqualTo, "OnEqualTo"),
	DEFINE_OUTPUT(m_OnNotEqualTo, "OnNotEqualTo"),
	DEFINE_OUTPUT(m_OnGreaterThan, "OnGreaterThan"),
	DEFINE_OUTPUT(m_OnLessThan, "OnLessThan"),

END_DATADESC()




//-----------------------------------------------------------------------------
// Purpose: Input handler for a new input value without performing a comparison.
//-----------------------------------------------------------------------------
void CLogicCompare::InputSetValue( inputdata_t &inputdata )
{
	m_flInValue = inputdata.value.Float();
}


//-----------------------------------------------------------------------------
// Purpose: Input handler for a setting a new value and doing the comparison.
//-----------------------------------------------------------------------------
void CLogicCompare::InputSetValueCompare( inputdata_t &inputdata )
{
	m_flInValue = inputdata.value.Float();
	DoCompare( inputdata.pActivator, m_flInValue );
}

//-----------------------------------------------------------------------------
// Purpose: Input handler for a new input value without performing a comparison.
//-----------------------------------------------------------------------------
void CLogicCompare::InputSetCompareValue( inputdata_t &inputdata )
{
	m_flCompareValue = inputdata.value.Float();
}

//-----------------------------------------------------------------------------
// Purpose: Input handler for forcing a recompare of the last input value.
//-----------------------------------------------------------------------------
void CLogicCompare::InputCompare( inputdata_t &inputdata )
{
	DoCompare( inputdata.pActivator, m_flInValue );
}


//-----------------------------------------------------------------------------
// Purpose: Compares the input value to the compare value, firing the appropriate
//			output(s) based on the comparison result.
// Input  : flInValue - Value to compare against the comparison value.
//-----------------------------------------------------------------------------
void CLogicCompare::DoCompare(CBaseEntity *pActivator, float flInValue)
{
	if (flInValue == m_flCompareValue)
	{
		m_OnEqualTo.Set(flInValue, pActivator, this);
	}
	else
	{
		m_OnNotEqualTo.Set(flInValue, pActivator, this);

		if (flInValue > m_flCompareValue)
		{
			m_OnGreaterThan.Set(flInValue, pActivator, this);
		}
		else
		{
			m_OnLessThan.Set(flInValue, pActivator, this);
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Draw any debug text overlays
// Output : Current text offset from the top
//-----------------------------------------------------------------------------
int CLogicCompare::DrawDebugTextOverlays( void ) 
{
	int text_offset = BaseClass::DrawDebugTextOverlays();

	if (m_debugOverlays & OVERLAY_TEXT_BIT) 
	{
		char tempstr[512];

		// print duration
		Q_snprintf(tempstr,sizeof(tempstr),"    Initial Value: %f", m_flInValue);
		EntityText(text_offset,tempstr,0);
		text_offset++;

		// print hold time
		Q_snprintf(tempstr,sizeof(tempstr),"    Compare Value: %f", m_flCompareValue);
		EntityText(text_offset,tempstr,0);
		text_offset++;
	}
	return text_offset;
}
