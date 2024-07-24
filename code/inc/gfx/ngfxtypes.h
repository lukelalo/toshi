#ifndef N_GFXTYPES_H
#define N_GFXTYPES_H
//-------------------------------------------------------------------
//  OVERVIEW
//  Gfx related typedefs and low level classes.
//
//  21-Oct-00   floh    + new vertex components: weight and joint index
//
//  (C) 1999 A.Weissflog
//-------------------------------------------------------------------

#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

//-- convert float color components to rgba or bgra color -----------
#define n_f2rgba(r,g,b,a) (ulong(((ulong((a)*255.0f)<<24)|(ulong((b)*255.0f)<<16)|(ulong((g)*255.0f)<<8)|(ulong((r)*255.0f)))))
#define n_f2bgra(r,g,b,a) (ulong(((ulong((a)*255.0f)<<24)|(ulong((r)*255.0f)<<16)|(ulong((g)*255.0f)<<8)|(ulong((b)*255.0f)))))

//-- matrix modes ---------------------------------------------------
enum nMatrixMode {
    N_MXM_VIEWER,
    N_MXM_MODELVIEW,
    N_MXM_PROJECTION,
    N_MXM_INVVIEWER,    // only GetMatrix()!
};

//-- primitive types ------------------------------------------------
enum nPrimType {
    N_PTYPE_TRIANGLE_LIST,
    N_PTYPE_TRIANGLE_FAN, 
    N_PTYPE_TRIANGLE_STRIP,
    N_PTYPE_LINE_LIST,
    N_PTYPE_LINE_STRIP,
    N_PTYPE_POINT_LIST,
};

//-- light types ----------------------------------------------------
enum nLightType {
    N_LIGHT_AMBIENT,
    N_LIGHT_POINT,
    N_LIGHT_SPOT,
    N_LIGHT_DIRECTIONAL,
};

//-- fog modes ------------------------------------------------------
enum nFogMode {
    N_FOGMODE_OFF,
    N_FOGMODE_LINEAR,
    N_FOGMODE_EXP,
    N_FOGMODE_EXP2,
};

//-- vertex components ----------------------------------------------
enum nVertexType {
    N_VT_VOID  = 0,         // undefined
    N_VT_COORD = (1<<0),    // has xyz
    N_VT_NORM  = (1<<1),    // has normals
    N_VT_RGBA  = (1<<2),    // has color

    N_VT_UV0   = (1<<3),    // has texcoord set 0
    N_VT_UV1   = (1<<4),    // has texcoord set 1
    N_VT_UV2   = (1<<5),    // has texcoord set 2
    N_VT_UV3   = (1<<6),    // has texcoord set 3

    N_VT_JW    = (1<<7),    // has up to 4 joint weights per vertex
};

enum {
    N_MAXNUM_TEXCOORDS  = 4,
    N_MAXNUM_WEIGHTSETS = 4,
    N_MAXNUM_JOINTSETS  = 4,
    N_MAXNUM_TEXSTAGES  = 4,
};

//-- platform specific color format ----------------------------------
enum nColorFormat {
    N_COLOR_RGBA,
    N_COLOR_BGRA,
};

//-- vertex buffer types --------------------------------------------
enum nVBufType {
    N_VBTYPE_STATIC = 0,        // vbuffer is written once and never touched again
    N_VBTYPE_READONLY = 1,      // this vbuffer is never rendered
    N_VBTYPE_WRITEONLY = 2,     // this vbuffer is only written to

    N_NUM_VBTYPES = 3,
};

//-- index buffer types ---------------------------------------------
enum nIBufType {
    N_IBTYPE_STATIC = 0,        // index buffer is written once never touched again
    N_IBTYPE_WRITEONLY = 1,     // index buffer will only be written

    N_NUM_IBTYPES = 2,
};

//-- render states --------------------------------------------------
enum nRStateType {
    N_RS_VOID             = 0,

    // texturing
    N_RS_TEXTURESTAGE     = 1,      // 0..n
    N_RS_TEXTUREHANDLE    = 2,      // nTexture *
    N_RS_TEXTUREADDRESSU  = 3,      // N_TADDR_*
    N_RS_TEXTUREADDRESSV  = 4,      // N_TADDR_*
    N_RS_TEXTUREBLEND     = 5,      // N_TBLEND_*
    N_RS_TEXTUREMINFILTER = 6,      // N_TFILTER_*
    N_RS_TEXTUREMAGFILTER = 7,      // N_TFILTER_*

    // z buffer
    N_RS_ZWRITEENABLE     = 8,      // N_TRUE/N_FALSE
    N_RS_ZFUNC            = 9,      // N_CMP_*
    N_RS_ZBIAS            = 10,     // float

    // alpha blending
    N_RS_ALPHABLENDENABLE = 11,     // N_TRUE/N_FALSE
    N_RS_ALPHASRCBLEND    = 12,     // N_ABLEND_*
    N_RS_ALPHADESTBLEND   = 13,     // N_ABLEND_*

    // lighting and material
    N_RS_LIGHTING         = 14,     // N_TRUE/N_FALSE
    N_RS_COLORMATERIAL    = 15,     // N_CMAT_*
    
    // textur coord generation
    N_RS_TEXGENMODE       = 16,     // N_TEXGEN_*
    N_RS_TEXGENPARAMU     = 17,     // float[4] *
    N_RS_TEXGENPARAMV     = 18,     // float[4] *

    // fog
    N_RS_FOGMODE          = 20,     // N_FOGMODE_*
    N_RS_FOGSTART         = 21,     // float
    N_RS_FOGEND           = 22,     // float
    N_RS_FOGDENSITY       = 23,     // float
    N_RS_FOGCOLOR         = 24,     // float[4] *

    // misc
    N_RS_MATERIALHANDLE   = 25,     // nMaterial *    
    N_RS_DONTCLIP         = 26,     // == ClipVolumeHint
    N_RS_CLIPPLANEENABLE  = 27,     // int
    N_RS_CLIPPLANEDISABLE = 28,     // int
    N_RS_CULLMODE         = 29,     // N_CULL_*
    N_RS_NORMALIZENORMALS = 30,     // N_TRUE/N_FALSE

    // alpha test
    N_RS_ALPHATESTENABLE  = 31,     // N_TRUE/N_FALSE
    N_RS_ALPHAREF         = 32,     // int
    N_RS_ALPHAFUNC        = 33,     // N_CMP_*

    // stencil buffer
    N_RS_STENCILENABLE    = 34,     // N_TRUE/N_FALSE
    N_RS_STENCILFAIL      = 35,     // N_STENCILOP_*
    N_RS_STENCILZFAIL     = 36,     // N_STENCILOP_*
    N_RS_STENCILPASS      = 37,     // N_STENCILOP_*
    N_RS_STENCILFUNC      = 38,     // N_CMP_*
    N_RS_STENCILREF       = 39,     // int
    N_RS_STENCILMASK      = 40,     // int
    N_RS_STENCILCLEAR     = 41,     // int

    N_RS_COLORWRITEENABLE = 42,     // N_TRUE/N_FALSE

    N_RS_NUM = 43,
};

//-- renderstate parameters -----------------------------------------
enum nRStateParam {
    N_FALSE = 0,
    N_TRUE = 1,

    N_TFILTER_NEAREST,              
    N_TFILTER_LINEAR,               
    N_TFILTER_NEAREST_MIPMAP_NEAREST,
    N_TFILTER_LINEAR_MIPMAP_NEAREST,
    N_TFILTER_NEAREST_MIPMAP_LINEAR,
    N_TFILTER_LINEAR_MIPMAP_LINEAR,

    N_TADDR_WRAP,       
    N_TADDR_CLAMP,      

    N_TBLEND_DECAL,     // cPix=(cSrc*(1.0-aTex))+(aTex*cTex), aPix=aSrc
    N_TBLEND_MODULATE,  // cPix=cSrc*cTex, aPix=aSrc*aTex
    N_TBLEND_REPLACE,   // cPix=cTex, aPix=aTex
    N_TBLEND_BLEND,     // cPix=cSrc*(1.0-cTex)+(cTex), aPix=aSrc*aTex

    N_CMP_NEVER,       
    N_CMP_LESS,        
    N_CMP_EQUAL,       
    N_CMP_LESSEQUAL,   
    N_CMP_GREATER,     
    N_CMP_NOTEQUAL,    
    N_CMP_GREATEREQUAL,
    N_CMP_ALWAYS,      

    N_ABLEND_ZERO,          // (0,0,0,0)
    N_ABLEND_ONE,           // (1,1,1,1)
    N_ABLEND_SRCCOLOR,      // (Rs,Gs,Bs,As)
    N_ABLEND_INVSRCCOLOR,   // (1-Rs,1-Gs,1-Bs,1-As)
    N_ABLEND_SRCALPHA,      // (As,As,As,As)
    N_ABLEND_INVSRCALPHA,   // (1-As,1-As,1-As,1-As)
    N_ABLEND_DESTALPHA,     // (Ad,Ad,Ad,Ad)
    N_ABLEND_INVDESTALPHA,  // (1-Ad,1-Ad,1-Ad,1-Ad)
    N_ABLEND_DESTCOLOR,     // (Rd,Gd,Bd,Ad)
    N_ABLEND_INVDESTCOLOR,  // (1-Rd,1-Gd,1-Bd,1-Ad)

    N_TEXGEN_OFF,
    N_TEXGEN_OBJECTLINEAR,  // == GL_OBJECT_LINEAR
    N_TEXGEN_EYELINEAR,     // == GL_EYE_LINEAR
    N_TEXGEN_SPHEREMAP,     // == GL_SPHERE_MAP

    N_CULL_NONE,
    N_CULL_CW,
    N_CULL_CCW,

    N_CMAT_MATERIAL,        // material completely defines light parameters
    N_CMAT_DIFFUSE,         // vertex color defines diffuse component
    N_CMAT_EMISSIVE,        // vertex color defines emissive component
    N_CMAT_AMBIENT,         // vertex color defines ambient component

    N_TCOORDSRC_UV0,        // the texture coordinate source
    N_TCOORDSRC_UV1,
    N_TCOORDSRC_UV2,
    N_TCOORDSRC_UV3,
    N_TCOORDSRC_OBJECTSPACE,
    N_TCOORDSRC_EYESPACE,
    N_TCOORDSRC_SPHEREMAP,

    N_STENCILOP_KEEP,
    N_STENCILOP_ZERO,
    N_STENCILOP_REPLACE,
    N_STENCILOP_INVERT,
    N_STENCILOP_INCR,       // increment with saturate
    N_STENCILOP_DECR,       // decrement with saturate
};

//-- render state class ---------------------------------------------
class nRState {
public:    
    nRStateType rtype;    
    union {
        void *p;
        int   i;
        float f;
        nRStateParam r;
    } rstate;
    inline nRState() { rtype=N_RS_VOID; };
    inline nRState(nRStateType t, void *p)          { rtype=t; rstate.p=p; };
    inline nRState(nRStateType t, int i)            { rtype=t; rstate.i=i; };
    inline nRState(nRStateType t, nRStateParam r)   { rtype=t; rstate.r=r; };
    inline nRState(nRStateType t, float f)          { rtype=t; rstate.f=f; };

    inline void Set(nRStateType t, void *p)         { rtype=t; rstate.p=p; };
    inline void Set(nRStateType t, int i)           { rtype=t; rstate.i=i; };
    inline void Set(nRStateType t, nRStateParam r)  { rtype=t; rstate.r=r; };
    inline void Set(nRStateType t, float f)         { rtype=t; rstate.f=f; };

    inline bool Cmp(nRState *rs) {
        if ((rs->rtype==rtype) && (rs->rstate.i==rstate.i)) return true;
        else                                                return false;
    };
};

//-------------------------------------------------------------------
#endif
