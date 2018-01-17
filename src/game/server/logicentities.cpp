//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Implements many of the entities that control logic flow within a map.
//
//=============================================================================

#include "cbase.h"
#include "entityinput.h"
#include "entityoutput.h"
#include "eventqueue.h"
#include "mathlib/mathlib.h"
#include "globalstate.h"
#include "ndebugoverlay.h"
#include "saverestore_utlvector.h"
#include "vstdlib/random.h"
#include "gameinterface.h"

#include "entities/logic/CLogicAutosave.h"
#include "entities/logic/CLogicBranch.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


extern CServerGameDLL g_ServerGameDLL;


//-----------------------------------------------------------------------------
// Purpose: Autosaves when triggered
//-----------------------------------------------------------------------------
class CLogicActiveAutosave : public CLogicAutosave
{
	DECLARE_CLASS( CLogicActiveAutosave, CLogicAutosave );

	void InputEnable( inputdata_t &inputdata )
	{
		m_flStartTime = -1;
		SetThink( &CLogicActiveAutosave::SaveThink );
		SetNextThink( gpGlobals->curtime );
	}

	void InputDisable( inputdata_t &inputdata )
	{
		SetThink( NULL );
	}

	void SaveThink()
	{
		CBasePlayer *pPlayer = UTIL_GetLocalPlayer();
		if ( pPlayer )
		{
			if ( m_flStartTime < 0 )
			{
				if ( pPlayer->GetHealth() <= m_minHitPoints )
				{
					m_flStartTime = gpGlobals->curtime;
				}
			}
			else
			{
				if ( pPlayer->GetHealth() >= m_TriggerHitPoints )
				{
					inputdata_t inputdata;
					DevMsg( 2, "logic_active_autosave (%s, %d) triggered\n", STRING( GetEntityName() ), entindex() );
					if ( !m_flDangerousTime )
					{
						InputSave( inputdata );
					}
					else
					{
						inputdata.value.SetFloat( m_flDangerousTime );
						InputSaveDangerous( inputdata );
					}
					m_flStartTime = -1;
				}
				else if ( m_flTimeToTrigger > 0 && gpGlobals->curtime - m_flStartTime > m_flTimeToTrigger )
				{
					m_flStartTime = -1;
				}
			}
		}

		float thinkInterval = ( m_flStartTime < 0 ) ? 1.0 : 0.5;
		SetNextThink( gpGlobals->curtime + thinkInterval );
	}

	DECLARE_DATADESC();

	int m_TriggerHitPoints;
	float m_flTimeToTrigger;
	float m_flStartTime;
	float m_flDangerousTime;
};

LINK_ENTITY_TO_CLASS(logic_active_autosave, CLogicActiveAutosave);

BEGIN_DATADESC( CLogicActiveAutosave )
	DEFINE_KEYFIELD( m_TriggerHitPoints, FIELD_INTEGER, "TriggerHitPoints" ),
	DEFINE_KEYFIELD( m_flTimeToTrigger, FIELD_FLOAT, "TimeToTrigger" ),
	DEFINE_KEYFIELD( m_flDangerousTime, FIELD_FLOAT, "DangerousTime" ),
	DEFINE_FIELD( m_flStartTime, FIELD_TIME ),
	DEFINE_THINKFUNC( SaveThink ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),
END_DATADESC()


//-----------------------------------------------------------------------------
// Purpose: Keyfield set func
//-----------------------------------------------------------------------------
void CLogicAutosave::InputSetMinHitpointsThreshold( inputdata_t &inputdata )
{
	int setTo = inputdata.value.Int();
	AssertMsg1(setTo >= 0 && setTo <= 100, "Tried to set autosave MinHitpointsThreshold to %d!\n", setTo);
	m_minHitPoints = setTo;
}

// Finds the named physics object.  If no name, returns the world
// If a name is specified and an object not found - errors are reported
IPhysicsObject *FindPhysicsObjectByNameOrWorld( string_t name, CBaseEntity *pErrorEntity )
{
	if ( !name )
		return g_PhysWorldObject;

	IPhysicsObject *pPhysics = FindPhysicsObjectByName( name.ToCStr(), pErrorEntity );
	if ( !pPhysics )
	{
		DevWarning("%s: can't find %s\n", pErrorEntity->GetClassname(), name.ToCStr());
	}
	return pPhysics;
}

class CLogicCollisionPair : public CLogicalEntity
{
	DECLARE_CLASS( CLogicCollisionPair, CLogicalEntity );
public:

	void EnableCollisions( bool bEnable )
	{
		IPhysicsObject *pPhysics0 = FindPhysicsObjectByNameOrWorld( m_nameAttach1, this );
		IPhysicsObject *pPhysics1 = FindPhysicsObjectByNameOrWorld( m_nameAttach2, this );

		// need two different objects to do anything
		if ( pPhysics0 && pPhysics1 && pPhysics0 != pPhysics1 )
		{
			m_disabled = !bEnable;
			m_succeeded = true;
			if ( bEnable )
			{
				PhysEnableEntityCollisions( pPhysics0, pPhysics1 );
			}
			else
			{
				PhysDisableEntityCollisions( pPhysics0, pPhysics1 );
			}
		}
		else
		{
			m_succeeded = false;
		}
	}

	void Activate( void )
	{
		if ( m_disabled )
		{
			EnableCollisions( false );
		}
		BaseClass::Activate();
	}

	void InputDisableCollisions( inputdata_t &inputdata )
	{
		if ( m_succeeded && m_disabled )
			return;
		EnableCollisions( false );
	}

	void InputEnableCollisions( inputdata_t &inputdata )
	{
		if ( m_succeeded && !m_disabled )
			return;
		EnableCollisions( true );
	}
	// If Activate() becomes PostSpawn()
	//void OnRestore() { Activate(); }

	DECLARE_DATADESC();

private:
	string_t		m_nameAttach1;
	string_t		m_nameAttach2;
	bool			m_disabled;
	bool			m_succeeded;
};

BEGIN_DATADESC( CLogicCollisionPair )
	DEFINE_KEYFIELD( m_nameAttach1, FIELD_STRING, "attach1" ),
	DEFINE_KEYFIELD( m_nameAttach2, FIELD_STRING, "attach2" ),
	DEFINE_KEYFIELD( m_disabled, FIELD_BOOLEAN, "startdisabled" ),
	DEFINE_FIELD( m_succeeded, FIELD_BOOLEAN ),

	// Inputs
	DEFINE_INPUTFUNC( FIELD_VOID, "DisableCollisions", InputDisableCollisions ),
	DEFINE_INPUTFUNC( FIELD_VOID, "EnableCollisions", InputEnableCollisions ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( logic_collision_pair, CLogicCollisionPair );


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define MAX_LOGIC_BRANCH_NAMES 16

class CLogicBranchList : public CLogicalEntity
{
	DECLARE_CLASS( CLogicBranchList, CLogicalEntity );

	virtual void Spawn();
	virtual void Activate();
	virtual int DrawDebugTextOverlays( void );

private:

	enum LogicBranchListenerLastState_t
	{
		LOGIC_BRANCH_LISTENER_NOT_INIT = 0,
		LOGIC_BRANCH_LISTENER_ALL_TRUE,
		LOGIC_BRANCH_LISTENER_ALL_FALSE,
		LOGIC_BRANCH_LISTENER_MIXED,
	};

	void DoTest( CBaseEntity *pActivator );

	string_t m_nLogicBranchNames[MAX_LOGIC_BRANCH_NAMES];
	CUtlVector<EHANDLE> m_LogicBranchList;
	LogicBranchListenerLastState_t m_eLastState;

	// Inputs
	void Input_OnLogicBranchRemoved( inputdata_t &inputdata );
	void Input_OnLogicBranchChanged( inputdata_t &inputdata );
	void InputTest( inputdata_t &inputdata );

	// Outputs
	COutputEvent m_OnAllTrue;			// Fired when all the registered logic_branches become true.
	COutputEvent m_OnAllFalse;			// Fired when all the registered logic_branches become false.
	COutputEvent m_OnMixed;				// Fired when one of the registered logic branches changes, but not all are true or false.

	DECLARE_DATADESC();
};

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
