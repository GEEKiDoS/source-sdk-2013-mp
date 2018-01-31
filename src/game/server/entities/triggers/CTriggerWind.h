//========= Copyright Valve Corporation, All rights reserved. ============//

#ifndef GAME_SERVER_ENTITIES_WEAPONS_CTRIGGERWIND_H
#define GAME_SERVER_ENTITIES_WEAPONS_CTRIGGERWIND_H

#ifdef WIN32
#pragma once
#endif

#define MAX_WIND_CHANGE		5.0f

#define WIND_THINK_CONTEXT		"WindThinkContext"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTriggerWind : public CBaseVPhysicsTrigger
{
	DECLARE_CLASS( CTriggerWind, CBaseVPhysicsTrigger );
public:
	DECLARE_DATADESC();

	void	Spawn( void );
	bool	KeyValue( const char *szKeyName, const char *szValue );
	void	OnRestore();
	void	UpdateOnRemove();
	bool	CreateVPhysics();
	void	StartTouch( CBaseEntity *pOther );
	void	EndTouch( CBaseEntity *pOther );
	void	WindThink( void );
	int		DrawDebugTextOverlays( void );

	// Input handlers
	void	InputEnable( inputdata_t &inputdata );
	void	InputSetSpeed( inputdata_t &inputdata );

private:
	int 	m_nSpeedBase;	// base line for how hard the wind blows
	int		m_nSpeedNoise;	// noise added to wind speed +/-
	int		m_nSpeedCurrent;// current wind speed
	int		m_nSpeedTarget;	// wind speed I'm approaching

	int		m_nDirBase;		// base line for direction the wind blows (yaw)
	int		m_nDirNoise;	// noise added to wind direction
	int		m_nDirCurrent;	// the current wind direction
	int		m_nDirTarget;	// wind direction I'm approaching

	int		m_nHoldBase;	// base line for how long to wait before changing wind
	int		m_nHoldNoise;	// noise added to how long to wait before changing wind

	bool	m_bSwitch;		// when does wind change

	IPhysicsMotionController*	m_pWindController;
	CPhysicsWind				m_WindCallback;

};

#endif // GAME_SERVER_ENTITIES_TRIGGERS_CTRIGGERWIND_H
