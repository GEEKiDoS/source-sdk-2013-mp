//========= Copyright Valve Corporation, All rights reserved. ============//

#include "cbase.h"

#include "entities/logic/CLogicLineToEntity.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS( logic_lineto, CLogicLineToEntity );


BEGIN_DATADESC( CLogicLineToEntity )

	// Keys
	// target is handled in the base class, stored in field m_target
	DEFINE_KEYFIELD( m_SourceName, FIELD_STRING, "source" ),
 	DEFINE_FIELD( m_StartEntity, FIELD_EHANDLE ),
	DEFINE_FIELD( m_EndEntity, FIELD_EHANDLE ),

	// Outputs
	DEFINE_OUTPUT( m_Line, "Line" ),

END_DATADESC()



//-----------------------------------------------------------------------------
// Find the entities
//-----------------------------------------------------------------------------
void CLogicLineToEntity::Activate(void)
{
	BaseClass::Activate();

	if (m_target != NULL_STRING)
	{
		m_EndEntity = gEntList.FindEntityByName( NULL, m_target );

		//
		// If we were given a bad measure target, just measure sound where we are.
		//
		if ((m_EndEntity == NULL) || (m_EndEntity->edict() == NULL))
		{
			Warning( "logic_lineto - Target not found or target with no origin!\n");
			m_EndEntity = this;
		}
	}
	else
	{
		m_EndEntity = this;
	}

	if (m_SourceName != NULL_STRING)
	{
		m_StartEntity = gEntList.FindEntityByName( NULL, m_SourceName );

		//
		// If we were given a bad measure target, just measure sound where we are.
		//
		if ((m_StartEntity == NULL) || (m_StartEntity->edict() == NULL))
		{
			Warning( "logic_lineto - Source not found or source with no origin!\n");
			m_StartEntity = this;
		}
	}
	else
	{
		m_StartEntity = this;
	}
}


//-----------------------------------------------------------------------------
// Find the entities
//-----------------------------------------------------------------------------
void CLogicLineToEntity::Spawn(void)
{
	SetNextThink( gpGlobals->curtime + 0.01f );
}


//-----------------------------------------------------------------------------
// Find the entities
//-----------------------------------------------------------------------------
void CLogicLineToEntity::Think(void)
{
	CBaseEntity* pDest = m_EndEntity.Get();
	CBaseEntity* pSrc = m_StartEntity.Get();
	if (!pDest || !pSrc || !pDest->edict() || !pSrc->edict())
	{
		// Can sleep for a long time, no more lines.
		m_Line.Set( vec3_origin, this, this );
		SetNextThink( gpGlobals->curtime + 10 );
		return;
	}

	Vector delta;
	VectorSubtract( pDest->GetAbsOrigin(), pSrc->GetAbsOrigin(), delta ); 
	m_Line.Set(delta, this, this);

	SetNextThink( gpGlobals->curtime + 0.01f );
}