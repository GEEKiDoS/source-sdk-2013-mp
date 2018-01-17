//========= Copyright Valve Corporation, All rights reserved. ============//

#ifndef GAME_SERVER_ENTITIES_LOGIC_CTIMERENTITY_H
#define GAME_SERVER_ENTITIES_LOGIC_CTIMERENTITY_H

#ifdef WIN32
#pragma once
#endif

//-----------------------------------------------------------------------------
// Purpose: Timer entity. Fires an output at regular or random intervals.
//-----------------------------------------------------------------------------
//
// Spawnflags and others constants.
//
const int SF_TIMER_UPDOWN = 1;
const float LOGIC_TIMER_MIN_INTERVAL = 0.01;


class CTimerEntity : public CLogicalEntity
{
public:
	DECLARE_CLASS( CTimerEntity, CLogicalEntity );

	void Spawn( void );
	void Think( void );

	void Toggle( void );
	void Enable( void );
	void Disable( void );
	void FireTimer( void );

	int DrawDebugTextOverlays(void);

	// outputs
	COutputEvent m_OnTimer;
	COutputEvent m_OnTimerHigh;
	COutputEvent m_OnTimerLow;

	// inputs
	void InputToggle( inputdata_t &inputdata );
	void InputEnable( inputdata_t &inputdata );
	void InputDisable( inputdata_t &inputdata );
	void InputFireTimer( inputdata_t &inputdata );
	void InputRefireTime( inputdata_t &inputdata );
	void InputResetTimer( inputdata_t &inputdata );
	void InputAddToTimer( inputdata_t &inputdata );
	void InputSubtractFromTimer( inputdata_t &inputdata );

	int m_iDisabled;
	float m_flRefireTime;
	bool m_bUpDownState;
	int m_iUseRandomTime;
	float m_flLowerRandomBound;
	float m_flUpperRandomBound;

	// methods
	void ResetTimer( void );
	
	DECLARE_DATADESC();
};

#endif // GAME_SERVER_ENTITIES_LOGIC_CTIMERENTITY_H
