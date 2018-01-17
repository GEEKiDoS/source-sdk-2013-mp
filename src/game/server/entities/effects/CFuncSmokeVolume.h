//========= Copyright Valve Corporation, All rights reserved. ============//

#ifndef GAME_SERVER_ENTITIES_EFFECTS_CFUNCSMOKEVOLUME_H
#define GAME_SERVER_ENTITIES_EFFECTS_CFUNCSMOKEVOLUME_H

#ifdef WIN32
#pragma once
#endif

class CFuncSmokeVolume : public CBaseParticleEntity
{
public:
	DECLARE_CLASS( CFuncSmokeVolume, CBaseParticleEntity );
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	CFuncSmokeVolume();
	void Spawn();
	void Activate( void );

	// Set the times it fades out at.
	void SetDensity( float density );

private:
	CNetworkVar( color32, m_Color1 );
	CNetworkVar( color32, m_Color2 );
	CNetworkString( m_MaterialName, 255 );
	string_t m_String_tMaterialName;
	CNetworkVar( float, m_ParticleDrawWidth );
	CNetworkVar( float, m_ParticleSpacingDistance );
	CNetworkVar( float, m_DensityRampSpeed );
	CNetworkVar( float, m_RotationSpeed );
	CNetworkVar( float, m_MovementSpeed );
	CNetworkVar( float, m_Density );
};

#endif // GAME_SERVER_ENTITIES_EFFECTS_CFUNCSMOKEVOLUME_H
