#define N_IMPLEMENTS nD3D8Server
//-----------------------------------------------------------------------------
//  nd3d8_device.cc
//  (C) 2001 A.Weissflog
//-----------------------------------------------------------------------------
#include "gfx/nd3d8server.h"
#include "gfx/nd3d8texture.h"

extern const char *nd3d8_Error(HRESULT hr);

//-----------------------------------------------------------------------------
/**
    Set projection matrix according to the given view
    frustum arguments, see glFrustum() docs for an explanation.
*/
void
nD3D8Server::setView(float clip_left,
                     float clip_right,
                     float clip_bottom, float clip_top,
                     float clip_near, float clip_far)
{
    n_assert(this->d3d8Dev);
    D3DMATRIX mx;

    float x = (2.0f*clip_near)/(clip_right-clip_left);
    float y = (2.0f*clip_near)/(clip_top-clip_bottom);
    float a = (clip_right+clip_left)/(clip_right-clip_left);
    float b = (clip_top+clip_bottom)/(clip_top-clip_bottom);
    float c = -((clip_far+clip_near)/(clip_far-clip_near));
    float d = -((2.0f*clip_far*clip_near)/(clip_far-clip_near));
    float q = (clip_far / (clip_far - clip_near));

    memset(&mx,0,sizeof(mx));
    mx._11 = x;
    mx._22 = y;
    mx._13 = a;
    mx._23 = a;
    mx._33 = q;
    mx._34 = 1.0f;
    mx._43 = -q*clip_near;
    this->d3d8Dev->SetTransform(D3DTS_PROJECTION,&mx);
    memcpy(&(this->d3dProjMatrix), &mx, sizeof(D3DMATRIX));

    // compute internal matrix strictly after OpenGL norm
    matrix44 m(   x, 0.0f, a,  0.0f,
               0.0f,    y, b,  0.0f,
               0.0f, 0.0f, c, -1.0f,
               0.0f, 0.0f, d,  0.0f);
    nGfxServer::SetMatrix(N_MXM_PROJECTION, m);
}


//-----------------------------------------------------------------------------
/**
    Set proper viewport
*/
bool nD3D8Server::adjustViewport(int w, int h) {
    HRESULT hr;

    // initialize the view port
    D3DVIEWPORT8 vp;
    vp.X      = 0;
    vp.Y      = 0;
    vp.Width  = this->renderWidth = w;
    vp.Height = this->renderHeight = h;
    vp.MinZ   = 0.0f;
    vp.MaxZ   = 1.0f;
    hr = this->d3d8Dev->SetViewport(&vp);
    if (FAILED(hr))
    {
        n_printf("nD3D8Server: SetViewPort() failed with '%s'\n",nd3d8_Error(hr));
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
/**
    Define the initial state of the d3d device. Called by
    nD3D8Server::devOpen() and when resetting the d3d device.
*/
void
nD3D8Server::initDeviceStates()
{
    n_assert(this->d3d8Dev);
    n_assert(this->d3dFont);

    HRESULT hr;

    if (!this->adjustViewport(this->renderWidth, this->renderHeight)) return;

    // initialize view matrix to mirror along z,
    // because d3d uses a left hand coordinate system
    // while Nebula uses a right hand system
    D3DMATRIX identity,mx;
    memset(&identity,0,sizeof(identity));
    identity._11 = identity._22 = identity._33 = identity._44 = 1.0f;
    mx = identity;
    mx._33 = -1.0f;
    this->d3d8Dev->SetTransform(D3DTS_VIEW,&mx);

    // initialize world matrix, we keep the view matrix constant,
    // and use the world matrix as the modelview matrix under gl
    mx = identity;
    this->d3d8Dev->SetTransform(D3DTS_WORLD,&mx);

    // initialize projection matrix
    this->setView(this->clip_minx, this->clip_maxx,
                  this->clip_miny, this->clip_maxy,
                  this->clip_minz, this->clip_maxz);

    // set initial render states
    this->d3d8Dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
    this->d3d8Dev->SetRenderState(D3DRS_DITHERENABLE, TRUE);
    this->d3d8Dev->SetRenderState(D3DRS_AMBIENT, 0xff808080);

    if (this->useWbuffer) 
    {
        HRESULT hr = this->d3d8Dev->SetRenderState(D3DRS_ZENABLE, D3DZB_USEW);
        if (FAILED(hr)) 
        {
            n_printf("nD3D8Server: SetRenderState for W buffer failed. Fallback to Z buffer.\n");
            this->d3d8Dev->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
        }
    }

    // fill the device caps
    hr = this->d3d8Dev->GetDeviceCaps(&(this->devCaps));
    if (FAILED(hr))
    {
        n_printf("nD3D8Server: GetDeviceCaps() failed with '%s'\n", nd3d8_Error(hr));
        return;
    }

    // restore the text renderer
    this->d3dFont->InvalidateDeviceObjects();
    this->d3dFont->RestoreDeviceObjects();

    // re-load cursor images
    this->LoadCursors();
    if (this->cursorShown)
    {
        this->ShowCursor();
    }
    else
    {
        this->HideCursor();
    }
}

//-----------------------------------------------------------------------------
/**
    Make back buffer visible.
*/
void
nD3D8Server::present()
{
    n_assert(this->d3d8Dev);
    HRESULT hr = this->d3d8Dev->Present(NULL, NULL, NULL, NULL);
    if (FAILED(hr))
    {
        n_printf("nD3D8Server: Present() failed with '%s'\n",nd3d8_Error(hr));
        return;
    }
}

//-----------------------------------------------------------------------------
/**
    Check if the device has been lost and if yes attempt to restore it.
    If the device can not be restored at this time, the routine will
    sleep for a while and return false.
*/
bool
nD3D8Server::testResetDevice()
{
    n_assert(this->d3d8Dev);

    HRESULT hr;
    hr = this->d3d8Dev->TestCooperativeLevel();
    if (D3DERR_DEVICENOTRESET == hr)
    {        
        // If we are windowed, read the desktop mode and use the same format for
        // the back buffer
        
        if( !this->windowFullscreen )
        {
            D3DDISPLAYMODE currentMode;
            hr = this->d3d8->GetAdapterDisplayMode(this->selAdapter, &currentMode);
            
            this->presentParams.BackBufferFormat =  currentMode.Format;
        }

        if (this->d3dFont)
            this->d3dFont->InvalidateDeviceObjects();

        // if the device is ready to be reset, try to reanimate it
        
        this->releaseResources();
        
        // release all vertex buffer managers (REQUIRED before Reset() if using D3DPOOL_DEFAULT vertex buffers)
        int i;
        for (i=0; i<N_NUM_VBTYPES; i++) {
             n_delete this->vpoolManager[i];
             this->vpoolManager[i] = NULL;
        }
        
        
        hr = this->d3d8Dev->Reset(&this->presentParams);

        if (this->d3dFont)
            this->d3dFont->RestoreDeviceObjects();
        if (FAILED(hr))
        {
            n_printf("nD3D8Server: failed to call Reset() with '%s'\n",nd3d8_Error(hr));
            return false;
        }

        // (re)create the vertex pool managers
        this->vpoolManager[N_VBTYPE_READONLY]  = n_new nVertexPoolManager(this, N_VBTYPE_READONLY, N_READONLYPOOLSIZE);
        this->vpoolManager[N_VBTYPE_WRITEONLY] = n_new nVertexPoolManager(this, N_VBTYPE_WRITEONLY, N_WRITEONLYPOOLSIZE);
        this->vpoolManager[N_VBTYPE_STATIC]    = n_new nVertexPoolManager(this, N_VBTYPE_STATIC, N_STATICPOOLSIZE);
        
        // set the initial render states for the device
        this->initDeviceStates();
        n_printf("nD3D8Server: device has been reset\n");
        return true;
    }
    else if (FAILED(hr))
    {
        // device cannot be restored at this time
        // FIXME: whole game app should go into paused state?
        // could be a different return value in nGfxServer::Trigger()
        n_sleep(0.1);
        return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
/**
    Initialize the Direct3D device as requested by SetDisplayMode(), adjust
    the app window as necessary, and initialize the d3d device. Normally
    called from OpenDisplay().

    Sets:

    d3d8Dev
    windowFullscreen
    renderWidth
    renderHeight

    history:
    - 29-Jan-2001   floh    created
    - 06-Mar-2001   floh    added support for software t&l devices
    - 17-Jul-2001   floh    removed stencil from zbuffer format
*/
bool
nD3D8Server::devOpen()
{
    n_assert(this->hWnd);
    n_assert(this->d3d8);
    n_assert(!this->d3d8Dev);
    n_assert(this->windowOpen);
    n_assert(this->windowMinimized);
    n_assert(this->selectAdapterCalled);
    n_assert(this->refSelAdapter.isvalid());
    n_assert(0 == this->vpoolManager[N_VBTYPE_STATIC]);
    n_assert(0 == this->vpoolManager[N_VBTYPE_READONLY]);
    n_assert(0 == this->vpoolManager[N_VBTYPE_WRITEONLY]);

    HRESULT hr;

    // copy over the render width and render heigt values
    // but when in child mode - already set in winOpen
    if (!this->parent_hWnd) {
        this->renderWidth  = this->disp_w;
        this->renderHeight = this->disp_h;
    } 

    // set status flags
    if (0 == strcmp("full",this->disp_type))
        this->windowFullscreen = true;
    else
        this->windowFullscreen = false;

    // prepare window for change
    this->adjustWindowForChange();

    // get format specifiers for display and zbuffer
    D3DFORMAT dispFormat;
    D3DFORMAT zbufFormat;
    bool      tryWbuffer;
    if (this->windowFullscreen)
    {
        // in fullscreen mode, both formats are already available
        // in the display mode database created by queryAdapters()
        n_assert(this->refSelMode.get());
        nEnv* dispFormatEnv = (nEnv*) this->refSelMode->Find("d3d8_dsp_format");
        nEnv* zbufFormatEnv = (nEnv*) this->refSelMode->Find("d3d8_zbuf_format");
        nEnv* tryWbufferEnv = (nEnv*) this->refSelMode->Find("tryWbuffer");
        n_assert(dispFormatEnv);
        n_assert(zbufFormatEnv);
        n_assert(tryWbufferEnv);
        dispFormat = (D3DFORMAT) dispFormatEnv->GetI();
        zbufFormat = (D3DFORMAT) zbufFormatEnv->GetI();
        tryWbuffer = tryWbufferEnv->GetB();
    }
    else
    {
        // In windowed mode, the display format is defined by the
        // current display mode. For the backbuffer and stencil formats
        // we will select an appropriate format on the fly
        D3DDISPLAYMODE currentMode;
        hr = this->d3d8->GetAdapterDisplayMode(this->selAdapter, &currentMode);
        if (FAILED(hr))
        {
            n_error("nD3D8Server: GetDisplayMode() failed with '%s'!\n", nd3d8_Error(hr));
        }
        dispFormat = currentMode.Format;

        // select 24 bit z, 8 bit stencil if adapter is currently in 32 bit color,
        // otherwise 16 bit zbuffer
        if (this->bppFromFormat(dispFormat) >= 24)
        {
            zbufFormat = D3DFMT_D24S8;
        }
        else
        {
            zbufFormat = D3DFMT_D16;
        }

        // see if the display mode and zbuffer mode matches
        hr = this->d3d8->CheckDepthStencilMatch(
            this->selAdapter,
            D3DDEVTYPE_HAL,
            dispFormat,
            dispFormat,
            zbufFormat);
        if (FAILED(hr))
        {
            // if failed try a smaller zbuffer format
            zbufFormat = D3DFMT_D16;
            hr = this->d3d8->CheckDepthStencilMatch(
                this->selAdapter,
                D3DDEVTYPE_HAL,
                dispFormat,
                dispFormat,
                zbufFormat);
            // if failed again, bail out
            n_error("nD3D8Server: could not find a zbuffer format for windowed mode!\n");
            return false;
        }
    }

    if (this->parent_hWnd) {
        RECT r;
        GetClientRect(this->parent_hWnd, &r);
        
        this->renderWidth = r.right - r.left;
        this->renderHeight = r.bottom - r.top;
    }

    // get the device caps
    hr = this->d3d8->GetDeviceCaps(
        this->selAdapter,
        D3DDEVTYPE_HAL,
        &(this->devCaps));
    if (FAILED(hr))
    {
        n_printf("nD3D8Server: GetDeviceCaps() failed with '%s'!\n", nd3d8_Error(hr));
        return false;
    }

    // W buffer stuff
    // 1. try W buffer in fullscreen mode, if try was requsted for this video mode
    // 2. try W buffer in windowed mode, if zbuf bpp = 16 bit
    if ((this->windowFullscreen && tryWbuffer) ||
        (!this->windowFullscreen && zbufFormat == D3DFMT_D16))
    {
        n_printf("nD3D8Server: trying W buffer support: ");
        if (this->devCaps.RasterCaps & D3DPRASTERCAPS_WBUFFER) 
        {
            n_printf("enabling wbuffer\n");
            this->useWbuffer = true;
        } 
        else 
        {
            this->useWbuffer = false;
            n_printf("no wbuffer support\n");
        }
    } 
    else
        this->useWbuffer = false;

    // fill the present parameters structure
    this->presentParams.BackBufferWidth                  = this->renderWidth;
    this->presentParams.BackBufferHeight                 = this->renderHeight;
    this->presentParams.BackBufferFormat                 = dispFormat;
    this->presentParams.BackBufferCount                  = this->windowFullscreen ? 2 : 1;
    this->presentParams.MultiSampleType                  = D3DMULTISAMPLE_NONE;
    this->presentParams.SwapEffect                       = D3DSWAPEFFECT_DISCARD;
    this->presentParams.hDeviceWindow                    = this->hWnd;
    this->presentParams.Windowed                         = !this->windowFullscreen;
    this->presentParams.EnableAutoDepthStencil           = TRUE;
    this->presentParams.AutoDepthStencilFormat           = zbufFormat;
    this->presentParams.Flags                            = 0;
    this->presentParams.FullScreen_RefreshRateInHz       = D3DPRESENT_RATE_DEFAULT;
    this->presentParams.FullScreen_PresentationInterval  = D3DPRESENT_INTERVAL_DEFAULT;

    // hardware or software vertex processing?
    DWORD behaviourFlags = D3DCREATE_FPU_PRESERVE;
    if (this->devCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
    {
        behaviourFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
        if (this->devCaps.DevCaps & D3DDEVCAPS_PUREDEVICE)
        {
            behaviourFlags |= D3DCREATE_PUREDEVICE;
            n_printf("nD3D8Server: creating PURE HWTRANSFORMANDLIGHT device\n");
        }
        else
        {
            n_printf("nD3D8Server: creating NONPURE HWTRANSFORMANDLIGHT device\n");
        }
    }
    else
    {
        behaviourFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
        n_printf("nD3D8Server: creating SOFTWARE T&L device\n");
    }

    // create the d3d device
    hr = this->d3d8->CreateDevice(
        this->selAdapter,
        D3DDEVTYPE_HAL,
        this->hWnd,
        behaviourFlags,
        &this->presentParams,
        &(this->d3d8Dev));
    if (FAILED(hr))
    {
        n_error("nD3D8Server: CreateDevice() failed with '%s'!\n", nd3d8_Error(hr));
    }

    // create the vertex pool managers
    this->vpoolManager[N_VBTYPE_READONLY]  = n_new nVertexPoolManager(this, N_VBTYPE_READONLY, N_READONLYPOOLSIZE);
    this->vpoolManager[N_VBTYPE_WRITEONLY] = n_new nVertexPoolManager(this, N_VBTYPE_WRITEONLY, N_WRITEONLYPOOLSIZE);
    this->vpoolManager[N_VBTYPE_STATIC]    = n_new nVertexPoolManager(this, N_VBTYPE_STATIC, N_STATICPOOLSIZE);

    // initialize the text renderer
    this->initTextRenderer();

    // restore the window
    // this->ShowWindow();

    // initialize the d3d device
    this->initDeviceStates();

    return true;
}

//-----------------------------------------------------------------------------
/**
    Shut down the d3d device, and minimize the app window. Normally called
    from CloseDisplay().

    history:
    - 29-Jan-2001   floh    created
*/
bool
nD3D8Server::devClose()
{
    n_assert(this->hWnd);
    n_assert(this->d3d8);
    n_assert(this->d3d8Dev);
    n_assert(this->windowOpen);
    n_assert(!this->windowMinimized);

    // destroy all Nebula resources
    this->releaseResources();

    // release all vertex buffer managers
    int i;
    for (i=0; i<N_NUM_VBTYPES; i++) {
        if (this->vpoolManager[i]) {
            n_delete this->vpoolManager[i];
            this->vpoolManager[i] = NULL;
        }
    }

    // release any leftover textures in texture stages
    for (i=0; i<8; i++) {
        HRESULT hr = this->d3d8Dev->SetTexture(i,NULL);
    }

    // shutdown the text renderer
    this->killTextRenderer();

    // destroy the d3d device
    this->d3d8Dev->Release();
    this->d3d8Dev = 0;

    // minimize the window if not child
    if (!this->parent_hWnd)
        this->minimizeWindow();
    else 
        this->windowMinimized = true;
    return true;
}

//-----------------------------------------------------------------------------
/**
    Show the current mouse cursor as set by see nGfxServer::SetCurrentCursor().
*/
void
nD3D8Server::ShowCursor()
{
    // are there custom cursors defined?
    if (this->numCursors > 0)
    {
        // then activate the custom cursor
        n_assert(this->d3d8Dev);
        HRESULT hr;
        int xHotspot, yHotspot;
        nD3D8Texture* tex;

        // get Nebula's cursor properties
        tex = (nD3D8Texture*) this->cursor[this->currentCursor].GetTexture();
        this->cursor[this->currentCursor].GetHotspot(xHotspot, yHotspot);
        n_assert(tex);

        IDirect3DTexture8* d3dTex = tex->GetD3DTexture();
        n_assert(d3dTex);

        // get IDirect3DSurface8 interface
        IDirect3DSurface8* surf;
        hr = d3dTex->GetSurfaceLevel(0, &surf);
        if (FAILED(hr))
        {
            n_printf("nD3D8Server::ShowCursor(): GetSurfaceLevel() failed with '%s'\n", nd3d8_Error(hr));
            return;
        }

        // set cursor property
        hr = this->d3d8Dev->SetCursorProperties(xHotspot, yHotspot, surf);
        if (FAILED(hr))
        {
            n_printf("nD3D8Server::ShowCursor(): SetCursorProperties() failed with '%s'\n", nd3d8_Error(hr));
        }
        surf->Release();

        // display custom cursor
        ::SetCursor(NULL);
        this->d3d8Dev->ShowCursor(TRUE);
        this->systemCursorShown = false;
        this->d3dCursorShown = true;
    }
    else
    {
        // otherwise show system cursor
        ::SetCursor(LoadCursor(NULL, IDC_ARROW));
        this->systemCursorShown = true;
        this->d3dCursorShown = false;
    }
    nGfxServer::ShowCursor();
}

//-----------------------------------------------------------------------------
/**
    Hide the mouse cursor.
*/
void
nD3D8Server::HideCursor()
{
    if (this->windowFullscreen)
    {
        // in fullscreen mode, turn off the d3d AND the system cursor
        ::SetCursor(NULL);
        this->d3d8Dev->ShowCursor(FALSE);
        this->systemCursorShown = false;
        this->d3dCursorShown = false;
    }
    else
    {
        // in windowed mode, turn off the d3d cursor, and show the system cursor
        ::SetCursor(LoadCursor(NULL, IDC_ARROW));
        this->d3d8Dev->ShowCursor(FALSE);
        this->systemCursorShown = true;
        this->d3dCursorShown = false;
    }
    nGfxServer::HideCursor();
}

//-----------------------------------------------------------------------------
