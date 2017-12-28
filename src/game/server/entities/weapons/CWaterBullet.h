//========= Copyright Valve Corporation, All rights reserved. ============//

#ifndef GAME_SERVER_ENTITIES_WEAPONS_CWATERBULLET_H
#define GAME_SERVER_ENTITIES_WEAPONS_CWATERBULLET_H

#ifdef WIN32
#pragma once
#endif

#define WATER_BULLET_BUBBLES_PER_INCH 0.05f

//=========================================================
//=========================================================
class CWaterBullet : public CBaseAnimating
{
	DECLARE_CLASS( CWaterBullet, CBaseAnimating );

public:
	void Precache();
	void Spawn( const Vector &vecOrigin, const Vector &vecDir );
	void Touch( CBaseEntity *pOther );
	void BulletThink();

	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();
};

#endif // GAME_SERVER_ENTITIES_WEAPONS_CWATERBULLET_H
