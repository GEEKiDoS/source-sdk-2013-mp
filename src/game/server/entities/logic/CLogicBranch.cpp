//========= Copyright Valve Corporation, All rights reserved. ============//

#include "cbase.h"

#include "eventqueue.h"
#include "saverestore_utlvector.h"

#include "entities/logic/CLogicBranch.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS(logic_branch, CLogicBranch);


BEGIN_DATADESC( CLogicBranch )

	// Keys
	DEFINE_KEYFIELD(m_bInValue, FIELD_BOOLEAN, "InitialValue"),

	DEFINE_UTLVECTOR( m_Listeners, FIELD_EHANDLE ),

	// Inputs
	DEFINE_INPUTFUNC(FIELD_BOOLEAN, "SetValue", InputSetValue),
	DEFINE_INPUTFUNC(FIELD_BOOLEAN, "SetValueTest", InputSetValueTest),
	DEFINE_INPUTFUNC(FIELD_VOID, "Toggle", InputToggle),
	DEFINE_INPUTFUNC(FIELD_VOID, "ToggleTest", InputToggleTest),
	DEFINE_INPUTFUNC(FIELD_VOID, "Test", InputTest),

	// Outputs
	DEFINE_OUTPUT(m_OnTrue, "OnTrue"),
	DEFINE_OUTPUT(m_OnFalse, "OnFalse"),

END_DATADESC()


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CLogicBranch::UpdateOnRemove()
{
	for ( int i = 0; i < m_Listeners.Count(); i++ )
	{
		CBaseEntity *pEntity = m_Listeners.Element( i ).Get();
		if ( pEntity )
		{
			g_EventQueue.AddEvent( this, "_OnLogicBranchRemoved", 0, this, this );
		}
	}
	
	BaseClass::UpdateOnRemove();
}
	

//-----------------------------------------------------------------------------
// Purpose: Input handler to set a new input value without firing outputs.
// Input  : Boolean value to set.
//-----------------------------------------------------------------------------
void CLogicBranch::InputSetValue( inputdata_t &inputdata )
{
	UpdateValue( inputdata.value.Bool(), inputdata.pActivator, LOGIC_BRANCH_NO_FIRE );
}


//-----------------------------------------------------------------------------
// Purpose: Input handler to set a new input value and fire appropriate outputs.
// Input  : Boolean value to set.
//-----------------------------------------------------------------------------
void CLogicBranch::InputSetValueTest( inputdata_t &inputdata )
{
	UpdateValue( inputdata.value.Bool(), inputdata.pActivator, LOGIC_BRANCH_FIRE );
}


//-----------------------------------------------------------------------------
// Purpose: Input handler for toggling the boolean value without firing outputs.
//-----------------------------------------------------------------------------
void CLogicBranch::InputToggle( inputdata_t &inputdata )
{
	UpdateValue( !m_bInValue, inputdata.pActivator, LOGIC_BRANCH_NO_FIRE );
}


//-----------------------------------------------------------------------------
// Purpose: Input handler for toggling the boolean value and then firing the
//			appropriate output based on the new value.
//-----------------------------------------------------------------------------
void CLogicBranch::InputToggleTest( inputdata_t &inputdata )
{
	UpdateValue( !m_bInValue, inputdata.pActivator, LOGIC_BRANCH_FIRE );
}


//-----------------------------------------------------------------------------
// Purpose: Input handler for forcing a test of the last input value.
//-----------------------------------------------------------------------------
void CLogicBranch::InputTest( inputdata_t &inputdata )
{
	UpdateValue( m_bInValue, inputdata.pActivator, LOGIC_BRANCH_FIRE );
}


//-----------------------------------------------------------------------------
// Purpose: Tests the last input value, firing the appropriate output based on
//			the test result.
// Input  : bInValue - 
//-----------------------------------------------------------------------------
void CLogicBranch::UpdateValue( bool bNewValue, CBaseEntity *pActivator, LogicBranchFire_t eFire )
{
	if ( m_bInValue != bNewValue )
	{
		m_bInValue = bNewValue;

		for ( int i = 0; i < m_Listeners.Count(); i++ )
		{
			CBaseEntity *pEntity = m_Listeners.Element( i ).Get();
			if ( pEntity )
			{
				g_EventQueue.AddEvent( pEntity, "_OnLogicBranchChanged", 0, this, this );
			}
		}
	}

	if ( eFire == LOGIC_BRANCH_FIRE )
	{
		if ( m_bInValue )
		{
			m_OnTrue.FireOutput( pActivator, this );
		}
		else
		{
			m_OnFalse.FireOutput( pActivator, this );
		}
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CLogicBranch::AddLogicBranchListener( CBaseEntity *pEntity )
{
	if ( m_Listeners.Find( pEntity ) == -1 )
	{
		m_Listeners.AddToTail( pEntity );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CLogicBranch::DrawDebugTextOverlays( void )
{
	int text_offset = BaseClass::DrawDebugTextOverlays();

	if (m_debugOverlays & OVERLAY_TEXT_BIT) 
	{
		char tempstr[512];

		// print refire time
		Q_snprintf( tempstr, sizeof(tempstr), "Branch value: %s", (m_bInValue) ? "TRUE" : "FALSE" );
		EntityText( text_offset, tempstr, 0 );
		text_offset++;
	}

	return text_offset;
}
