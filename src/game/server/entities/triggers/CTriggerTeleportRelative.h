//========= Copyright Valve Corporation, All rights reserved. ============//

#ifndef GAME_SERVER_ENTITIES_WEAPONS_CTRIGGERTELEPORTRELATIVE_H
#define GAME_SERVER_ENTITIES_WEAPONS_CTRIGGERTELEPORTRELATIVE_H

#ifdef WIN32
#pragma once
#endif

//-----------------------------------------------------------------------------
// Teleport Relative trigger
//-----------------------------------------------------------------------------
class CTriggerTeleportRelative : public CBaseTrigger
{
public:
	DECLARE_CLASS(CTriggerTeleportRelative, CBaseTrigger);

	virtual void Spawn( void ) OVERRIDE;
	virtual void Touch( CBaseEntity *pOther ) OVERRIDE;

	Vector m_TeleportOffset;

	DECLARE_DATADESC();
};

#endif // GAME_SERVER_ENTITIES_WEAPONS_CTRIGGERTELEPORTRELATIVE_H
