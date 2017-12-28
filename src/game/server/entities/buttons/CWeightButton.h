//========= Copyright Valve Corporation, All rights reserved. ============//

#ifndef GAME_SERVER_ENTITIES_BUTTONS_CWEIGHTBUTTON_H
#define GAME_SERVER_ENTITIES_BUTTONS_CWEIGHTBUTTON_H

#ifdef WIN32
#pragma once
#endif

class CWeightButton : public CBaseEntity
{
public:

	DECLARE_DATADESC();
	DECLARE_CLASS( CWeightButton, CBaseEntity ); 

	void Spawn( void );
	bool CreateVPhysics( void );

	COutputEvent m_OnPressed;				// After threshold weight has been reached
	COutputEvent m_OnReleased;				// After weight has been removed to go below weight threshold

	float m_fStressToActivate;				// Amount of weight required to activate
	bool m_bHasBeenPressed;					// Once the button has been pressed, fire one 
											// output until the weight is reduced below the threshold

	void TriggerThink ( void );

};

#endif // GAME_SERVER_ENTITIES_BUTTONS_CWEIGHTBUTTON_H
