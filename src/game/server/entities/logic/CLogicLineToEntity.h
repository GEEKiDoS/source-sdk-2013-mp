//========= Copyright Valve Corporation, All rights reserved. ============//

#ifndef GAME_SERVER_ENTITIES_LOGIC_CLOGICLINETOENTITY_H
#define GAME_SERVER_ENTITIES_LOGIC_CLOGICLINETOENTITY_H

#ifdef WIN32
#pragma once
#endif

//-----------------------------------------------------------------------------
// Purpose: Computes a line between two entities
//-----------------------------------------------------------------------------
class CLogicLineToEntity : public CLogicalEntity
{
public:
	DECLARE_CLASS( CLogicLineToEntity, CLogicalEntity );

	void Activate(void);
	void Spawn( void );
	void Think( void );

	// outputs
	COutputVector m_Line;

	DECLARE_DATADESC();

private:
	string_t m_SourceName;
	EHANDLE	m_StartEntity;
	EHANDLE m_EndEntity;
};

#endif // GAME_SERVER_ENTITIES_LOGIC_CLOGICLINETOENTITY_H
