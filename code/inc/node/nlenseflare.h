//==============================================================================
//  node/nlenseflare.h
//  author: Jeremy Bishop
//  (C) 2000 Radon Labs GmbH
//------------------------------------------------------------------------------

#ifndef N_NLENSEFLARE_H
#define N_NLENSEFLARE_H

#ifndef N_VISNODE_H
#include "node/nvisnode.h"
#endif

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#ifndef N_SPECIALFXSERVER_H
#include "misc/nspecialfxserver.h"
#endif

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#ifndef N_DYNVERTEXBUFFER_H
#include "gfx/ndynvertexbuffer.h"
#endif

#undef N_DEFINES
#define N_DEFINES nLenseFlare

#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
struct nFlareDesc
{
    friend class nLenseFlare;

    nFlareDesc();
   ~nFlareDesc();

private:
   float fSize;         //  length of one side of quad (flare)
   float fHalfSize;     //  1/2 of the size (used when placing quad on line)
   float fPosition;     //  % distance from light source to center of screen;
                        //  0.0f places the flare over the light source, 1.0f
                        //  places the flare at the center of the screen; higher
                        //  values place the flare further along the line (2.0f
                        //  places the flare opposite the light source, etc.),
                        //  and negative values place the flare behind the light
                      
   float fRed;          //  the vertex coloring of the flare
   float fGreen;        //  ...
   float fBlue;         //  ...
   float fAlpha;        //  ...
};
//------------------------------------------------------------------------------
inline
nFlareDesc::nFlareDesc(): fSize(0.125f),
                          fHalfSize(fSize / 2),
                          fPosition(1.0f),
                          fRed(1.0f),
                          fGreen(1.0f),
                          fBlue(1.0f),
                          fAlpha(1.0f)
{}
//------------------------------------------------------------------------------
inline
nFlareDesc::~nFlareDesc() {}

//------------------------------------------------------------------------------
class N_DLLCLASS nLenseFlare : public nVisNode
{
public:
    nLenseFlare();
   ~nLenseFlare();

    static nClass *local_cl;
    static nKernelServer *ks;

    virtual bool SaveCmds(nPersistServer*);
    virtual bool Attach(nSceneGraph2*);
    virtual void Compute(nSceneGraph2*);

    virtual void BeginFlares(int iNumberOfFlares);
    virtual void EndFlares();

    virtual void SetBaseColor(float red, float grn, float blu, float alpha);
    virtual void SetBlindColor(const vector4& vBlindClr);

    virtual void SetFlareSizeAt(int ndx, float size);
    virtual void SetFlarePosAt(int ndx, float pos);
    virtual void SetFlareColorAt(int ndx,
                                 float red,
                                 float grn,
                                 float blu,
                                 float alpha);

    virtual inline int GetNumberOfFlares();
    virtual void GetBaseColor(float& red, float& grn, float& blu, float& alpha);
    virtual vector4 GetBlindColor() const;

    virtual inline float GetFlareSizeAt(int ndx);
    virtual inline float GetFlarePosAt(int ndx);
    virtual void GetFlareColorAt(int ndx,
                                 float& red,
                                 float& grn,
                                 float& blu,
                                 float& alpha);

protected:
    virtual void InitVBuffer();
    virtual void CalcSource2d();
    virtual void PlaceFlares();
    virtual void ColorFlares();
    virtual void UVFlares();
    virtual void Blind();

private:
    enum {
        MAXFLARES = 16,
    };

    nAutoRef<nGfxServer>        ref_gs;
    nDynVertexBuffer            ref_dynvbuf;
    nRef<nIndexBuffer>          ref_ibuf;
    nAutoRef<nSpecialFxServer>  ref_fx;

    nVertexBuffer *vb_dest;

    matrix44 mModelView;
    matrix44 mProjection;
    matrix44 mIdentity;

    nFlareDesc* aFlares;
    int iNumberOfFlares;
    vector4 vSource2d;      // eventual pseudo-screen space position of light
    bool bBegun;            // signals that BeginFlares(...) has completed
    bool bEnded;            // signals that EndFlares(...) has completed
    bool bWithinBounds;     // signals that the lenseflare is visible

    float fBaseRed, fBaseBlue, fBaseGreen, fBaseAlpha;  //  base color that is
                                                        //  combined with float
                                                        //  values received from
                                                        //  a mixer (fix so that
                                                        //  nlenseflare works
                                                        //  with a mixer
    vector4 vBlindColor;    // blinding color when looking at light source
};
//------------------------------------------------------------------------------
inline
int
nLenseFlare::GetNumberOfFlares() 
{
    return iNumberOfFlares;
}
//------------------------------------------------------------------------------
inline
float
nLenseFlare::GetFlareSizeAt(int ndx) 
{
    return aFlares[ndx].fSize;
}
//------------------------------------------------------------------------------
inline
float
nLenseFlare::GetFlarePosAt(int ndx) 
{
    return aFlares[ndx].fPosition;
}
//------------------------------------------------------------------------------    
#endif
