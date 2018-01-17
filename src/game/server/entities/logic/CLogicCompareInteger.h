//========= Copyright Valve Corporation, All rights reserved. ============//

#ifndef GAME_SERVER_ENTITIES_LOGIC_CLOGICCOMPAREINTEGER_H
#define GAME_SERVER_ENTITIES_LOGIC_CLOGICCOMPAREINTEGER_H

#ifdef WIN32
#pragma once
#endif

//-----------------------------------------------------------------------------
// Purpose: Compares a set of integer inputs to the one main input
//			Outputs true if they are all equivalant, false otherwise
//-----------------------------------------------------------------------------
class CLogicCompareInteger : public CLogicalEntity
{
public:
	DECLARE_CLASS( CLogicCompareInteger, CLogicalEntity );

	// outputs
	COutputEvent m_OnEqual;
	COutputEvent m_OnNotEqual;

	// data
	int m_iIntegerValue;
	int m_iShouldCompareToValue;

	DECLARE_DATADESC();

	CMultiInputVar m_AllIntCompares;

	// Input handlers
	void InputValue( inputdata_t &inputdata );
	void InputCompareValues( inputdata_t &inputdata );
};

#endif // GAME_SERVER_ENTITIES_LOGIC_CLOGICCOMPAREINTEGER_H
