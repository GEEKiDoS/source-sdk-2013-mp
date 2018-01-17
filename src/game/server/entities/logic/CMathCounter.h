//========= Copyright Valve Corporation, All rights reserved. ============//

#ifndef GAME_SERVER_ENTITIES_LOGIC_CMATHCOUNTER_H
#define GAME_SERVER_ENTITIES_LOGIC_CMATHCOUNTER_H

#ifdef WIN32
#pragma once
#endif

//-----------------------------------------------------------------------------
// Purpose: Holds a value that can be added to and subtracted from.
//-----------------------------------------------------------------------------
class CMathCounter : public CLogicalEntity
{
	DECLARE_CLASS( CMathCounter, CLogicalEntity );
private:
	float m_flMin;		// Minimum clamp value. If min and max are BOTH zero, no clamping is done.
	float m_flMax;		// Maximum clamp value.
	bool m_bHitMin;		// Set when we reach or go below our minimum value, cleared if we go above it again.
	bool m_bHitMax;		// Set when we reach or exceed our maximum value, cleared if we fall below it again.

	bool m_bDisabled;

	bool KeyValue(const char *szKeyName, const char *szValue);
	void Spawn(void);

	int DrawDebugTextOverlays(void);

	void UpdateOutValue(CBaseEntity *pActivator, float fNewValue);

	// Inputs
	void InputAdd( inputdata_t &inputdata );
	void InputDivide( inputdata_t &inputdata );
	void InputMultiply( inputdata_t &inputdata );
	void InputSetValue( inputdata_t &inputdata );
	void InputSetValueNoFire( inputdata_t &inputdata );
	void InputSubtract( inputdata_t &inputdata );
	void InputSetHitMax( inputdata_t &inputdata );
	void InputSetHitMin( inputdata_t &inputdata );
	void InputGetValue( inputdata_t &inputdata );
	void InputEnable( inputdata_t &inputdata );
	void InputDisable( inputdata_t &inputdata );

	// Outputs
	COutputFloat m_OutValue;
	COutputFloat m_OnGetValue;	// Used for polling the counter value.
	COutputEvent m_OnHitMin;
	COutputEvent m_OnHitMax;

	DECLARE_DATADESC();
};

#endif // GAME_SERVER_ENTITIES_LOGIC_CMATHCOUNTER_H
