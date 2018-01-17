//========= Copyright Valve Corporation, All rights reserved. ============//

#include "cbase.h"

#include "entities/logic/CLogicAutosave.h"
#include "entities/logic/CLogicActiveAutosave.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS(logic_active_autosave, CLogicActiveAutosave);

BEGIN_DATADESC( CLogicActiveAutosave )
	DEFINE_KEYFIELD( m_TriggerHitPoints, FIELD_INTEGER, "TriggerHitPoints" ),
	DEFINE_KEYFIELD( m_flTimeToTrigger, FIELD_FLOAT, "TimeToTrigger" ),
	DEFINE_KEYFIELD( m_flDangerousTime, FIELD_FLOAT, "DangerousTime" ),
	DEFINE_FIELD( m_flStartTime, FIELD_TIME ),
	DEFINE_THINKFUNC( SaveThink ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),
END_DATADESC()


//-----------------------------------------------------------------------------
// Purpose: Keyfield set func
//-----------------------------------------------------------------------------
void CLogicAutosave::InputSetMinHitpointsThreshold( inputdata_t &inputdata )
{
	int setTo = inputdata.value.Int();
	AssertMsg1(setTo >= 0 && setTo <= 100, "Tried to set autosave MinHitpointsThreshold to %d!\n", setTo);
	m_minHitPoints = setTo;
}
