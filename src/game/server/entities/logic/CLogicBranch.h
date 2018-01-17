//========= Copyright Valve Corporation, All rights reserved. ============//

#ifndef GAME_SERVER_ENTITIES_LOGIC_CLOGICBRANCH_H
#define GAME_SERVER_ENTITIES_LOGIC_CLOGICBRANCH_H

#ifdef WIN32
#pragma once
#endif

//-----------------------------------------------------------------------------
// Purpose: Tests a boolean value, firing an output to indicate whether the
//			value was true or false.
//-----------------------------------------------------------------------------
class CLogicBranch : public CLogicalEntity
{
	DECLARE_CLASS( CLogicBranch, CLogicalEntity );
	
public:

	void UpdateOnRemove();

	void AddLogicBranchListener( CBaseEntity *pEntity );
	/**
	 * Accessor for logic_branchlist to test the value of the branch on demand.
	 */
	inline bool GetLogicBranchState() { return m_bInValue; }
	virtual int DrawDebugTextOverlays( void );

private:

	enum LogicBranchFire_t
	{
		LOGIC_BRANCH_FIRE,
		LOGIC_BRANCH_NO_FIRE,
	};

	// Inputs
	void InputSetValue( inputdata_t &inputdata );
	void InputSetValueTest( inputdata_t &inputdata );
	void InputToggle( inputdata_t &inputdata );
	void InputToggleTest( inputdata_t &inputdata );
	void InputTest( inputdata_t &inputdata );

	void UpdateValue(bool bNewValue, CBaseEntity *pActivator, LogicBranchFire_t eFire);

	bool m_bInValue;					// Place to hold the last input value for a future test.
	
	CUtlVector<EHANDLE> m_Listeners;	// A list of logic_branch_listeners that are monitoring us.

	// Outputs
	COutputEvent m_OnTrue;				// Fired when the value is true.
	COutputEvent m_OnFalse;				// Fired when the value is false.

	DECLARE_DATADESC();
};

#endif // GAME_SERVER_ENTITIES_LOGIC_CLOGICBRANCH_H
