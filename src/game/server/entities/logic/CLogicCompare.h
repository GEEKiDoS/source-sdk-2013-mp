//========= Copyright Valve Corporation, All rights reserved. ============//

#ifndef GAME_SERVER_ENTITIES_LOGIC_CLOGICCOMPARE_H
#define GAME_SERVER_ENTITIES_LOGIC_CLOGICCOMPARE_H

#ifdef WIN32
#pragma once
#endif

//-----------------------------------------------------------------------------
// Purpose: Compares a floating point input to a predefined value, firing an
//			output to indicate the result of the comparison.
//-----------------------------------------------------------------------------
class CLogicCompare : public CLogicalEntity
{
	DECLARE_CLASS( CLogicCompare, CLogicalEntity );

public:
	int DrawDebugTextOverlays(void);

private:
	// Inputs
	void InputSetValue( inputdata_t &inputdata );
	void InputSetValueCompare( inputdata_t &inputdata );
	void InputSetCompareValue( inputdata_t &inputdata );
	void InputCompare( inputdata_t &inputdata );

	void DoCompare(CBaseEntity *pActivator, float flInValue);

	float m_flInValue;					// Place to hold the last input value for a recomparison.
	float m_flCompareValue;				// The value to compare the input value against.

	// Outputs
	COutputFloat m_OnLessThan;			// Fired when the input value is less than the compare value.
	COutputFloat m_OnEqualTo;			// Fired when the input value is equal to the compare value.
	COutputFloat m_OnNotEqualTo;		// Fired when the input value is not equal to the compare value.
	COutputFloat m_OnGreaterThan;		// Fired when the input value is greater than the compare value.

	DECLARE_DATADESC();
};

#endif // GAME_SERVER_ENTITIES_LOGIC_CLOGICCOMPARE_H
