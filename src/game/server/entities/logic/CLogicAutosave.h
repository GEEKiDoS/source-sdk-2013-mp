//========= Copyright Valve Corporation, All rights reserved. ============//

#ifndef GAME_SERVER_ENTITIES_LOGIC_CLOGICAUTOSAVE_H
#define GAME_SERVER_ENTITIES_LOGIC_CLOGICAUTOSAVE_H

#ifdef WIN32
#pragma once
#endif

//-----------------------------------------------------------------------------
// Purpose: Autosaves when triggered
//-----------------------------------------------------------------------------
class CLogicAutosave : public CLogicalEntity
{
	DECLARE_CLASS( CLogicAutosave, CLogicalEntity );

protected:
	// Inputs
	void InputSave( inputdata_t &inputdata );
	void InputSaveDangerous( inputdata_t &inputdata );
	void InputSetMinHitpointsThreshold( inputdata_t &inputdata );

	DECLARE_DATADESC();
	bool m_bForceNewLevelUnit;
	int m_minHitPoints;
	int m_minHitPointsToCommit;
};

#endif // GAME_SERVER_ENTITIES_LOGIC_CLOGICAUTOSAVE_H
