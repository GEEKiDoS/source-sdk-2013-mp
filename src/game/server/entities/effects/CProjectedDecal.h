//========= Copyright Valve Corporation, All rights reserved. ============//

#ifndef GAME_SERVER_ENTITIES_EFFECTS_CPROJECTEDDECAL_H
#define GAME_SERVER_ENTITIES_EFFECTS_CPROJECTEDDECAL_H

#ifdef WIN32
#pragma once
#endif

//-----------------------------------------------------------------------------
// Purpose: Projects a decal against a prop
//-----------------------------------------------------------------------------
class CProjectedDecal : public CPointEntity
{
public:
	DECLARE_CLASS( CProjectedDecal, CPointEntity );

	void	Spawn( void );
	bool	KeyValue( const char *szKeyName, const char *szValue );

	// Need to apply static decals here to get them into the signon buffer for the server appropriately
	virtual void Activate();

	void	TriggerDecal( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	// Input handlers.
	void	InputActivate( inputdata_t &inputdata );

	DECLARE_DATADESC();

public:
	int		m_nTexture;
	float	m_flDistance;

private:
	void	ProjectDecal( CRecipientFilter& filter );

	void	StaticDecal( void );
};

#endif // GAME_SERVER_ENTITIES_EFFECTS_CPROJECTEDDECAL_H
