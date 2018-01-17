//========= Copyright Valve Corporation, All rights reserved. ============//

#include "cbase.h"

#include "entities/logic/CLogicCase.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

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
