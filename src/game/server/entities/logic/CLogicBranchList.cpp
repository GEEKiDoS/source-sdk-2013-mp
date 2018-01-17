//========= Copyright Valve Corporation, All rights reserved. ============//

#include "cbase.h"

#include "saverestore_utlvector.h"

#include "entities/logic/CLogicBranch.h"
#include "entities/logic/CLogicBranchList.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS(logic_branch_listener, CLogicBranchList);


BEGIN_DATADESC( CLogicBranchList )

	// Silence, classcheck!
	//DEFINE_ARRAY( m_nLogicBranchNames, FIELD_STRING, MAX_LOGIC_BRANCH_NAMES ),

	// Keys
	DEFINE_KEYFIELD( m_nLogicBranchNames[0], FIELD_STRING, "Branch01" ),
	DEFINE_KEYFIELD( m_nLogicBranchNames[1], FIELD_STRING, "Branch02" ),
	DEFINE_KEYFIELD( m_nLogicBranchNames[2], FIELD_STRING, "Branch03" ),
	DEFINE_KEYFIELD( m_nLogicBranchNames[3], FIELD_STRING, "Branch04" ),
	DEFINE_KEYFIELD( m_nLogicBranchNames[4], FIELD_STRING, "Branch05" ),
	DEFINE_KEYFIELD( m_nLogicBranchNames[5], FIELD_STRING, "Branch06" ),
	DEFINE_KEYFIELD( m_nLogicBranchNames[6], FIELD_STRING, "Branch07" ),
	DEFINE_KEYFIELD( m_nLogicBranchNames[7], FIELD_STRING, "Branch08" ),
	DEFINE_KEYFIELD( m_nLogicBranchNames[8], FIELD_STRING, "Branch09" ),
	DEFINE_KEYFIELD( m_nLogicBranchNames[9], FIELD_STRING, "Branch10" ),
	DEFINE_KEYFIELD( m_nLogicBranchNames[10], FIELD_STRING, "Branch11" ),
	DEFINE_KEYFIELD( m_nLogicBranchNames[11], FIELD_STRING, "Branch12" ),
	DEFINE_KEYFIELD( m_nLogicBranchNames[12], FIELD_STRING, "Branch13" ),
	DEFINE_KEYFIELD( m_nLogicBranchNames[13], FIELD_STRING, "Branch14" ),
	DEFINE_KEYFIELD( m_nLogicBranchNames[14], FIELD_STRING, "Branch15" ),
	DEFINE_KEYFIELD( m_nLogicBranchNames[15], FIELD_STRING, "Branch16" ),
	
	DEFINE_UTLVECTOR( m_LogicBranchList, FIELD_EHANDLE ),
	
	DEFINE_FIELD( m_eLastState, FIELD_INTEGER ),

	// Inputs
	DEFINE_INPUTFUNC( FIELD_INPUT, "Test", InputTest ),
	DEFINE_INPUTFUNC( FIELD_INPUT, "_OnLogicBranchChanged", Input_OnLogicBranchChanged ),
	DEFINE_INPUTFUNC( FIELD_INPUT, "_OnLogicBranchRemoved", Input_OnLogicBranchRemoved ),

	// Outputs
	DEFINE_OUTPUT( m_OnAllTrue, "OnAllTrue" ),
	DEFINE_OUTPUT( m_OnAllFalse, "OnAllFalse" ),
	DEFINE_OUTPUT( m_OnMixed, "OnMixed" ),

END_DATADESC()


//-----------------------------------------------------------------------------
// Purpose: Called before spawning, after key values have been set.
//-----------------------------------------------------------------------------
void CLogicBranchList::Spawn( void )
{
}


//-----------------------------------------------------------------------------
// Finds all the logic_branches that we are monitoring and register ourselves with them.
//-----------------------------------------------------------------------------
void CLogicBranchList::Activate( void )
{
	for ( int i = 0; i < MAX_LOGIC_BRANCH_NAMES; i++ )
	{
		CBaseEntity *pEntity = NULL;
		while ( ( pEntity = gEntList.FindEntityGeneric( pEntity, STRING( m_nLogicBranchNames[i] ), this ) ) != NULL )
		{
			if ( FClassnameIs( pEntity, "logic_branch" ) )
			{
				CLogicBranch *pBranch = (CLogicBranch *)pEntity;
				pBranch->AddLogicBranchListener( this );
				m_LogicBranchList.AddToTail( pBranch );
			}
			else
			{
				DevWarning( "logic_branchlist %s refers to entity %s, which is not a logic_branch\n", GetDebugName(), pEntity->GetDebugName() );
			}
		}
	}
	
	BaseClass::Activate();
}


//-----------------------------------------------------------------------------
// Called when a monitored logic branch is deleted from the world, since that
// might affect our final result.
//-----------------------------------------------------------------------------
void CLogicBranchList::Input_OnLogicBranchRemoved( inputdata_t &inputdata )
{
	int nIndex = m_LogicBranchList.Find( inputdata.pActivator );
	if ( nIndex != -1 )
	{
		m_LogicBranchList.FastRemove( nIndex );
	}

	// See if this logic_branch's deletion affects the final result.
	DoTest( inputdata.pActivator );
}


//-----------------------------------------------------------------------------
// Called when the value of a monitored logic branch changes.
//-----------------------------------------------------------------------------
void CLogicBranchList::Input_OnLogicBranchChanged( inputdata_t &inputdata )
{
	DoTest( inputdata.pActivator );
}


//-----------------------------------------------------------------------------
// Input handler to manually test the monitored logic branches and fire the
// appropriate output.
//-----------------------------------------------------------------------------
void CLogicBranchList::InputTest( inputdata_t &inputdata )
{
	// Force an output.
	m_eLastState = LOGIC_BRANCH_LISTENER_NOT_INIT;

	DoTest( inputdata.pActivator );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CLogicBranchList::DoTest( CBaseEntity *pActivator )
{
	bool bOneTrue = false;
	bool bOneFalse = false;
	
	for ( int i = 0; i < m_LogicBranchList.Count(); i++ )
	{
		CLogicBranch *pBranch = (CLogicBranch *)m_LogicBranchList.Element( i ).Get();
		if ( pBranch && pBranch->GetLogicBranchState() )
		{
			bOneTrue = true;
		}
		else
		{
			bOneFalse = true;
		}
	}

	// Only fire the output if the new result differs from the last result.
	if ( bOneTrue && !bOneFalse )
	{
		if ( m_eLastState != LOGIC_BRANCH_LISTENER_ALL_TRUE )
		{
			m_OnAllTrue.FireOutput( pActivator, this );
			m_eLastState = LOGIC_BRANCH_LISTENER_ALL_TRUE;
		}
	}
	else if ( bOneFalse && !bOneTrue )
	{
		if ( m_eLastState != LOGIC_BRANCH_LISTENER_ALL_FALSE )
		{
			m_OnAllFalse.FireOutput( pActivator, this );
			m_eLastState = LOGIC_BRANCH_LISTENER_ALL_FALSE;
		}
	}
	else
	{
		if ( m_eLastState != LOGIC_BRANCH_LISTENER_MIXED )
		{
			m_OnMixed.FireOutput( pActivator, this );
			m_eLastState = LOGIC_BRANCH_LISTENER_MIXED;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CLogicBranchList::DrawDebugTextOverlays( void )
{
	int text_offset = BaseClass::DrawDebugTextOverlays();

	if (m_debugOverlays & OVERLAY_TEXT_BIT) 
	{
		char tempstr[512];

		for ( int i = 0; i < m_LogicBranchList.Count(); i++ )
		{
			CLogicBranch *pBranch = (CLogicBranch *)m_LogicBranchList.Element( i ).Get();
			if ( pBranch )
			{
				Q_snprintf( tempstr, sizeof(tempstr), "Branch (%s): %s", STRING(pBranch->GetEntityName()), (pBranch->GetLogicBranchState()) ? "TRUE" : "FALSE" );
				EntityText( text_offset, tempstr, 0 );
				text_offset++;
			}
		}
	}

	return text_offset;
}
