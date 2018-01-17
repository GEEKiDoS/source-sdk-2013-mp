//========= Copyright Valve Corporation, All rights reserved. ============//

#include "cbase.h"

#include "gameinterface.h"

#include "entities/logic/CLogicAutosave.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern CServerGameDLL g_ServerGameDLL;

LINK_ENTITY_TO_CLASS(logic_autosave, CLogicAutosave);

BEGIN_DATADESC( CLogicAutosave )
	DEFINE_KEYFIELD( m_bForceNewLevelUnit, FIELD_BOOLEAN, "NewLevelUnit" ),
	DEFINE_KEYFIELD( m_minHitPoints, FIELD_INTEGER, "MinimumHitPoints" ),
	DEFINE_KEYFIELD( m_minHitPointsToCommit, FIELD_INTEGER, "MinHitPointsToCommit" ),
	// Inputs
	DEFINE_INPUTFUNC( FIELD_VOID, "Save", InputSave ),
	DEFINE_INPUTFUNC( FIELD_FLOAT, "SaveDangerous", InputSaveDangerous ),
	DEFINE_INPUTFUNC( FIELD_INTEGER, "SetMinHitpointsThreshold", InputSetMinHitpointsThreshold ),
END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: Save!
//-----------------------------------------------------------------------------
void CLogicAutosave::InputSave( inputdata_t &inputdata )
{
	if ( m_bForceNewLevelUnit )
	{
		engine->ClearSaveDir();
	}

	engine->ServerCommand( "autosave\n" );
}

//-----------------------------------------------------------------------------
// Purpose: Save safely!
//-----------------------------------------------------------------------------
void CLogicAutosave::InputSaveDangerous( inputdata_t &inputdata )
{
	CBasePlayer *pPlayer = UTIL_PlayerByIndex( 1 );

	if ( g_ServerGameDLL.m_fAutoSaveDangerousTime != 0.0f && g_ServerGameDLL.m_fAutoSaveDangerousTime >= gpGlobals->curtime )
	{
		// A previous dangerous auto save was waiting to become safe

		if ( pPlayer->GetDeathTime() == 0.0f || pPlayer->GetDeathTime() > gpGlobals->curtime )
		{
			// The player isn't dead, so make the dangerous auto save safe
			engine->ServerCommand( "autosavedangerousissafe\n" );
		}
	}

	if ( m_bForceNewLevelUnit )
	{
		engine->ClearSaveDir();
	}

	if ( pPlayer->GetHealth() >= m_minHitPoints )
	{
		engine->ServerCommand( "autosavedangerous\n" );
		g_ServerGameDLL.m_fAutoSaveDangerousTime = gpGlobals->curtime + inputdata.value.Float();

		// Player must have this much health when we go to commit, or we don't commit.
		g_ServerGameDLL.m_fAutoSaveDangerousMinHealthToCommit = m_minHitPointsToCommit;
	}
}
