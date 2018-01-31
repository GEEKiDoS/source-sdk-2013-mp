//========= Copyright Valve Corporation, All rights reserved. ============//

#ifndef GAME_SERVER_ENTITIES_WEAPONS_CTRIGGERTELEPORT_H
#define GAME_SERVER_ENTITIES_WEAPONS_CTRIGGERTELEPORT_H

#ifdef WIN32
#pragma once
#endif

//-----------------------------------------------------------------------------
// Teleport trigger
//-----------------------------------------------------------------------------
const int SF_TELEPORT_PRESERVE_ANGLES = 0x20;	// Preserve angles even when a local landmark is not specified

class CTriggerTeleport : public CBaseTrigger
{
public:
	DECLARE_CLASS( CTriggerTeleport, CBaseTrigger );

	virtual void Spawn( void ) OVERRIDE;
	virtual void Touch( CBaseEntity *pOther ) OVERRIDE;

	string_t m_iLandmark;

	DECLARE_DATADESC();
};

#endif // GAME_SERVER_ENTITIES_WEAPONS_CTRIGGERTELEPORT_H
