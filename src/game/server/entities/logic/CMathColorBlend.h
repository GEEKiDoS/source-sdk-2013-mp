//========= Copyright Valve Corporation, All rights reserved. ============//

#ifndef GAME_SERVER_ENTITIES_LOGIC_CMATHCOLORBLEND_H
#define GAME_SERVER_ENTITIES_LOGIC_CMATHCOLORBLEND_H

#ifdef WIN32
#pragma once
#endif

//-----------------------------------------------------------------------------
// Purpose: Remaps a given input range to an output range.
//-----------------------------------------------------------------------------
const int SF_COLOR_BLEND_IGNORE_OUT_OF_RANGE = 1;

class CMathColorBlend : public CLogicalEntity
{
public:

	DECLARE_CLASS( CMathColorBlend, CLogicalEntity );

	void Spawn(void);

	// Keys
	float m_flInMin;
	float m_flInMax;
	color32 m_OutColor1;		// Output color when input is m_fInMin
	color32 m_OutColor2;		// Output color when input is m_fInMax

	// Inputs
	void InputValue( inputdata_t &inputdata );

	// Outputs
	COutputColor32 m_OutValue;

	DECLARE_DATADESC();
};

#endif // GAME_SERVER_ENTITIES_LOGIC_CMATHCOLORBLEND_H
