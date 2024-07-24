//-----------------------------------------------------------------------------
//
// @doc
//
// @module	SkyClouds.cpp - Sky body |
//
// This module contains the support for the sky cloud.
//
// Copyright (c) 1999 - Descartes Systems Sciences, Inc.
//
// @end
//
// $History: DcComEngGenConfigurePage.h $
//
//-----------------------------------------------------------------------------

#include "nemesis/SkyClouds.h"

//-----------------------------------------------------------------------------
//
// @mfunc <c SkyClouds> constructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

SkyClouds::SkyClouds () 
{
	//
	// Initialize
	//
}

//-----------------------------------------------------------------------------
//
// @mfunc <c SkyClouds> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

SkyClouds::~SkyClouds () 
{
	//
	// Rundown
	//
}

//-----------------------------------------------------------------------------
//
// @mfunc Create the sky body
//
// @parm _Type | nType | Type of sky cloud
//
// @parm const char * | pszTexture | Name of the texture
//
// @parm const vector2 & | vRate | Shift rate of the texture (per day)
//
// @parm float | fScale | Scale of the texture
//
// @parm const vector3 & | vDayColor | Color of the clouds during the day
//
// @parm const vector3 & | vNightColor | Color of the clouds during the night
//
// @parm float | fTransitionExp | Transition exponent.  Controls how quickly
//		the transition is between day to night or night to day.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void SkyClouds::Create (_Type nType, const char *pszTexture, 
		const vector2 &vRate, float fScale, const vector3 &vDayColor, 
		const vector3 &vNightColor, float fTransitionExp) 
{

	//
	// Initialize the variables
	//

	m_nType = nType;
//	m_strTexture = pszTexture;
	m_vRate = vRate;
	m_fScale = fScale;
	m_vBaseDayColor = vDayColor;
	m_vBaseNightColor = vNightColor;
	m_fTransitionExp = fTransitionExp;
}

//-----------------------------------------------------------------------------
//
// @mfunc Compute the position of the sky body
//
// @parm float | fTimeInDays | The current time in days
//
// @parm float | fDayFactor | Factor of the day.  0 = night, 1 = day
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void SkyClouds::ComputePosition (float fTimeInDays, float fDayFactor)
{

	//
	// Compute the shift
	//

	m_vOffset = m_vRate * fTimeInDays;

	//
	// Adjust the factor
	//

	if (m_fTransitionExp != 1.0f)
	{
		fDayFactor = (float)pow(fDayFactor, m_fTransitionExp);
	}

	//
	// Set the day color
	//

	vector3 vDelta = m_vBaseDayColor - m_vBaseNightColor;
	m_vCurrentColor = m_vBaseNightColor + vDelta * fDayFactor;
}

//-----------------------------------------------------------------------------
//
// @mfunc Prepare a sky cloud
//
//		Prepare a sky cloud.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void SkyClouds::Prep ()
{

	//
	// Load the texture
	//

//	const char *pszTexture = GetTextureName ();
//	SetTextureIndex (g_sTextureManager.FindNormalN(pszTexture));
}

