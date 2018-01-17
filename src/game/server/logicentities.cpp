//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Implements many of the entities that control logic flow within a map.
//
//=============================================================================

#include "cbase.h"
#include "entityinput.h"
#include "entityoutput.h"
#include "eventqueue.h"
#include "mathlib/mathlib.h"
#include "globalstate.h"
#include "ndebugoverlay.h"
#include "saverestore_utlvector.h"
#include "vstdlib/random.h"
#include "gameinterface.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


extern CServerGameDLL g_ServerGameDLL;

//-----------------------------------------------------------------------------
// Purpose: Holds a value that can be added to and subtracted from.
//-----------------------------------------------------------------------------
class CMathCounter : public CLogicalEntity
{
	DECLARE_CLASS( CMathCounter, CLogicalEntity );
private:
	float m_flMin;		// Minimum clamp value. If min and max are BOTH zero, no clamping is done.
	float m_flMax;		// Maximum clamp value.
	bool m_bHitMin;		// Set when we reach or go below our minimum value, cleared if we go above it again.
	bool m_bHitMax;		// Set when we reach or exceed our maximum value, cleared if we fall below it again.

	bool m_bDisabled;

	bool KeyValue(const char *szKeyName, const char *szValue);
	void Spawn(void);

	int DrawDebugTextOverlays(void);

	void UpdateOutValue(CBaseEntity *pActivator, float fNewValue);

	// Inputs
	void InputAdd( inputdata_t &inputdata );
	void InputDivide( inputdata_t &inputdata );
	void InputMultiply( inputdata_t &inputdata );
	void InputSetValue( inputdata_t &inputdata );
	void InputSetValueNoFire( inputdata_t &inputdata );
	void InputSubtract( inputdata_t &inputdata );
	void InputSetHitMax( inputdata_t &inputdata );
	void InputSetHitMin( inputdata_t &inputdata );
	void InputGetValue( inputdata_t &inputdata );
	void InputEnable( inputdata_t &inputdata );
	void InputDisable( inputdata_t &inputdata );

	// Outputs
	COutputFloat m_OutValue;
	COutputFloat m_OnGetValue;	// Used for polling the counter value.
	COutputEvent m_OnHitMin;
	COutputEvent m_OnHitMax;

	DECLARE_DATADESC();
};

LINK_ENTITY_TO_CLASS(math_counter, CMathCounter);


BEGIN_DATADESC( CMathCounter )

	DEFINE_FIELD(m_bHitMax, FIELD_BOOLEAN),
	DEFINE_FIELD(m_bHitMin, FIELD_BOOLEAN),

	// Keys
	DEFINE_KEYFIELD(m_flMin, FIELD_FLOAT, "min"),
	DEFINE_KEYFIELD(m_flMax, FIELD_FLOAT, "max"),

	DEFINE_KEYFIELD(m_bDisabled, FIELD_BOOLEAN, "StartDisabled" ),

	// Inputs
	DEFINE_INPUTFUNC(FIELD_FLOAT, "Add", InputAdd),
	DEFINE_INPUTFUNC(FIELD_FLOAT, "Divide", InputDivide),
	DEFINE_INPUTFUNC(FIELD_FLOAT, "Multiply", InputMultiply),
	DEFINE_INPUTFUNC(FIELD_FLOAT, "SetValue", InputSetValue),
	DEFINE_INPUTFUNC(FIELD_FLOAT, "SetValueNoFire", InputSetValueNoFire),
	DEFINE_INPUTFUNC(FIELD_FLOAT, "Subtract", InputSubtract),
	DEFINE_INPUTFUNC(FIELD_FLOAT, "SetHitMax", InputSetHitMax),
	DEFINE_INPUTFUNC(FIELD_FLOAT, "SetHitMin", InputSetHitMin),
	DEFINE_INPUTFUNC(FIELD_VOID, "GetValue", InputGetValue),
	DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),

	// Outputs
	DEFINE_OUTPUT(m_OutValue, "OutValue"),
	DEFINE_OUTPUT(m_OnHitMin, "OnHitMin"),
	DEFINE_OUTPUT(m_OnHitMax, "OnHitMax"),
	DEFINE_OUTPUT(m_OnGetValue, "OnGetValue"),

END_DATADESC()




//-----------------------------------------------------------------------------
// Purpose: Handles key values from the BSP before spawn is called.
//-----------------------------------------------------------------------------
bool CMathCounter::KeyValue(const char *szKeyName, const char *szValue)
{
	//
	// Set the initial value of the counter.
	//
	if (!stricmp(szKeyName, "startvalue"))
	{
		m_OutValue.Init(atoi(szValue));
		return(true);
	}

	return(BaseClass::KeyValue(szKeyName, szValue));
}


//-----------------------------------------------------------------------------
// Purpose: Called before spawning, after key values have been set.
//-----------------------------------------------------------------------------
void CMathCounter::Spawn( void )
{
	//
	// Make sure max and min are ordered properly or clamp won't work.
	//
	if (m_flMin > m_flMax)
	{
		float flTemp = m_flMax;
		m_flMax = m_flMin;
		m_flMin = flTemp;
	}

	//
	// Clamp initial value to within the valid range.
	//
	if ((m_flMin != 0) || (m_flMax != 0))
	{
		float flStartValue = clamp(m_OutValue.Get(), m_flMin, m_flMax);
		m_OutValue.Init(flStartValue);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Draw any debug text overlays
// Input  :
// Output : Current text offset from the top
//-----------------------------------------------------------------------------
int CMathCounter::DrawDebugTextOverlays( void ) 
{
	int text_offset = BaseClass::DrawDebugTextOverlays();

	if (m_debugOverlays & OVERLAY_TEXT_BIT) 
	{
		char tempstr[512];

		Q_snprintf(tempstr,sizeof(tempstr),"    min value: %f", m_flMin);
		EntityText(text_offset,tempstr,0);
		text_offset++;

		Q_snprintf(tempstr,sizeof(tempstr),"    max value: %f", m_flMax);
		EntityText(text_offset,tempstr,0);
		text_offset++;

		Q_snprintf(tempstr,sizeof(tempstr),"current value: %f", m_OutValue.Get());
		EntityText(text_offset,tempstr,0);
		text_offset++;

		if( m_bDisabled )
		{	
			Q_snprintf(tempstr,sizeof(tempstr),"*DISABLED*");		
		}
		else
		{
			Q_snprintf(tempstr,sizeof(tempstr),"Enabled.");
		}
		EntityText(text_offset,tempstr,0);
		text_offset++;

	}
	return text_offset;
}

//-----------------------------------------------------------------------------
// Change min/max
//-----------------------------------------------------------------------------
void CMathCounter::InputSetHitMax( inputdata_t &inputdata )
{
	m_flMax = inputdata.value.Float();
	if ( m_flMax < m_flMin )
	{
		m_flMin = m_flMax;
	}
	UpdateOutValue( inputdata.pActivator, m_OutValue.Get() );
}

void CMathCounter::InputSetHitMin( inputdata_t &inputdata )
{
	m_flMin = inputdata.value.Float();
	if ( m_flMax < m_flMin )
	{
		m_flMax = m_flMin;
	}
	UpdateOutValue( inputdata.pActivator, m_OutValue.Get() );
}

	
//-----------------------------------------------------------------------------
// Purpose: Input handler for adding to the accumulator value.
// Input  : Float value to add.
//-----------------------------------------------------------------------------
void CMathCounter::InputAdd( inputdata_t &inputdata )
{
	if( m_bDisabled )
	{
		DevMsg("Math Counter %s ignoring ADD because it is disabled\n", GetDebugName() );
		return;
	}

	float fNewValue = m_OutValue.Get() + inputdata.value.Float();
	UpdateOutValue( inputdata.pActivator, fNewValue );
}


//-----------------------------------------------------------------------------
// Purpose: Input handler for multiplying the current value.
// Input  : Float value to multiply the value by.
//-----------------------------------------------------------------------------
void CMathCounter::InputDivide( inputdata_t &inputdata )
{
	if( m_bDisabled )
	{
		DevMsg("Math Counter %s ignoring DIVIDE because it is disabled\n", GetDebugName() );
		return;
	}

	if (inputdata.value.Float() != 0)
	{
		float fNewValue = m_OutValue.Get() / inputdata.value.Float();
		UpdateOutValue( inputdata.pActivator, fNewValue );
	}
	else
	{
		DevMsg( 1, "LEVEL DESIGN ERROR: Divide by zero in math_value\n" );
		UpdateOutValue( inputdata.pActivator, m_OutValue.Get() );
	}
}


//-----------------------------------------------------------------------------
// Purpose: Input handler for multiplying the current value.
// Input  : Float value to multiply the value by.
//-----------------------------------------------------------------------------
void CMathCounter::InputMultiply( inputdata_t &inputdata )
{
	if( m_bDisabled )
	{
		DevMsg("Math Counter %s ignoring MULTIPLY because it is disabled\n", GetDebugName() );
		return;
	}

	float fNewValue = m_OutValue.Get() * inputdata.value.Float();
	UpdateOutValue( inputdata.pActivator, fNewValue );
}


//-----------------------------------------------------------------------------
// Purpose: Input handler for updating the value.
// Input  : Float value to set.
//-----------------------------------------------------------------------------
void CMathCounter::InputSetValue( inputdata_t &inputdata )
{
	if( m_bDisabled )
	{
		DevMsg("Math Counter %s ignoring SETVALUE because it is disabled\n", GetDebugName() );
		return;
	}

	UpdateOutValue( inputdata.pActivator, inputdata.value.Float() );
}


//-----------------------------------------------------------------------------
// Purpose: Input handler for updating the value.
// Input  : Float value to set.
//-----------------------------------------------------------------------------
void CMathCounter::InputSetValueNoFire( inputdata_t &inputdata )
{
	if( m_bDisabled )
	{
		DevMsg("Math Counter %s ignoring SETVALUENOFIRE because it is disabled\n", GetDebugName() );
		return;
	}

	float flNewValue = inputdata.value.Float();
	if (( m_flMin != 0 ) || (m_flMax != 0 ))
	{
		flNewValue = clamp(flNewValue, m_flMin, m_flMax);
	}

	m_OutValue.Init( flNewValue );
}


//-----------------------------------------------------------------------------
// Purpose: Input handler for subtracting from the current value.
// Input  : Float value to subtract.
//-----------------------------------------------------------------------------
void CMathCounter::InputSubtract( inputdata_t &inputdata )
{
	if( m_bDisabled )
	{
		DevMsg("Math Counter %s ignoring SUBTRACT because it is disabled\n", GetDebugName() );
		return;
	}

	float fNewValue = m_OutValue.Get() - inputdata.value.Float();
	UpdateOutValue( inputdata.pActivator, fNewValue );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CMathCounter::InputGetValue( inputdata_t &inputdata )
{
	float flOutValue = m_OutValue.Get();
	m_OnGetValue.Set( flOutValue, inputdata.pActivator, inputdata.pCaller );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CMathCounter::InputEnable( inputdata_t &inputdata )
{
	m_bDisabled = false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CMathCounter::InputDisable( inputdata_t &inputdata )
{
	m_bDisabled = true;
}

//-----------------------------------------------------------------------------
// Purpose: Sets the value to the new value, clamping and firing the output value.
// Input  : fNewValue - Value to set.
//-----------------------------------------------------------------------------
void CMathCounter::UpdateOutValue(CBaseEntity *pActivator, float fNewValue)
{
	if ((m_flMin != 0) || (m_flMax != 0))
	{
		//
		// Fire an output any time we reach or exceed our maximum value.
		//
		if ( fNewValue >= m_flMax )
		{
			if ( !m_bHitMax )
			{
				m_bHitMax = true;
				m_OnHitMax.FireOutput( pActivator, this );
			}
		}
		else
		{
			m_bHitMax = false;
		}

		//
		// Fire an output any time we reach or go below our minimum value.
		//
		if ( fNewValue <= m_flMin )
		{
			if ( !m_bHitMin )
			{
				m_bHitMin = true;
				m_OnHitMin.FireOutput( pActivator, this );
			}
		}
		else
		{
			m_bHitMin = false;
		}

		fNewValue = clamp(fNewValue, m_flMin, m_flMax);
	}

	m_OutValue.Set(fNewValue, pActivator, this);
}



//-----------------------------------------------------------------------------
// Purpose: Compares a single string input to up to 16 case values, firing an
//			output corresponding to the case value that matched, or a default
//			output if the input value didn't match any of the case values.
//
//			This can also be used to fire a random output from a set of outputs.
//-----------------------------------------------------------------------------
#define MAX_LOGIC_CASES 16

class CLogicCase : public CLogicalEntity
{
	DECLARE_CLASS( CLogicCase, CLogicalEntity );
private:
	string_t m_nCase[MAX_LOGIC_CASES];

	int m_nShuffleCases;
	int m_nLastShuffleCase;
	unsigned char m_uchShuffleCaseMap[MAX_LOGIC_CASES];

	void Spawn(void);

	int BuildCaseMap(unsigned char *puchMap);

	// Inputs
	void InputValue( inputdata_t &inputdata );
	void InputPickRandom( inputdata_t &inputdata );
	void InputPickRandomShuffle( inputdata_t &inputdata );

	// Outputs
	COutputEvent m_OnCase[MAX_LOGIC_CASES];		// Fired when the input value matches one of the case values.
	COutputVariant m_OnDefault;					// Fired when no match was found.

	DECLARE_DATADESC();
};

LINK_ENTITY_TO_CLASS(logic_case, CLogicCase);


BEGIN_DATADESC( CLogicCase )

// Silence, Classcheck!
//	DEFINE_ARRAY( m_nCase, FIELD_STRING, MAX_LOGIC_CASES ),

	// Keys
	DEFINE_KEYFIELD(m_nCase[0], FIELD_STRING, "Case01"),
	DEFINE_KEYFIELD(m_nCase[1], FIELD_STRING, "Case02"),
	DEFINE_KEYFIELD(m_nCase[2], FIELD_STRING, "Case03"),
	DEFINE_KEYFIELD(m_nCase[3], FIELD_STRING, "Case04"),
	DEFINE_KEYFIELD(m_nCase[4], FIELD_STRING, "Case05"),
	DEFINE_KEYFIELD(m_nCase[5], FIELD_STRING, "Case06"),
	DEFINE_KEYFIELD(m_nCase[6], FIELD_STRING, "Case07"),
	DEFINE_KEYFIELD(m_nCase[7], FIELD_STRING, "Case08"),
	DEFINE_KEYFIELD(m_nCase[8], FIELD_STRING, "Case09"),
	DEFINE_KEYFIELD(m_nCase[9], FIELD_STRING, "Case10"),
	DEFINE_KEYFIELD(m_nCase[10], FIELD_STRING, "Case11"),
	DEFINE_KEYFIELD(m_nCase[11], FIELD_STRING, "Case12"),
	DEFINE_KEYFIELD(m_nCase[12], FIELD_STRING, "Case13"),
	DEFINE_KEYFIELD(m_nCase[13], FIELD_STRING, "Case14"),
	DEFINE_KEYFIELD(m_nCase[14], FIELD_STRING, "Case15"),
	DEFINE_KEYFIELD(m_nCase[15], FIELD_STRING, "Case16"),
	
	DEFINE_FIELD( m_nShuffleCases, FIELD_INTEGER ),
	DEFINE_FIELD( m_nLastShuffleCase, FIELD_INTEGER ),
	DEFINE_ARRAY( m_uchShuffleCaseMap, FIELD_CHARACTER, MAX_LOGIC_CASES ),

	// Inputs
	DEFINE_INPUTFUNC(FIELD_INPUT, "InValue", InputValue),
	DEFINE_INPUTFUNC(FIELD_VOID, "PickRandom", InputPickRandom),
	DEFINE_INPUTFUNC(FIELD_VOID, "PickRandomShuffle", InputPickRandomShuffle),

	// Outputs
	DEFINE_OUTPUT(m_OnCase[0], "OnCase01"),
	DEFINE_OUTPUT(m_OnCase[1], "OnCase02"),
	DEFINE_OUTPUT(m_OnCase[2], "OnCase03"),
	DEFINE_OUTPUT(m_OnCase[3], "OnCase04"),
	DEFINE_OUTPUT(m_OnCase[4], "OnCase05"),
	DEFINE_OUTPUT(m_OnCase[5], "OnCase06"),
	DEFINE_OUTPUT(m_OnCase[6], "OnCase07"),
	DEFINE_OUTPUT(m_OnCase[7], "OnCase08"),
	DEFINE_OUTPUT(m_OnCase[8], "OnCase09"),
	DEFINE_OUTPUT(m_OnCase[9], "OnCase10"),
	DEFINE_OUTPUT(m_OnCase[10], "OnCase11"),
	DEFINE_OUTPUT(m_OnCase[11], "OnCase12"),
	DEFINE_OUTPUT(m_OnCase[12], "OnCase13"),
	DEFINE_OUTPUT(m_OnCase[13], "OnCase14"),
	DEFINE_OUTPUT(m_OnCase[14], "OnCase15"),
	DEFINE_OUTPUT(m_OnCase[15], "OnCase16"),

	DEFINE_OUTPUT(m_OnDefault, "OnDefault"),

END_DATADESC()




//-----------------------------------------------------------------------------
// Purpose: Called before spawning, after key values have been set.
//-----------------------------------------------------------------------------
void CLogicCase::Spawn( void )
{
	m_nLastShuffleCase = -1;
}


//-----------------------------------------------------------------------------
// Purpose: Evaluates the new input value, firing the appropriate OnCaseX output
//			if the input value matches one of the "CaseX" keys.
// Input  : Value - Variant value to compare against the values of the case fields.
//				We use a variant so that we can convert any input type to a string.
//-----------------------------------------------------------------------------
void CLogicCase::InputValue( inputdata_t &inputdata )
{
	const char *pszValue = inputdata.value.String();
	for (int i = 0; i < MAX_LOGIC_CASES; i++)
	{
		if ((m_nCase[i] != NULL_STRING) && !stricmp(STRING(m_nCase[i]), pszValue))
		{
			m_OnCase[i].FireOutput( inputdata.pActivator, this );
			return;
		}
	}
	
	m_OnDefault.Set( inputdata.value, inputdata.pActivator, this );
}


//-----------------------------------------------------------------------------
// Count the number of valid cases, building a packed array
// that maps 0..NumCases to the actual CaseX values.
//
// This allows our zany mappers to set up cases sparsely if they desire.
// NOTE: assumes pnMap points to an array of MAX_LOGIC_CASES
//-----------------------------------------------------------------------------
int CLogicCase::BuildCaseMap(unsigned char *puchCaseMap)
{
	memset(puchCaseMap, 0, sizeof(unsigned char) * MAX_LOGIC_CASES);

	int nNumCases = 0;
	for (int i = 0; i < MAX_LOGIC_CASES; i++)
	{
		if (m_OnCase[i].NumberOfElements() > 0)
		{
			puchCaseMap[nNumCases] = (unsigned char)i;
			nNumCases++;
		}
	}
	
	return nNumCases;
}


//-----------------------------------------------------------------------------
// Purpose: Makes the case statement choose a case at random.
//-----------------------------------------------------------------------------
void CLogicCase::InputPickRandom( inputdata_t &inputdata )
{
	unsigned char uchCaseMap[MAX_LOGIC_CASES];
	int nNumCases = BuildCaseMap( uchCaseMap );

	//
	// Choose a random case from the ones that were set up by the level designer.
	//
	if ( nNumCases > 0 )
	{
		int nRandom = random->RandomInt(0, nNumCases - 1);
		int nCase = (unsigned char)uchCaseMap[nRandom];

		Assert(nCase < MAX_LOGIC_CASES);

		if (nCase < MAX_LOGIC_CASES)
		{
			m_OnCase[nCase].FireOutput( inputdata.pActivator, this );
		}
	}
	else
	{
		DevMsg( 1, "Firing PickRandom input on logic_case %s with no cases set up\n", GetDebugName() );
	}
}


//-----------------------------------------------------------------------------
// Purpose: Makes the case statement choose a case at random.
//-----------------------------------------------------------------------------
void CLogicCase::InputPickRandomShuffle( inputdata_t &inputdata )
{
	int nAvoidCase = -1;
	int nCaseCount = m_nShuffleCases;
	
	if ( nCaseCount == 0 )
	{
		// Starting a new shuffle batch.
		nCaseCount = m_nShuffleCases = BuildCaseMap( m_uchShuffleCaseMap );
		
		if ( ( m_nShuffleCases > 1 ) && ( m_nLastShuffleCase != -1 ) )
		{
			// Remove the previously picked case from the case map for this pick only.
			// This avoids repeats across shuffle batch boundaries.		
			nAvoidCase = m_nLastShuffleCase;
			
			for (int i = 0; i < m_nShuffleCases; i++ )
			{
				if ( m_uchShuffleCaseMap[i] == nAvoidCase )
				{
					unsigned char uchSwap = m_uchShuffleCaseMap[i];
					m_uchShuffleCaseMap[i] = m_uchShuffleCaseMap[nCaseCount - 1];
					m_uchShuffleCaseMap[nCaseCount - 1] = uchSwap;
					nCaseCount--;
					break;
				}
			}
		}
	}
	
	//
	// Choose a random case from the ones that were set up by the level designer.
	// Never repeat a case within a shuffle batch, nor consecutively across batches.
	//
	if ( nCaseCount > 0 )
	{
		int nRandom = random->RandomInt( 0, nCaseCount - 1 );

		int nCase = m_uchShuffleCaseMap[nRandom];
		Assert(nCase < MAX_LOGIC_CASES);

		if (nCase < MAX_LOGIC_CASES)
		{
			m_OnCase[nCase].FireOutput( inputdata.pActivator, this );
		}
		
		m_uchShuffleCaseMap[nRandom] = m_uchShuffleCaseMap[m_nShuffleCases - 1];
		m_nShuffleCases--;

		m_nLastShuffleCase = nCase;
	}
	else
	{
		DevMsg( 1, "Firing PickRandom input on logic_case %s with no cases set up\n", GetDebugName() );
	}
}


//-----------------------------------------------------------------------------
// Purpose: Compares a floating point input to a predefined value, firing an
//			output to indicate the result of the comparison.
//-----------------------------------------------------------------------------
class CLogicCompare : public CLogicalEntity
{
	DECLARE_CLASS( CLogicCompare, CLogicalEntity );

public:
	int DrawDebugTextOverlays(void);

private:
	// Inputs
	void InputSetValue( inputdata_t &inputdata );
	void InputSetValueCompare( inputdata_t &inputdata );
	void InputSetCompareValue( inputdata_t &inputdata );
	void InputCompare( inputdata_t &inputdata );

	void DoCompare(CBaseEntity *pActivator, float flInValue);

	float m_flInValue;					// Place to hold the last input value for a recomparison.
	float m_flCompareValue;				// The value to compare the input value against.

	// Outputs
	COutputFloat m_OnLessThan;			// Fired when the input value is less than the compare value.
	COutputFloat m_OnEqualTo;			// Fired when the input value is equal to the compare value.
	COutputFloat m_OnNotEqualTo;		// Fired when the input value is not equal to the compare value.
	COutputFloat m_OnGreaterThan;		// Fired when the input value is greater than the compare value.

	DECLARE_DATADESC();
};

LINK_ENTITY_TO_CLASS(logic_compare, CLogicCompare);


BEGIN_DATADESC( CLogicCompare )

	// Keys
	DEFINE_KEYFIELD(m_flCompareValue, FIELD_FLOAT, "CompareValue"),
	DEFINE_KEYFIELD(m_flInValue, FIELD_FLOAT, "InitialValue"),

	// Inputs
	DEFINE_INPUTFUNC(FIELD_FLOAT, "SetValue", InputSetValue),
	DEFINE_INPUTFUNC(FIELD_FLOAT, "SetValueCompare", InputSetValueCompare),
	DEFINE_INPUTFUNC(FIELD_FLOAT, "SetCompareValue", InputSetCompareValue),
	DEFINE_INPUTFUNC(FIELD_VOID, "Compare", InputCompare),

	// Outputs
	DEFINE_OUTPUT(m_OnEqualTo, "OnEqualTo"),
	DEFINE_OUTPUT(m_OnNotEqualTo, "OnNotEqualTo"),
	DEFINE_OUTPUT(m_OnGreaterThan, "OnGreaterThan"),
	DEFINE_OUTPUT(m_OnLessThan, "OnLessThan"),

END_DATADESC()




//-----------------------------------------------------------------------------
// Purpose: Input handler for a new input value without performing a comparison.
//-----------------------------------------------------------------------------
void CLogicCompare::InputSetValue( inputdata_t &inputdata )
{
	m_flInValue = inputdata.value.Float();
}


//-----------------------------------------------------------------------------
// Purpose: Input handler for a setting a new value and doing the comparison.
//-----------------------------------------------------------------------------
void CLogicCompare::InputSetValueCompare( inputdata_t &inputdata )
{
	m_flInValue = inputdata.value.Float();
	DoCompare( inputdata.pActivator, m_flInValue );
}

//-----------------------------------------------------------------------------
// Purpose: Input handler for a new input value without performing a comparison.
//-----------------------------------------------------------------------------
void CLogicCompare::InputSetCompareValue( inputdata_t &inputdata )
{
	m_flCompareValue = inputdata.value.Float();
}

//-----------------------------------------------------------------------------
// Purpose: Input handler for forcing a recompare of the last input value.
//-----------------------------------------------------------------------------
void CLogicCompare::InputCompare( inputdata_t &inputdata )
{
	DoCompare( inputdata.pActivator, m_flInValue );
}


//-----------------------------------------------------------------------------
// Purpose: Compares the input value to the compare value, firing the appropriate
//			output(s) based on the comparison result.
// Input  : flInValue - Value to compare against the comparison value.
//-----------------------------------------------------------------------------
void CLogicCompare::DoCompare(CBaseEntity *pActivator, float flInValue)
{
	if (flInValue == m_flCompareValue)
	{
		m_OnEqualTo.Set(flInValue, pActivator, this);
	}
	else
	{
		m_OnNotEqualTo.Set(flInValue, pActivator, this);

		if (flInValue > m_flCompareValue)
		{
			m_OnGreaterThan.Set(flInValue, pActivator, this);
		}
		else
		{
			m_OnLessThan.Set(flInValue, pActivator, this);
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Draw any debug text overlays
// Output : Current text offset from the top
//-----------------------------------------------------------------------------
int CLogicCompare::DrawDebugTextOverlays( void ) 
{
	int text_offset = BaseClass::DrawDebugTextOverlays();

	if (m_debugOverlays & OVERLAY_TEXT_BIT) 
	{
		char tempstr[512];

		// print duration
		Q_snprintf(tempstr,sizeof(tempstr),"    Initial Value: %f", m_flInValue);
		EntityText(text_offset,tempstr,0);
		text_offset++;

		// print hold time
		Q_snprintf(tempstr,sizeof(tempstr),"    Compare Value: %f", m_flCompareValue);
		EntityText(text_offset,tempstr,0);
		text_offset++;
	}
	return text_offset;
}

//-----------------------------------------------------------------------------
// Purpose: Tests a boolean value, firing an output to indicate whether the
//			value was true or false.
//-----------------------------------------------------------------------------
class CLogicBranch : public CLogicalEntity
{
	DECLARE_CLASS( CLogicBranch, CLogicalEntity );
	
public:

	void UpdateOnRemove();

	void AddLogicBranchListener( CBaseEntity *pEntity );
	inline bool GetLogicBranchState();
	virtual int DrawDebugTextOverlays( void );

private:

	enum LogicBranchFire_t
	{
		LOGIC_BRANCH_FIRE,
		LOGIC_BRANCH_NO_FIRE,
	};

	// Inputs
	void InputSetValue( inputdata_t &inputdata );
	void InputSetValueTest( inputdata_t &inputdata );
	void InputToggle( inputdata_t &inputdata );
	void InputToggleTest( inputdata_t &inputdata );
	void InputTest( inputdata_t &inputdata );

	void UpdateValue(bool bNewValue, CBaseEntity *pActivator, LogicBranchFire_t eFire);

	bool m_bInValue;					// Place to hold the last input value for a future test.
	
	CUtlVector<EHANDLE> m_Listeners;	// A list of logic_branch_listeners that are monitoring us.

	// Outputs
	COutputEvent m_OnTrue;				// Fired when the value is true.
	COutputEvent m_OnFalse;				// Fired when the value is false.

	DECLARE_DATADESC();
};

LINK_ENTITY_TO_CLASS(logic_branch, CLogicBranch);


BEGIN_DATADESC( CLogicBranch )

	// Keys
	DEFINE_KEYFIELD(m_bInValue, FIELD_BOOLEAN, "InitialValue"),

	DEFINE_UTLVECTOR( m_Listeners, FIELD_EHANDLE ),

	// Inputs
	DEFINE_INPUTFUNC(FIELD_BOOLEAN, "SetValue", InputSetValue),
	DEFINE_INPUTFUNC(FIELD_BOOLEAN, "SetValueTest", InputSetValueTest),
	DEFINE_INPUTFUNC(FIELD_VOID, "Toggle", InputToggle),
	DEFINE_INPUTFUNC(FIELD_VOID, "ToggleTest", InputToggleTest),
	DEFINE_INPUTFUNC(FIELD_VOID, "Test", InputTest),

	// Outputs
	DEFINE_OUTPUT(m_OnTrue, "OnTrue"),
	DEFINE_OUTPUT(m_OnFalse, "OnFalse"),

END_DATADESC()


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CLogicBranch::UpdateOnRemove()
{
	for ( int i = 0; i < m_Listeners.Count(); i++ )
	{
		CBaseEntity *pEntity = m_Listeners.Element( i ).Get();
		if ( pEntity )
		{
			g_EventQueue.AddEvent( this, "_OnLogicBranchRemoved", 0, this, this );
		}
	}
	
	BaseClass::UpdateOnRemove();
}
	

//-----------------------------------------------------------------------------
// Purpose: Input handler to set a new input value without firing outputs.
// Input  : Boolean value to set.
//-----------------------------------------------------------------------------
void CLogicBranch::InputSetValue( inputdata_t &inputdata )
{
	UpdateValue( inputdata.value.Bool(), inputdata.pActivator, LOGIC_BRANCH_NO_FIRE );
}


//-----------------------------------------------------------------------------
// Purpose: Input handler to set a new input value and fire appropriate outputs.
// Input  : Boolean value to set.
//-----------------------------------------------------------------------------
void CLogicBranch::InputSetValueTest( inputdata_t &inputdata )
{
	UpdateValue( inputdata.value.Bool(), inputdata.pActivator, LOGIC_BRANCH_FIRE );
}


//-----------------------------------------------------------------------------
// Purpose: Input handler for toggling the boolean value without firing outputs.
//-----------------------------------------------------------------------------
void CLogicBranch::InputToggle( inputdata_t &inputdata )
{
	UpdateValue( !m_bInValue, inputdata.pActivator, LOGIC_BRANCH_NO_FIRE );
}


//-----------------------------------------------------------------------------
// Purpose: Input handler for toggling the boolean value and then firing the
//			appropriate output based on the new value.
//-----------------------------------------------------------------------------
void CLogicBranch::InputToggleTest( inputdata_t &inputdata )
{
	UpdateValue( !m_bInValue, inputdata.pActivator, LOGIC_BRANCH_FIRE );
}


//-----------------------------------------------------------------------------
// Purpose: Input handler for forcing a test of the last input value.
//-----------------------------------------------------------------------------
void CLogicBranch::InputTest( inputdata_t &inputdata )
{
	UpdateValue( m_bInValue, inputdata.pActivator, LOGIC_BRANCH_FIRE );
}


//-----------------------------------------------------------------------------
// Purpose: Tests the last input value, firing the appropriate output based on
//			the test result.
// Input  : bInValue - 
//-----------------------------------------------------------------------------
void CLogicBranch::UpdateValue( bool bNewValue, CBaseEntity *pActivator, LogicBranchFire_t eFire )
{
	if ( m_bInValue != bNewValue )
	{
		m_bInValue = bNewValue;

		for ( int i = 0; i < m_Listeners.Count(); i++ )
		{
			CBaseEntity *pEntity = m_Listeners.Element( i ).Get();
			if ( pEntity )
			{
				g_EventQueue.AddEvent( pEntity, "_OnLogicBranchChanged", 0, this, this );
			}
		}
	}

	if ( eFire == LOGIC_BRANCH_FIRE )
	{
		if ( m_bInValue )
		{
			m_OnTrue.FireOutput( pActivator, this );
		}
		else
		{
			m_OnFalse.FireOutput( pActivator, this );
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: Accessor for logic_branchlist to test the value of the branch on demand.
//-----------------------------------------------------------------------------
bool CLogicBranch::GetLogicBranchState()
{
	return m_bInValue;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CLogicBranch::AddLogicBranchListener( CBaseEntity *pEntity )
{
	if ( m_Listeners.Find( pEntity ) == -1 )
	{
		m_Listeners.AddToTail( pEntity );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CLogicBranch::DrawDebugTextOverlays( void )
{
	int text_offset = BaseClass::DrawDebugTextOverlays();

	if (m_debugOverlays & OVERLAY_TEXT_BIT) 
	{
		char tempstr[512];

		// print refire time
		Q_snprintf( tempstr, sizeof(tempstr), "Branch value: %s", (m_bInValue) ? "TRUE" : "FALSE" );
		EntityText( text_offset, tempstr, 0 );
		text_offset++;
	}

	return text_offset;
}

//-----------------------------------------------------------------------------
// Purpose: Autosaves when triggered
//-----------------------------------------------------------------------------
class CLogicAutosave : public CLogicalEntity
{
	DECLARE_CLASS( CLogicAutosave, CLogicalEntity );

protected:
	// Inputs
	void InputSave( inputdata_t &inputdata );
	void InputSaveDangerous( inputdata_t &inputdata );
	void InputSetMinHitpointsThreshold( inputdata_t &inputdata );

	DECLARE_DATADESC();
	bool m_bForceNewLevelUnit;
	int m_minHitPoints;
	int m_minHitPointsToCommit;
};

LINK_ENTITY_TO_CLASS(logic_autosave, CLogicAutosave);

BEGIN_DATADESC( CLogicAutosave )
	DEFINE_KEYFIELD( m_bForceNewLevelUnit, FIELD_BOOLEAN, "NewLevelUnit" ),
	DEFINE_KEYFIELD( m_minHitPoints, FIELD_INTEGER, "MinimumHitPoints" ),
	DEFINE_KEYFIELD( m_minHitPointsToCommit, FIELD_INTEGER, "MinHitPointsToCommit" ),
	// Inputs
	DEFINE_INPUTFUNC( FIELD_VOID, "Save", InputSave ),
	DEFINE_INPUTFUNC( FIELD_FLOAT, "SaveDangerous", InputSaveDangerous ),
	DEFINE_INPUTFUNC( FIELD_INTEGER, "SetMinHitpointsThreshold", InputSetMinHitpointsThreshold ),
END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: Save!
//-----------------------------------------------------------------------------
void CLogicAutosave::InputSave( inputdata_t &inputdata )
{
	if ( m_bForceNewLevelUnit )
	{
		engine->ClearSaveDir();
	}

	engine->ServerCommand( "autosave\n" );
}

//-----------------------------------------------------------------------------
// Purpose: Save safely!
//-----------------------------------------------------------------------------
void CLogicAutosave::InputSaveDangerous( inputdata_t &inputdata )
{
	CBasePlayer *pPlayer = UTIL_PlayerByIndex( 1 );

	if ( g_ServerGameDLL.m_fAutoSaveDangerousTime != 0.0f && g_ServerGameDLL.m_fAutoSaveDangerousTime >= gpGlobals->curtime )
	{
		// A previous dangerous auto save was waiting to become safe

		if ( pPlayer->GetDeathTime() == 0.0f || pPlayer->GetDeathTime() > gpGlobals->curtime )
		{
			// The player isn't dead, so make the dangerous auto save safe
			engine->ServerCommand( "autosavedangerousissafe\n" );
		}
	}

	if ( m_bForceNewLevelUnit )
	{
		engine->ClearSaveDir();
	}

	if ( pPlayer->GetHealth() >= m_minHitPoints )
	{
		engine->ServerCommand( "autosavedangerous\n" );
		g_ServerGameDLL.m_fAutoSaveDangerousTime = gpGlobals->curtime + inputdata.value.Float();

		// Player must have this much health when we go to commit, or we don't commit.
		g_ServerGameDLL.m_fAutoSaveDangerousMinHealthToCommit = m_minHitPointsToCommit;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Autosaves when triggered
//-----------------------------------------------------------------------------
class CLogicActiveAutosave : public CLogicAutosave
{
	DECLARE_CLASS( CLogicActiveAutosave, CLogicAutosave );

	void InputEnable( inputdata_t &inputdata )
	{
		m_flStartTime = -1;
		SetThink( &CLogicActiveAutosave::SaveThink );
		SetNextThink( gpGlobals->curtime );
	}

	void InputDisable( inputdata_t &inputdata )
	{
		SetThink( NULL );
	}

	void SaveThink()
	{
		CBasePlayer *pPlayer = UTIL_GetLocalPlayer();
		if ( pPlayer )
		{
			if ( m_flStartTime < 0 )
			{
				if ( pPlayer->GetHealth() <= m_minHitPoints )
				{
					m_flStartTime = gpGlobals->curtime;
				}
			}
			else
			{
				if ( pPlayer->GetHealth() >= m_TriggerHitPoints )
				{
					inputdata_t inputdata;
					DevMsg( 2, "logic_active_autosave (%s, %d) triggered\n", STRING( GetEntityName() ), entindex() );
					if ( !m_flDangerousTime )
					{
						InputSave( inputdata );
					}
					else
					{
						inputdata.value.SetFloat( m_flDangerousTime );
						InputSaveDangerous( inputdata );
					}
					m_flStartTime = -1;
				}
				else if ( m_flTimeToTrigger > 0 && gpGlobals->curtime - m_flStartTime > m_flTimeToTrigger )
				{
					m_flStartTime = -1;
				}
			}
		}

		float thinkInterval = ( m_flStartTime < 0 ) ? 1.0 : 0.5;
		SetNextThink( gpGlobals->curtime + thinkInterval );
	}

	DECLARE_DATADESC();

	int m_TriggerHitPoints;
	float m_flTimeToTrigger;
	float m_flStartTime;
	float m_flDangerousTime;
};

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

// Finds the named physics object.  If no name, returns the world
// If a name is specified and an object not found - errors are reported
IPhysicsObject *FindPhysicsObjectByNameOrWorld( string_t name, CBaseEntity *pErrorEntity )
{
	if ( !name )
		return g_PhysWorldObject;

	IPhysicsObject *pPhysics = FindPhysicsObjectByName( name.ToCStr(), pErrorEntity );
	if ( !pPhysics )
	{
		DevWarning("%s: can't find %s\n", pErrorEntity->GetClassname(), name.ToCStr());
	}
	return pPhysics;
}

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

BEGIN_DATADESC( CLogicCollisionPair )
	DEFINE_KEYFIELD( m_nameAttach1, FIELD_STRING, "attach1" ),
	DEFINE_KEYFIELD( m_nameAttach2, FIELD_STRING, "attach2" ),
	DEFINE_KEYFIELD( m_disabled, FIELD_BOOLEAN, "startdisabled" ),
	DEFINE_FIELD( m_succeeded, FIELD_BOOLEAN ),

	// Inputs
	DEFINE_INPUTFUNC( FIELD_VOID, "DisableCollisions", InputDisableCollisions ),
	DEFINE_INPUTFUNC( FIELD_VOID, "EnableCollisions", InputEnableCollisions ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( logic_collision_pair, CLogicCollisionPair );


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define MAX_LOGIC_BRANCH_NAMES 16

class CLogicBranchList : public CLogicalEntity
{
	DECLARE_CLASS( CLogicBranchList, CLogicalEntity );

	virtual void Spawn();
	virtual void Activate();
	virtual int DrawDebugTextOverlays( void );

private:

	enum LogicBranchListenerLastState_t
	{
		LOGIC_BRANCH_LISTENER_NOT_INIT = 0,
		LOGIC_BRANCH_LISTENER_ALL_TRUE,
		LOGIC_BRANCH_LISTENER_ALL_FALSE,
		LOGIC_BRANCH_LISTENER_MIXED,
	};

	void DoTest( CBaseEntity *pActivator );

	string_t m_nLogicBranchNames[MAX_LOGIC_BRANCH_NAMES];
	CUtlVector<EHANDLE> m_LogicBranchList;
	LogicBranchListenerLastState_t m_eLastState;

	// Inputs
	void Input_OnLogicBranchRemoved( inputdata_t &inputdata );
	void Input_OnLogicBranchChanged( inputdata_t &inputdata );
	void InputTest( inputdata_t &inputdata );

	// Outputs
	COutputEvent m_OnAllTrue;			// Fired when all the registered logic_branches become true.
	COutputEvent m_OnAllFalse;			// Fired when all the registered logic_branches become false.
	COutputEvent m_OnMixed;				// Fired when one of the registered logic branches changes, but not all are true or false.

	DECLARE_DATADESC();
};

LINK_ENTITY_TO_CLASS(logic_branch_listener, CLogicBranchList);


BEGIN_DATADESC( CLogicBranchList )

	// Silence, classcheck!
	//DEFINE_ARRAY( m_nLogicBranchNames, FIELD_STRING, MAX_LOGIC_BRANCH_NAMES ),

	// Keys
	DEFINE_KEYFIELD( m_nLogicBranchNames[0], FIELD_STRING, "Branch01" ),
	DEFINE_KEYFIELD( m_nLogicBranchNames[1], FIELD_STRING, "Branch02" ),
	DEFINE_KEYFIELD( m_nLogicBranchNames[2], FIELD_STRING, "Branch03" ),
	DEFINE_KEYFIELD( m_nLogicBranchNames[3], FIELD_STRING, "Branch04" ),
	DEFINE_KEYFIELD( m_nLogicBranchNames[4], FIELD_STRING, "Branch05" ),
	DEFINE_KEYFIELD( m_nLogicBranchNames[5], FIELD_STRING, "Branch06" ),
	DEFINE_KEYFIELD( m_nLogicBranchNames[6], FIELD_STRING, "Branch07" ),
	DEFINE_KEYFIELD( m_nLogicBranchNames[7], FIELD_STRING, "Branch08" ),
	DEFINE_KEYFIELD( m_nLogicBranchNames[8], FIELD_STRING, "Branch09" ),
	DEFINE_KEYFIELD( m_nLogicBranchNames[9], FIELD_STRING, "Branch10" ),
	DEFINE_KEYFIELD( m_nLogicBranchNames[10], FIELD_STRING, "Branch11" ),
	DEFINE_KEYFIELD( m_nLogicBranchNames[11], FIELD_STRING, "Branch12" ),
	DEFINE_KEYFIELD( m_nLogicBranchNames[12], FIELD_STRING, "Branch13" ),
	DEFINE_KEYFIELD( m_nLogicBranchNames[13], FIELD_STRING, "Branch14" ),
	DEFINE_KEYFIELD( m_nLogicBranchNames[14], FIELD_STRING, "Branch15" ),
	DEFINE_KEYFIELD( m_nLogicBranchNames[15], FIELD_STRING, "Branch16" ),
	
	DEFINE_UTLVECTOR( m_LogicBranchList, FIELD_EHANDLE ),
	
	DEFINE_FIELD( m_eLastState, FIELD_INTEGER ),

	// Inputs
	DEFINE_INPUTFUNC( FIELD_INPUT, "Test", InputTest ),
	DEFINE_INPUTFUNC( FIELD_INPUT, "_OnLogicBranchChanged", Input_OnLogicBranchChanged ),
	DEFINE_INPUTFUNC( FIELD_INPUT, "_OnLogicBranchRemoved", Input_OnLogicBranchRemoved ),

	// Outputs
	DEFINE_OUTPUT( m_OnAllTrue, "OnAllTrue" ),
	DEFINE_OUTPUT( m_OnAllFalse, "OnAllFalse" ),
	DEFINE_OUTPUT( m_OnMixed, "OnMixed" ),

END_DATADESC()


//-----------------------------------------------------------------------------
// Purpose: Called before spawning, after key values have been set.
//-----------------------------------------------------------------------------
void CLogicBranchList::Spawn( void )
{
}


//-----------------------------------------------------------------------------
// Finds all the logic_branches that we are monitoring and register ourselves with them.
//-----------------------------------------------------------------------------
void CLogicBranchList::Activate( void )
{
	for ( int i = 0; i < MAX_LOGIC_BRANCH_NAMES; i++ )
	{
		CBaseEntity *pEntity = NULL;
		while ( ( pEntity = gEntList.FindEntityGeneric( pEntity, STRING( m_nLogicBranchNames[i] ), this ) ) != NULL )
		{
			if ( FClassnameIs( pEntity, "logic_branch" ) )
			{
				CLogicBranch *pBranch = (CLogicBranch *)pEntity;
				pBranch->AddLogicBranchListener( this );
				m_LogicBranchList.AddToTail( pBranch );
			}
			else
			{
				DevWarning( "logic_branchlist %s refers to entity %s, which is not a logic_branch\n", GetDebugName(), pEntity->GetDebugName() );
			}
		}
	}
	
	BaseClass::Activate();
}


//-----------------------------------------------------------------------------
// Called when a monitored logic branch is deleted from the world, since that
// might affect our final result.
//-----------------------------------------------------------------------------
void CLogicBranchList::Input_OnLogicBranchRemoved( inputdata_t &inputdata )
{
	int nIndex = m_LogicBranchList.Find( inputdata.pActivator );
	if ( nIndex != -1 )
	{
		m_LogicBranchList.FastRemove( nIndex );
	}

	// See if this logic_branch's deletion affects the final result.
	DoTest( inputdata.pActivator );
}


//-----------------------------------------------------------------------------
// Called when the value of a monitored logic branch changes.
//-----------------------------------------------------------------------------
void CLogicBranchList::Input_OnLogicBranchChanged( inputdata_t &inputdata )
{
	DoTest( inputdata.pActivator );
}


//-----------------------------------------------------------------------------
// Input handler to manually test the monitored logic branches and fire the
// appropriate output.
//-----------------------------------------------------------------------------
void CLogicBranchList::InputTest( inputdata_t &inputdata )
{
	// Force an output.
	m_eLastState = LOGIC_BRANCH_LISTENER_NOT_INIT;

	DoTest( inputdata.pActivator );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CLogicBranchList::DoTest( CBaseEntity *pActivator )
{
	bool bOneTrue = false;
	bool bOneFalse = false;
	
	for ( int i = 0; i < m_LogicBranchList.Count(); i++ )
	{
		CLogicBranch *pBranch = (CLogicBranch *)m_LogicBranchList.Element( i ).Get();
		if ( pBranch && pBranch->GetLogicBranchState() )
		{
			bOneTrue = true;
		}
		else
		{
			bOneFalse = true;
		}
	}

	// Only fire the output if the new result differs from the last result.
	if ( bOneTrue && !bOneFalse )
	{
		if ( m_eLastState != LOGIC_BRANCH_LISTENER_ALL_TRUE )
		{
			m_OnAllTrue.FireOutput( pActivator, this );
			m_eLastState = LOGIC_BRANCH_LISTENER_ALL_TRUE;
		}
	}
	else if ( bOneFalse && !bOneTrue )
	{
		if ( m_eLastState != LOGIC_BRANCH_LISTENER_ALL_FALSE )
		{
			m_OnAllFalse.FireOutput( pActivator, this );
			m_eLastState = LOGIC_BRANCH_LISTENER_ALL_FALSE;
		}
	}
	else
	{
		if ( m_eLastState != LOGIC_BRANCH_LISTENER_MIXED )
		{
			m_OnMixed.FireOutput( pActivator, this );
			m_eLastState = LOGIC_BRANCH_LISTENER_MIXED;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CLogicBranchList::DrawDebugTextOverlays( void )
{
	int text_offset = BaseClass::DrawDebugTextOverlays();

	if (m_debugOverlays & OVERLAY_TEXT_BIT) 
	{
		char tempstr[512];

		for ( int i = 0; i < m_LogicBranchList.Count(); i++ )
		{
			CLogicBranch *pBranch = (CLogicBranch *)m_LogicBranchList.Element( i ).Get();
			if ( pBranch )
			{
				Q_snprintf( tempstr, sizeof(tempstr), "Branch (%s): %s", STRING(pBranch->GetEntityName()), (pBranch->GetLogicBranchState()) ? "TRUE" : "FALSE" );
				EntityText( text_offset, tempstr, 0 );
				text_offset++;
			}
		}
	}

	return text_offset;
}
