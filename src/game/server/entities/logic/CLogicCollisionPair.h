//========= Copyright Valve Corporation, All rights reserved. ============//

#ifndef GAME_SERVER_ENTITIES_LOGIC_CLOGICCOLLISIONPAIR_H
#define GAME_SERVER_ENTITIES_LOGIC_CLOGICCOLLISIONPAIR_H

#ifdef WIN32
#pragma once
#endif

// Finds the named physics object.  If no name, returns the world
// If a name is specified and an object not found - errors are reported
IPhysicsObject *FindPhysicsObjectByNameOrWorld( string_t name, CBaseEntity *pErrorEntity );

class CLogicCollisionPair : public CLogicalEntity
{
	DECLARE_CLASS( CLogicCollisionPair, CLogicalEntity );
public:

	void EnableCollisions( bool bEnable )
	{
		IPhysicsObject *pPhysics0 = FindPhysicsObjectByNameOrWorld( m_nameAttach1, this );
		IPhysicsObject *pPhysics1 = FindPhysicsObjectByNameOrWorld( m_nameAttach2, this );

		// need two different objects to do anything
		if ( pPhysics0 && pPhysics1 && pPhysics0 != pPhysics1 )
		{
			m_disabled = !bEnable;
			m_succeeded = true;
			if ( bEnable )
			{
				PhysEnableEntityCollisions( pPhysics0, pPhysics1 );
			}
			else
			{
				PhysDisableEntityCollisions( pPhysics0, pPhysics1 );
			}
		}
		else
		{
			m_succeeded = false;
		}
	}

	void Activate( void )
	{
		if ( m_disabled )
		{
			EnableCollisions( false );
		}
		BaseClass::Activate();
	}

	void InputDisableCollisions( inputdata_t &inputdata )
	{
		if ( m_succeeded && m_disabled )
			return;
		EnableCollisions( false );
	}

	void InputEnableCollisions( inputdata_t &inputdata )
	{
		if ( m_succeeded && !m_disabled )
			return;
		EnableCollisions( true );
	}
	// If Activate() becomes PostSpawn()
	//void OnRestore() { Activate(); }

	DECLARE_DATADESC();

private:
	string_t		m_nameAttach1;
	string_t		m_nameAttach2;
	bool			m_disabled;
	bool			m_succeeded;
};

#endif // GAME_SERVER_ENTITIES_LOGIC_CLOGICCOLLISIONPAIR_H
