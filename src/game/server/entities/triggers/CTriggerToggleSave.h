//========= Copyright Valve Corporation, All rights reserved. ============//

#ifndef GAME_SERVER_ENTITIES_WEAPONS_CTRIGGERTOGGLESAVE_H
#define GAME_SERVER_ENTITIES_WEAPONS_CTRIGGERTOGGLESAVE_H

#ifdef WIN32
#pragma once
#endif

//-----------------------------------------------------------------------------
// Purpose: Saves the game when the player touches the trigger. Can be enabled or disabled
//-----------------------------------------------------------------------------
class CTriggerToggleSave : public CBaseTrigger
{
public:
	DECLARE_CLASS( CTriggerToggleSave, CBaseTrigger );

	void Spawn( void );
	void Touch( CBaseEntity *pOther );

	void InputEnable( inputdata_t &inputdata )
	{
		m_bDisabled = false;
	}

	void InputDisable( inputdata_t &inputdata )
	{
		m_bDisabled = true;
	}

	bool	m_bDisabled;		// Initial state
	
	DECLARE_DATADESC();
};

#endif // GAME_SERVER_ENTITIES_WEAPONS_CTRIGGERTOGGLESAVE_H
