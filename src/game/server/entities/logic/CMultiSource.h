//========= Copyright Valve Corporation, All rights reserved. ============//

#ifndef GAME_SERVER_ENTITIES_LOGIC_CMULTISOURCE_H
#define GAME_SERVER_ENTITIES_LOGIC_CMULTISOURCE_H

#ifdef WIN32
#pragma once
#endif

#define MS_MAX_TARGETS 32

const int SF_MULTI_INIT	= 1;

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CMultiSource : public CLogicalEntity
{
public:
	DECLARE_CLASS( CMultiSource, CLogicalEntity );

	void Spawn( );
	bool KeyValue( const char *szKeyName, const char *szValue );
	void Use( ::CBaseEntity *pActivator, ::CBaseEntity *pCaller, USE_TYPE useType, float value );
	int	ObjectCaps( void ) { return(BaseClass::ObjectCaps() | FCAP_MASTER); }
	bool IsTriggered( ::CBaseEntity *pActivator );
	void Register( void );

	DECLARE_DATADESC();

	EHANDLE		m_rgEntities[MS_MAX_TARGETS];
	int			m_rgTriggered[MS_MAX_TARGETS];

	COutputEvent m_OnTrigger;		// Fired when all connections are triggered.

	int			m_iTotal;
	string_t	m_globalstate;
};

#endif // GAME_SERVER_ENTITIES_LOGIC_CMULTISOURCE_H
