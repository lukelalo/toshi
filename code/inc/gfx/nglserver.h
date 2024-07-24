#ifndef N_GLSERVER_H
#define N_GLSERVER_H
//-------------------------------------------------------------------
/**
    @class nGlServer

     @brief gfx server on top of OpenGL 1.1

    (C) 1999 A.Weissflog
*/
//-------------------------------------------------------------------
#include <stdlib.h>

#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_GFXSERVER_H
#include "gfx/ngfxserver.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#ifdef __WIN32__
#include <windows.h>
#endif

#define GL_GLEXT_PROTOTYPES (1)

#ifdef __LINUX__
// Minimally, this define is needed to enable the definition of
// some of the GL_EXT_texture_env_dot3 type functions with Mesa
// 4.0 headers.  (This version will be with XFree 4.3, so more
// attention will need to be paid to it then.)
#define GL_GLEXT_LEGACY 1
#endif
#ifdef __MACOSX__
// Define this to force some constants to be initialized from
// the Apple glext.h header.  Apple defines the ARB versions
// of the texture env stuff from OpenGL 1.3 while Nebula uses
// the EXT form from OpenGL 1.2.
#define GL_EXT_texture_env_combine 1
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <Carbon/carbon.h>
#include <AGL/agl.h>
#undef GL_EXT_texture_env_combine
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#ifdef __LINUX__
#include <GL/glx.h>
#include <X11/Xatom.h>
#include <X11/Xmu/StdCmap.h>
#include <X11/keysym.h>
#include <X11/extensions/xf86vmode.h>
#endif

#include "gfx/glext.h"

#undef N_DEFINES
#define N_DEFINES nGlServer
#include "kernel/ndefdllclass.h"


//-------------------------------------------------------------------
class nInputServer;
class nProfiler;
class N_DLLCLASS nGlServer : public nGfxServer {
public:
    static nKernelServer *kernelServer;

    enum {
        N_STATICPOOLSIZE    = 8000,
        N_READONLYPOOLSIZE  = 8000,
        N_WRITEONLYPOOLSIZE = 8000,
    };

    nAutoRef<nInputServer> ref_is;
    nRef<nRoot> ref_devdir;
    
    bool display_open;
    bool in_begin_scene;
    bool in_begin;
    bool render_ok;
    bool is_fullscreen;
    int render_width;
    int render_height;
    float clear_color[4];
    int cur_vidmode_id; 
    int num_texture_units;

    enum {
        N_GLCMD_TOGGLEFULLSCREEN = 1001,
        N_GLCMD_NEXTMODE         = 1002,
        N_GLCMD_PREVMODE         = 1003,
    };

#   ifdef __WIN32__
    char wnd_classname[32];
    HINSTANCE hinst;       
    HWND  hwnd;            
    HDC   hdc;             
    HGLRC hrc;             
    HACCEL accel;

    // extension function pointers
    PFNGLCOLORTABLEEXTPROC              procColorTableEXT;
    PFNGLLOCKARRAYSEXTPROC              procLockArraysEXT;
    PFNGLUNLOCKARRAYSEXTPROC            procUnlockArraysEXT;
    PFNGLACTIVETEXTUREARBPROC           procActiveTextureARB;
    PFNGLCLIENTACTIVETEXTUREARBPROC     procClientActiveTextureARB;
    PFNGLBLENDCOLOREXTPROC              procBlendColorEXT;
    
    bool class_registered;
    bool quit_requested;
#   endif
    
#   ifdef __LINUX__
    Display *dsp;
    Window ctx_win;
    Window wm_win;
    Window fs_win;

    XFontStruct *fs;
    XEvent event;
    GLXContext cx; 
    XVisualInfo *xvis;
    Atom wmDeleteWindow;

    bool resize;
    bool focused;
    unsigned int keyboard_grabbed;

    // full screen support status
    bool ext_XF86VIDMODE;
    XF86VidModeModeInfo fs_mode;
    XF86VidModeModeInfo wm_mode;
    int wm_mode_x, wm_mode_y;
#   endif

#   ifdef __MACOSX__
    WindowRef window;
    AGLContext ctx;
    bool quit_requested;
#   endif

    // Text Support
    bool text_initialized;
    bool in_begin_text;
    int text_listbase;
    int text_numglyphs;
    float text_height;
    float text_x, text_y;

    // supported extensions
    bool ext_paletted_texture;          // EXT_paletted_texture supported?
    bool ext_compiled_vertex_array;     // EXT_compiled_vertex_array supported?
    bool ext_clip_volume_hint;          // EXT_clip_volume_hint supported?
    bool arb_multitexture;              // ARB_multitexture supported?
    bool ext_texture_env_combine;       // EXT_texture_env_combine supported?
    bool ext_texture_env_add;           // EXT_texture_env_add supported?
    bool ext_texture_env_dot3;          // EXT_texture_env_dot3 supported?
    bool ext_blend_color;               // EXT_blend_color supported?
    bool ext_bgra;                      // EXT_bgra supported?

    // misc...
    bool global_fog_enable;

    // the vertex pool manager objects for the different types
    // of vertex pools
    nVertexPoolManager *vpool_manager[N_NUM_VBTYPES];

    GLenum alpha_src;
    GLenum alpha_dst;
    GLenum stencil_fail;
    GLenum stencil_zfail;
    GLenum stencil_pass;
    GLenum stencil_func;
    GLint  stencil_ref;
    GLuint stencil_mask;
    GLint  stencil_clear;

public:
    nGlServer();
    virtual ~nGlServer();

    // inherited
    virtual bool SetDisplayMode(const char *);
    virtual bool OpenDisplay(void);
    virtual void CloseDisplay(void);
    virtual bool GetDisplayDesc(int& x0, int& y0, int& x1, int& y1);
    virtual bool GetFontDesc(int& height);
    virtual void SetViewVolume(float, float, float, float, float, float);
    virtual void SetClearColor(float, float, float, float);
    virtual void GetClearColor(float&, float&, float&, float&);
    virtual bool Screenshot(const char *);
    virtual bool ScaledScreenshot(const char *, int, int);
    virtual void SetWindowMode(nWindowMode);

    virtual bool BeginScene(void);
    virtual void EndScene(void);

    virtual void SetMatrix(nMatrixMode, matrix44&);
    virtual void PushMatrix(nMatrixMode);
    virtual void PopMatrix(nMatrixMode);

    virtual void SetState(nRState&);
    virtual bool SetLight(nLight&);
    virtual bool SetClipPlane(int, vector4&);

    virtual void Begin(nPrimType);
    virtual void Coord(float, float, float);
    virtual void Norm(float, float, float);
    virtual void Rgba(float, float, float, float);
    virtual void Uv(ulong layer, float, float);
    virtual void End(void);    

    virtual nTexture *NewTexture(const char *id);
    virtual nPixelShader *NewPixelShader(const char *id);
    virtual nVertexPool *NewVertexPool(nVertexPoolManager *, nVBufType, int, int);
    virtual nVertexBuffer *NewVertexBuffer(const char *, nVBufType vbtype, int vtype, int vnum);

    // Text-Support
    virtual bool BeginText(void);
    virtual void TextPos(float, float);
    virtual bool Text(const char *);
    virtual bool EndText(void);

    /// make mouse cursor visible
    virtual void ShowCursor();
    /// make mouse cursor invisible
    virtual void HideCursor();

    virtual bool Trigger(void);

    // private
    void _reshapeDisplay(long w, long h);
    void initGl(void);
    void initExtensions(void);
    bool hasExtension(const char *);

    bool winOpen(void);
    bool winClose(void);
    void invalidate_context(void);
    bool dispOpen(bool fullscreen, int w, int h, int bpp);
    bool dispClose(void);
    GLenum getGlFilterMode(nRStateParam);

    char *nextDisplayMode(char *, int);
    char *prevDisplayMode(char *, int);

#ifdef __WIN32__
    void enumDisplayModes(void);
    bool classRegister(void);
    bool classUnregister(void);
#endif

#ifdef __LINUX__
    void enumDisplayModes(void);
    bool GetVideoMode (XF86VidModeModeInfo &, int w, int h);
    bool SwitchVideoMode (XF86VidModeModeInfo *, Window, int, int, int, int, bool);
    bool HotKeyVideoMode (KeySym &ks);
    bool GrabInputs ();
    void UngrabInputs ();
    bool xwin_proc(void);
#endif    
};
//-------------------------------------------------------------------
#endif    

 
