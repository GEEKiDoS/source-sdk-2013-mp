//========= Copyright Valve Corporation, All rights reserved. ============//

#ifndef GAME_SERVER_TOOLS_NWCEDIT_H
#define GAME_SERVER_TOOLS_NWCEDIT_H

#ifdef WIN32
#pragma once
#endif

class CBaseEntity;

//=============================================================================
//	>> NWCEdit
//=============================================================================
namespace NWCEdit
{
	Vector	AirNodePlacementPosition( void );
	bool	IsWCVersionValid(void);
	void	CreateAINode(   CBasePlayer *pPlayer );
	void	DestroyAINode(  CBasePlayer *pPlayer );
	void	CreateAILink(	CBasePlayer *pPlayer );
	void	DestroyAILink(  CBasePlayer *pPlayer );
	void	UndoDestroyAINode(void);
	void	RememberEntityPosition( CBaseEntity *pEntity );
	void	UpdateEntityPosition( CBaseEntity *pEntity );
};

#endif // GAME_SERVER_TOOLS_NWCEDIT_H
