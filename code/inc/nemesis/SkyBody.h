#ifndef __SkyBody__
#define __SkyBody__

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module	SkyBody.h - Sky body support |
//
// This module contains the support for the sky body.
//
// Copyright (c) 1999 - Descartes Systems Sciences, Inc.
//
// @end
//
// $History: DcComEngGenConfigurePage.h $
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Required include files
//
//-----------------------------------------------------------------------------

#include "mathlib/vector.h"
#include "mathlib/matrix.h"

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// @class SkyBody | The class provides support for the sky body
//
//-----------------------------------------------------------------------------

class SkyBody
{
// @access Types and enumerations
public:
	enum _Type {
		Type_Sun	= 0,
		Type_Moon	= 1,
		Type_Flare	= 2,
	};

// @access Constructors and Destructors
public:

	// @cmember Constructor.

	SkyBody ();

	// @cmember Destructor

	~SkyBody ();

// @access Public Methods
public:

	// @cmember Create the skybody

	void Create (_Type nType, const char *pszTexture, float fN, 
		float fi, float fw, float fa, float fe, float fM, float fdM,
		float fRadius, const vector3 &vDayColor, const vector3 &vNightColor);

	// @cmember Compute the position of the body
	
	void ComputePosition (float fTimeInDays);

	// @cmember Compute the color of the body
	
	void ComputeColor (float fDayFactor);

	// @cmember Prepare a sky dome for rendering

	void Prep ();

// @access Public Inline Methods
public:

	// @cmember Given a link, return the address of the sky body
	
//	static SkyBody *FromLink (CDoubleLink *pLink)
//	{
//		return (SkyBody *) ((BYTE *) pLink - 
//			offsetof (SkyBody, m_sLink));
//	}

	// @cmember Get the type of the body
	
	_Type GetType () const
	{
		return m_nType;
	}

	// @cmember Attach the body to the end of the given list

//	void InsertTail (CDoubleLink *pList)
//	{
//		m_sLink .InsertTail (pList);
//	}

	// @cmember Get the normal of the object

	const vector3 &GetNormal () const
	{
		return m_vNormal;
	}

	// @cmember Get the distance of the object

	float GetDistance () const 
	{
		return m_fDistance;
	}

	// @cmember Set the position of the body
	
	void SetPosition (const vector3 vPosition)
	{
		m_vNormal = vPosition;
		m_vNormal.norm();
		m_fDistance = m_vNormal.len();
	}

	// @cmember Get the name of the texture

//	const char *GetTextureName () const
//	{
//		return m_strTexture;
//	}

	// @cmember Get the texture index

	int GetTextureIndex () const
	{
		return m_nTexture;
	}

	// @cmember Set the texture index

	void SetTextureIndex (int nTextureIndex)
	{
		m_nTexture = nTextureIndex;
	}

	// @cmember Get the mask texture index

	int GetMaskTextureIndex () const
	{
		return m_nMaskTexture;
	}

	// @cmember Set the texture index

	void SetMaskTextureIndex (int nTextureIndex)
	{
		m_nMaskTexture = nTextureIndex;
	}

	// @cmember Get the mean distance

	float GetA () const
	{
		return m_fa;
	}

	// @cmember Get the radius of the object

	float GetRadius () const
	{
		return m_fRadius;
	}

	// @cmember Get the current color

	const vector3 &GetCurrentColor () const
	{
		return m_vCurrentColor;
	}

// @access Protected variables
protected:

	// @cmember Type of the body

	_Type			m_nType;

	// @cmember Longitude of the ascending node

	float			m_fN;

	// @cmember Incliation to the ecliptic

	float			m_fi;

	// @cmember Argument of perihelion

	float			m_fw;

	// @cmember Mean distance

	float			m_fa;

	// @cmember Eccentricity (0=circle, 0-1=ellipse, 1=parabole)

	float			m_fe;

	// @cmember Mean anomaly (0 at perihelion)

	float			m_fM;

	// @cmember Mean anomaly adjust factor for days

	float			m_fdM;

	// @cmember Radius of the body

	float			m_fRadius;

	// @cmember Normal vector of the object

	vector3		m_vNormal;

	// @cmember Distance of the object

	float			m_fDistance;

	// @cmember Base day sky color

	vector3		m_vBaseDayColor;

	// @cmember Base night sky color

	vector3		m_vBaseNightColor;

	// @cmember Current color
	
	vector3		m_vCurrentColor;

	// @cmember List of all objects

//	CDoubleLink		m_sLink;

	// @cmember Name of the body texture

//	CDcTString		m_strTexture;

	// @cmember Index of the texture

	int				m_nTexture;

	// @cmember Index of the mask texture

	int				m_nMaskTexture;
};

#endif // __SkyBody__
