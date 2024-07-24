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

#include "nemesis/SkyDome.h"
#include "mathlib/matrix.h"
#include "gfx/ngfxserver.h"
#include "gfx/nscenegraph2.h"
#include "kernel/ntimeserver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//-----------------------------------------------------------------------------
//
// @mfunc <c SkyDome> constructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

SkyDome::SkyDome () :
	ref_gs( ks, this );
{
	//
	// Invoke the helper routine
	//
	//
	// Initialize the sun
	//
	m_sSkySun = new SkyBody();
	m_sSkySun.Create (
		SkyBody::Type_Sun,						// type
		"env/sun",								// texture
		0.0f,									// longitude of the ascending node
		23.4393f,								// Incliation to the ecliptic
		282.9404f,								// Argument of perihelion
		149600000.0f,							// Mean distance
		0.016709f,								// Eccentricity
		356.0470f,								// Mean anomaly
		0.9856002585f,							// Mean anomaly adjust
		695000.0f,								// Radius of body
		vector3 (.7f, .7f, .5f),				// Daytime color
		vector3 (.7f, .5f, .5f)				// Nighttime color
		);

	//
	// Initialize the sun flare
	//
	m_sSkySunFlare = new SkyBody();
	m_sSkySunFlare .Create (
		SkyBody::Type_Flare,					// type
		"effects/flare1",						// texture
		0.0f,									// longitude of the ascending node
		23.4393f,								// Incliation to the ecliptic
		282.9404f,								// Argument of perihelion
		149600000.0f,							// Mean distance
		0.016709f,								// Eccentricity
		356.0470f,								// Mean anomaly
		0.9856002585f,							// Mean anomaly adjust
		8 * 695000.0f,							// Radius of body
		vector3 (.4f, .4f, .0f),				// Daytime color
		vector3 (.4f, .2f, .0f)				// Nighttime color
		);

	//
	// Initialize the moon
	//
	m_sSkyMoon = new SkyBody();
	m_sSkyMoon.Create (
		SkyBody::Type_Moon, 					// type
		"env/moon", 							// texture
		125.1228f, 								// longitude of the ascending node
		5.1454f, 								// Incliation to the ecliptic
		318.0634f, 								// Argument of perihelion
		384000.0f,								// Mean distance
		0.054900f,								// Eccentricity
		115.3654f, 								// Mean anomaly
		13.0649929509f, 						// Mean anomaly adjust
		1738.0f,								// Radius of body
		vector3 (.1f, .1f, .1f), 				// Daytime color
		vector3 (.4f, .4f, .4f)				// Nighttime color
		);

	//
	// Initialize the clouds
	//
	m_sSkyClouds1 = new SkyClouds();
	m_sSkyClouds1.Create (
		SkyClouds::Type_Clouds,				// type
		"env/clouds",							// texture
		vector2 (0.0, 1.0),					// texture speed
		1.0f,									// texture scale
		vector3 (0.5f, 0.5f, 0.5f),			// daytime color
		vector3 (0.11f, 0.10f, 0.09f),			// nighttime color
		1.0f									// transition gamma
		);

	m_sSkyClouds2 = new SkyClouds(); 
	m_sSkyClouds2.Create (
		SkyClouds::Type_Clouds, 				// type
		"env/clouds", 							// texture
		vector2 (1.0, 5.0), 					// texture speed
		0.25f, 									// texture scale
		vector3 (0.3f, 0.3f, 0.3f),			// daytime color
		vector3 (0.11f, 0.10f, 0.09f),			// nighttime color
		1.0f									// transition gamma
		);

	m_sSkyStars = new SkyClouds();
	m_sSkyStars.Create (
		SkyClouds::Type_Stars, 				// type
		"env/stars", 							// texture
		vector2 (7.0f, 0.0), 					// texture speed
		1.0f / 24.0f,							// texture scale 
		vector3 (0.0f, 0.0f, 0.0f),			// daytime color
		vector3 (0.2f, 0.2f, 0.2f),			// nighttime color
		0.3f									// transition gamma
		);

	//
	// Initialize the sky dome
	//

	g_sSkyDome = this;
	g_sSkyDome.Create (
		8,										// resolution
		55,										// sweep in degrees
		1024 * 12,								// size at horizon
		1.0f,									// height scale factor
		vector3 (0, 0, -512 * 10),				// offset
		vector3 (1.0f, 1.0f, 1.0f),			// day ambient
		vector3 (0.4f, 0.4f, 0.4f),			// night ambient
		vector3 (0.25f, 0.31f, .63f),			// day sky color
		vector3 (0.1f, 0.1f, 0.11f)			// night sky color 
		);
	
	//
	// Add the bodies and clouds to the sky dome
	//

	g_sSkyDome.AddBody (&m_sSkyMoon);
	g_sSkyDome.AddBody (&m_sSkySunFlare);
	g_sSkyDome.AddBody (&m_sSkySun);
	g_sSkyDome.AddClouds (&m_sSkyStars);
	g_sSkyDome.AddClouds (&m_sSkyClouds1);
	g_sSkyDome.AddClouds (&m_sSkyClouds2);

	//
	// Compute starting values for the sky
	// (Must be done at start of every frame)
	//

	g_sSkyDome.ComputePositions (0);

	Initialize ();
}

//-----------------------------------------------------------------------------
//
// @mfunc <c SkyDome> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

SkyDome::~SkyDome () 
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

void SkyDome::Initialize () 
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

	isCreated = false;
}

//-----------------------------------------------------------------------------
//
// @mfunc Destroy the sky dome
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void SkyDome::Destroy () 
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

void SkyDome::Create (int nResolution, float fVertSweep, float fRadius, 
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
	fRadius /= cos (fRadAngle);

	//
	// Compute the z adjustment
	//

	float fZAdj = fRadius * sin (fRadAngle);

	//
	// Start the vertex list with the very top of the dome
	//

	m_pVertex [0].vVertex = vector3 (0, 0, 
		(fRadius - fZAdj) * fHeightScale) + vOrigin;

	//
	// From the resolution, compute the angular sweep of one section
	// of the dome
	//

	float fHorzSweep = 90.0 / nResolution;

	//
	// Adjust the vertical resolution
	//

	fVertSweep /= nResolution;
	
	//
	// Starting from the top, populate with nResolution number of rings
	//

	int nVertex = 1;
	for (int i = 0; i < nResolution; i++)
	{

		//
		// Compute the vertex that will be rotated around to make a ring
		//

		vector3 vPoint (0, 0, fRadius);
		matrix44 m;
		m.rotate_x(fVertSweep * (i + 1));
//		m .MakeHPR (0, fVertSweep * (i + 1), 0);
		vPoint = m * vPoint; //?
//		m.PreMultiply (vPoint, vPoint);
		n_assert(vPoint.z >= fZAdj - ON_EPSILON);
		vPoint.z = (vPoint.z - fZAdj) * fHeightScale;

		//
		// Loop through the ring creating the points
		//

		for (int j = 0; j < nResolution * 4; j++)
		{

			//
			// Map the point
			//

			m.rotate_z(fHorzSweep * j);
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

SkyDome::InitVBuffer()
{
//  assert precondition(s)
    n_assert(!this->ref_ibuf.isvalid());

    int iNumberOfVertices = iNumberOfFlares * 4;    // 4 vert's per quad (flare)

    // UV0 = dome, UV1 = clouds, UV2 = bodies
	this->ref_dynvbuf.Initialize((N_VT_COORD|N_VT_RGBA|N_VT_UV0|N_VT_UV1|N_VT_UV2), m_nVertex);


    nIndexBuffer *ibuf = this->ref_gs->FindIndexBuffer("skydome_ibuf");
    if (!ibuf)
    {
        ibuf = this->ref_gs->NewIndexBuffer("skydome_ibuf");

        int iNumberOfIndices = m_nVertex;

        ibuf->Begin(N_PTYPE_TRIANGLE_LIST, iNumberOfIndices);

        for (int i = 0; i < m_nVertex; i++)
        {
            ibuf->Index(i);
        }

        ibuf->End();
    }

//  assert postcondition(s)
    n_assert(ibuf);
    this->ref_ibuf = ibuf;
}

bool SkyDome::Attach(nSceneGraph *sg, nSGNode *sgn)
{
    if (isCreated && nVisNode::Attach(sg, sgn))    // 1.
    {
        if (!dyn_vb.IsValid())
            InitVBuffer();

        sgn->compnode = this;
        return true;
    }
    else
        return false;
}

//==============================================================================
//------------------------------------------------------------------------------
//  KEY:
//------------------------------------------------------------------------------
bool SkyDome::Compute(nSceneGraph *sg, nSGNode *sgn)
{
    if (isCreated && nVisNode::Compute(sg, sgn))   // 1.
    {
		nGfxServer *gs = ref_gs.get();
		this->cur_vertex = 0;

        cur_vb = this->dyn_vb.Begin(ref_ibuf.get(),sgn->pixelshader,sgn->texturearray);

		ComputePositions( ks->ts->GetFrameTime() );
		
		DrawSky();

        this->dyn_vb.End(m_nVertex, ref_ibuf.get()->GetNumIndices());

        }
        return true;
    }
    else
        return false;
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

void SkyDome::MapVertices (const matrix44 &sModelToEye)
{


	//
	// Loop through the vertices
	//

	for (int i = 0; i < m_nVertex; i++)
	{

		//
		// Map
		//

//		sModelToEye .PreNormalMultiply (
		
		// ??
		// m_pVertex[i].vVertex;
		m_pVertex[i].vMapVertex = m_pVertex[i].vVertex * sModelToEye;
		m_pVertex[i].vMapVertex.normalize();

//		sModelToEye .PreMultiply (
//			m_pVertex [i] .vVertex,
//			m_pVertex [i] .vMapVertex);
//		m_pVertex [i] .vMapVertex .m_y += sModelToEye .m_m [3] [1];
	}
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

void SkyDome::ComputePositions (float fTimeInDays)
{

	SkyBody *pSun = NULL;

	//
	// Loop through the sky bodies computing positions
	//

	SkyBody *pStart = m_sBodyList.begin();
	for (m_sBodyList::iterator *i = m_sBodyList.begin(); 
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

	//
	// If a sun was found, then compute the sun light
	//
	//
	// TODO -- LenseFlare

	float fDayFactor;
	if (pSun)
	{
		//
		// Get the normal
		//

		vector3 vNormal(pSun->GetNormal());
		m_vCurrentSunNormal = vNormal;

		//
		// Compute the angle of the sun
		//

		float fSunAngle = asin(vNormal.z) * 180.0f / PI;
		bool fSunrise = vNormal .m_x >= 0.0;

		//
		// Compute the day factor
		//

		if (fTimeInDays != -1) 
		{
			if (fSunAngle > 20.0) 
			{
				fDayFactor = 1.0;
				m_vCurrentSkyColor = m_vBaseDayColor;
			} 
			else if (fSunAngle >= -10.0)
			{
				fDayFactor = (fSunAngle + 10.0f) / 30.0f;
			} 
			else 
			{
				fDayFactor = 0.0;
				m_vCurrentSkyColor = m_vBaseNightColor;
			}
		}
		else
		{
			fDayFactor = 1.0;
			m_vCurrentSkyColor = m_vBaseDayColor;
		}

		//
		// Compute the sky color
		//

		vector3 vDeltaSky(m_vBaseDayColor - m_vBaseNightColor);
		m_vCurrentSkyColor = m_vBaseNightColor + vDeltaSky * fDayFactor;

		//
		// Compute the ambient
		//

		vector3 vDeltaAmbient(m_vBaseDayAmbient - m_vBaseNightAmbient);
		m_vCurrentAmbient = m_vBaseNightAmbient + vDeltaAmbient * fDayFactor;
	}

	//
	// If not, just used the default values
	//

	else
	{
		m_vCurrentSunNormal = vector3 (1,0,1);
		fDayFactor = 1.0;
		m_vCurrentAmbient = m_vBaseDayAmbient;
		m_vCurrentSkyColor = m_vBaseDayColor;
	}

	//
	// Force the w value for the colors to 1
	//

	m_vCurrentAmbient.w = 1.0;
	m_vCurrentSkyColor.w = 1.0;

	//
	// Compute the positions of the clouds
	//
	for (m_sCloudsList::iterator *i = m_sCloudsList.begin(); 
		 i != m_sCloudsList.end(); i++ )
	{
		//
		// Get a pointer to the clouds
		//

		SkyClouds *pClouds = (*i);

		//
		// Compute the position.
		//

		pClouds->ComputePosition(fTimeInDays, fDayFactor);
	}

	//
	// Compute the colors of the bodies
	//

	for (m_sBodyList::iterator *i = m_sBodyList.begin(); 
		 i != m_sBodyList.end(); i++ )
	{
		//
		// Get a pointer to the body
		//

		SkyBody *pBody = (*i);

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

void SkyDome::Prep ()
{

	//
	// Loop through the bodies in the sky dome
	//

	for (m_sBodyList::iterator *i = m_sBodyList.begin(); 
		 i != m_sBodyList.end(); i++ )
	{
		SkyBody *pBody = (*i);
		pBody->Prep ();
	}

	//
	// Loop through the clounds in the sky dome
	//

	for (m_sCloudsList::iterator *i = m_sCloudsList.begin(); 
		 i != m_sCloudsList.end(); i++ )
	{
		SkyClouds *pClouds = (*i);
		pClouds->Prep ();
	}
}

