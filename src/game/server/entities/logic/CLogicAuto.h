//========= Copyright Valve Corporation, All rights reserved. ============//

#ifndef GAME_SERVER_ENTITIES_LOGIC_CLOGICAUTO_H
#define GAME_SERVER_ENTITIES_LOGIC_CLOGICAUTO_H

#ifdef WIN32
#pragma once
#endif

const int SF_AUTO_FIREONCE = 0x01;
const int SF_AUTO_FIREONRELOAD = 0x02;

class CLogicAuto : public CBaseEntity
{
public:
	DECLARE_CLASS(CLogicAuto, CBaseEntity);

	void Activate(void);
	void Think(void);

	int ObjectCaps(void) { return BaseClass::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }

	DECLARE_DATADESC();

private:

	// fired no matter why the map loaded
	COutputEvent m_OnMapSpawn;

	// fired for specified types of map loads
	COutputEvent m_OnNewGame;
	COutputEvent m_OnLoadGame;
	COutputEvent m_OnMapTransition;
	COutputEvent m_OnBackgroundMap;
	COutputEvent m_OnMultiNewMap;
	COutputEvent m_OnMultiNewRound;

	string_t m_globalstate;
};

#endif // GAME_SERVER_ENTITIES_LOGIC_CLOGICAUTO_H
