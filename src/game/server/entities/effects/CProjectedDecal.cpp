//========= Copyright Valve Corporation, All rights reserved. ============//

#include "cbase.h"

#include "entities/effects/CDecal.h"
#include "entities/effects/CProjectedDecal.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

BEGIN_DATADESC( CProjectedDecal )

	DEFINE_FIELD( m_nTexture, FIELD_INTEGER ),

	DEFINE_KEYFIELD( m_flDistance, FIELD_FLOAT, "Distance" ),

	// Function pointers
	DEFINE_FUNCTION( StaticDecal ),
	DEFINE_FUNCTION( TriggerDecal ),

	DEFINE_INPUTFUNC( FIELD_VOID, "Activate", InputActivate ),

END_DATADESC()

LINK_ENTITY_TO_CLASS( info_projecteddecal, CProjectedDecal );

// UNDONE:  These won't get sent to joining players in multi-player
void CProjectedDecal::Spawn( void )
{
	if ( m_nTexture < 0 || 
		(gpGlobals->deathmatch && HasSpawnFlags( SF_DECAL_NOTINDEATHMATCH )) )
	{
		UTIL_Remove( this );
		return;
	} 
}

void CProjectedDecal::Activate()
{
	BaseClass::Activate();

	if ( !GetEntityName() )
	{
		StaticDecal();
	}
	else
	{
		// if there IS a targetname, the decal sprays itself on when it is triggered.
		SetThink ( &CProjectedDecal::SUB_DoNothing );
		SetUse(&CProjectedDecal::TriggerDecal);
	}
}

void CProjectedDecal::InputActivate( inputdata_t &inputdata )
{
	TriggerDecal( inputdata.pActivator, inputdata.pCaller, USE_ON, 0 );
}

void CProjectedDecal::ProjectDecal( CRecipientFilter& filter )
{
	te->ProjectDecal( filter, 0.0, 
		&GetAbsOrigin(), &GetAbsAngles(), m_flDistance, m_nTexture );
}

void CProjectedDecal::TriggerDecal ( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	CBroadcastRecipientFilter filter;

	ProjectDecal( filter );

	SetThink( &CProjectedDecal::SUB_Remove );
	SetNextThink( gpGlobals->curtime + 0.1f );
}

void CProjectedDecal::StaticDecal( void )
{
	CBroadcastRecipientFilter initFilter;
	initFilter.MakeInitMessage();

	ProjectDecal( initFilter );

	SUB_Remove();
}


bool CProjectedDecal::KeyValue( const char *szKeyName, const char *szValue )
{
	if (FStrEq(szKeyName, "texture"))
	{
		// FIXME:  should decals all be preloaded?
		m_nTexture = UTIL_PrecacheDecal( szValue, true );
		
		// Found
		if (m_nTexture >= 0 )
			return true;
		Warning( "Can't find decal %s\n", szValue );
	}
	else
	{
		return BaseClass::KeyValue( szKeyName, szValue );
	}

	return true;
}
