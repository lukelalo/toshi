#ifndef N_GFXSERVER_H
#define N_GFXSERVER_H
//--------------------------------------------------------------------
/**
    @class nGfxServer

    @brief base class for gfx servers

    Wraps around OpenGL and Direct3D, with the intention to
    have a simple, somewhat OpenGL like interface for managing the
    display and fast rendering of vertex buffers.
  
    (C) 1999 A.Weissflog
*/
//--------------------------------------------------------------------

#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#ifndef N_GFXTYPES_H
#include "gfx/ngfxtypes.h"
#endif

#ifndef N_LIGHT_H
#include "gfx/nlight.h"
#endif

#ifndef N_ENV_H
#include "kernel/nenv.h"
#endif

#ifndef N_VERTEXPOOLMANAGER_H
#include "gfx/nvertexpoolmanager.h"
#endif

#ifndef N_TEXTURE_H
#include "gfx/ntexture.h"
#endif

#ifndef N_MOUSECURSOR_H
#include "gfx/nmousecursor.h"
#endif

#undef N_DEFINES
#define N_DEFINES nGfxServer
#include "kernel/ndefdllclass.h"

//--------------------------------------------------------------------
class nTexture;
class nPixelShader;
class nVertexPool;
class nVertexBuffer;
class nIndexBuffer;
class nProfiler;

#define N_GFX_TEXDIR        "/sys/share/tex"
#define N_GFX_PSHADERDIR    "/sys/share/pshader"
#define N_GFX_VXBUFDIR      "/sys/share/vxbuf"
#define N_GFX_IXBUFDIR      "/sys/share/ixbuf"
#define N_GFX_HNDDIR        "/sys/share/hnd"

//-------------------------------------------------------------------
/**
    @class nGfxStat

    @brief A small graphics statistics object.
*/
//-------------------------------------------------------------------
class nGfxStat {
    int val;            // the statistics variable
    nRef<nEnv> ref_env; // the watcher variable

public: 
    nGfxStat(nKernelServer *ks, nRoot *owner, const char *name)
        : ref_env(owner)
    {
        char buf[N_MAXPATH];
        sprintf(buf,"/sys/var/%s",name);
        nEnv *e = (nEnv *) ks->New("nenv",buf);
        n_assert(e);
        this->ref_env = e;
        this->val = 0;
    };
    ~nGfxStat() {
        if (ref_env.isvalid()) ref_env->Release();
    };

    void Reset(void) {
        val = 0;
    };
    void SetVal(int v) {
        val = v;
    };
    void AddVal(int v) {
        val += v;
    };
    void Flush(void) {
        this->ref_env->SetI(val);
    };
private:
    nGfxStat(nRoot *owner) : ref_env(owner) {};
};

//-------------------------------------------------------------------
template <class TYPE, int SIZE> class nMatrixStack {
    int pos;
    TYPE stack[SIZE];

public:
    nMatrixStack() : pos(0) {};
    void Push(TYPE& m) {
        n_assert(pos < SIZE);
        stack[pos++] = m;
    };
    TYPE Pop(void) {
        n_assert(pos > 0);
        return stack[--pos];
    };
};

//-------------------------------------------------------------------
class N_PUBLIC nGfxServer : public nRoot {
public:
    enum {
        N_MAXLIGHTS = 8,
        N_MXSTACKDEPTH = 4,
        N_MAXCURSORS = 8,
    };

    enum nGfxStatType {
        N_GFXSTATS_TRIANGLES,
        N_GFXSTATS_FRAMERATE,
        N_GFXSTATS_MODELVIEWS,
        N_GFXSTATS_LIGHTS,
        N_GFXSTATS_TEXTURES,
        N_GFXSTATS_TEXTURESIZE,
        N_GFXSTATS_PSHADER_SWITCHES,
        N_GFXSTATS_MESH_SWITCHES,
        N_GFXSTATS_TEXTURE_SWITCHES,
    };

    enum nWindowMode {
        N_WINDOWMODE_UNDEFINED,
        N_WINDOWMODE_ACTIVE,
        N_WINDOWMODE_HIDDEN,
        N_WINDOWMODE_SHOWN,
    };

    // display mode stuff
    char disp_mode[256];
    char disp_type[32];
    char disp_dev[64];
    int disp_w;
    int disp_h;
    int disp_bpp;
    int disp_zbufbits;          // requested zbuffer bit depth
    int disp_sbufbits;          // requested stencil buffer bit depth

    // misc
    ulong unique_id;
    // frustum extents
    float clip_minx,clip_maxx;
    float clip_miny,clip_maxy;
    float clip_minz,clip_maxz;
    double timeStamp;           // time stamp of last frame

    // lighting
    int numLights;
    nLight lights[N_MAXLIGHTS];

    // Shared Resource Directories
    nRef<nRoot> ref_texdir;
    nRef<nRoot> ref_pshaderdir;
    nRef<nRoot> ref_vxbufdir;
    nRef<nRoot> ref_ixbufdir;

    // matrices
    matrix44 modelview;
    matrix44 view;         
    matrix44 inv_view;
    matrix44 projection;   
    nMatrixStack<matrix44,N_MXSTACKDEPTH> modelview_stack;
    nMatrixStack<matrix44,N_MXSTACKDEPTH> view_stack;
    nMatrixStack<matrix44,N_MXSTACKDEPTH> invview_stack;
    nMatrixStack<matrix44,N_MXSTACKDEPTH> proj_stack;

    // minimize render state changes
    nVertexBubble *current_vertexbubble;
    nPixelShader *current_pixelshader;
    nTexture *current_texture[N_MAXNUM_TEXSTAGES];

    // statistics
    nGfxStat stats_triangles;
    nGfxStat stats_framerate;
    nGfxStat stats_modelviews;
    nGfxStat stats_lights;
    nGfxStat stats_textures;
    nGfxStat stats_texturesize;
    nGfxStat stats_pshader_switches;
    nGfxStat stats_mesh_switches;
    nGfxStat stats_texture_switches;

    // mouse cursor stuff
    bool cursorShown;
    int currentCursor;
    int numCursors;
    nMouseCursor cursor[N_MAXCURSORS];

public:    
    static nKernelServer *kernelServer;

    static nProfiler *prof_fps;

    nGfxServer();
    virtual ~nGfxServer();

    // Display Management
    virtual bool SetDisplayMode(const char *);
    virtual const char *GetDisplayMode(void);
    virtual bool OpenDisplay(void);
    virtual void CloseDisplay(void);
    virtual bool GetDisplayDesc(int& x0, int& y0, int& x1, int& y1);
    virtual bool GetFontDesc(int& height);
    virtual void SetViewVolume(float minx, float maxx, float miny, float maxy, float minz, float maxz);
    virtual void GetViewVolume(float&, float&, float&, float&, float&, float&);
    void SetPerspective(float fovDegrees, float aspectRatio, float zNear, float zFar);
    void GetPerspective(float&, float&, float&, float&);
    void SetFOV(float fovDegrees);
    float GetFOV();
    virtual void SetClearColor(float, float, float, float);
    virtual void GetClearColor(float&, float&, float&, float&);
    virtual bool Screenshot(const char *);
    virtual bool ScaledScreenshot(const char *, int, int);
    virtual nColorFormat GetColorFormat(void);
    virtual void SetWindowMode(nWindowMode);

    // Scene Management (between OpenDisplay/CloseDisplay)
    virtual bool BeginScene(void);
    virtual void EndScene(void);

    virtual void SetMatrix(nMatrixMode, matrix44&);
    virtual void GetMatrix(nMatrixMode, matrix44&);
    virtual void PushMatrix(nMatrixMode);
    virtual void PopMatrix(nMatrixMode);

    virtual void SetState(nRState&);
    virtual bool SetClipPlane(int, vector4&);
    virtual bool SetLight(nLight&);
    const nLight& GetLight(int);
    int GetNumLights();

    virtual void Begin(nPrimType);
    virtual void Coord(float, float, float);
    virtual void Norm(float, float, float);
    virtual void Rgba(float, float, float, float);
    virtual void Uv(ulong layer, float, float);
    virtual void End(void);   
	virtual void ShowDisplay(void); 
    
    // Object Factory
    virtual nTexture      *NewTexture(const char *);
    virtual nPixelShader  *NewPixelShader(const char *);
    virtual nVertexPool   *NewVertexPool(nVertexPoolManager *, nVBufType, int, int);
    virtual nVertexBuffer *NewVertexBuffer(const char *, nVBufType vbtype, int vtype, int vnum);
    virtual nIndexBuffer  *NewIndexBuffer(const char *);

    // Resource Sharing and Management
    virtual nTexture      *FindTexture(const char *);
    virtual nPixelShader  *FindPixelShader(const char *);
    virtual nVertexBuffer *FindVertexBuffer(const char *);
    virtual nIndexBuffer  *FindIndexBuffer(const char *);

    // redundant render state handling
    void SetCurrentPixelShader(nPixelShader *ps) 
    {
        // ps can be NULL
        if (ps != this->current_pixelshader) {
            this->SetStats(N_GFXSTATS_PSHADER_SWITCHES,1);
        }
        this->current_pixelshader = ps;
    };
    nPixelShader *GetCurrentPixelShader(void) 
    {
        return this->current_pixelshader;
    };

    void SetCurrentVertexBubble(nVertexBubble *vb) 
    {
        // mb can be NULL
        if (vb != this->current_vertexbubble) {
            this->SetStats(N_GFXSTATS_MESH_SWITCHES,1);
        }
        this->current_vertexbubble = vb;
    };
    nVertexBubble *GetCurrentVertexBubble(void) 
    {
        return this->current_vertexbubble;
    };
    
    void SetCurrentTexture(int stage, nTexture *tex) 
    {
        n_assert((stage>=0) && (stage<N_MAXNUM_TEXSTAGES));
        if (tex)
        {
            int useCount = tex->GetUseCount();
            if (tex != this->current_texture[stage])
            {
                this->SetStats(N_GFXSTATS_TEXTURE_SWITCHES, 1);
                if (0 == useCount)
                {
                    this->SetStats(N_GFXSTATS_TEXTURES, 1);
                    this->SetStats(N_GFXSTATS_TEXTURESIZE, tex->GetByteSize());
                }
            }        
            // bump use count
            tex->SetUseCount(++useCount);
        }
        this->current_texture[stage] = tex;
    };
    nTexture *GetCurrentTexture(int stage) 
    {
        n_assert((stage>=0) && (stage<N_MAXNUM_TEXSTAGES));
        return this->current_texture[stage];
    };

    // Text Support
    virtual bool BeginText(void);
    virtual void TextPos(float, float);
    virtual bool Text(const char *);
    virtual bool EndText(void);

    // Window-System-Message-Pump
    virtual bool Trigger(void);

    // Statistics
    void BeginStats(void);
    void SetStats(nGfxStatType t, int n);
    void EndStats(void);

    /// begin defining mouse cursors
    void BeginCursors(int num);
    /// set a single mouse cursor
    void SetCursor(int index, const char* texPath, int xHotspot, int yHotspot);
    /// finish defining mouse cursors
    void EndCursors();
    /// select the current mouse cursor
    void SetCurrentCursor(int index, bool show);
    /// get the current mouse cursor
    int GetCurrentCursor();
    /// show mouse cursor
    virtual void ShowCursor();
    /// hide mouse cursor
    virtual void HideCursor();

protected:
    /// load a mouse cursors texture from its path
    void LoadCursors();

    char *getResourceID(const char *, char *, ulong);
    const char *getModeTag(const char *, const char *, char *, int);
    void releaseResources();

private:
    /// vertical field of view for frustum (in degrees)
    float fov;
};
//--------------------------------------------------------------------
#endif
