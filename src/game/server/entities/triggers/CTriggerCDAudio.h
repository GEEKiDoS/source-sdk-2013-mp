//========= Copyright Valve Corporation, All rights reserved. ============//

#ifndef GAME_SERVER_ENTITIES_TRIGGERS_CTRIGGERCDAUDIO_H
#define GAME_SERVER_ENTITIES_TRIGGERS_CTRIGGERCDAUDIO_H

#ifdef WIN32
#pragma once
#endif

//-----------------------------------------------------------------------------
// Purpose: Starts/stops cd audio tracks
//-----------------------------------------------------------------------------
class CTriggerCDAudio : public CBaseTrigger
{
public:
	DECLARE_CLASS( CTriggerCDAudio, CBaseTrigger );

	void Spawn( void );

	virtual void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void PlayTrack( void );
	void Touch ( CBaseEntity *pOther );
};

#endif // GAME_SERVER_ENTITIES_TRIGGERS_CTRIGGERCDAUDIO_H
