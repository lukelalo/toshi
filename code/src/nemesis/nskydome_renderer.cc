#define N_IMPLEMENTS nSkyDome

#include "nemesis/nskydome.h"
#include "nemesis/SkyBody.h"
#include "nemesis/SkyClouds.h"
#include "mathlib/matrix.h"
#include "gfx/ngfxserver.h"
#include "gfx/nscenegraph2.h"
#include "gfx/ndynvertexbuffer.h"
#include "gfx/npixelshader.h"
#include "kernel/ntimeserver.h"
//
// Helper macros
//
inline void nSkyDome::DoSkyCoord (const nSkyDome::_Vertex &sVertex)
{
	// - Need to put this vertex into the nDynVbuffer now

	this->coord[0]=sVertex.vVertex.x; this->coord[1]=sVertex.vVertex.y; 
	this->coord[2]=sVertex.vVertex.z; this->coord+=this->stride;
	this->color[0]=g_lLastColor; this->color += this->stride;

	cur_vertex++;
}

inline void nSkyDome::DoSkyUV(const nSkyDome::_Vertex &sVertex, const TextureMatrix &tm)
{
	// Finding U/V coordinates based on texture matrix (?)
	//	vector2 vTC;
	//	vTC.x = sVertex.vVertex.dot(tm.vsTexture) + tm.fsTexShift;
	//	vTC.y = sVertex.vVertex.dot(tm.vtTexture) + tm.ftTexShift;

	// Reading this as vTC.v = float v[2] = u/v coords.
	// sVertex.vMapVertex.v  = float v[3] = x/y/z/ coordinates
	//
    this->uv[0] = (sVertex.vVertex % tm.vsTexture) + tm.fsTexShift;
    this->uv[1] = (sVertex.vVertex % tm.vtTexture) + tm.ftTexShift;
    this->uv += this->stride;
	
	uvs++;
}

inline void nSkyDome::SetColor (const vector3 &vColor)
{
//	if (!(vColor.equals(g_vLastColor)))
//	{
//		this->cur_vb->Color( cur_vertex, n_f2rgba(vColor.x, vColor.y, vColor.z, 1 ) );
//		glColor4fv (vColor.v);
		g_lLastColor = n_f2rgba( vColor.x, vColor.y, vColor.z, 1);
//	}
}

void nSkyDome::InitVBuffer()
{
//  assert precondition(s)
    n_assert(!this->ref_ifan.isvalid());
    n_assert(!this->ref_istrip.isvalid());

    int iNumberOfVertices = m_nVertex;

    // UV0 = dome, UV1 = clouds, UV2 = bodies
	this->dyn_vb.Initialize((N_VT_COORD|N_VT_RGBA|N_VT_UV0|N_VT_UV1|N_VT_UV2),0);

//    nIndexBuffer *ibuf_fan   = this->ref_gs->FindIndexBuffer("skydome_ibuf_fan");
    nIndexBuffer *ibuf_strip = this->ref_gs->FindIndexBuffer("skydome_ibuf_strip");

	/*    if (!ibuf_fan)
    {
        ibuf_fan = this->ref_gs->NewIndexBuffer("skydome_ibuf_fan");

        int iNumberOfIndices = this->dyn_vb.GetNumVertices();

        ibuf_fan->Begin(N_PTYPE_TRIANGLE_FAN, iNumberOfIndices);

        for (int i = 0; i < iNumberOfIndices; i++)
        {
            ibuf_fan->Index(i);
        }

        ibuf_fan->End();
    }
	*/

    if (!ibuf_strip)
    {
        ibuf_strip = this->ref_gs->NewIndexBuffer("skydome_ibuf_strip");

        int iNumberOfIndices = this->dyn_vb.GetNumVertices();

        ibuf_strip->Begin(N_IBTYPE_WRITEONLY, N_PTYPE_TRIANGLE_STRIP, iNumberOfIndices);

        for (int i = 0; i < iNumberOfIndices; i++)
        {
            ibuf_strip->Index(i, i);
        }

        ibuf_strip->End();
    }


//  assert postcondition(s)
//  n_assert(ibuf_fan);
    n_assert(ibuf_strip);
//    this->ref_ifan = ibuf_fan;
    this->ref_istrip = ibuf_strip;
}

bool nSkyDome::Attach(nSceneGraph2 *sg)
{
    if (isCreated && nVisNode::Attach(sg))    // 1.
    {
        if (!dyn_vb.IsValid())
            InitVBuffer();

        sg->AttachVisualNode(this);
        return true;
    }
    else
        return false;
}

//==============================================================================
void nSkyDome::Compute(nSceneGraph2 *sg)
{
    if (isCreated)
	{
		nVisNode::Compute(sg);

		nGfxServer *gs = ref_gs.get();
		cur_vertex = 0;
		uvs = 0;
		cur_sg = sg;
		cur_vb = NULL;

		ComputePositions(pKernelServer->ts->GetFrameTime());
//		ComputePositions(0);

		BeginStrip();
		
		DrawSky();


		EndBuffer();
//        this->dyn_vb.End(cur_vertex, cur_vertex); //ref_ibuf.get()->GetNumIndices());
//		cur_vb = this->dyn_vb.Swap(cur_vertex - 1, cur_vertex - 1);

		cur_sg = NULL;
    }
}

/*nVertexBuffer *SkyDome::BeginFan()
{
	if (!cur_vb)
	{
		cur_vb = this->dyn_vb.Begin(ref_ifan.get(),cur_sgn->pixelshader,cur_sgn->texturearray);
		this->coord = cur_vb->coord_ptr;
		this->color = cur_vb->color_ptr;
		this->stride= cur_vb->stride4;
		int i;
		for (i=0; i<N_MAXNUM_TEXCOORDS; i++) {
			this->uv[i] = cur_vb->uv_ptr[i];
		}
	} else {
		if (cur_vertex >= cur_vb->GetNumVertices() - 1)
		{
			cur_vb = this->dyn_vb.Swap(cur_vertex, cur_vertex);
			cur_vertex = 0;

			this->coord = cur_vb->coord_ptr;
			this->color = cur_vb->color_ptr;
			this->stride= cur_vb->stride4;
			int i;
			for (i=0; i<N_MAXNUM_TEXCOORDS; i++) {
				this->uv[i] = cur_vb->uv_ptr[i];
			}
		}
	}
	
	return( cur_vb );
}
*/

nVertexBuffer *nSkyDome::BeginStrip()
{
	if (!cur_vb)
	{
	    cur_vb = this->dyn_vb.Begin(ref_istrip.get(),cur_sg->GetPixelShader(),cur_sg->GetTextureArray());
		this->coord = cur_vb->coord_ptr;
		this->color = cur_vb->color_ptr;
		this->stride= cur_vb->stride4;
		this->uv    = cur_vb->uv_ptr[0];
	} else {
		if (cur_vertex >= cur_vb->GetNumVertices() - 1)
		{
			cur_vb = this->dyn_vb.Swap(cur_vertex, cur_vertex);
			cur_vertex = 0;

			this->coord = cur_vb->coord_ptr;
			this->color = cur_vb->color_ptr;
			this->stride= cur_vb->stride4;
			this->uv    = cur_vb->uv_ptr[0];
		}
	}
	return( cur_vb );
}

void nSkyDome::EndBuffer()
{
    this->dyn_vb.End(cur_vertex, cur_vertex); //ref_ibuf.get()->GetNumIndices());
//    this->dyn_vb.End(cur_vertex - 1, cur_vertex - 1); //ref_ibuf.get()->GetNumIndices());
	cur_vertex = 0;
	cur_vb = NULL;
	//	cur_index = 0;
}

//-----------------------------------------------------------------------------
//
// @mfunc Draw a cloud layer
//
//		Draw a cloud layer
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void nSkyDome::DrawSkyClouds (SkyClouds *pSkyClouds, 
									const vector3 &vColor)
{
	int i;

	//
	// Selected the proper texture
	//
	this->uv = cur_vb->uv_ptr[pSkyClouds->GetTextureIndex()];

	// Eh????
	//	BindTextureIndex (pSkyClouds ->GetTextureIndex());

	//
	// Create the texture mapping object
	//

	vector2 vOffset(pSkyClouds->GetOffset());
	float fScale = 1.0f / (m_fRadius * 2.0f * pSkyClouds->GetScale());
	TextureMatrix tm;
	tm.fsTexShift = .5f + vOffset.x;
	tm.ftTexShift = .5f + vOffset.y;
	tm.vsTexture = vector3 (fScale, 0, 0);
	tm.vtTexture = vector3 (0, 0, fScale);

	//
	// Get a pointer to the vertex list to make things easier
	//

	nSkyDome::_Vertex *pVertex = m_pVertex;
	int nResolution = m_nResolution;

	//
	// Loop through top row of triangles
	//
	SetColor (vColor);

	for (i = 1; i < nResolution * 4 + 1; i++)
	{
		DoSkyUV(pVertex [0], tm);
		DoSkyUV(pVertex [i], tm);
	}

	// Close the loop
	DoSkyUV(pVertex [1], tm);

	//
	// Loop through squares
	//
	int nDelta = nResolution * 4;
	for (int j = 1; j < nResolution; j++)
	{
		int nStart = nDelta * j + 1;
		int nEnd = nStart + nDelta;
		for (i = nStart; i < nEnd; i++)
		{
			DoSkyUV (pVertex [i - nDelta], tm);
			DoSkyUV (pVertex [i], tm);
		}
		DoSkyUV (pVertex [nStart - nDelta], tm);
 		DoSkyUV (pVertex [nStart], tm);
	}
}

//-----------------------------------------------------------------------------
//
// @mfunc Draw a sky body
//
//		Draw a sky body to the screen
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void nSkyDome::DrawSkyBody (SkyBody *pSkyBody)
{

	/*
	//
	// Distance factor
	//

	static const float fDistanceFactor = 100.0f;

	//
	// Compute the position of the body
	//

	vector3 vPos;
	matrix44 vm;
	ref_gs->GetMatrix( N_MXM_MODELVIEW, vm );

	vPos = vm * pSkyBody->GetNormal();
	vPos *= fDistanceFactor;
	float fScale = fDistanceFactor * 16 * 
		pSkyBody->GetRadius() / pSkyBody->GetDistance();
	vector3 vXDel(fScale,0,0);
	vector3 vYDel(0,0,fScale);

	//
	// Draw the body as a billboard (Maybe seperate into different nodes? Eww)
	//

	vector3 v;

	cur_vb->Uv(cur_vertex, 2, vector2(0, 0));
	cur_vb->Uv(cur_vertex+1, 2, vector2(0, 0));
	v = vPos - vXDel - vYDel;
	cur_vb->Coord(cur_vertex++, v);
	cur_vb->Coord(cur_vertex++, v);
	
	cur_vb->Uv(cur_vertex, 2, vector2(0, 1));
	v = vPos - vXDel + vYDel;
	cur_vb->Coord(cur_vertex++, v);

	cur_vb->Uv(cur_vertex, 2, vector2(1, 1));
	v = vPos + vXDel + vYDel;
	cur_vb->Coord(cur_vertex++, v);

	cur_vb->Uv(cur_vertex, 2, vector2(1, 1));
	v = vPos + vXDel + vYDel;
	cur_vb->Coord(cur_vertex++, v);
*/
}

// Dump the vertices into the nDynVertexBuffer
void nSkyDome::DrawDome ( const vector3 &vColor )
{
	//
	// Loop through top row of triangles
	//
	SetColor (vColor);
	nSkyDome::_Vertex *pVertex = m_pVertex;

	for (int i = 1; i < m_nResolution * 4 + 1; i++)
	{
		DoSkyCoord (pVertex [0] );
		DoSkyCoord (pVertex [i] );
	}

	// Close the loop
	DoSkyCoord ( pVertex [1] );

	//
	// Loop through squares
	//

	int nDelta = m_nResolution * 4;
	for (int j = 1; j < m_nResolution; j++)
	{
		int nStart = nDelta * j + 1;
		int nEnd = nStart + nDelta;
		for (i = nStart; i < nEnd; i++)
		{
			DoSkyCoord (pVertex [i - nDelta] );
			DoSkyCoord (pVertex [i] );
		}
		DoSkyCoord (pVertex [nStart - nDelta] );
 		DoSkyCoord (pVertex [nStart] );
	}
}

//-----------------------------------------------------------------------------
//
// @mfunc Draw the sky
//
//		Draw the sky to the screen
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void nSkyDome::DrawSky ()
{
	//
	// Initialize opengl.  Turn on blending and disable the z buffer
	//

//	glEnable (GL_BLEND);
//	glDepthMask (GL_FALSE);

	//
	// Map the sky dome triangles
	//
	matrix44 vm;
	ref_gs->GetMatrix( N_MXM_MODELVIEW, vm );


//	this->MapVertices(vm);
	vector3 vClearColor(m_vCurrentSkyColor);

	//
	// Loop through the clouds in the sky dome and count the
	// number of clouds in the system
	//

	int nClouds = 0;
	for (CloudList::iterator i = m_sCloudsList.begin(); 
		 i != m_sCloudsList.end(); i++ )
	{
		SkyClouds *pClouds = (*i);
		if (pClouds->GetType() == SkyClouds::Type_Clouds)
			nClouds++;
	}

	//
	// Compute the basic cloud color
	//

	vector3 vBlendColor;
	vBlendColor.x = 1.0f - m_vCurrentSkyColor.x;
	vBlendColor.y = 1.0f - m_vCurrentSkyColor.y;
	vBlendColor.z = 1.0f - m_vCurrentSkyColor.z;
	vector3 vCloudColor (vBlendColor * (1.0f / (float) nClouds));

	// Draw the dome on which we map
	DrawDome( vCloudColor );

	//
	// Loop through the clouds in the sky dome and draw the stars
	//
	for (CloudList::iterator j = m_sCloudsList.begin(); 
		 j != m_sCloudsList.end(); j++ )
	{
		SkyClouds *pClouds = (*j);
		if (pClouds->GetType() != SkyClouds::Type_Stars) 
			continue;
		DrawSkyClouds (pClouds, pClouds->GetCurrentColor());
	}

	//
	// Loop through the bodies in the sky dome
	//
    // UNIMPLEMENTED, VERY BAD STUFF HERE..

	for (BodyList::iterator k = m_sBodyList.begin(); 
		 k != m_sBodyList.end(); k++ )
	{

		SkyBody *pBody = (*k);
		switch (pBody->GetType())
		{
			case SkyBody::Type_Sun:
//				????????
//				BindTextureIndex (pBody ->GetTextureIndex ());
//				glBlendFunc (GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
				SetColor (pBody->GetCurrentColor());
				DrawSkyBody (pBody);
				break;
			case SkyBody::Type_Moon:
//				BindTextureIndex (pBody ->GetMaskTextureIndex ());
//				glBlendFunc (GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
				SetColor (this->m_vCurrentSkyColor);
				DrawSkyBody (pBody);
//				BindTextureIndex (pBody ->GetTextureIndex ());
//				glBlendFunc (GL_ONE, GL_ONE);
				SetColor (pBody->GetCurrentColor());
				DrawSkyBody (pBody);
				break;
			case SkyBody::Type_Flare:
//				BindTextureIndex (pBody ->GetTextureIndex ());
//				glBlendFunc (GL_ONE, GL_ONE);
				SetColor (pBody->GetCurrentColor());
				DrawSkyBody (pBody);
				break;
			default:
				n_assert(FALSE);
				break;
		}
	}

    // This is tricky ;)
    ref_gs->SetClearColor( vClearColor.x, vClearColor.y, vClearColor.z, 1 );

	//
	// Loop through the clouds in the sky dome rendering the clouds
	//

	for (CloudList::iterator l = m_sCloudsList.begin(); l != m_sCloudsList.end(); l++ )
	{
		SkyClouds *pClouds = (*l);
		if (pClouds ->GetType () != SkyClouds::Type_Clouds) 
			continue;
		vector3 vColor(pClouds->GetCurrentColor());
	    vector3 current( pClouds->GetCurrentColor() );
		vColor.set( vColor.x * vCloudColor.x, vColor.y * vCloudColor.y, vColor.z * vCloudColor.z );
		DrawSkyClouds (pClouds, vColor);
	}
}

