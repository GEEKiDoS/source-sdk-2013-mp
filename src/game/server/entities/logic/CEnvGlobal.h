//========= Copyright Valve Corporation, All rights reserved. ============//

#ifndef GAME_SERVER_ENTITIES_LOGIC_CENVGLOBAL_H
#define GAME_SERVER_ENTITIES_LOGIC_CENVGLOBAL_H

#ifdef WIN32
#pragma once
#endif

//-----------------------------------------------------------------------------
// Purpose: Holds a global state that can be queried by other entities to change
//			their behavior, such as "predistaster".
//-----------------------------------------------------------------------------
const int SF_GLOBAL_SET = 1;	// Set global state to initial state on spawn

class CEnvGlobal : public CLogicalEntity
{
public:
	DECLARE_CLASS( CEnvGlobal, CLogicalEntity );

	void Spawn( void );

	// Input handlers
	void InputTurnOn( inputdata_t &inputdata );
	void InputTurnOff( inputdata_t &inputdata );
	void InputRemove( inputdata_t &inputdata );
	void InputToggle( inputdata_t &inputdata );
	void InputSetCounter( inputdata_t &inputdata );
	void InputAddToCounter( inputdata_t &inputdata );
	void InputGetCounter( inputdata_t &inputdata );

	int DrawDebugTextOverlays(void);

	DECLARE_DATADESC();

	COutputInt m_outCounter;
		
	string_t	m_globalstate;
	int			m_triggermode;
	int			m_initialstate;
	int			m_counter;			// A counter value associated with this global.
};

#endif // GAME_SERVER_ENTITIES_LOGIC_CENVGLOBAL_H
