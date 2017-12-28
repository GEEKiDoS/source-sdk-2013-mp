//========= Copyright Valve Corporation, All rights reserved. ============//

#ifndef GAME_SERVER_ENTITIES_EFFECTS_CWATERLODCONTROL_H
#define GAME_SERVER_ENTITIES_EFFECTS_CWATERLODCONTROL_H

#ifdef WIN32
#pragma once
#endif

//------------------------------------------------------------------------------
// FIXME: This really should inherit from something	more lightweight
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Purpose : Water LOD control entity
//------------------------------------------------------------------------------
class CWaterLODControl : public CBaseEntity
{
public:
	DECLARE_CLASS( CWaterLODControl, CBaseEntity );

	CWaterLODControl();

	void Spawn( void );
	bool KeyValue( const char *szKeyName, const char *szValue );
	int  UpdateTransmitState();
	void SetCheapWaterStartDistance( inputdata_t &inputdata );
	void SetCheapWaterEndDistance( inputdata_t &inputdata );

	virtual int	ObjectCaps( void ) { return BaseClass::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }

	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

private:
	CNetworkVar( float, m_flCheapWaterStartDistance );
	CNetworkVar( float, m_flCheapWaterEndDistance );
};

#endif // GAME_SERVER_ENTITIES_EFFECTS_CWATERLODCONTROL_H
