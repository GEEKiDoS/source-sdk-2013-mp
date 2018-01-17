//========= Copyright Valve Corporation, All rights reserved. ============//

#include "cbase.h"

#include "entityinput.h"
#include "eventqueue.h"

#include "entities/logic/CLogicCompareInteger.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS( logic_multicompare, CLogicCompareInteger );


BEGIN_DATADESC( CLogicCompareInteger )

	DEFINE_OUTPUT( m_OnEqual, "OnEqual" ),
	DEFINE_OUTPUT( m_OnNotEqual, "OnNotEqual" ),

	DEFINE_KEYFIELD( m_iIntegerValue, FIELD_INTEGER, "IntegerValue" ),
	DEFINE_KEYFIELD( m_iShouldCompareToValue, FIELD_INTEGER, "ShouldComparetoValue" ),

	DEFINE_FIELD( m_AllIntCompares, FIELD_INPUT ),

	DEFINE_INPUTFUNC( FIELD_INPUT, "InputValue", InputValue ),
	DEFINE_INPUTFUNC( FIELD_INPUT, "CompareValues", InputCompareValues ),

END_DATADESC()




//-----------------------------------------------------------------------------
// Purpose: Adds to the list of compared values
//-----------------------------------------------------------------------------
void CLogicCompareInteger::InputValue( inputdata_t &inputdata )
{
	// make sure it's an int, if it can't be converted just throw it away
	if ( !inputdata.value.Convert(FIELD_INTEGER) )
		return;

	// update the value list with the new value
	m_AllIntCompares.AddValue( inputdata.value, inputdata.nOutputID );

	// if we haven't already this frame, send a message to ourself to update and fire
	if ( !m_AllIntCompares.m_bUpdatedThisFrame )
	{
		// TODO: need to add this event with a lower priority, so it gets called after all inputs have arrived
		g_EventQueue.AddEvent( this, "CompareValues", 0, inputdata.pActivator, this, inputdata.nOutputID );
		m_AllIntCompares.m_bUpdatedThisFrame = TRUE;
	}
}


//-----------------------------------------------------------------------------
// Purpose: Forces a recompare
//-----------------------------------------------------------------------------
void CLogicCompareInteger::InputCompareValues( inputdata_t &inputdata )
{
	m_AllIntCompares.m_bUpdatedThisFrame = FALSE;

	// loop through all the values comparing them
	int value = m_iIntegerValue;
	CMultiInputVar::inputitem_t *input = m_AllIntCompares.m_InputList;

	if ( !m_iShouldCompareToValue && input )
	{
		value = input->value.Int();
	}

	while ( input )
	{
		if ( input->value.Int() != value )
		{
			// false
			m_OnNotEqual.FireOutput( inputdata.pActivator, this );
			return;
		}

		input = input->next;
	}

	// true! all values equal
	m_OnEqual.FireOutput( inputdata.pActivator, this );
}
