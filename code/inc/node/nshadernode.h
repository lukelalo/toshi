#ifndef N_SHADERNODE_H
#define N_SHADERNODE_H
//--------------------------------------------------------------------
/**
    @class nShaderNode
    @ingroup NebulaVisnodeModule
    
    @brief nVisNode wrapper around nPixelShader
*/
//--------------------------------------------------------------------
#ifndef N_VISNODE_H
#include "node/nvisnode.h"
#endif

#ifndef N_PIXELSHADERDESC_H
#include "gfx/npixelshaderdesc.h"
#endif

#ifndef N_RSRCPATH_H
#include "misc/nrsrcpath.h"
#endif

#ifndef N_TEXTURE_H
#include "gfx/ntexture.h"
#endif

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

//--------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nShaderNode
#include "kernel/ndefdllclass.h"

//--------------------------------------------------------------------
class nGfxServer;
class nPixelShader;
class N_PUBLIC nShaderNode : public nVisNode {
    bool in_begin_tunit;
    int cur_tunit;
    
    nRef<nPixelShader> ref_ps;
    nPixelShaderDesc ps_desc;
    int render_pri;
    int mipLodBias;
    
public:
    static nKernelServer *kernelServer;

    nShaderNode()
        : in_begin_tunit(false),
          cur_tunit(0),
          ref_ps(this),
          render_pri(0),
          mipLodBias(0)
    {
        // empty
    }
    virtual ~nShaderNode();
    virtual bool SaveCmds(nPersistServer*);
    virtual bool Attach(nSceneGraph2*);
    virtual void Compute(nSceneGraph2*);
    virtual void Preload();

    nPixelShader *GetPixelShader(void) 
    {
        if (!this->ref_ps.isvalid()) this->init_pixelshader();
        return this->ref_ps.get();
    };

    //--- render pri ---
    void SetRenderPri(int p) 
    {
        this->render_pri = p;
    };
    int GetRenderPri(void) 
    {
        return this->render_pri;
    };

    //--- setting shader stage related render states ---
    void SetNumStages(int n) 
    {
        ps_desc.SetNumStages(n);
    };
    int GetNumStages(void) 
    {
        return ps_desc.GetNumStages();
    };

    bool SetColorOp(int, const char *);
    bool SetAlphaOp(int, const char *);

    void BeginTUnit(int tunit) 
    {
        n_assert(!in_begin_tunit);
        n_assert(tunit <= ps_desc.GetNumStages());
        n_assert((tunit>=0) && (tunit<nPixelShaderDesc::MAXSTAGES));
        cur_tunit = tunit;
        in_begin_tunit = true;
    };
    void EndTUnit(void) 
    {
        n_assert(in_begin_tunit);
        in_begin_tunit = false;
    };

    void SetAddress(nRStateParam tau, nRStateParam tav) 
    {
        n_assert(in_begin_tunit);
        ps_desc.SetAddressU(cur_tunit,tau);
        ps_desc.SetAddressV(cur_tunit,tav);
    };
    void SetMinMagFilter(nRStateParam tfmin, nRStateParam tfmag) 
    {
        n_assert(in_begin_tunit);
        ps_desc.SetMinFilter(cur_tunit,tfmin);
        ps_desc.SetMagFilter(cur_tunit,tfmag);
    };
    void SetTexCoordSrc(nRStateParam tgm) 
    {
        n_assert(in_begin_tunit);
        ps_desc.SetTexCoordSrc(cur_tunit,tgm);
    };
    void SetConst(int tunit, const vector4& v) 
    {
        ps_desc.SetConst(tunit,v);
    };
    void SetEnableTransform(bool b) 
    {
        n_assert(in_begin_tunit);
        ps_desc.SetEnableTransform(cur_tunit,b);
    };
    void SetMatrix(const matrix44& m) 
    {
        n_assert(in_begin_tunit);
        ps_desc.SetMatrix(cur_tunit,m);
    };
    void Txyz(const vector3& v) 
    {
        n_assert(in_begin_tunit);
        ps_desc.Txyz(cur_tunit,v);
    };
    void Txyz(int tunit, const vector3& v) 
    {
        ps_desc.Txyz(tunit,v);
    };
    void Rxyz(const vector3& v) 
    {
        n_assert(in_begin_tunit);
        vector3 v_rad;
        v_rad.x = n_deg2rad(v.x);
        v_rad.y = n_deg2rad(v.y);
        v_rad.z = n_deg2rad(v.z);
        ps_desc.Rxyz(cur_tunit,v_rad);
    };
    void Rxyz(int tunit, const vector3& v) 
    {
        vector3 v_rad;
        v_rad.x = n_deg2rad(v.x);
        v_rad.y = n_deg2rad(v.y);
        v_rad.z = n_deg2rad(v.z);
        ps_desc.Rxyz(tunit,v_rad);
    };
    void Sxyz(const vector3& v) 
    {
        n_assert(in_begin_tunit);
        ps_desc.Sxyz(cur_tunit,v);
    };
    void Sxyz(int tunit, const vector3& v) 
    {
        ps_desc.Sxyz(tunit,v);
    };
    //--- getting shader stage related render states ---
    void GetColorOp(int, char *, int);
    void GetAlphaOp(int, char *, int);

    void GetAddress(int stage, nRStateParam& tau, nRStateParam& tav) 
    {
        tau = ps_desc.GetAddressU(stage);
        tav = ps_desc.GetAddressV(stage);
    };
    void GetMinMagFilter(int stage, nRStateParam& tfmin, nRStateParam& tfmag) 
    {
        tfmin = ps_desc.GetMinFilter(stage);
        tfmag = ps_desc.GetMagFilter(stage);
    };
    nRStateParam GetTexCoordSrc(int stage) 
    {
        return ps_desc.GetTexCoordSrc(stage);
    };
    const vector4& GetConst(int stage) 
    {
        return ps_desc.GetConst(stage);
    };
    bool GetEnableTransform(int stage) 
    {
        return ps_desc.GetEnableTransform(stage);
    };
    const matrix44& GetMatrix(int stage) 
    {
        return ps_desc.GetMatrix(stage);
    };
    const vector3& GetT(int stage) 
    {
        return ps_desc.GetT(stage);
    };
    const vector3 GetR(int stage) 
    {
        vector3 v_rad = ps_desc.GetR(stage);
        v_rad.x = n_rad2deg(v_rad.x);
        v_rad.y = n_rad2deg(v_rad.y);
        v_rad.z = n_rad2deg(v_rad.z);
        return v_rad;
    };
    const vector3& GetS(int stage) 
    {
        return ps_desc.GetS(stage);
    };

    //--- set stage independent render states ---
    void SetDiffuse(vector4& v) 
    {
        ps_desc.SetDiffuse(v);
    };
    void SetEmissive(vector4& v) 
    {
        ps_desc.SetEmissive(v);
    };
    void SetAmbient(vector4& v) 
    {
        ps_desc.SetAmbient(v);
    };
    void SetLightEnable(bool b) 
    {
        ps_desc.SetLightEnable(b);
    };
    void SetAlphaEnable(bool b) 
    {
        ps_desc.SetAlphaEnable(b);
    };
    void SetZWriteEnable(bool b) 
    {
        ps_desc.SetZWriteEnable(b);
    };
    void SetFogEnable(bool b) 
    {
        ps_desc.SetFogEnable(b);
    };
    void SetAlphaBlend(nRStateParam src, nRStateParam dest) 
    {
        ps_desc.SetAlphaSrcBlend(src);
        ps_desc.SetAlphaDestBlend(dest);
    };
    void SetZFunc(nRStateParam p) 
    {
        ps_desc.SetZFunc(p);
    };
    void SetCullMode(nRStateParam p) 
    {
        ps_desc.SetCullMode(p);
    };
    void SetColorMaterial(nRStateParam p) 
    {
        ps_desc.SetColorMaterial(p);
    };
    void SetAlphaTestEnable(bool b)
    {
        ps_desc.SetAlphaTestEnable(b);
    };
    void SetAlphaRef(float r)
    {
        ps_desc.SetAlphaRef(r);
    };
    void SetAlphaFunc(nRStateParam p)
    {
        ps_desc.SetAlphaFunc(p);
    };
	void SetWireframe(bool b)
	{
		ps_desc.SetWireframe(b);
	};

	void SetNormalizeNormals(bool b)
	{
		ps_desc.SetNormalizeNormals(b);
	}


    //--- get stage independent render states ---
    const vector4& GetDiffuse(void) 
    {
        return ps_desc.GetDiffuse();
    };
    const vector4& GetEmissive(void) 
    {
        return ps_desc.GetEmissive();
    };
    const vector4& GetAmbient(void) 
    {
        return ps_desc.GetAmbient();
    };
    bool GetLightEnable(void) 
    {
        return ps_desc.GetLightEnable();
    };
    bool GetAlphaEnable(void) 
    {
        return ps_desc.GetAlphaEnable();
    };
    bool GetZWriteEnable(void) 
    {
        return ps_desc.GetZWriteEnable();
    };
    bool GetFogEnable(void) 
    {
        return ps_desc.GetFogEnable();
    };
    void GetAlphaBlend(nRStateParam& src, nRStateParam& dest) 
    {
        src  = ps_desc.GetAlphaSrcBlend();
        dest = ps_desc.GetAlphaDestBlend();
    }
    nRStateParam GetZFunc(void) 
    {
        return ps_desc.GetZFunc();
    };
    nRStateParam GetCullMode(void) 
    {
        return ps_desc.GetCullMode();
    };
    nRStateParam GetColorMaterial(void) 
    {
        return ps_desc.GetColorMaterial();
    };
    bool GetAlphaTestEnable()
    {
        return ps_desc.GetAlphaTestEnable();
    };
    float GetAlphaRef()
    {
        return ps_desc.GetAlphaRef();
    };
    nRStateParam GetAlphaFunc()
    {
        return ps_desc.GetAlphaFunc();
    };

    /// set mipmap lod bias
    void SetMipLodBias(int bias)
    {
        this->mipLodBias = bias;
    }
    /// get mipmap lod bias
    int GetMipLodBias()
    {
        return this->mipLodBias;
    }
	bool GetWireframe()
	{
		return ps_desc.GetWireframe();
	};

	bool GetNormalizeNormals()
	{
		return ps_desc.GetNormalizeNormals();
	};

private:
    bool load_texture(int);
    void init_pixelshader(void);
};
//--------------------------------------------------------------------
#endif

