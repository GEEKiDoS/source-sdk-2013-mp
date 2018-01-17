//========= Copyright Valve Corporation, All rights reserved. ============//

#ifndef GAME_SERVER_ENTITIES_LOGIC_CMATHREMAP_H
#define GAME_SERVER_ENTITIES_LOGIC_CMATHREMAP_H

#ifdef WIN32
#pragma once
#endif

//-----------------------------------------------------------------------------
// Purpose: Remaps a given input range to an output range.
//-----------------------------------------------------------------------------
const int SF_MATH_REMAP_IGNORE_OUT_OF_RANGE = 1;
const int SF_MATH_REMAP_CLAMP_OUTPUT_TO_RANGE = 2;

class CMathRemap : public CLogicalEntity
{
public:

	DECLARE_CLASS( CMathRemap, CLogicalEntity );

	void Spawn(void);

	// Keys
	float m_flInMin;
	float m_flInMax;
	float m_flOut1;		// Output value when input is m_fInMin
	float m_flOut2;		// Output value when input is m_fInMax

	bool  m_bEnabled;

	// Inputs
	void InputValue( inputdata_t &inputdata );
	void InputEnable( inputdata_t &inputdata );
	void InputDisable( inputdata_t &inputdata );

	// Outputs
	COutputFloat m_OutValue;

	DECLARE_DATADESC();
};

#endif // GAME_SERVER_ENTITIES_LOGIC_CMATHREMAP_H
