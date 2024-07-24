#ifndef SKYDOME_H
#define SKYDOME_H


//-----------------------------------------------------------------------------
// 
// @doc
//
// @module	SkyDome.h - Sky done support |
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

//-----------------------------------------------------------------------------
//
// Required include files
//
//-----------------------------------------------------------------------------

#include "mathlib/vector.h"
#include "mathlib/matrix.h"
#include "nemesis/SkyBody.h"
#include "nemesis/SkyClouds.h"

#ifndef N_VERTEXBUFFER_H
#include "gfx/nvertexbuffer.h"
#endif

#ifndef N_DYNVERTEXBUFFER_H
#include "gfx/ndynvertexbuffer.h"
#endif

#ifndef N_INDEXBUFFER_H
#include "gfx/nindexbuffer.h"
#endif

#ifndef N_VISNODE_H
#include "node/nvisnode.h"
#endif

//--------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nSkyDome
#include "kernel/ndefdllclass.h"

//--------------------------------------------------------------------

#include <vector>

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

class matrix44;
class nGfxServer;
class SkyBody;
class SkyClouds;


//class CFrustum;

// Used to shift the UV's
struct TextureMatrix
{
	float fsTexShift;
	float ftTexShift;
	vector3 vsTexture;
	vector3 vtTexture;
};

typedef std::vector<SkyBody*> BodyList;
typedef std::vector<SkyClouds*> CloudList;
//-----------------------------------------------------------------------------
//
// @class SkyDome | The class provides support for the sky dome
//
//-----------------------------------------------------------------------------

class N_PUBLIC nSkyDome : public nVisNode 
{
// @access Types and enumerations
public:
	struct _Vertex
	{
		vector3 vVertex;
		vector3 vMapVertex;
	};

// @access Constructors and Destructors
public:
	static nClass* pClass;
	static nKernelServer* pKernelServer;

	nSkyDome ();
	~nSkyDome ();
	
    virtual bool Attach(nSceneGraph2 *);
    virtual void Compute(nSceneGraph2 *);

	// @access Public Methods
public:

	// @cmember Create the skydome

	void Create (int nResolution, float fVertSweep, float fRadius, 
		float fHeightScale, const vector3 &vOrigin, const vector3 &vDayAmbient,
		const vector3 &vNightAmbient, const vector3 &vDayColor,
		const vector3 &vNightColor);

	// @cmember Map the vertices

	void MapVertices (const matrix44 &sModelToEye);

	// @cmember Compute positions of the sky objects
	void ComputePositions( double frameTime );


	// NONSTANDARD::   0 = UP, 
	//				  70 = twilight, 100 = dark
	//				 260 = dawn,     290 = Daylight
	void SetSunAngle(float sunAngle)
	{
		mySunAngle = sunAngle;
	}

	// Set the texture speed -- default is 1000
	// this number is divided into the frame time
	// to achieve a good speed for cloud textures
	void SetCloudFactor(float cloudFactor)
	{
		myCloudFactor = cloudFactor;
	}

	// @cmember Prepare a sky dome for rendering

	void Prep ();
	
	inline void SetColor (const vector3 &vColor);

// @access Public inline methods
public:

	// @cmember Insert a new body into the list

	void AddBody (SkyBody *pBody)
	{
		m_sBodyList.push_back( pBody );
//		pBody ->InsertTail (&m_sBodyList);
	}

	// @cmember Insert a new clounds into the list

	void AddClouds (SkyClouds *pClouds)
	{
		m_sCloudsList.push_back( pClouds );
//		pClouds ->InsertTail (&m_sCloudsList);
	}

// @access Protected methods
protected:
	// Nemesis additions
	bool isCreated;
	
	// Our vertex buffer and indices
	nDynVertexBuffer	dyn_vb;
    nRef<nIndexBuffer>  ref_ifan;
    nRef<nIndexBuffer>  ref_istrip;
    nVertexBuffer *cur_vb;      // the current vertex buffer

    int stride;                 // the vertex buffers stride
    float *coord;               // the current vertex buffers current coord pointer
    ulong *color;               // the current vertex buffers current norm pointer
    float *uv;                  // the current vertex buffers current uv pointer
	int current_ptype;

	nVertexBuffer *BeginFan();
	nVertexBuffer *BeginStrip();
	void EndBuffer();

	nSceneGraph2 *cur_sg;

	nAutoRef<nGfxServer> ref_gs;
//	nRef<nPixelShader> myShader;
//	nPixelShaderDesc myShaderDesc;	

	void InitVBuffer(); // Setup indices and vbuffer
	// Routines to draw the actual sky
	void DrawSky ();
	void DrawSkyClouds (SkyClouds *pSkyClouds, const vector3 &vColor);
	void DrawSkyBody (SkyBody *pSkyBody);
	void DrawDome(  const vector3 &vColor );

	void DoSkyCoord (const _Vertex &sVertex );
	void DoSkyUV (const _Vertex &sVertex, const TextureMatrix &tm);
	int cur_vertex;
	int uvs;

	// Our reference materials
	SkyBody *m_sSkySun;
	SkyBody *m_sSkySunFlare;
	SkyBody *m_sSkyMoon;
	SkyClouds *m_sSkyClouds1;
	SkyClouds *m_sSkyClouds2;
	SkyClouds *m_sSkyStars;
	nSkyDome  *g_sSkyDome; // us

	// @cmember Initialize the skydome
	
	void Init();

	// @cmember Destroy the skydome

	void Destroy ();

// @access Public variables
public:

	// @cmember Number of vertices

	int				m_nVertex;

	// @cmember List of vertices

	_Vertex			*m_pVertex;

	// @cmember Resolution of the skydome

	int				m_nResolution;

	// @cmember Vertical sweep of the skydome

	float			m_fVertSweep;

	// @cmember Radius of the skydome

	float			m_fRadius;

	// @cmember Height scale of the skydome

	float			m_fHeightScale;

	// @cmember Origin of the skydome

	vector3		m_vOrigin;

	// @cmember Link list to all of the sky bodies

	BodyList		m_sBodyList;

	// @cmember Link list to all of the sky clouds

	CloudList		m_sCloudsList;

	// @cmember Base sun light ambient

	vector3		m_vBaseDayAmbient;

	// @cmember Base sun light ambient

	vector3		m_vBaseNightAmbient;

	// @cmember Base day sky color

	vector3		m_vBaseDayColor;

	// @cmember Base night sky color

	vector3		m_vBaseNightColor;

	// @cmember Current position of the sun

	vector3		m_vCurrentSunNormal;

	// @cmember Current sun light ambient

	vector3		m_vCurrentAmbient;

	// @cmember Current sky color
	
	vector3		m_vCurrentSkyColor;

	// How fast the sky moves
	// TODO: Unmarry this from the starfield
	float		myCloudFactor;

	// The angle of the sun, for day/night transitions
	float		mySunAngle;

protected:
	// Last redered color
	long		 g_lLastColor;
};

#endif // __SkyDome__
