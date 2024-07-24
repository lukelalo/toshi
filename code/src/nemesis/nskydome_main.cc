#define N_IMPLEMENTS nSkyDome
// TODO: Optimize

//-----------------------------------------------------------------------------
//
// @doc
//
// @module	SkyDome.cpp - Sky dome |
//
// This module contains the support for the sky dome.
//
// Copyright (c) 1999 - Descartes Systems Sciences, Inc.
//
// @end
//
// $History: DcComEngGenConfigurePage.h $
//
//-----------------------------------------------------------------------------

#include "nemesis/nskydome.h"
#include "mathlib/matrix.h"
#include "mathlib/quaternion.h"
#include "gfx/ngfxserver.h"
#include "gfx/nscenegraph2.h"
#include "kernel/ntimeserver.h"
#include <vector>

//-----------------------------------------------------------------------------
//
// @mfunc <c nSkyDome> constructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

nSkyDome::nSkyDome () :
	ref_gs( pKernelServer, this ),
	dyn_vb( pKernelServer,this)//,
//	myShader(this)
{
	//
	// Invoke the helper routine
	//
	//
	Init();

	// Initialize the sun
	//
	m_sSkySun = new SkyBody();
	m_sSkySun->Create (
		SkyBody::Type_Sun,						// type
		"env/sun",								// texture
		0.0f,									// longitude of the ascending node
		23.4393f,								// Incliation to the ecliptic
		282.9404f,								// Argument of perihelion
		149600000.0f,							// Mean distance
		0.016709f,								// Eccentricity
		356.0470f,								// Mean anomaly
		0.9856002585f,							// Mean anomaly adjust
		6.950000f,								// Radius of body
		vector3 (.7f, .7f, .5f),				// Daytime color
		vector3 (.7f, .5f, .5f)					// Nighttime color
		);

	//
	// Initialize the sun flare
	//
	m_sSkySunFlare = new SkyBody();
	m_sSkySunFlare->Create (
		SkyBody::Type_Flare,					// type
		"effects/flare1",						// texture
		0.0f,									// longitude of the ascending node
		23.4393f,								// Incliation to the ecliptic
		282.9404f,								// Argument of perihelion
		149600000.0f,							// Mean distance
		0.016709f,								// Eccentricity
		356.0470f,								// Mean anomaly
		0.9856002585f,							// Mean anomaly adjust
		8 * 6.95000f,							// Radius of body
		vector3 (.4f, .4f, .0f),				// Daytime color
		vector3 (.4f, .2f, .0f)				// Nighttime color
		);

	//
	// Initialize the moon
	//
	m_sSkyMoon = new SkyBody();
	m_sSkyMoon->Create (
		SkyBody::Type_Moon, 					// type
		"env/moon", 							// texture
		125.1228f, 								// longitude of the ascending node
		5.1454f, 								// Incliation to the ecliptic
		318.0634f, 								// Argument of perihelion
		384000.0f,								// Mean distance
		0.054900f,								// Eccentricity
		115.3654f, 								// Mean anomaly
		13.0649929509f, 						// Mean anomaly adjust
		1.7380f,								// Radius of body
		vector3 (.1f, .1f, .1f), 				// Daytime color
		vector3 (.4f, .4f, .4f)				// Nighttime color
		);

	//
	// Initialize the clouds
	//
	m_sSkyClouds1 = new SkyClouds();
	m_sSkyClouds1->Create (
		SkyClouds::Type_Clouds,				// type
		"env/clouds",							// texture
		vector2 (5.0, 1.0),					// texture speed
		1.0f / 2,									// texture scale
		vector3 (0.5f, 0.5f, 0.5f),			// daytime color
		vector3 (0.11f, 0.10f, 0.09f),			// nighttime color
		.30f									// transition gamma
		);

	m_sSkyClouds2 = new SkyClouds(); 
	m_sSkyClouds2->Create (
		SkyClouds::Type_Clouds, 				// type
		"env/clouds", 							// texture
		vector2 (1.0, 5.0), 					// texture speed
		1.0f / 12, 									// texture scale
		vector3 (0.3f, 0.3f, 0.3f),			// daytime color
		vector3 (0.11f, 0.10f, 0.09f),			// nighttime color
		1.0f									// transition gamma
		);

	m_sSkyStars = new SkyClouds();
	m_sSkyStars->Create (
		SkyClouds::Type_Stars, 				// type
		"env/stars", 							// texture
		vector2 (5.0f, 8.3f), 					// texture speed
		1.0f / 4,							// texture scale 
		vector3 (0.0f, 0.0f, 0.0f),			// daytime color
		vector3 (0.2f, 0.2f, 0.3f),			// nighttime color
		0.01f									// transition gamma
		);

	//
	// Initialize the sky dome
	//

	g_sSkyDome = this;
	Create (
		9,										// resolution
		30,										// sweep in degrees
		2400, // * 12,								// size at horizon
		1.0f,									// height scale factor
		vector3 (0, -100, 0 /*-512 * 10*/),				// offset
		vector3 (1.0f, 1.0f, 1.0f),			// day ambient
		vector3 (0.4f, 0.4f, 0.4f),			// night ambient
		vector3 (0.65f, 0.71f, 1.0f),			// day sky color
		vector3 (0.2f, 0.15f, 0.17f)			// night sky color 
		);
	
	//
	// Add the bodies and clouds to the sky dome
	//

//	AddBody (m_sSkyMoon);
//	AddBody (m_sSkySunFlare);
	AddBody (m_sSkySun);
	m_sSkyStars->SetTextureIndex( 0 );
	m_sSkyClouds1->SetTextureIndex( 1 );
//	m_sSkyClouds2->SetTextureIndex( 2 );

	AddClouds (m_sSkyStars);
	AddClouds (m_sSkyClouds1);
//	AddClouds (m_sSkyClouds2);

	//
	// Compute starting values for the sky
	// (Must be done at start of every frame)
	//

	SetSunAngle(0.0f);
	myCloudFactor = 1000.0f;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c nSkyDome> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

nSkyDome::~nSkyDome () 
{

	//
	// Invoke the helper routine
	//

	Destroy ();
}

//-----------------------------------------------------------------------------
//
// @mfunc Initialize the sky dome
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void nSkyDome::Init() 
{
	//
	// Initialize the variables
	//

	m_nVertex = 0;
	m_pVertex = NULL;
	m_nResolution = 0;

	//
	// Initialize the sky light information
	//

	m_vBaseDayAmbient    = vector3 (1, 1, 1);
	m_vBaseNightAmbient  = vector3 (0, 0, 0);
	m_vBaseDayColor      = vector3 (0, 0, 1);
	m_vBaseNightColor    = vector3 (0, 0, 0);

	ref_gs = "/sys/servers/gfx";
	cur_sg = NULL;
	cur_vb = NULL;
	current_ptype = 0;
	uvs = 0;

	isCreated = false;
}

//-----------------------------------------------------------------------------
//
// @mfunc Destroy the sky dome
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void nSkyDome::Destroy () 
{

	//
	// Remove all bodies and clouds
	//

	m_sBodyList.clear();
	m_sCloudsList.clear();

	//
	// Deallocate the vertex array
	//

	if (m_pVertex)
		delete [] m_pVertex;

	//
	// Initialize
	//

	Initialize ();
}

//-----------------------------------------------------------------------------
//
// @mfunc Create a new sky dome
//
// @parm int | nResolution | Defines the number of subdivisions in a 90
//		degree slice of the sky dome.  This applies to the horizontal and
//		vertical aspects of the dome.
//
// @parm float | fVertSweep | Vertical sweep.  Must be greater than zero 
//		and less or equal to 90.  This defines the amount of the hemisphere
//		that will be included in the dome.
//
// @parm float | fRadius | Radius of the sky dome at the base
//
// @parm float | fHeightScale | Height scale of the sky dome.  Used to either
//		increase or decrease the height of the dome after generation.
//
// @parm const vector3 & | vOrigin | Origin of the center of the sky dome.
//		Usually, the sky dome is place slightly below the landscape.
//
// @parm const vector3 & | vDayAmbient | Base ambient color for the sun light.
//
// @parm const vector3 & | vNightAmbient | Base ambient color for the night light.
//
// @parm const vector3 & | vDayColor | Base color for the day sky
//
// @parm const vector3 & | vNightColor | Base color for the night sky
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void nSkyDome::Create (int nResolution, float fVertSweep, float fRadius, 
		float fHeightScale, const vector3 &vOrigin, const vector3 &vDayAmbient,
		const vector3 &vNightAmbient, const vector3 &vDayColor,
		const vector3 &vNightColor)
{
	//
	// Validate the arguments
	//

	n_assert(nResolution > 0 && fRadius > 0);
	n_assert(fVertSweep > 0 && fVertSweep <= 90);

	//
	// Destroy the old dome
	//

	Destroy ();

	//
	// Save the generation arguments
	//

	m_nResolution = nResolution;
	m_fVertSweep = fVertSweep;
	m_fRadius = fRadius;
	m_fHeightScale = fHeightScale;
	m_vOrigin = vOrigin;
	m_vBaseDayAmbient = vDayAmbient;
	m_vBaseNightAmbient = vNightAmbient;
	m_vBaseDayColor = vDayColor;
	m_vBaseNightColor = vNightColor;


	//
	// Compute the number of vertices
	//

	m_nVertex = 1 + 4 * nResolution * nResolution;

	//
	// Allocate the buffer to contain vertices
	//

	m_pVertex = new _Vertex [m_nVertex];


	//
	// Adjust the radius based on the vertical sweep
	//

	float fRadAngle = (90 - fVertSweep) / 180 * PI;
	fRadius /= (float)cos (fRadAngle);

	//
	// Compute the z adjustment
	//

	float fZAdj = fRadius * (float)sin (fRadAngle);

	//
	// Start the vertex list with the very top of the dome
	//

	m_pVertex[0].vVertex = vector3(0, (fRadius - fZAdj) * fHeightScale, 0) + vOrigin;

	//
	// From the resolution, compute the angular sweep of one section
	// of the dome
	//

	float fHorzSweep = 90.0f / nResolution;

	//
	// Adjust the vertical resolution
	//

	fVertSweep /= nResolution;
	
	//
	// Starting from the top, populate with nResolution number of rings
	//

	int nVertex = 1;
	matrix44 m;
	for (int i = 0; i < nResolution; i++)
	{

		//
		// Compute the vertex that will be rotated around to make a ring
		//

		vector3 vPoint (0, fRadius, 0);
		m.rotate_x(n_deg2rad(fVertSweep) );
//		m .MakeHPR (0, fVertSweep * (i + 1), 0);
		vPoint = m * vPoint; //?
//		m.PreMultiply (vPoint, vPoint);
//		n_assert(vPoint.z >= fZAdj - ON_EPSILON);
		vPoint.y = (vPoint.y - fZAdj) * fHeightScale;

		//
		// Loop through the ring creating the points
		//

		for (int j = 0; j < nResolution * 4; j++)
		{
			//
			// Map the point
			//

			m.rotate_y(n_deg2rad(fHorzSweep));
			m_pVertex [nVertex] .vVertex = m * vPoint;
			m_pVertex [nVertex] .vVertex += vOrigin;
			nVertex++;
		}
	}

	//
	// All done
	//
	isCreated = true;

	return;
}


//-----------------------------------------------------------------------------
//
// @mfunc Map the vertices
//
// @parm const matrix44 & | sModelToEye | Matrix used to map the sky dome
//		vertices to the eye space.  The translation part of the matrix is
//		not used.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void nSkyDome::MapVertices (const matrix44 &sModelToEye)
{


	//
	// Loop through the vertices
	//
/*
	for (int i = 0; i < m_nVertex; i++)
	{

		//
		// Map
		//

//		sModelToEye .PreNormalMultiply (
		
		// ??
		// m_pVertex[i].vVertex;
		m_pVertex[i].vMapVertex = m_pVertex[i].vVertex; //sModelToEye * m_pVertex[i].vVertex ;
		m_pVertex[i].vMapVertex.normalize();

//		sModelToEye .PreMultiply (
//			m_pVertex [i] .vVertex,
//			m_pVertex [i] .vMapVertex);
//		m_pVertex [i] .vMapVertex .m_y += sModelToEye .m_m [3] [1];
	}
	*/
}

//-----------------------------------------------------------------------------
//
// @mfunc Compute the position of the sky objects
//
// @parm float | fTimeInDays | The current time in days
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void nSkyDome::ComputePositions(double frameTime)
{
 
	SkyBody *pSun = NULL;
	float fSunAngle = mySunAngle;

	// Get the ratio right
	float fTimeInDays 	= (float)frameTime / myCloudFactor;
	
	//
	// Loop through the sky bodies computing positions
	//
/*
	for (BodyList::iterator i = m_sBodyList.begin(); 
		 i != m_sBodyList.end(); i++ )
	{

		//
		// Get a pointer to the body
		//

		SkyBody *pBody = (*i);

		//
		// Compute the position.  If the time is -1, just compute
		// a constant position in the sky
		//

		if (fTimeInDays == -1)
		{
			vector3 vNormal(1, -1, 1);
			vNormal.normalize();
			pBody->SetPosition(vNormal * pBody->GetA());
		}
		else
		{
			pBody->ComputePosition (fTimeInDays);
		}

		//
		// If this is a sun, save the pointer
		//

		if (pBody->GetType() == SkyBody::Type_Sun)
			pSun = pBody;
	}
*/
	//
	// If a sun was found, then compute the sun light
	//
	//
	// TODO -- LenseFlare

	float fDayFactor;
//	if (pSun)
//	{
		//
		// Get the normal
		//

//		vector3 vNormal(pSun->GetNormal());
//		m_vCurrentSunNormal = vNormal;

		//
		// Compute the angle of the sun
		//

//		float fSunAngle = (float)asin(vNormal.z) * 180.0f / PI;
//		bool fSunrise = (270.0f > fSunAngle && fSunAngle > 90.0f);

		//
		// Compute the day factor
		//
		// Refactoring for 0 being straight UP

		if (fTimeInDays != -1) 
		{
			if (fSunAngle > 290.0f || fSunAngle < 70.0f ) 
			{
				fDayFactor = 1.0f;
				m_vCurrentSkyColor = m_vBaseDayColor;
			}
			else if (fSunAngle >= 260.0f || fSunAngle < 100.0f)
			{
				if (fSunAngle >= 260.0f)
				{
					fSunAngle -= 260.0f;
				} else {
					//need to factor 30->0
					fSunAngle = 30.0f - ( fSunAngle - 70.0f );
				}
				fDayFactor = fSunAngle / 30.0f;
			} 
			else 
			{
				fDayFactor = 0.0f;
				m_vCurrentSkyColor = m_vBaseNightColor;
			}
		}
		else
		{
			fDayFactor = 1.0f;
			m_vCurrentSkyColor = m_vBaseDayColor;
		}

		//
		// Compute the sky color
		//

		vector3 vDeltaSky(m_vBaseDayColor - m_vBaseNightColor);
		m_vCurrentSkyColor = (m_vBaseNightColor + vDeltaSky) * fDayFactor;

		//
		// Compute the ambient
		//

		vector3 vDeltaAmbient(m_vBaseDayAmbient - m_vBaseNightAmbient);
		m_vCurrentAmbient = (m_vBaseNightAmbient + vDeltaAmbient) * fDayFactor;
//	}

	//
	// If not, just used the default values
	//

//	else
//	{
//		m_vCurrentSunNormal = vector3 (1,0,1);
//		fDayFactor = 1.0;
//		m_vCurrentAmbient = m_vBaseDayAmbient;
//		m_vCurrentSkyColor = m_vBaseDayColor;
//	}

	//
	// Force the w value for the colors to 1
	//

//	m_vCurrentAmbient.w = 1.0;
//	m_vCurrentSkyColor.w = 1.0;

	//
	// Compute the positions of the clouds
	//

		// Get the ratio right
	frameTime /= myCloudFactor;
	for (CloudList::iterator j = m_sCloudsList.begin(); 
		 j != m_sCloudsList.end(); j++ )
	{
		//
		// Get a pointer to the clouds
		//

		SkyClouds *pClouds = (*j);

		//
		// Compute the position.
		//

		pClouds->ComputePosition(fTimeInDays, fDayFactor);
	}

	//
	// Compute the colors of the bodies
	//

	for (BodyList::iterator k = m_sBodyList.begin(); 
		 k != m_sBodyList.end(); k++ )
	{
		//
		// Get a pointer to the body
		//

		SkyBody *pBody = (*k);

		//
		// Compute the color.
		//

		pBody->ComputeColor(fDayFactor);
	}
}

//-----------------------------------------------------------------------------
//
// @mfunc Prepare a sky dome
//
//		Prepare a sky dome.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void nSkyDome::Prep ()
{

	//
	// Loop through the bodies in the sky dome
	//

	for (BodyList::iterator i = m_sBodyList.begin(); 
		 i != m_sBodyList.end(); i++ )
	{
		SkyBody *pBody = (*i);
		pBody->Prep ();
	}

	//
	// Loop through the clounds in the sky dome
	//

	for (CloudList::iterator j = m_sCloudsList.begin(); 
		 j != m_sCloudsList.end(); j++ )
	{
		SkyClouds *pClouds = (*j);
		pClouds->Prep ();
	}
}

