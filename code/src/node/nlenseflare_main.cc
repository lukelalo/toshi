#define N_IMPLEMENTS nLenseFlare

//==============================================================================
//  node/nlenseflare_main.cc
//  author: Jeremy Bishop
//  (C) 2000 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "gfx/ngfxserver.h"
#include "gfx/nscenegraph2.h"
#include "node/nlenseflare.h"

//==============================================================================
//  2001.01.09  jeremy    + port to ndynvertexbuffer
//------------------------------------------------------------------------------
nLenseFlare::nLenseFlare() :
    ref_gs(ks,this),
    ref_dynvbuf(ks,this),
    ref_fx(ks,this),
    aFlares(0),
    iNumberOfFlares(0),
    bBegun(false),
    bEnded(false),
    bWithinBounds(false),
    fBaseRed(1.0f),
    fBaseGreen(1.0f),
    fBaseBlue(1.0f),
    fBaseAlpha(1.0f),
    vBlindColor(1.0f, 1.0f, 0.8f, 1.0f),
    vb_dest(0)
{
    ref_gs = "/sys/servers/gfx";
    ref_fx = "/sys/servers/specialfx";
}
//==============================================================================
//  yyyy.mm.dd  init    change remarks
//------------------------------------------------------------------------------
nLenseFlare::~nLenseFlare()
{
   delete[] aFlares;
}
//==============================================================================
//  yyyy.mm.dd  init    change remarks
//------------------------------------------------------------------------------
void
nLenseFlare::BeginFlares(int iNumFlares)
{
    n_assert(iNumFlares <= MAXFLARES);

    iNumberOfFlares = iNumFlares;

    nFlareDesc tmp;
    aFlares = new nFlareDesc[iNumberOfFlares];
    for (int i = 0; i < iNumberOfFlares; i++)
        aFlares[i] = tmp;

    bBegun = true;
}
//==============================================================================
//  yyyy.mm.dd  init    change remarks
//------------------------------------------------------------------------------
void
nLenseFlare::EndFlares()
{
    n_assert(bBegun == true);

    bEnded = true;
}
//==============================================================================
//  yyyy.mm.dd  init    change remarks
//------------------------------------------------------------------------------
void
nLenseFlare::SetFlareSizeAt(int ndx, float size)
{
    n_assert(bBegun == true);
    n_assert(aFlares != 0 && ndx < iNumberOfFlares);

    aFlares[ndx].fSize = size;
    aFlares[ndx].fHalfSize = size / 2;  // used for more efficient placement of
                                        // flares
}
//==============================================================================
//  yyyy.mm.dd  init    change remarks
//  2001.03.30  floh    removed the asserts for colors to be between 0 and 1,
//                      this can not be guaranteed if mixed from several
//                      weathers, and should also not be critical since
//                      OpenGL and D3D can handle vertex colors outside 0 and 1
//------------------------------------------------------------------------------
void
nLenseFlare::SetBaseColor(float red, float grn, float blu, float alpha)
{
    n_assert(bBegun == true);
    fBaseRed   = red;
    fBaseGreen = grn;
    fBaseBlue  = blu;
    fBaseAlpha = alpha;
}
//==============================================================================
//  2000.06.06  jeremy      created
//  2001.03.30  floh    removed the asserts for colors to be between 0 and 1,
//                      this can not be guaranteed if mixed from several
//                      weathers, and should also not be critical since
//                      OpenGL and D3D can handle vertex colors outside 0 and 1
//------------------------------------------------------------------------------
void
nLenseFlare::SetBlindColor(const vector4& vBlindClr)
{
    n_assert(bBegun == true);

    vBlindColor = vBlindClr;
}
//==============================================================================
//  yyyy.mm.dd  init    change remarks
//------------------------------------------------------------------------------
void
nLenseFlare::SetFlarePosAt(int ndx, float pos)
{
    n_assert(bBegun == true);
    n_assert(aFlares != 0 && ndx < iNumberOfFlares);

    aFlares[ndx].fPosition = pos;
}
//==============================================================================
//  yyyy.mm.dd  init    change remarks
//  2001.03.30  floh    removed the asserts for colors to be between 0 and 1,
//                      this can not be guaranteed if mixed from several
//                      weathers, and should also not be critical since
//                      OpenGL and D3D can handle vertex colors outside 0 and 1
//------------------------------------------------------------------------------
void
nLenseFlare::SetFlareColorAt(int ndx,
                             float red,
                             float grn,
                             float blu,
                             float alpha)
{
    n_assert(bBegun == true);
    n_assert(aFlares != 0 && ndx < iNumberOfFlares);

    aFlares[ndx].fRed = red;
    aFlares[ndx].fGreen = grn;
    aFlares[ndx].fBlue = blu;
    aFlares[ndx].fAlpha = alpha;
}
//==============================================================================
//  yyyy.mm.dd  init    change remarks
//------------------------------------------------------------------------------
void
nLenseFlare::GetBaseColor(float& red,
                          float& grn,
                          float& blu,
                          float& alpha)
{
    red = fBaseRed;
    grn = fBaseGreen;
    blu = fBaseBlue;
    alpha = fBaseAlpha;
}
//==============================================================================
//  2000.06.06  jeremy      created
//------------------------------------------------------------------------------
vector4
nLenseFlare::GetBlindColor() const
{
   return vBlindColor;
}
//==============================================================================
//  yyyy.mm.dd  init    change remarks
//------------------------------------------------------------------------------
void
nLenseFlare::GetFlareColorAt(int ndx,
                             float& red,
                             float& grn,
                             float& blu,
                             float& alpha)
{
    red = aFlares[ndx].fRed;
    grn = aFlares[ndx].fGreen;
    blu = aFlares[ndx].fBlue;
    alpha = aFlares[ndx].fAlpha;
}
//==============================================================================
//  2001.06.27  floh    + converted to new scenegraph stuff  
//  2001.01.09  jeremy  + port to ndynvertexbuffer
//------------------------------------------------------------------------------
//  KEY:
//  1)  ensure that Attach is not called before the lenseflare has been
//      completely defined and inititalized and parent's Attach has
//      successfully returned
//------------------------------------------------------------------------------
bool
nLenseFlare::Attach(nSceneGraph2* sceneGraph)
{
    n_assert(sceneGraph);

    if (bEnded && nVisNode::Attach(sceneGraph))
    {
        if (!ref_dynvbuf.IsValid())
        {
            InitVBuffer();
        }
        sceneGraph->AttachVisualNode(this);
        return true;
    }
    return false;
}

//==============================================================================
//  2001.06.27  floh      + converted to new scenegraph stuff
//  2001.01.09  jeremy    + port to ndynvertexbuffer
//  2000.06.27  floh        converted to matrix44
//------------------------------------------------------------------------------
//  KEY:
//  1)  ensure that Compute is not called before the lenseflare has been
//      completely defined and inititalized and parent's Compute has
//      successfully returned
//
//------------------------------------------------------------------------------
void
nLenseFlare::Compute(nSceneGraph2* sceneGraph)
{
    n_assert(sceneGraph);

    if (bEnded)
    {
        nVisNode::Compute(sceneGraph);

        // store the current model-view and projection matrices
        ref_gs->GetMatrix(N_MXM_MODELVIEW, mModelView);
        ref_gs->GetMatrix(N_MXM_PROJECTION, mProjection);

        // get the n3dnode (light source origin) in pseudo-screen space
        CalcSource2d();

        // test to see if the n3dnode appears on the screen
        if (vSource2d.x >= -1.0f && vSource2d.x <= 1.0f &&
            vSource2d.y >= -1.0f && vSource2d.y <= 1.0f &&
            vSource2d.z <= 1.0f)
        {
            bWithinBounds = true;
        }
        else
        {
            bWithinBounds = false;
        }

        if (bWithinBounds)
        {
            vb_dest = this->ref_dynvbuf.Begin(
                ref_ibuf.get(), 
                sceneGraph->GetPixelShader(),
                sceneGraph->GetTextureArray());

            PlaceFlares();
            ColorFlares();
            UVFlares();
            Blind();

            //  set the current model-view and projection matrices to
            //  identity matrices so we are working in pseudo-screen space
            ref_gs->PushMatrix(N_MXM_MODELVIEW);
            ref_gs->PushMatrix(N_MXM_PROJECTION);
            ref_gs->SetMatrix(N_MXM_MODELVIEW,mIdentity);
            ref_gs->SetMatrix(N_MXM_PROJECTION,mIdentity);

            this->ref_dynvbuf.End(iNumberOfFlares * 4, ref_ibuf.get()->GetNumIndices());

            //  reset the current model-view and projection matrices to their
            //  original values
            ref_gs->PopMatrix(N_MXM_PROJECTION);
            ref_gs->PopMatrix(N_MXM_MODELVIEW);
        }
    }
}
//==============================================================================
//  2001.01.09  jeremy    + port to ndynvertexbuffer
//------------------------------------------------------------------------------
void
nLenseFlare::InitVBuffer()
{
    //  assert precondition(s)
    n_assert(!this->ref_ibuf.isvalid());

    int iNumberOfVertices = iNumberOfFlares * 4;    // 4 vert's per quad (flare)
    this->ref_dynvbuf.Initialize((N_VT_COORD|N_VT_RGBA|N_VT_UV0), iNumberOfVertices);


    nIndexBuffer *ibuf = this->ref_gs->FindIndexBuffer("nlenseflare_ibuf");
    if (!ibuf)
    {
        ibuf = this->ref_gs->NewIndexBuffer("nlenseflare_ibuf");

        int iNumberOfIndices = iNumberOfFlares * 6;     // 6 indices per quad
        ibuf->Begin(N_IBTYPE_STATIC, N_PTYPE_TRIANGLE_LIST, iNumberOfIndices);

        int indexIndex  = 0;
        int vertexIndex = 0; 
        for (; vertexIndex < iNumberOfFlares * 4; vertexIndex += 4)
        {
            //  upper-left triangle of quad
            ibuf->Index(indexIndex++, vertexIndex);
            ibuf->Index(indexIndex++, vertexIndex + 1);
            ibuf->Index(indexIndex++, vertexIndex + 2);

            //  lower-right triangle of quad
            ibuf->Index(indexIndex++, vertexIndex + 2);
            ibuf->Index(indexIndex++, vertexIndex + 3);
            ibuf->Index(indexIndex++, vertexIndex);
        }

        ibuf->End();
    }

    n_assert(ibuf);
    this->ref_ibuf = ibuf;
}
//==============================================================================
//  yyyy.mm.dd  init    change remarks
//------------------------------------------------------------------------------
void
nLenseFlare::CalcSource2d()
{
    //  get the current position of the lenseflare's n3dnode in model-view space
    vSource2d.set(mModelView.M41,
                  mModelView.M42,
                  mModelView.M43,
                  1.0f);

    //  calculate the n3dnode's position in pseudo-screen space
    vSource2d = mProjection * vSource2d;
    vSource2d.x = vSource2d.x / vSource2d.w;
    vSource2d.y = vSource2d.y / vSource2d.w;
    vSource2d.z = vSource2d.z / vSource2d.w;
}
//==============================================================================
//  2001.01.09  jeremy      created
//------------------------------------------------------------------------------
void
nLenseFlare::UVFlares()
{
    //  assert precondition(s)
    n_assert(vb_dest);

    vector2 c1(1.0f, 1.0f);
    vector2 c2(0.0f, 1.0f);
    vector2 c3(0.0f, 0.0f);
    vector2 c4(1.0f, 0.0f);
    
    int iCnt = 0;
    for (int i = 0; i < iNumberOfFlares * 4; i += 4, iCnt++)
    {
        n_assert(iCnt < iNumberOfFlares);

        vb_dest->Uv(i    , 0, c1); // uv upper right
        vb_dest->Uv(i + 1, 0, c2); // uv up
        vb_dest->Uv(i + 2, 0, c3); // uv origin
        vb_dest->Uv(i + 3, 0, c4); // uv right
    }
}
//==============================================================================
//  2001.01.09  jeremy    + port to ndynvertexbuffer
//------------------------------------------------------------------------------
void
nLenseFlare::PlaceFlares()
{
    //  assert precondition(s)
    n_assert(vb_dest);

    float fARAdj = 0.75f;   // adjusts for aspect ratio so flares are square
    vector3 v(0.0f, 0.0f, 0.0f);

    int iCnt = 0;
    for (int i = 0; i < iNumberOfFlares * 4; i += 4, iCnt++)
    {
        n_assert(iCnt < iNumberOfFlares);

        float fRealPos_y = aFlares[iCnt].fPosition * -vSource2d.y;
        float fRealPos_x = aFlares[iCnt].fPosition * -vSource2d.x;

        //  position the flare (quad)
        v.x = vSource2d.x + (aFlares[iCnt].fHalfSize * fARAdj) + fRealPos_x;
        v.y = vSource2d.y + aFlares[iCnt].fHalfSize + fRealPos_y;
        vb_dest->Coord(i, v);

        v.x = vSource2d.x - (aFlares[iCnt].fHalfSize * fARAdj) + fRealPos_x;
        v.y = vSource2d.y + aFlares[iCnt].fHalfSize + fRealPos_y;
        vb_dest->Coord(i + 1, v);

        v.x = vSource2d.x - (aFlares[iCnt].fHalfSize * fARAdj) + fRealPos_x;
        v.y = vSource2d.y - aFlares[iCnt].fHalfSize + fRealPos_y;
        vb_dest->Coord(i + 2, v);

        v.x = vSource2d.x + (aFlares[iCnt].fHalfSize * fARAdj) + fRealPos_x;
        v.y = vSource2d.y - aFlares[iCnt].fHalfSize + fRealPos_y;
        vb_dest->Coord(i + 3, v);
    }
}
//==============================================================================
//  2001.01.09  jeremy    + port to ndynvertexbuffer
//------------------------------------------------------------------------------
void
nLenseFlare::ColorFlares()
{            
    //  assert precondition(s)
    n_assert(vb_dest);

    nColorFormat color_format = vb_dest->GetColorFormat();

    int iCnt = 0;
    for (int i = 0; i < iNumberOfFlares * 4; i += 4, iCnt++)
    {
        n_assert(iCnt < iNumberOfFlares);

        float r = aFlares[iCnt].fRed * fBaseRed;
        float g = aFlares[iCnt].fGreen * fBaseGreen;
        float b = aFlares[iCnt].fBlue * fBaseBlue;
        float a = aFlares[iCnt].fAlpha * fBaseAlpha;

        ulong c;
        if (N_COLOR_RGBA == color_format) 
            c = n_f2rgba(r,g,b,a);
        else
            c = n_f2bgra(r,g,b,a);

        vb_dest->Color(i, c);
        vb_dest->Color(i + 1, c);
        vb_dest->Color(i + 2, c);
        vb_dest->Color(i + 3, c);

    }
}
//==============================================================================
//  2000.06.06  jeremy      created
//------------------------------------------------------------------------------
void
nLenseFlare::Blind()
{
    float fAlpha = 0.0f;
    if (vSource2d.x >= -1.0f &&  vSource2d.x <= 1.0f &&
        vSource2d.y >= -1.0f &&  vSource2d.y <= 1.0f)
    {
        float fRadius = n_sqrt((vSource2d.x * vSource2d.x) +
                               (vSource2d.y * vSource2d.y));

        if (fRadius <= 0.25f)
            fAlpha = 4 * (0.25f - fRadius);

        fAlpha *= vBlindColor.w;

        //  update the overlay plane
        ref_fx->AddOverlay(vBlindColor.x,
                           vBlindColor.y,
                           vBlindColor.z,
                           fAlpha);
    }

}
//------------------------------------------------------------------------------
