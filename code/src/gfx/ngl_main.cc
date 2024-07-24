#define N_IMPLEMENTS nGlServer
//-------------------------------------------------------------------
//  ngl_main.cc -- Main-Modul fuer nGlServer
//
//  (C) 1998 Andre Weissflog
//-------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/npersistserver.h"
#include "kernel/nenv.h"
#include "gfx/nglserver.h"
#include "gfx/ntexture.h"
#include "gfx/nvertexpool.h"
#include "gfx/nglvertexpool.h"
#include "gfx/nvertexbuffer.h"

#if defined(__MACOSX__)
#include <Carbon/carbon.h>
#endif

nNebulaClass(nGlServer, "ngfxserver");

//-------------------------------------------------------------------
//  nGlServer()
//  13-Nov-98   floh    created
//  08-Jan-99   floh    aufgeraeumt
//  13-May-99   floh    erzeugt jetzt auch unter X sofort das Fenster
//  18-Jul-99   floh    Objekt-Referenzen eingefuehrt
//  23-Dec-99   floh    + Win32: this->accel
//  08-Sep-00   floh    + vertex pool manager array to zero
//  31-Jul-01   floh    + stencil buffer stuff
//-------------------------------------------------------------------
nGlServer::nGlServer()
         : ref_is(kernelServer,this), ref_devdir(this)
{
    this->display_open   = false;
    this->in_begin_scene = false;
    this->in_begin       = false;
    this->render_ok      = false;
    this->is_fullscreen  = false;
    this->num_texture_units = 1;

    this->ref_is = "/sys/servers/input";
    this->ref_devdir = kernelServer->New("nroot","/sys/share/display/0");
    
    this->text_initialized = false;
    this->in_begin_text    = false;
    this->text_listbase    = 0;
    this->text_numglyphs   = 0;
    this->text_height      = 0.0f;
    this->text_x           = 0.0f;
    this->text_y           = 0.0f;
    
    this->ext_paletted_texture      = false;
    this->ext_compiled_vertex_array = false; 
    this->ext_clip_volume_hint      = false;
    this->arb_multitexture          = false;
    this->ext_texture_env_combine   = false;
    this->ext_texture_env_add       = false;
    this->ext_texture_env_dot3      = false;
    this->ext_blend_color           = false;
    this->ext_bgra                  = false;

    this->global_fog_enable     = false;

    this->alpha_src     = GL_ONE;
    this->alpha_dst     = GL_ONE;
    this->stencil_fail  = GL_KEEP;
    this->stencil_zfail = GL_KEEP;
    this->stencil_pass  = GL_KEEP;
    this->stencil_func  = GL_ALWAYS;
    this->stencil_ref   = 0;
    this->stencil_mask  = 0xffffffff;
    this->stencil_clear = 0;

    memset(this->clear_color,0,sizeof(this->clear_color));
    this->cur_vidmode_id = 0;

#   ifdef __WIN32__
    nEnv *env;
    this->hinst = GetModuleHandle(NULL);
    this->hwnd  = NULL;
    this->hdc   = NULL;
    this->hrc   = NULL;
    this->procColorTableEXT             = NULL;
    this->procLockArraysEXT             = NULL;
    this->procUnlockArraysEXT           = NULL;
    this->procActiveTextureARB          = NULL;
    this->procClientActiveTextureARB    = NULL;
    this->procBlendColorEXT             = NULL;

    // definiere Accelerators:
    //  Alt-Enter -> toggle fullscreen/windowed
    //  Alt-Plus  -> naechst hoehere Aufloesung
    //  Alt-Minus -> naechst niedrigere Aufloesung
    ACCEL acc[3];
    acc[0].fVirt = FALT|FNOINVERT|FVIRTKEY;
    acc[0].key   = VK_RETURN;
    acc[0].cmd   = N_GLCMD_TOGGLEFULLSCREEN;

    acc[1].fVirt = FALT|FNOINVERT|FVIRTKEY;
    acc[1].key   = VK_ADD;
    acc[1].cmd   = N_GLCMD_NEXTMODE;

    acc[2].fVirt = FALT|FNOINVERT|FVIRTKEY;
    acc[2].key   = VK_SUBTRACT;
    acc[2].cmd   = N_GLCMD_PREVMODE;
    this->accel = CreateAcceleratorTable(acc,3);
    
    this->class_registered = false;
    this->quit_requested = false;
    this->classRegister();
    this->winOpen();
    this->enumDisplayModes();
    if ((env = (nEnv *) kernelServer->New("nenv","/sys/env/hwnd")))  env->SetI((int)this->hwnd);

#   elif defined(__LINUX__)
    this->dsp = NULL;
    this->ctx_win = 0;
    this->wm_win = 0;
    this->fs_win = 0; 
    this->resize = false;
    this->keyboard_grabbed = false;
    this->fs  = NULL;
    memset(&(this->event),0,sizeof(this->event));
    memset(&(this->cx),0,sizeof(this->cx));
    memset(&(this->wmDeleteWindow),0,sizeof(this->wmDeleteWindow));
    this->winOpen();
    this->enumDisplayModes();

#   elif defined(__MACOSX__)
    this->winOpen();
#   endif

    memset(this->vpool_manager,0,sizeof(this->vpool_manager));

    // initialize projection matrix
    this->SetViewVolume(this->clip_minx, this->clip_maxx,
                        this->clip_miny, this->clip_maxy,
                        this->clip_minz, this->clip_maxz);

    // Default-Display-Mode
    this->SetDisplayMode("dev(0)-type(win)-w(200)-h(200)");
}

//-------------------------------------------------------------------
//  ~nGlServer()
//  13-Nov-98   floh    created
//  13-May-99   floh    killt jetzt auch unter X das Fenster
//  18-Jul-99   floh    raeumt "/sys/share/display" korrekt weg
//  21-Jul-99   floh    CloseDisplay()
//  27-Jul-99   floh    + bevor der OpenGL-Kontext ungueltig wird,
//                      muessen die Textur- und VBuffer-Objekte
//                      gekillt werden!
//  23-Dec-99   floh    + this->accel
//  23-Aug-00   floh    + release pixel shaders (this must happen
//                        before the OpenGL context is destroyed!
//  05-Sep-00   floh    + delete meshes
//-------------------------------------------------------------------
nGlServer::~nGlServer()
{
    n_assert(!this->in_begin_scene);
    n_assert(!this->in_begin);

    if (this->ref_texdir.isvalid())      this->ref_texdir->Release();
    if (this->ref_pshaderdir.isvalid())  this->ref_pshaderdir->Release();
    if (this->ref_vxbufdir.isvalid())    this->ref_vxbufdir->Release();
    if (this->ref_ixbufdir.isvalid())    this->ref_ixbufdir->Release();

    int i;
    for (i=0; i<N_NUM_VBTYPES; i++) {
        if (this->vpool_manager[i]) {
            n_delete this->vpool_manager[i];
            this->vpool_manager[i] = NULL;
        }
    }
    this->CloseDisplay();
#   ifdef __LINUX__
    if (! this->is_fullscreen)
#   endif
        this->winClose();
#   ifdef __WIN32__
    this->classUnregister();
    if (this->accel) DestroyAcceleratorTable(this->accel);
#   endif
    if (this->ref_devdir.isvalid()) this->ref_devdir->Release();
}

//-------------------------------------------------------------------
//  SetClearColor()
//  12-Aug-99   floh    created
//-------------------------------------------------------------------
void nGlServer::SetClearColor(float r, float g, float b, float a)
{
    this->clear_color[0] = r;
    this->clear_color[1] = g;
    this->clear_color[2] = b;
    this->clear_color[3] = a;
}

//-------------------------------------------------------------------
//  GetClearColor()
//  12-Aug-99   floh    created
//-------------------------------------------------------------------
void nGlServer::GetClearColor(float& r, float& g, float& b, float& a)
{
    r = this->clear_color[0];
    g = this->clear_color[1];
    b = this->clear_color[2];
    a = this->clear_color[3];
}

//-------------------------------------------------------------------
//  SetViewVolume()
//  05-May-99   floh    created
//  04-Jun-99   floh    + interne Projection-Matrix des GfxServers
//                        wird geupdated...
//-------------------------------------------------------------------
void nGlServer::SetViewVolume(float minx, float maxx, float miny, float maxy,
                              float minz, float maxz)
{
    nGfxServer::SetViewVolume(minx,maxx,miny,maxy,minz,maxz);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(minx,maxx,miny,maxy,minz,maxz);
    glMatrixMode(GL_MODELVIEW);

    // update Projection-Matrix des Gfx-Servers...
    matrix44 m;
    glGetFloatv(GL_PROJECTION_MATRIX,(float *)&m);
    nGfxServer::SetMatrix(N_MXM_PROJECTION,m);
}

//-------------------------------------------------------------------
//  NewTexture()
//  17-Feb-99   floh    created
//  16-Jul-99   floh    + ref_texdir
//  28-Sep-00   floh    + PushCwd()/PopCwd()
//-------------------------------------------------------------------
nTexture *nGlServer::NewTexture(const char *id)
{
    nTexture *t;
    char resid[N_MAXNAMELEN];
    this->getResourceID(id,resid,sizeof(resid));
    kernelServer->PushCwd(this->ref_texdir.get());
    t = (nTexture *) kernelServer->New("ngltexture",resid);
    kernelServer->PopCwd();
    return t;
}

//-------------------------------------------------------------------
//  NewPixelShader()
//  23-Aug-00   floh    created
//  28-Sep-00   floh    + PushCwd()/PopCwd()
//-------------------------------------------------------------------
nPixelShader *nGlServer::NewPixelShader(const char *id)
{
    nPixelShader *ps;
    char resid[N_MAXNAMELEN];
    this->getResourceID(id,resid,sizeof(resid));
    kernelServer->PushCwd(this->ref_pshaderdir.get());
    ps = (nPixelShader *) kernelServer->New("nglpixelshader",resid);
    kernelServer->PopCwd();
    return ps;
}

//-------------------------------------------------------------------
//  NewVertexPool()
//  08-Sep-00   floh    created
//-------------------------------------------------------------------
nVertexPool *nGlServer::NewVertexPool(nVertexPoolManager *vpm,
                                      nVBufType vbtype,
                                      int vtype,
                                      int size)
{
    // readonly vertex buffer are never rendered and don't require
    // the overhead of wrapping a gl vertex array around it
    nVertexPool *vp = NULL;
    if (N_VBTYPE_READONLY == vbtype) {
        vp = (nVertexPool *) n_new nVertexPool(this,vpm,vbtype,vtype,size);
    } else {
        vp = (nVertexPool *) n_new nGlVertexPool(this,vpm,vbtype,vtype,size);
    }
    vp->Initialize();
    return vp;
}

//-------------------------------------------------------------------
//  NewVertexBuffer()
//  08-Sep-00   floh    created
//  27-Sep-00   floh    + handle WRITEONLY buffer size
//  28-Sep-00   floh    + PushCwd()/PopCwd()
//-------------------------------------------------------------------
nVertexBuffer *nGlServer::NewVertexBuffer(const char *id,
                                      nVBufType vbuf_type,
                                      int vertex_type,
                                      int num_v)
{
    nVertexBuffer *vb;
    char resid[N_MAXNAMELEN];
    this->getResourceID(id,resid,sizeof(resid));
    kernelServer->PushCwd(this->ref_vxbufdir.get());

    // create a new vertex buffer object
    vb = (nVertexBuffer *) kernelServer->New("nvertexbuffer",resid);
    n_assert(vb);
    kernelServer->PopCwd();

    // create a vertex bubble object and initialize vertex buffer object
    n_assert((vbuf_type>=0) && (vbuf_type<N_NUM_VBTYPES));
    
    nVertexPoolManager *vpm = this->vpool_manager[vbuf_type];
    n_assert(vpm);

    // special case:
    if (vbuf_type==N_VBTYPE_WRITEONLY) {
        // D3D handles double buffering for us if we specify
        // DISCARDCONTENTS, so we just return the maximal 
        // size for the vertex buffer
        num_v = vpm->GetPreferedPoolSize();
    }

    nVertexBubble *vbubble = vpm->NewVertexBubble(vertex_type,num_v);
    n_assert(vbubble);

    vb->Initialize(this,vbubble);
    return vb;
}

//-------------------------------------------------------------------
//  GetDisplayDesc()
//  22-Feb-99   floh    created
//-------------------------------------------------------------------
bool nGlServer::GetDisplayDesc(int& x0, int& y0, int& x1, int& y1)
{
    x0 = 0;
    x1 = this->render_width;
    y0 = 0;
    y1 = this->render_height;
    return true;
}

//-------------------------------------------------------------------
//  GetFontDesc()
//  22-Feb-99   floh    created
//-------------------------------------------------------------------
bool nGlServer::GetFontDesc(int& height)
{
    height = (int) this->text_height;
    return true;
}

//-------------------------------------------------------------------
/**
    Check given display mode against database under
    '/sys/share/display/0/modes', if no matching mode is
    found, select a similar mode.

    24-Dec-99   floh    created
                        + oops, hier wurde ein alter Mode-String
                          hochgegeben...
*/
//-------------------------------------------------------------------
bool nGlServer::SetDisplayMode(const char *mode)
{
    bool retval;

    // let superclass desect mode string
    if (retval = nGfxServer::SetDisplayMode(mode)) {

        int w,h,bpp;
        bool needs_change = false;
        nRoot *cwd;
        nRoot *mode_dir;

        w   = this->disp_w;
        h   = this->disp_h;
        bpp = this->disp_bpp;
        
        // everything is only relevant for fullscreen
        if (strcmp("full",this->disp_type)!=0) return true;

        // traverse display database for the requested mode,
        // if none is found, adjust w,h,bpp to a close match
        cwd = kernelServer->GetCwd();
        mode_dir = this->ref_devdir->Find("modes");
        if (mode_dir) {
            nRoot *act_mode;
            bool direct_hit = false;
            int best_w   = 640;
            int best_h   = 480;
            int best_bpp = 16;
            int min_dist = 1<<30;
            for (act_mode = mode_dir->GetHead();
                 act_mode;
                 act_mode = act_mode->GetSucc())
            {
                nEnv *env;
                int mw = 0;
                int mh = 0;
                int mbpp = 0;
                int dist;
                if ((env = (nEnv *) act_mode->Find("w")))   mw = env->GetI();
                if ((env = (nEnv *) act_mode->Find("h")))   mh = env->GetI();
                if ((env = (nEnv *) act_mode->Find("bpp"))) mbpp = env->GetI();
                // direct hit?
                if ((mw == w) && (mh == h) && (mbpp == bpp)) {
                    this->cur_vidmode_id = atoi(act_mode->GetName());
                    direct_hit = true;
                    break;
                }
                // none, compute distance to requested mode
                dist = abs(mw-w) + abs(mh-h) + abs(mbpp-bpp);
                if (dist < min_dist) {
                    min_dist = dist;
                    best_w   = mw;
                    best_h   = mh;
                    best_bpp = mbpp;
                    this->cur_vidmode_id = atoi(act_mode->GetName());
                }
            }
            // if no direct hit, adjust with "best fit" parameters
            if (!direct_hit) {
                w   = best_w;
                h   = best_h;
                bpp = best_bpp;
                needs_change = true;
            }
        }
        if (needs_change) {
            char mode_str[256];
            sprintf(mode_str,"dev(0)-type(full)-w(%d)-h(%d)-bpp(%d)",w,h,bpp);
            retval = nGfxServer::SetDisplayMode(mode_str);
        }
    }
    return retval;
}

//-------------------------------------------------------------------
/**
    24-Dec-99   floh    taken over from nd3dserver
    28-Sep-00   floh    + PushCwd()/PopCwd()
    01-Aug-01   samuel  now supported in linux
                        (transferred from ngl_windisp.cc)
*/
//-------------------------------------------------------------------
char *nGlServer::nextDisplayMode(char *buf, int buf_size)
{
    char *retval = NULL;
    nRoot *mode_dir;
    mode_dir = this->ref_devdir->Find("modes");
    if (mode_dir) {
        char mode_name[128];
        nEnv *env;
        sprintf(mode_name,"%d/name",this->cur_vidmode_id+1);
        kernelServer->PushCwd(mode_dir);
        env = (nEnv *) kernelServer->Lookup(mode_name);
        if (!env) {
            sprintf(mode_name,"%d/name",this->cur_vidmode_id);
            env = (nEnv *) kernelServer->Lookup("0/name");
        }
        if ((env = (nEnv *) kernelServer->Lookup(mode_name))) {
            strncpy(buf,env->GetS(),buf_size);
            buf[buf_size-1] = 0;
            retval = buf; 
        }
        kernelServer->PopCwd();
    }
    return retval;
}

//-------------------------------------------------------------------
/**
    24-Dec-99   floh    taken over from nd3dserver
    28-Sep-00   floh    + PushCwd()/PopCwd()
    01-Aug-01   samuel  now supported in linux 
                        (transferred from ngl_windisp.cc)
*/
//-------------------------------------------------------------------
char *nGlServer::prevDisplayMode(char *buf, int buf_size)
{
    char *retval = NULL;
    nRoot *mode_dir;
    mode_dir = this->ref_devdir->Find("modes");
    if (mode_dir) {
        char mode_name[128];
        nEnv *env;
        sprintf(mode_name,"%d/name",this->cur_vidmode_id-1);
        kernelServer->PushCwd(mode_dir);
        env = (nEnv *) kernelServer->Lookup(mode_name);
        if (!env) {
            sprintf(mode_name,"%d/name",this->cur_vidmode_id);
            env = (nEnv *) kernelServer->Lookup("0/name");
        }
        if ((env = (nEnv *) kernelServer->Lookup(mode_name))) {
            strncpy(buf,env->GetS(),buf_size);
            buf[buf_size-1] = 0;
            retval = buf; 
        }
        kernelServer->PopCwd();
    }
    return retval;
}

//-------------------------------------------------------------------
/**
    13-Nov-98   floh    created
    16-Mar-99   floh    + InputServer wird jetzt bei jedem OpenDisplay
                          neu ermittelt
    08-Sep-00   floh    + create vertex pool managers
*/
//-------------------------------------------------------------------
bool nGlServer::OpenDisplay(void)
{
    n_assert(!this->in_begin_scene);
    n_assert(!this->in_begin);

    // display already open?
    if (this->display_open) {
        n_warn("nGlServer: Display already open!");
        return FALSE;
    }

    // initialize width, height
    bool fullscreen;
    int w   = this->disp_w;
    int h   = this->disp_h;
    int bpp = this->disp_bpp;
    if (strcmp((const char *)&(this->disp_type),"full")==0) fullscreen=true;
    else                                                    fullscreen=false;
    this->dispOpen(fullscreen,w,h,bpp);

    // create vertex pool managers
    this->vpool_manager[N_VBTYPE_READONLY] = n_new nVertexPoolManager(this,N_VBTYPE_READONLY,N_READONLYPOOLSIZE);
    this->vpool_manager[N_VBTYPE_WRITEONLY] = n_new nVertexPoolManager(this,N_VBTYPE_WRITEONLY,N_WRITEONLYPOOLSIZE);
    this->vpool_manager[N_VBTYPE_STATIC] = n_new nVertexPoolManager(this,N_VBTYPE_STATIC,N_STATICPOOLSIZE);

    this->render_ok = true;
    return true;
}

//-------------------------------------------------------------------
/**
    13-Nov-98   floh    created
    16-Mar-99   floh    + InputServer-Pointer wird invalidiert
    08-Sep-00   floh    + destroy vertex pool managers
*/
//-------------------------------------------------------------------
void nGlServer::CloseDisplay(void)
{
    n_assert(!this->in_begin_scene);
    n_assert(!this->in_begin);
    if (this->display_open) this->dispClose();
}

//------------------------------------------------------------------------------
/**
    Activate, hide, or show the app window. nGlServer currently only
    implements activate.

    @param  wm  nWindowMode into which the app window should be switched
*/
void 
nGlServer::SetWindowMode(nWindowMode wm)
{
#ifdef __WIN32__
    if (this->hwnd)
    {
        switch (wm)
        {
            case N_WINDOWMODE_ACTIVE:
                SetForegroundWindow(this->hwnd);
                break;
            default:
                break;
        }
    }
#else
    // FIXME: handle X11 stuff
    // quiet "unused variable" warning
    wm = wm; 
#endif
}

//-------------------------------------------------------------------
/**
    Take a screenshot and save it with the given filename.

    @param fname  filename to save the screenshot as.
    
    17-Dec-99   floh    created
*/
//-------------------------------------------------------------------
bool nGlServer::Screenshot(const char *fname)
{
    n_assert(this->display_open);
    n_assert(!this->in_begin_scene);
    n_assert(!this->in_begin);

    // prepare gl to read pixels from back buffer
    glReadBuffer(GL_FRONT);
    glFinish();

    // create bmp file object for saving...
    nBmpFile bmp;
    bmp.SetWidth(this->render_width);
    bmp.SetHeight(this->render_height);
    if (bmp.Open(fname,"wb")) {
        int y;
        uchar *buf;
        int buf_size = 3*this->render_width+4;
        buf = (uchar *) n_malloc(buf_size);
        for (y=0; y<this->render_height; y++) {
            glReadPixels(0,y,
                         this->render_width,1,
                         GL_RGB,
                         GL_UNSIGNED_BYTE,
                         buf);
            // swap red and blue
            int i;
            uchar *p = buf;
            for (i=0; i<this->render_width; i++) {
                uchar c = p[2];
                p[2] = p[0];
                p[0] = c;
                p += 3;
            }
            bmp.WriteLine(buf);
        }
        n_free(buf);
        bmp.Close();
        return true;
    }
    return false;
}

//-------------------------------------------------------------------
/**
    Take a screenshot and save it with the given filename and
    with the specified height and width.

    @param fname  Filename to save the screenshot as.
    @param w      Width of the saved screenshot.
    @param h      Height of the saved screenshot.
    
    17-Dec-99   floh    created
*/
//-------------------------------------------------------------------
bool nGlServer::ScaledScreenshot(const char *fname, int w, int h)
{
    n_assert(this->display_open);
    n_assert(!this->in_begin_scene);
    n_assert(!this->in_begin);

    // prepare gl to read pixels from back buffer
    glReadBuffer(GL_FRONT);
    glFinish();

    // create bmp file object for saving...
    nBmpFile bmp;
    bmp.SetWidth(w);
    bmp.SetHeight(h);
    if (bmp.Open(fname,"wb")) {
        // fixed point scalers...
        int y_shift = 0;
        int y_add   = ((1<<16)*this->render_height) / h;
        int x_add   = ((1<<16)*this->render_width) / w;
        int y;
        uchar *unscaled_buf = (uchar *) n_malloc(3*this->render_width+4);
        uchar *scaled_buf = (uchar *) n_malloc(3*w+4);
        for (y=0; y<h; y++) {
            int x_shift = 0;
            int x;
            glReadPixels(0,(y_shift>>16),this->render_width,1,
                         GL_RGB,GL_UNSIGNED_BYTE,
                         &(unscaled_buf[(x_shift>>16)*3]));
            for (x=0; x<w; x++) {
                memcpy(&(scaled_buf[x*3]),&(unscaled_buf[(x_shift>>16)*3]),3);
                x_shift += x_add;
            }
            y_shift += y_add;

            // swap red and blue
            int i;
            uchar *p = scaled_buf;
            for (i=0; i<w; i++) {
                uchar c = p[2];
                p[2] = p[0];
                p[0] = c;
                p += 3;
            }
            bmp.WriteLine(scaled_buf);
        }
        n_free(unscaled_buf);
        n_free(scaled_buf);
        bmp.Close();
        return true;
    }
    return false;
}

//-------------------------------------------------------------------
//  Trigger()
//  20-Jul-99   floh    created
//  21-Jul-99   floh    Message-Loop umstrukturiert. Bisher wurde
//                      nach einem GetMessage() mit Ergebnis WM_QUIT
//                      sofort abgebrochen, jetzt wird zuerst
//                      sichergestellt, dass die Message-Loop
//                      wirklich leer ist!
//  24-Dec-99   floh    + support for accelerators
//-------------------------------------------------------------------
bool nGlServer::Trigger(void)
{
    #ifdef __WIN32__
        
        // alle anstehenden WMs abhandeln, und zurueck
        MSG msg;
        while (PeekMessage(&msg,NULL,0,0,PM_NOREMOVE)) {
            if (GetMessage(&msg,NULL,0,0)) {
                int msg_handled = false;
                if (this->hwnd && this->accel) {
                    msg_handled = TranslateAccelerator(this->hwnd,this->accel,&msg);
                }
                if (!msg_handled) {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
        }
        return (!this->quit_requested);

    #elif defined(__LINUX__)
        return this->xwin_proc();
    #elif defined(__MACOSX__)
        EventTargetRef theTarget = GetEventDispatcherTarget();
        EventRef theEvent = 0;
        ReceiveNextEvent( 0, NULL, kEventDurationSecond / 10000, true, &theEvent );
        while ( theEvent != 0 ) {
            SendEventToEventTarget( theEvent, theTarget );
            ReleaseEvent( theEvent );
            ReceiveNextEvent( 0, NULL, kEventDurationSecond / 10000, true,
                              &theEvent );
        }
        return !this->quit_requested;
    #else
        #error nGlServer::Trigger() not implemented
    #endif
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
