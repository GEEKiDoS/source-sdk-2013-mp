//========= Copyright Valve Corporation, All rights reserved. ============//

#include "cbase.h"

#include "activitylist.h"
#include "ai_networkmanager.h"
#include "ai_schedule.h"
#include "client.h"
#include "decals.h"
#include "EnvMessage.h"
#include "eventlist.h"
#include "eventqueue.h"
#include "globalstate.h"
#include "particle_parse.h"
#include "soundent.h"

#include "entities/CWorld.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern CBaseEntity *g_pLastSpawn;

extern void InitBodyQue( void );
extern void W_Precache( void );

//=======================
// CWorld
//
// This spawns first when each level begins.
//=======================
LINK_ENTITY_TO_CLASS( worldspawn, CWorld );

BEGIN_DATADESC( CWorld )

	DEFINE_FIELD( m_flWaveHeight, FIELD_FLOAT ),

	// keyvalues are parsed from map, but not saved/loaded
	DEFINE_KEYFIELD( m_iszChapterTitle, FIELD_STRING, "chaptertitle" ),
	DEFINE_KEYFIELD( m_bStartDark,		FIELD_BOOLEAN, "startdark" ),
	DEFINE_KEYFIELD( m_bDisplayTitle,	FIELD_BOOLEAN, "gametitle" ),
	DEFINE_FIELD( m_WorldMins, FIELD_VECTOR ),
	DEFINE_FIELD( m_WorldMaxs, FIELD_VECTOR ),
#ifdef _X360
	DEFINE_KEYFIELD( m_flMaxOccludeeArea, FIELD_FLOAT, "maxoccludeearea_x360" ),
	DEFINE_KEYFIELD( m_flMinOccluderArea, FIELD_FLOAT, "minoccluderarea_x360" ),
#else
	DEFINE_KEYFIELD( m_flMaxOccludeeArea, FIELD_FLOAT, "maxoccludeearea" ),
	DEFINE_KEYFIELD( m_flMinOccluderArea, FIELD_FLOAT, "minoccluderarea" ),
#endif
	DEFINE_KEYFIELD( m_flMaxPropScreenSpaceWidth, FIELD_FLOAT, "maxpropscreenwidth" ),
	DEFINE_KEYFIELD( m_flMinPropScreenSpaceWidth, FIELD_FLOAT, "minpropscreenwidth" ),
	DEFINE_KEYFIELD( m_iszDetailSpriteMaterial, FIELD_STRING, "detailmaterial" ),
	DEFINE_KEYFIELD( m_bColdWorld,		FIELD_BOOLEAN, "coldworld" ),

END_DATADESC()


// SendTable stuff.
IMPLEMENT_SERVERCLASS_ST(CWorld, DT_WORLD)
	SendPropFloat	(SENDINFO(m_flWaveHeight), 8, SPROP_ROUNDUP,	0.0f,	8.0f),
	SendPropVector	(SENDINFO(m_WorldMins),	-1,	SPROP_COORD),
	SendPropVector	(SENDINFO(m_WorldMaxs),	-1,	SPROP_COORD),
	SendPropInt		(SENDINFO(m_bStartDark), 1, SPROP_UNSIGNED ),
	SendPropFloat	(SENDINFO(m_flMaxOccludeeArea), 0, SPROP_NOSCALE ),
	SendPropFloat	(SENDINFO(m_flMinOccluderArea), 0, SPROP_NOSCALE ),
	SendPropFloat	(SENDINFO(m_flMaxPropScreenSpaceWidth), 0, SPROP_NOSCALE ),
	SendPropFloat	(SENDINFO(m_flMinPropScreenSpaceWidth), 0, SPROP_NOSCALE ),
	SendPropStringT (SENDINFO(m_iszDetailSpriteMaterial) ),
	SendPropInt		(SENDINFO(m_bColdWorld), 1, SPROP_UNSIGNED ),
END_SEND_TABLE()

//
// Just to ignore the "wad" field.
//
bool CWorld::KeyValue( const char *szKeyName, const char *szValue )
{
	if ( FStrEq(szKeyName, "skyname") )
	{
		// Sent over net now.
		ConVarRef skyname( "sv_skyname" );
		skyname.SetValue( szValue );
	}
	else if ( FStrEq(szKeyName, "newunit") )
	{
		// Single player only.  Clear save directory if set
		if ( atoi(szValue) )
		{
			extern void Game_SetOneWayTransition();
			Game_SetOneWayTransition();
		}
	}
	else if ( FStrEq(szKeyName, "world_mins") )
	{
		Vector vec;
		sscanf(	szValue, "%f %f %f", &vec.x, &vec.y, &vec.z );
		m_WorldMins = vec;
	}
	else if ( FStrEq(szKeyName, "world_maxs") )
	{
		Vector vec;
		sscanf(	szValue, "%f %f %f", &vec.x, &vec.y, &vec.z ); 
		m_WorldMaxs = vec;
	}
	else
		return BaseClass::KeyValue( szKeyName, szValue );

	return true;
}


extern bool		g_fGameOver;
static CWorld *g_WorldEntity = NULL;

CWorld* GetWorldEntity()
{
	return g_WorldEntity;
}

CWorld::CWorld( )
{
	AddEFlags( EFL_NO_AUTO_EDICT_ATTACH | EFL_KEEP_ON_RECREATE_ENTITIES );
	NetworkProp()->AttachEdict( INDEXENT(RequiredEdictIndex()) );
	ActivityList_Init();
	EventList_Init();
	
	SetSolid( SOLID_BSP );
	SetMoveType( MOVETYPE_NONE );

	m_bColdWorld = false;
}

CWorld::~CWorld( )
{
	EventList_Free();
	ActivityList_Free();
	if ( g_pGameRules )
	{
		g_pGameRules->LevelShutdown();
		delete g_pGameRules;
		g_pGameRules = NULL;
	}
	g_WorldEntity = NULL;
}


//------------------------------------------------------------------------------
// Purpose : Add a decal to the world
// Input   :
// Output  :
//------------------------------------------------------------------------------
void CWorld::DecalTrace( trace_t *pTrace, char const *decalName)
{
	int index = decalsystem->GetDecalIndexForName( decalName );
	if ( index < 0 )
		return;

	CBroadcastRecipientFilter filter;
	if ( pTrace->hitbox != 0 )
	{
		te->Decal( filter, 0.0f, &pTrace->endpos, &pTrace->startpos, 0, pTrace->hitbox, index );
	}
	else
	{
		te->WorldDecal( filter, 0.0, &pTrace->endpos, index );
	}
}

void CWorld::RegisterSharedActivities( void )
{
	ActivityList_RegisterSharedActivities();
}

void CWorld::RegisterSharedEvents( void )
{
	EventList_RegisterSharedEvents();
}


void CWorld::Spawn( void )
{
	SetLocalOrigin( vec3_origin );
	SetLocalAngles( vec3_angle );
	// NOTE:  SHOULD NEVER BE ANYTHING OTHER THAN 1!!!
	SetModelIndex( 1 );
	// world model
	SetModelName( AllocPooledString( modelinfo->GetModelName( GetModel() ) ) );
	AddFlag( FL_WORLDBRUSH );

	g_EventQueue.Init();
	Precache( );
	GlobalEntity_Add( "is_console", STRING(gpGlobals->mapname), ( IsConsole() ) ? GLOBAL_ON : GLOBAL_OFF );
	GlobalEntity_Add( "is_pc", STRING(gpGlobals->mapname), ( !IsConsole() ) ? GLOBAL_ON : GLOBAL_OFF );
}

static const char *g_DefaultLightstyles[] =
{
	// 0 normal
	"m",
	// 1 FLICKER (first variety)
	"mmnmmommommnonmmonqnmmo",
	// 2 SLOW STRONG PULSE
	"abcdefghijklmnopqrstuvwxyzyxwvutsrqponmlkjihgfedcba",
	// 3 CANDLE (first variety)
	"mmmmmaaaaammmmmaaaaaabcdefgabcdefg",
	// 4 FAST STROBE
	"mamamamamama",
	// 5 GENTLE PULSE 1
	"jklmnopqrstuvwxyzyxwvutsrqponmlkj",
	// 6 FLICKER (second variety)
	"nmonqnmomnmomomno",
	// 7 CANDLE (second variety)
	"mmmaaaabcdefgmmmmaaaammmaamm",
	// 8 CANDLE (third variety)
	"mmmaaammmaaammmabcdefaaaammmmabcdefmmmaaaa",
	// 9 SLOW STROBE (fourth variety)
	"aaaaaaaazzzzzzzz",
	// 10 FLUORESCENT FLICKER
	"mmamammmmammamamaaamammma",
	// 11 SLOW PULSE NOT FADE TO BLACK
	"abcdefghijklmnopqrrqponmlkjihgfedcba",
	// 12 UNDERWATER LIGHT MUTATION
	// this light only distorts the lightmap - no contribution
	// is made to the brightness of affected surfaces
	"mmnnmmnnnmmnn",
};


const char *GetDefaultLightstyleString( int styleIndex )
{
	if ( styleIndex < ARRAYSIZE(g_DefaultLightstyles) )
	{
		return g_DefaultLightstyles[styleIndex];
	}
	return "m";
}

void CWorld::Precache( void )
{
	g_WorldEntity = this;
	g_fGameOver = false;
	g_pLastSpawn = NULL;

	ConVarRef stepsize( "sv_stepsize" );
	stepsize.SetValue( 18 );

	ConVarRef roomtype( "room_type" );
	roomtype.SetValue( 0 );

	// Set up game rules
	Assert( !g_pGameRules );
	if (g_pGameRules)
	{
		delete g_pGameRules;
	}

	InstallGameRules();
	Assert( g_pGameRules );
	g_pGameRules->Init();

	CSoundEnt::InitSoundEnt();

	// Only allow precaching between LevelInitPreEntity and PostEntity
	CBaseEntity::SetAllowPrecache( true );
	IGameSystem::LevelInitPreEntityAllSystems( STRING( GetModelName() ) );

	// Create the player resource
	g_pGameRules->CreateStandardEntities();

	// UNDONE: Make most of these things server systems or precache_registers
	// =================================================
	//	Activities
	// =================================================
	ActivityList_Free();
	RegisterSharedActivities();

	EventList_Free();
	RegisterSharedEvents();

	InitBodyQue();
// init sentence group playback stuff from sentences.txt.
// ok to call this multiple times, calls after first are ignored.

	SENTENCEG_Init();

	// Precache standard particle systems
	PrecacheStandardParticleSystems( );

// the area based ambient sounds MUST be the first precache_sounds

// player precaches     
	W_Precache ();									// get weapon precaches
	ClientPrecache();
	g_pGameRules->Precache();
	// precache all temp ent stuff
	CBaseTempEntity::PrecacheTempEnts();

	g_Language.SetValue( LANGUAGE_ENGLISH );	// TODO use VGUI to get current language

	if ( g_Language.GetInt() == LANGUAGE_GERMAN )
	{
		PrecacheModel( "models/germangibs.mdl" );
	}
	else
	{
		PrecacheModel( "models/gibs/hgibs.mdl" );
	}

	PrecacheScriptSound( "BaseEntity.EnterWater" );
	PrecacheScriptSound( "BaseEntity.ExitWater" );

//
// Setup light animation tables. 'a' is total darkness, 'z' is maxbright.
//
	for ( int i = 0; i < ARRAYSIZE(g_DefaultLightstyles); i++ )
	{
		engine->LightStyle( i, GetDefaultLightstyleString(i) );
	}

	// styles 32-62 are assigned by the light program for switchable lights

	// 63 testing
	engine->LightStyle(63, "a");

	// =================================================
	//	Load and Init AI Networks
	// =================================================
	CAI_NetworkManager::InitializeAINetworks();
	// =================================================
	//	Load and Init AI Schedules
	// =================================================
	g_AI_SchedulesManager.LoadAllSchedules();
	// =================================================
	//	Initialize NPC Relationships
	// =================================================
	g_pGameRules->InitDefaultAIRelationships();
	CBaseCombatCharacter::InitInteractionSystem();

	// Call all registered precachers.
	CPrecacheRegister::Precache();	

	if ( m_iszChapterTitle != NULL_STRING )
	{
		DevMsg( 2, "Chapter title: %s\n", STRING(m_iszChapterTitle) );
		CMessage *pMessage = (CMessage *)CBaseEntity::Create( "env_message", vec3_origin, vec3_angle, NULL );
		if ( pMessage )
		{
			pMessage->SetMessage( m_iszChapterTitle );
			m_iszChapterTitle = NULL_STRING;

			// send the message entity a play message command, delayed by 1 second
			pMessage->AddSpawnFlags( SF_MESSAGE_ONCE );
			pMessage->SetThink( &CMessage::SUB_CallUseToggle );
			pMessage->SetNextThink( gpGlobals->curtime + 1.0f );
		}
	}

	g_iszFuncBrushClassname = AllocPooledString("func_brush");
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : float
//-----------------------------------------------------------------------------
float GetRealTime()
{
	return engine->Time();
}


bool CWorld::GetDisplayTitle() const
{
	return m_bDisplayTitle;
}

bool CWorld::GetStartDark() const
{
	return m_bStartDark;
}

void CWorld::SetDisplayTitle( bool display )
{
	m_bDisplayTitle = display;
}

void CWorld::SetStartDark( bool startdark )
{
	m_bStartDark = startdark;
}

bool CWorld::IsColdWorld( void )
{
	return m_bColdWorld;
}
