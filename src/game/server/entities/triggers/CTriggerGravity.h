//========= Copyright Valve Corporation, All rights reserved. ============//

#ifndef GAME_SERVER_ENTITIES_TRIGGERS_CTRIGGERGRAVITY_H
#define GAME_SERVER_ENTITIES_TRIGGERS_CTRIGGERGRAVITY_H

#ifdef WIN32
#pragma once
#endif

class CTriggerGravity : public CBaseTrigger
{
public:
	DECLARE_CLASS( CTriggerGravity, CBaseTrigger );
	DECLARE_DATADESC();

	void Spawn( void );
	void GravityTouch( CBaseEntity *pOther );
};

#endif // GAME_SERVER_ENTITIES_TRIGGERS_CTRIGGERGRAVITY_H
