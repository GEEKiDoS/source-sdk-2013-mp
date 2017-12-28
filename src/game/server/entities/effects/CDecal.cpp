//========= Copyright Valve Corporation, All rights reserved. ============//

#include "cbase.h"

#include "entities/effects/CDecal.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

BEGIN_DATADESC( CDecal )

	DEFINE_FIELD( m_nTexture, FIELD_INTEGER ),
	DEFINE_KEYFIELD( m_bLowPriority, FIELD_BOOLEAN, "LowPriority" ), // Don't mark as FDECAL_PERMANENT so not save/restored and will be reused on the client preferentially

	// Function pointers
	DEFINE_FUNCTION( StaticDecal ),
	DEFINE_FUNCTION( TriggerDecal ),

	DEFINE_INPUTFUNC( FIELD_VOID, "Activate", InputActivate ),

END_DATADESC()

LINK_ENTITY_TO_CLASS( infodecal, CDecal );

// UNDONE:  These won't get sent to joining players in multi-player
void CDecal::Spawn( void )
{
	if ( m_nTexture < 0 || 
		(gpGlobals->deathmatch && HasSpawnFlags( SF_DECAL_NOTINDEATHMATCH )) )
	{
		UTIL_Remove( this );
		return;
	} 
}

void CDecal::Activate()
{
	BaseClass::Activate();

	if ( !GetEntityName() )
	{
		StaticDecal();
	}
	else
	{
		// if there IS a targetname, the decal sprays itself on when it is triggered.
		SetThink ( &CDecal::SUB_DoNothing );
		SetUse(&CDecal::TriggerDecal);
	}
}

void CDecal::TriggerDecal ( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	// this is set up as a USE function for info_decals that have targetnames, so that the
	// decal doesn't get applied until it is fired. (usually by a scripted sequence)
	trace_t		trace;
	int			entityIndex;

	UTIL_TraceLine( GetAbsOrigin() - Vector(5,5,5), GetAbsOrigin() + Vector(5,5,5), MASK_SOLID_BRUSHONLY, this, COLLISION_GROUP_NONE, &trace );

	entityIndex = trace.m_pEnt ? trace.m_pEnt->entindex() : 0;

	CBroadcastRecipientFilter filter;

	te->BSPDecal( filter, 0.0, 
		&GetAbsOrigin(), entityIndex, m_nTexture );

	SetThink( &CDecal::SUB_Remove );
	SetNextThink( gpGlobals->curtime + 0.1f );
}


void CDecal::InputActivate( inputdata_t &inputdata )
{
	TriggerDecal( inputdata.pActivator, inputdata.pCaller, USE_ON, 0 );
}


void CDecal::StaticDecal( void )
{
	class CTraceFilterValidForDecal : public CTraceFilterSimple
	{
	public:
		CTraceFilterValidForDecal(const IHandleEntity *passentity, int collisionGroup )
		 :	CTraceFilterSimple( passentity, collisionGroup )
		{
		}

		virtual bool ShouldHitEntity( IHandleEntity *pServerEntity, int contentsMask )
		{
			static const char *ppszIgnoredClasses[] = 
			{
				"weapon_*",
				"item_*",
				"prop_ragdoll",
				"prop_dynamic",
				"prop_static",
				"prop_physics",
				"npc_bullseye",  // Tracker 15335
			};

			CBaseEntity *pEntity = EntityFromEntityHandle( pServerEntity );

			// Tracker 15335:  Never impact decals against entities which are not rendering, either.
			if ( pEntity->IsEffectActive( EF_NODRAW ) )
				return false;

			for ( int i = 0; i < ARRAYSIZE(ppszIgnoredClasses); i++ )
			{
				if ( pEntity->ClassMatches( ppszIgnoredClasses[i] ) )
					return false;
			}


			return CTraceFilterSimple::ShouldHitEntity( pServerEntity, contentsMask );
		}
	};

	trace_t trace;
	CTraceFilterValidForDecal traceFilter( this, COLLISION_GROUP_NONE );
	int entityIndex, modelIndex = 0;

	Vector position = GetAbsOrigin();
	UTIL_TraceLine( position - Vector(5,5,5), position + Vector(5,5,5),  MASK_SOLID, &traceFilter, &trace );

	bool canDraw = true;

	entityIndex = trace.m_pEnt ? (short)trace.m_pEnt->entindex() : 0;
	if ( entityIndex )
	{
		CBaseEntity *ent = trace.m_pEnt;
		if ( ent )
		{
			modelIndex = ent->GetModelIndex();
			VectorITransform( GetAbsOrigin(), ent->EntityToWorldTransform(), position );

			canDraw = ( modelIndex != 0 );
			if ( !canDraw )
			{
				Warning( "Suppressed StaticDecal which would have hit entity %i (class:%s, name:%s) with modelindex = 0\n",
					ent->entindex(),
					ent->GetClassname(),
					STRING( ent->GetEntityName() ) );
			}
		}
	}

	if ( canDraw )
	{
		engine->StaticDecal( position, m_nTexture, entityIndex, modelIndex, m_bLowPriority );
	}

	SUB_Remove();
}


bool CDecal::KeyValue( const char *szKeyName, const char *szValue )
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
