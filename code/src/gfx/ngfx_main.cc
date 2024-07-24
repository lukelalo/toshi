#define N_IMPLEMENTS nGfxServer
//-------------------------------------------------------------------
//  ngfx_main.cc
//
//  (C) 1998 Andre Weissflog
//-------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "kernel/nkernelserver.h"
#include "kernel/nenv.h"
#include "kernel/ntimeserver.h"
#include "gfx/ngfxserver.h"
#include "gfx/nbmpfile.h"
#include "gfx/nvertexpool.h"
#include "gfx/nvertexbuffer.h"
#include "gfx/nindexbuffer.h"

nNebulaScriptClass(nGfxServer, "nroot");

//-------------------------------------------------------------------
//  nGfxServer()
//  13-Nov-98   floh    created
//  27-Nov-98   floh    + neues Subdir, "mat"
//  14-Dec-98   floh    + neues Subdir, "hnd"
//  08-Jan-99   floh    + bissel aufgeraeumt.
//  16-Feb-99   floh    + neue Hints...
//  06-Jun-99   floh    + disp_dev ist jetzt ein String...
//  06-Jun-99   floh    + das Hint-System wurde gekillt
//  20-Jun-99   floh    + initialisiert das Viewvolume jetzt
//                        "richtig" per SetViewVolume(), damit 
//                        Subklassen ihre Matrizen korrekt updaten
//                        koennen -> FUCKMIST C++ BLOEDES! Offensichtlich
//                        ist die vtable noch nicht inialisiert, so
//                        dass kein SetViewVolume() in den Subklassen
//                        aufgerufen wird!
//  16-Jul-99   floh    + TexDir und VBufDir per nRef
//  25-Aug-99   floh    + Glow-States werden initialisiert
//  29-Feb-00   floh    + removed glow states
//  28-Apr-00   floh    + statistics infrastructure
//  21-Aug-00   floh    + Pixel shader stuff
//  01-Sep-00   floh    + initialize vertex pool manager
//                      + initialize mesh resource dir
//  14-Sep-00   floh    + initialize current_pixelshader and
//                        current_vertexbubble
//  18-Sep-00   floh    + new nIndexBuffer handling
//  16-Nov-00   floh    + current_texture handling
//  24-Jan-01   floh    + major cleanup session
//  28-Jan-02   floh    + mouse cursor init
//  17-Oct-02   vadim   + SetPerspective/GetPerspective/SetFOV/GetFOV
//-------------------------------------------------------------------
nGfxServer::nGfxServer()
           : ref_texdir(this), 
             ref_pshaderdir(this),
             ref_vxbufdir(this),
             ref_ixbufdir(this),
             stats_triangles(kernelServer,this,"gfx_triangles"),
             stats_framerate(kernelServer,this,"gfx_framerate"),
             stats_modelviews(kernelServer,this,"gfx_modelviews"),
             stats_lights(kernelServer,this,"gfx_lights"),
             stats_textures(kernelServer,this,"gfx_textures"),    
             stats_texturesize(kernelServer,this,"gfx_texturesize"),
             stats_pshader_switches(kernelServer,this,"gfx_pshader_switches"),
             stats_mesh_switches(kernelServer,this,"gfx_mesh_switches"),
             stats_texture_switches(kernelServer,this,"gfx_texture_switches")
{
    memset(this->disp_mode,0,sizeof(this->disp_mode));
    memset(this->disp_type,0,sizeof(this->disp_type));
    memset(this->disp_dev,0,sizeof(this->disp_dev));
    this->disp_w   = 0;
    this->disp_h   = 0;
    this->disp_bpp = 0;
    this->disp_zbufbits = 0;
    this->disp_sbufbits = 0;
    this->ref_texdir     = kernelServer->New("nroot",N_GFX_TEXDIR);
    this->ref_pshaderdir = kernelServer->New("nroot",N_GFX_PSHADERDIR);
    this->ref_vxbufdir   = kernelServer->New("nroot",N_GFX_VXBUFDIR);
    this->ref_ixbufdir   = kernelServer->New("nroot",N_GFX_IXBUFDIR);
    this->unique_id = 0;
    this->timeStamp = 0;
    this->current_vertexbubble = NULL;
    this->current_pixelshader  = NULL;
    this->cursorShown   = true;
    this->numCursors    = 0;
    this->currentCursor = 0;

    memset(this->current_texture,0,sizeof(this->current_texture));
    this->fov = -1.0f;  // force recomputation
    this->SetViewVolume(-0.1f,+0.1f,-0.1f,+0.1f,+0.1f,+25000.0f);
}

//-------------------------------------------------------------------
//  ~nGfxServer()
//  13-Nov-98   floh    created
//  08-Jan-99   floh    + die Shared-Resource-Dirs werden jetzt
//                        released
//  06-Feb-99   floh    + Shared-Resource-Dirs werden nicht
//                        freigegeben...
//  16-Jul-99   floh    + da die Shared-Resource-Dirs jetzt Refs
//                        sind, koennen sie auch problemlos gekillt
//                        werden, Ende einer Odysee ...
//  27-Jul-99   floh    + prueft nach, dass die Subklassen auch
//                        korrekt das Textur- und VBuffer-Dir weggeraeumt
//                        haben!
//  23-Aug-00   floh    + resource dirs are released if they are still
//                        valid
//  01-Sep-00   floh    + release vertex pool managers
//                      + release mesh resource dir
//  24-Jan-01   floh    + major cleanup session
//-------------------------------------------------------------------
nGfxServer::~nGfxServer()
{
    if (this->ref_texdir.isvalid())     this->ref_texdir->Release();
    if (this->ref_pshaderdir.isvalid()) this->ref_pshaderdir->Release();
    if (this->ref_vxbufdir.isvalid())   this->ref_vxbufdir->Release();
    if (this->ref_ixbufdir.isvalid())   this->ref_ixbufdir->Release();
}

//-------------------------------------------------------------------
/**
   @brief Release all texture, pixelshader, vertexbuffer and index
   buffer objects.

    - 02-Feb-01   floh    created
*/
//-------------------------------------------------------------------
void nGfxServer::releaseResources()
{
    nRoot *o;
    if (this->ref_texdir.isvalid()) 
    {
        while ((o = this->ref_texdir->GetHead())) o->Release();
    }
    if (this->ref_pshaderdir.isvalid()) 
    {
        while ((o = this->ref_pshaderdir->GetHead())) o->Release();
    }
    if (this->ref_vxbufdir.isvalid()) 
    {
        while ((o = this->ref_vxbufdir->GetHead())) o->Release();
    }
    if (this->ref_ixbufdir.isvalid()) 
    {
        while ((o = this->ref_ixbufdir->GetHead())) o->Release();
    }
}

//-------------------------------------------------------------------
/**
    @brief Setup the view volume.

    WARNING: If you intend to make use of GetPerspective,
    SetFOV, GetFOV, please ensure that MinX == -MaxX and
    MinY == -MaxY. If you break these rules you're likely
    to get bogus values from the aformentioned methods.

    @param  minx    the left x coord where view volume cuts near plane
    @param  maxx    the right x coord where view volume cuts near plane
    @param  miny    the upper y coord where view volume cuts near plane
    @param  maxy    the lower y coord where view volume cuts near plane
    @param  minz    distance from eye to near plane of view volume
    @param  maxz    distance from eye to far plane of view volume

     - 05-May-99   floh    created
     - 17-Oct-02   vadim   + invalidate field of view
*/
//-------------------------------------------------------------------
void nGfxServer::SetViewVolume(float minx, float maxx, float miny, float maxy,
                               float minz, float maxz)
{
    this->clip_minx = minx;
    this->clip_maxx = maxx;
    this->clip_miny = miny;
    this->clip_maxy = maxy;
    this->clip_minz = minz;
    this->clip_maxz = maxz;
    this->fov = -1.0f;
}

//-------------------------------------------------------------------
/**
    @brief Get the view volume.

     - 05-May-99   floh    created
*/
//-------------------------------------------------------------------
void nGfxServer::GetViewVolume(float& minx, float& maxx, float& miny, float& maxy,
                               float& minz, float& maxz)
{
    minx = this->clip_minx;
    maxx = this->clip_maxx;
    miny = this->clip_miny;
    maxy = this->clip_maxy;
    minz = this->clip_minz;
    maxz = this->clip_maxz;
}

//-------------------------------------------------------------------
//  SetPerspective()
//  16-Oct-02   vadim    created
//-------------------------------------------------------------------
void nGfxServer::SetPerspective(float fovDegrees, float aspectRatio, 
                                float zNear, float zFar)
{
  // fovDegrees is the vertical field of view
  if ((fovDegrees <= 0.0f) || (fovDegrees >= 180.0f))
    fovDegrees = 90.0f;  // set to default if value out of range
  float top = zNear * n_tan(n_deg2rad(fovDegrees * 0.5f));
  float right = aspectRatio * top;
  // setup the frustum
  this->SetViewVolume(-right, right, -top, top, zNear, zFar);
  this->fov = fovDegrees;
}

//-------------------------------------------------------------------
//  GetPerspective()
//  16-Oct-02   vadim    created
//-------------------------------------------------------------------
void nGfxServer::GetPerspective(float& fovDegrees, float& aspectRatio, 
                                float& zNear, float& zFar)
{
  fovDegrees = this->GetFOV();
  aspectRatio = this->clip_maxx / this->clip_maxy;
  zNear = this->clip_minz;
  zFar = this->clip_maxz;
}

//-------------------------------------------------------------------
//  SetFOV()
//  17-Oct-02   vadim    created
//-------------------------------------------------------------------
void nGfxServer::SetFOV(float fovDegrees)
{
  SetPerspective(fovDegrees, this->clip_maxx / this->clip_maxy,
                 this->clip_minz, this->clip_maxz);
}

//-------------------------------------------------------------------
//  GetFOV()
//  17-Oct-02   vadim    created
//-------------------------------------------------------------------
float nGfxServer::GetFOV()
{
  if (this->fov < 0.0f) // recompute if needed
    this->fov = n_rad2deg(n_atan(this->clip_maxy / this->clip_minz)) * 2.0f;
  return this->fov;
}

//-------------------------------------------------------------------
/**
    @brief Set the specified matrix.

    @param mm   The matrix mode can be one of:
                 - N_MXM_MODELVIEW
                 - N_MXM_VIEWER
                 - N_MXM_PROJECTION

     - 14-Dec-98   floh    schreibt jetzt die aktuelle Modelview-
                            und Viewer-Matrix mit
     - 04-Jun-99   floh    + Projection-Matrix
     - 01-Aug-99   floh    + invertierte Modelview-Matrix
     - 29-Feb-00   floh    + inverted view matrix
     - 28-Apr-00   floh    + statistics: Modelview-Changes
     - 02-May-00   floh    + InvModelView raus...
     - 27-Jun-00   floh    + converted to matrix44
*/
//-------------------------------------------------------------------
void nGfxServer::SetMatrix(nMatrixMode mm, matrix44& mx)
{
    switch(mm) {
        case N_MXM_MODELVIEW:
            this->modelview = mx; 
            this->SetStats(N_GFXSTATS_MODELVIEWS,1);
            break;
        case N_MXM_VIEWER:
            this->view = mx;
            this->inv_view = this->view;
            this->inv_view.invert_simple();
            break;
        case N_MXM_PROJECTION:
            this->projection = mx;
            break;
        default: break;
    }        
}

//-------------------------------------------------------------------
/**
    @brief Get the specified matrix.

    @param mm   The matrix mode can be one of:
                 - N_MXM_MODELVIEW
                 - N_MXM_VIEWER
                 - N_MXM_PROJECTION

     - 02-Jun-99   floh    created
     - 04-Jun-99   floh    + Projection-Matrix
     - 28-Jan-00   floh    + InvModelView
     - 29-Feb-00   floh    + InvView
     - 27-Jun-00   floh    + converted to matrix44
*/
//-------------------------------------------------------------------
void nGfxServer::GetMatrix(nMatrixMode mm, matrix44& mx)
{
    switch(mm) {
        case N_MXM_MODELVIEW:       mx = this->modelview; break;
        case N_MXM_VIEWER:          mx = this->view; break;
        case N_MXM_PROJECTION:      mx = this->projection; break;
        case N_MXM_INVVIEWER:       mx = this->inv_view; break;
        default: break;
    }
}

//-------------------------------------------------------------------
/**
    @brief Push the current matrix for the specified matrix mode.

    Each matrix mode maintains separate stacks.

    @param mm   The matrix mode can be one of:
                 - N_MXM_MODELVIEW
                 - N_MXM_VIEWER
                 - N_MXM_PROJECTION

     - 02-May-00   floh    created
*/
//-------------------------------------------------------------------
void nGfxServer::PushMatrix(nMatrixMode mm)
{
    switch (mm) {
        case N_MXM_MODELVIEW:   
            this->modelview_stack.Push(this->modelview); 
            break;
        case N_MXM_VIEWER:
            this->view_stack.Push(this->view);
            this->invview_stack.Push(this->inv_view);
            break;
        case N_MXM_PROJECTION:
            this->proj_stack.Push(this->projection);
            break;
        default:
            break;
    }
}

//-------------------------------------------------------------------
/**
    @brief Pop the current matrix off of the stack for the specified
    matrix mode and make it the current matrix.

    @param mm   The matrix mode can be one of:
                 - N_MXM_MODELVIEW
                 - N_MXM_VIEWER
                 - N_MXM_PROJECTION

     - 02-May-00   floh    created
*/
//-------------------------------------------------------------------
void nGfxServer::PopMatrix(nMatrixMode mm)
{
    switch (mm) {
        case N_MXM_MODELVIEW:
            this->modelview = this->modelview_stack.Pop();
            break;
        case N_MXM_VIEWER:
            this->view = this->view_stack.Pop();
            this->inv_view = this->invview_stack.Pop();
            break;
        case N_MXM_PROJECTION:
            this->projection = this->proj_stack.Pop();
            break;
        default: break;
    }
}

//-------------------------------------------------------------------
/**
    @brief Return the color format used by this graphics server.

     - 19-Oct-00   floh    created
*/
//-------------------------------------------------------------------
nColorFormat nGfxServer::GetColorFormat(void)
{
    return N_COLOR_RGBA;
}

//-------------------------------------------------------------------
//  FindTexture()
//  17-Feb-99   floh    created
//  16-Jul-99   floh    + ref_texdir
//  28-Sep-00   floh    + PushCwd()/PopCwd()
//-------------------------------------------------------------------
nTexture *nGfxServer::FindTexture(const char *id)
{
    nRoot *t;
    char resid[N_MAXNAMELEN];
    this->getResourceID(id,resid,sizeof(resid));
    kernelServer->PushCwd(this->ref_texdir.get());
    t = kernelServer->Lookup(resid);
    if (t) t->AddRef();
    kernelServer->PopCwd();
    return (nTexture *) t;
}

//-------------------------------------------------------------------
//  FindPixelShader()
//  21-Aug-00   floh    created
//  28-Sep-00   floh    + PushCwd()/PopCwd()
//-------------------------------------------------------------------
nPixelShader *nGfxServer::FindPixelShader(const char *id)
{
    nRoot *ps;
    char resid[N_MAXNAMELEN];
    this->getResourceID(id,resid,sizeof(resid));
    kernelServer->PushCwd(this->ref_pshaderdir.get());
    ps = kernelServer->Lookup(resid);
    if (ps) ps->AddRef();
    kernelServer->PopCwd();
    return (nPixelShader *) ps;
}

//-------------------------------------------------------------------
//  FindVertexBuffer()
//  01-Sep-00   floh    created
//  28-Sep-00   floh    + PushCwd()/PopCwd()
//-------------------------------------------------------------------
nVertexBuffer *nGfxServer::FindVertexBuffer(const char *id)
{
    nRoot *vb;
    char resid[N_MAXNAMELEN];
    this->getResourceID(id,resid,sizeof(resid));
    kernelServer->PushCwd(this->ref_vxbufdir.get());
    vb = kernelServer->Lookup(resid);
    if (vb) vb->AddRef();
    kernelServer->PopCwd();
    return (nVertexBuffer *) vb;
}

//-------------------------------------------------------------------
//  FindIndexBuffer()
//  01-Sep-00   floh    created
//  28-Sep-00   floh    + PushCwd()/PopCwd()
//-------------------------------------------------------------------
nIndexBuffer *nGfxServer::FindIndexBuffer(const char *id)
{
    nRoot *ib;
    char resid[N_MAXNAMELEN];
    this->getResourceID(id,resid,sizeof(resid));
    kernelServer->PushCwd(this->ref_ixbufdir.get());
    ib = kernelServer->Lookup(resid);
    if (ib) ib->AddRef();
    kernelServer->PopCwd();
    return (nIndexBuffer *) ib;
}

//-------------------------------------------------------------------
//  NewVertexPool()
//  Create a new generic vertex pool object. Subclasses should
//  return API-specific vertex pool objects. Delete the vertex
//  pool through the delete operator when done with it.
//  This method is only called back by the vertex pool manager
//  classes if they need to create new vertex pools.
//  01-Sep-00   floh    created
//-------------------------------------------------------------------
nVertexPool *nGfxServer::NewVertexPool(nVertexPoolManager *, nVBufType, int, int)
{
    n_error("Pure virtual function called!\n");
    return NULL;
}

//-------------------------------------------------------------------
//  NewVertexBuffer()
//  Create a new vertex buffer object (vertex buffer objects are
//  shared resources).
//  01-Sep-00   floh    created
//-------------------------------------------------------------------
nVertexBuffer *nGfxServer::NewVertexBuffer(const char *, nVBufType, int, int)
{
    n_error("Pure virtual function called!\n");
    return NULL;
}

//-------------------------------------------------------------------
//  NewIndexBuffer()
//  Create a new empty index buffer object.
//  01-Sep-00   floh    created
//  28-Sep-00   floh    + PushCwd()/PopCwd()
//-------------------------------------------------------------------
nIndexBuffer *nGfxServer::NewIndexBuffer(const char *id)
{
    nIndexBuffer *ib;
    char resid[N_MAXNAMELEN];
    this->getResourceID(id,resid,sizeof(resid));
    kernelServer->PushCwd(this->ref_ixbufdir.get());
    ib = (nIndexBuffer *) kernelServer->New("nindexbuffer",resid);
    n_assert(ib);
    kernelServer->PopCwd();
    return ib;
}

//-------------------------------------------------------------------
/**
    @brief Take a screenshot.

    The screenshot will be taken and saved to a BMP file.

    @param fname   The name of the file to store the screenshot in.

     - 17-Dec-99   floh    created
*/
//-------------------------------------------------------------------
bool nGfxServer::Screenshot(const char *fname)
{
    nBmpFile bmp;
    bmp.SetWidth(this->disp_w);
    bmp.SetHeight(this->disp_h);
    if (bmp.Open(fname,"wb")) {
        int y;
        uchar *buf;
        int buf_size = 3*this->disp_w;
        buf = (uchar *) n_malloc(buf_size);
        for (y=0; y<this->disp_h; y++) {
            memset(buf,(uchar)(y&255),buf_size);
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
    @brief Take a screenshot, scaled to a different size.

    @param fname   The name of the file to store the screenshot in.
    @param w    The new width for the BMP.
    @param h    The new height for the BMP.

     - 22-Mar-00   floh    created
*/
//-------------------------------------------------------------------
bool nGfxServer::ScaledScreenshot(const char *fname, int w, int h)
{
    nBmpFile bmp;
    bmp.SetWidth(w);
    bmp.SetHeight(h);
    if (bmp.Open(fname,"wb")) {
        int y;
        uchar *buf;
        int buf_size = 3*w;
        buf = (uchar *) n_malloc(buf_size);
        for (y=0; y<h; y++) {
            memset(buf,(uchar)((y&15)<<4),buf_size);
            bmp.WriteLine(buf);
        }
        n_free(buf);
        bmp.Close();
        return true;
    }
    return false;
}

//--------------------------------------------------------------------
/**
*/
bool
nGfxServer::SetLight(nLight& l)
{
    if (this->numLights < N_MAXLIGHTS)
    {
        this->lights[this->numLights++] = l;
        return true;
    }
    return false;
}

//--------------------------------------------------------------------
/**
*/
int
nGfxServer::GetNumLights()
{
    return this->numLights;
}

//--------------------------------------------------------------------
/**
*/
const nLight&
nGfxServer::GetLight(int num)
{
    n_assert(num < this->numLights);
    return this->lights[num];
}

//-------------------------------------------------------------------
/**
    Set the background clear color which is used to fill the back buffer
    at the start of a new frame.

    @param r    red component
    @param g    green component
    @param b    blue component
    @param a    alpha component (usually ignored)

     - 12-Aug-99   floh    created
*/
//-------------------------------------------------------------------
void nGfxServer::SetClearColor(float, float, float, float)
{
    n_error("Pure virtual function called.");
}

//-------------------------------------------------------------------
/**
    @brief Return the background clear color used to fill the back
    buffer at the start of each frame.

     - 12-Aug-99   floh    created
*/
//-------------------------------------------------------------------
void nGfxServer::GetClearColor(float&, float&, float&, float&)
{
    n_error("Pure virtual function called.");
}

//-------------------------------------------------------------------
//  NewTexture()
//  17-Feb-99   floh    created
//-------------------------------------------------------------------
nTexture *nGfxServer::NewTexture(const char *)
{
    n_error("Pure virtual function called.");
    return NULL;
}

//-------------------------------------------------------------------
//  NewPixelShader()
//  21-Aug-00   floh    created
//-------------------------------------------------------------------
nPixelShader *nGfxServer::NewPixelShader(const char *)
{
    n_error("Pure virtual function called.");
    return NULL;
}

//-------------------------------------------------------------------
//  getResourceID()
//
//  Ermittelt eine ResourceID fuer den Pfad, das ist entweder
//  ein "einzigartiger" String, wenn der Pfad NULL ist,
//  oder einfach die letzten 'buf_size' Buchstaben des uebergebenen
//  Pfades (dieser sollte bereits "verabsolutiert" worden sein).
//  Alle ".", "/", "\" und ":" werden in "_" konvertiert.
//
//  08-Jan-99   floh    created
//-------------------------------------------------------------------
char *nGfxServer::getResourceID(const char *name, char *buf, ulong buf_size)
{
    if (!name) sprintf(buf,"res%d",(int)this->unique_id++);
    else {

        // kopiere die letzten 32 Buchstaben nach buf
        char c;
        char *str;

        int len = strlen(name)+1;
        int off = len - buf_size;
        if (off < 0) off = 0;   // String ist kuerzer als BufSize
        len -= off;
        strcpy(buf,&(name[off]));

        // ersetze alle illegalen Zeichen
        str = buf;
        while ((c = *str)) {
            if ((c=='.')||(c=='/')||(c=='\\')||(c==':')) *str='_';
            str++;
        }
    }
    return buf;
}

//-------------------------------------------------------------------
//  BeginScene()
//  Initialize per-frame-statistics.
//  28-Apr-00   floh    created
//-------------------------------------------------------------------
bool nGfxServer::BeginScene(void)
{
    this->BeginStats();

    // reset lighting
    this->numLights = 0;

    // flush the current pixel shader and mesh buffer
    this->SetCurrentPixelShader(NULL);
    this->SetCurrentVertexBubble(NULL);
    int i;
    for (i=0; i<N_MAXNUM_TEXSTAGES; i++) {
        this->SetCurrentTexture(i,NULL);
    };
    return true;
}

//-------------------------------------------------------------------
//  EndScene()
//  Finish per-frame-statistics.
//  28-Apr-00   floh    created
//  23-Jul-01   floh    + frame rate calculation
//-------------------------------------------------------------------
void nGfxServer::EndScene(void)
{
    double curTime = kernelServer->ts->GetTime();
    double frameTime = curTime - this->timeStamp;
    if (frameTime > 0.0)
    {
        float fps = float(1.0 / frameTime);
        this->SetStats(N_GFXSTATS_FRAMERATE, (int) fps);
    }
    this->timeStamp = curTime;
    this->EndStats();
}

//-------------------------------------------------------------------
//  BeginStats()
//  Reset statistics objects.
//  28-Apr-00   floh    created
//-------------------------------------------------------------------
void nGfxServer::BeginStats(void)
{
    this->stats_triangles.Reset();
    this->stats_framerate.Reset();
    this->stats_modelviews.Reset();
    this->stats_lights.Reset();
    this->stats_textures.Reset();
    this->stats_texturesize.Reset();
    this->stats_pshader_switches.Reset();
    this->stats_mesh_switches.Reset();
    this->stats_texture_switches.Reset();
}

//-------------------------------------------------------------------
//  SetStats()
//  Update statistic object.
//  28-Apr-00   floh    created
//-------------------------------------------------------------------
void nGfxServer::SetStats(nGfxStatType t, int v)
{
    switch (t) {
        case N_GFXSTATS_TRIANGLES:              this->stats_triangles.AddVal(v); break;
        case N_GFXSTATS_FRAMERATE:              this->stats_framerate.SetVal(v); break;
        case N_GFXSTATS_MODELVIEWS:             this->stats_modelviews.AddVal(v); break;
        case N_GFXSTATS_LIGHTS:                 this->stats_lights.AddVal(v); break;
        case N_GFXSTATS_TEXTURES:               this->stats_textures.AddVal(v); break;
        case N_GFXSTATS_TEXTURESIZE:            this->stats_texturesize.AddVal(v); break;
        case N_GFXSTATS_PSHADER_SWITCHES:       this->stats_pshader_switches.AddVal(v); break;
        case N_GFXSTATS_MESH_SWITCHES:          this->stats_mesh_switches.AddVal(v); break;
        case N_GFXSTATS_TEXTURE_SWITCHES:       this->stats_texture_switches.AddVal(v); break;
    };
}

//-------------------------------------------------------------------
//  EndStats()
//  Write statistics to watcher variables.
//  28-Apr-00   floh    created
//-------------------------------------------------------------------
void nGfxServer::EndStats(void)
{
    this->stats_triangles.Flush();
    this->stats_framerate.Flush();
    this->stats_modelviews.Flush();
    this->stats_lights.Flush();
    this->stats_textures.Flush();
    this->stats_texturesize.Flush();
    this->stats_pshader_switches.Flush();
    this->stats_mesh_switches.Flush();
    this->stats_texture_switches.Flush();
}

//-------------------------------------------------------------------
bool nGfxServer::Trigger(void)
{
    n_error("Pure virtual function called.");
    return false;
}
//-------------------------------------------------------------------
void nGfxServer::ShowDisplay(void)
{
}
//-------------------------------------------------------------------
bool nGfxServer::OpenDisplay(void)
{
    n_error("Pure virtual function called.");
    return false;
}
//-------------------------------------------------------------------
void nGfxServer::CloseDisplay(void)
{
    n_error("Pure virtual function called.");
}
//-------------------------------------------------------------------
void nGfxServer::SetState(nRState&)
{
    n_error("Pure virtual function called.");
}
//-------------------------------------------------------------------
void nGfxServer::Begin(nPrimType)
{ }
//-------------------------------------------------------------------
void nGfxServer::Coord(float, float, float)
{ }
//-------------------------------------------------------------------
void nGfxServer::Norm(float, float, float)
{ }
//-------------------------------------------------------------------
void nGfxServer::Rgba(float, float, float, float)
{ }
//-------------------------------------------------------------------
void nGfxServer::Uv(ulong, float, float)
{ }
//-------------------------------------------------------------------
void nGfxServer::End(void)
{ }
//-------------------------------------------------------------------
/**
    Set model space coefficients of a user defined clip plane. The
    coefficients take the form of the general plane equation
    Ax + By + Cz + Dw = 0. A point [xyzw] is visible if
    Ax + By + Cz + Dw >= 0.

    @param index    index of clip plane (0..N)
    @param plane    ABCD coefficients of clip plane in model space
*/
bool nGfxServer::SetClipPlane(int, vector4&)
{
    n_error("Pure virtual function called.");
    return false;
}
//-------------------------------------------------------------------
bool nGfxServer::BeginText(void)
{
    n_error("Pure virtual function called.");
    return false;
}
//-------------------------------------------------------------------
void nGfxServer::TextPos(float, float)
{
    n_error("Pure virtual function called.");
}
//-------------------------------------------------------------------
bool nGfxServer::Text(const char *)
{
    n_error("Pure virtual function called.");
    return false;
}
//-------------------------------------------------------------------
bool nGfxServer::EndText(void)
{
    n_error("Pure virtual function called.");
    return false;
}
//-------------------------------------------------------------------
bool nGfxServer::GetDisplayDesc(int&, int&, int&, int&)
{
    n_error("Pure virtual function called.");
    return false;
}
//-------------------------------------------------------------------
bool nGfxServer::GetFontDesc(int&)
{
    n_error("Pure virtual function called.");
    return false;
}
//-------------------------------------------------------------------
void nGfxServer::SetWindowMode(nWindowMode)
{
    // empty
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
