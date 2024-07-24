#define N_IMPLEMENTS nD3D8Server
//-----------------------------------------------------------------------------
//  nd3d8_display.cc
//  (C) 2001 Andre Weissflog
//-----------------------------------------------------------------------------
#include "gfx/nd3d8server.h"

extern LONG WINAPI nd3d8_WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern const char *nd3d8_Error(HRESULT hr);

//-----------------------------------------------------------------------------
/**
    Register window class for app window, initializes icons and stuff...

    @return     return false if RegisterClass() failed

    history:
    - 23-Jan-2001   floh    created
*/
bool
nD3D8Server::classRegister()
{
    n_assert(this->hInst);
    n_assert(!this->classRegistered);

    // initialize app icon
    HICON icon;
    HMODULE dllModule = GetModuleHandle("ndirect3d8.dll");
    if (dllModule) 
        icon = LoadIcon((HINSTANCE)dllModule, "nebula_icon");
    else
        icon = LoadIcon(NULL, IDI_APPLICATION);

    // initialize wndclass structure and call RegisterClass()
    WNDCLASSEX wc;
    memset(&wc, 0, sizeof(wc));
    wc.cbSize        = sizeof(wc);
    wc.style         = CS_CLASSDC;
    wc.lpfnWndProc   = nd3d8_WinProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(void *);   // used to hold 'this' pointer
    wc.hInstance     = this->hInst;
    wc.hIcon         = icon;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) GetStockObject(NULL_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "nD3D8Server window class";
    wc.hIconSm       = NULL;
    if (RegisterClassEx(&wc)) 
    {
        this->classRegistered = true;
        return true;
    }
    else
    {
        n_error("nD3D8Server::classRegister(): RegisterClass() failed!\n");
        return false;
    }
}

//-----------------------------------------------------------------------------
/**
    Unregister the window class registered by classRegister().

    @return     true if class was unregistered successfully

    history:
    - 23-Jan-2001   floh    created
*/
bool
nD3D8Server::classUnregister()
{
    n_assert(this->hInst);
    n_assert(this->classRegistered);
    
    this->classRegistered = false;
    if (0 == UnregisterClass("nD3D8Server window class", this->hInst))
    {
        n_error("nD3D8Server::classUnregister(): UnregisterClass() failed!\n");
        return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
/**
    Open the application window, but keep it minimized. nD3D8Server keeps
    the app window open during the object's lifetime. CloseDisplay() 
    simply minimizes it, and OpenDisplay() restores it, taking any
    new display mode settings into account. Makes life easier.

    history:
    - 23-Jan-2001   floh    created
*/
bool
nD3D8Server::winOpen()
{
    n_assert(this->hInst);
    n_assert(this->classRegistered);
    n_assert(!this->windowOpen);

    // check if an environment variable named "/sys/env/parent_hwnd" exists
    nAutoRef<nEnv> parent_hwnd(this->kernelServer, 0);
    parent_hwnd = "/sys/env/parent_hwnd";
    HWND parent_window = 0;
    if(parent_hwnd.isvalid()) {
        n_printf("nD3D8Server::winOpen(): Parent window exists\n");
        this->parent_hWnd = parent_window = (HWND)parent_hwnd.get()->GetI();
    } 
    else
        n_printf("nD3D8Server::winOpen(): Parent window doesn't exist\n");
  
    if (this->parent_hWnd) {
        // we are a child, so set dimension from parent
        RECT r;
        GetClientRect(this->parent_hWnd, &r);
        this->renderWidth  = r.right - r.left;
        this->renderHeight = r.bottom - r.top;
    }
    
    this->hWnd = CreateWindow("nD3D8Server window class",       // lpClassName
                              "nD3D8Server Window",             // lpWindowName
                              parent_window != NULL ? this->childStyle : this->windowedStyle,  // dwStyle
                              0,                                // x
                              0,                                // y
                              this->renderWidth,                // nWidth
                              this->renderHeight,               // nHeight
                              parent_window,                    // hWndParent
                              NULL,                             // hMenu
                              this->hInst,                      // hInstance
                              NULL);                            // lpParam
    if (this->hWnd)
    {
        // initialize the user data field with this object's this pointer,
        // WndProc uses the user data field to get a pointer to
        // the nD3D8Server object
        SetWindowLong(this->hWnd,0,(LONG)this);
        
        // minimize the window
        if (!this->parent_hWnd)
            ShowWindow(this->hWnd, SW_SHOWMINIMIZED);
        this->windowOpen       = true;
        this->windowMinimized  = true;
        this->windowFullscreen = false;
        return true;
    }
    else
    {
        n_error("nD3D8Server::winOpen(): CreateWindow() failed!\n");
        return false;
    }
}

//-----------------------------------------------------------------------------
/**
    Close the window, if not already closed by some external force such
    as Alt-F4.
    
    @return     true if window could be closed successfully

    history:
    - 23-Jan-2001   floh    created
*/
bool
nD3D8Server::winClose()
{
    if (this->hWnd)
    {
        n_assert(this->windowOpen);
        if(!this->parent_hWnd) 
        {
            if (DestroyWindow(this->hWnd))
            {
                this->hWnd = 0;
                this->windowOpen = false;
                return true;
            }
            else
            {
                n_error("nD3D8Server::winClose(): DestroyWindow() failed!\n");
                return false;
            }
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
/**
    Adjust window attributes to prepare creating a new device. Fullscreen/
    windowed modes require different window attributes. Must be called from 
    within devOpen() before creating the d3d device.

    history:
    - 29-Jan-2001   floh    created
*/
void 
nD3D8Server::adjustWindowForChange()
{
    n_assert(this->hWnd);
    n_assert(this->windowOpen);
    n_assert(this->windowMinimized);

    if (this->parent_hWnd) {
        // adjust for child mode
        SetWindowLong(this->hWnd, GWL_STYLE, this->childStyle);
    } 
    else {
        if (this->windowFullscreen) {
            // adjust for fullscreen mode
            SetWindowLong(this->hWnd, GWL_STYLE, this->fullscreenStyle);
        }
        else {
            // adjust for windowed mode
            SetWindowLong(this->hWnd, GWL_STYLE, this->windowedStyle);
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Restore the minimized app window to the size requested by SetDisplayMode(),
    Must be called from within devOpen() after creating the d3d device.

    Sets windowMinimized to false.

    history:
    - 29-Jan-2001   floh    created
    - 05-Mar-2001   leaf    adjusted window size to match back buffer
*/
void
nD3D8Server::ShowDisplay()
{
    n_assert(this->hWnd);
    n_assert(this->windowOpen);
    n_assert(this->windowMinimized);

    // switch from minimized to fullscreen mode
    ShowWindow(this->hWnd, SW_RESTORE);

    int w, h;
    if (!this->windowFullscreen) {
        if (!this->parent_hWnd) {
            // Need to adjust adjust w & h so that the *client* area
            // is equal to renderWidth/Height.
            RECT r = {0, 0, this->renderWidth, this->renderHeight};
            AdjustWindowRect(&r, this->windowedStyle, 0);
            w = r.right - r.left;
            h = r.bottom - r.top;
        } 
        else {
            // We are child window, so get dimesions from parent
            RECT r;
            GetClientRect(this->parent_hWnd, &r);

            w = this->renderWidth = r.right - r.left;
            h = this->renderHeight = r.bottom - r.top;
        }
    }
    else {
        w = this->renderWidth;
        h = this->renderHeight;
    }

    SetWindowPos(this->hWnd,               // the window handle
                    HWND_NOTOPMOST,        // placement order
                    0,                     // x position
                    0,                     // y position
                    w,                     // adjusted width
                    h,                     // adjusted height
                    SWP_SHOWWINDOW);

    if (this->parent_hWnd) {
        // somehow SetWindowPos not works when in child mode
        MoveWindow(this->hWnd, 0, 0, w, h, 0);
    }

    this->windowMinimized = false;
	this->displayOpen = true;
}

//-----------------------------------------------------------------------------
/**
    Minimize the app window. Must be called from within devClose() after
    the d3d device has been destroyed.

    Sets windowMinimized to true.

    history:
    - 29-Jan-2001   floh    created
*/
void 
nD3D8Server::minimizeWindow()
{
    n_assert(this->hWnd);
    n_assert(this->windowOpen);
    n_assert(!this->windowMinimized);

    ShowWindow(this->hWnd, SW_MINIMIZE);
    this->windowMinimized = true;
}

//-----------------------------------------------------------------------------
/**
    Initializes Direct3D8 and validates the d3d8 member. Should
    be called after registerClass() and winOpen(). The same
    Direct3D8 object should be used for the lifetime of the
    nD3D8Server object.

    @return     true if the Direct3D8 object could be created

    history:
    - 23-Jan-2001   floh    created
*/
bool
nD3D8Server::d3dOpen()
{
    n_assert(0 == this->d3d8);
    n_assert(0 == this->d3d8Dev);
    
    this->d3d8 = Direct3DCreate8(D3D_SDK_VERSION);
    if (0 == this->d3d8)
    {
        n_error("Direct3DCreate8() failed! (DX8 not installed?)\n");
        return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
/**
    Shuts down Direct3D8. The D3D device already has to be dead (devClose()).
    Should be called right before winClose() and classUnregister().

    @return     true if the Direct3D8 object could be released

    history:
    - 23-Jan-2001   floh    created
*/
bool
nD3D8Server::d3dClose()
{
    n_assert(this->d3d8);
    n_assert(0 == this->d3d8Dev);

    int refCount = this->d3d8->Release();
    if (0 < refCount) {
        n_printf("WARNING: Direct3D8 interface was still referenced (count = %d)\n",refCount);
    }
    this->d3d8 = 0;
    return true;
}

//-----------------------------------------------------------------------------
/**
    Implements the Windows message pump. Must be called once a
    frame OUTSIDE of BeginScene() / EndScene().

    @return     false if nD3D8Server requests to shutdown Nebula

    history:
    - 23-Jan-2001   floh    created
*/
bool
nD3D8Server::Trigger()
{
    // handle all pending WM's
    MSG msg;
    if(this->parent_hWnd) {
        if (PeekMessage(&msg,NULL,WM_SIZE,WM_SIZE,PM_NOREMOVE)) {
            int w = LOWORD(msg.lParam);
            int h = HIWORD(msg.lParam);
        }
    }
    while (PeekMessage(&msg,this->hWnd,0,0,PM_NOREMOVE))
    {
        if (GetMessage(&msg,this->hWnd,0,0))
        {
            int msg_handled = false;
            if (this->hWnd && this->hAccel)
            {
                msg_handled = TranslateAccelerator(this->hWnd,this->hAccel,&msg);
            }
            if (!msg_handled)
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }
    return (!this->quitRequested);
}

//-----------------------------------------------------------------------------
/**
    Define a new display mode. The new display mode will not show up until
    the display is actually (re-)opened via CloseDisplay()/OpenDisplay().
    The display mode descriptor doesn't have to fit. nD3D8Server will
    switch to the display mode that comes closest to the 
    display mode descriptor.

    @param modeDesc the display mode descriptor string
    @return         succes indicator

    history:
    - 25-Jan-01 floh    created
*/
bool
nD3D8Server::SetDisplayMode(const char *modeDesc)
{
    bool retval;

    // let nGfxServer evaluate the display mode string
    if (retval = nGfxServer::SetDisplayMode(modeDesc)) 
    {
        int adapter,w,h,bpp,zbuf;
        bool needs_change = false;
        nRoot *cwd;
        nRoot *mode_dir;

        adapter = atoi(this->disp_dev);
        w    = this->disp_w;
        h    = this->disp_h;
        bpp  = this->disp_bpp;
        zbuf = this->disp_zbufbits;

        // select the adapter
        this->selectAdapter(adapter);
                
        // if fullscreen mode, align to closest available mode
        if (strcmp("full",this->disp_type)==0) 
        {
            // browse display data base 
            cwd = kernelServer->GetCwd();
            mode_dir = this->refSelAdapter->Find("modes");
            if (mode_dir) 
            {
                nRoot *act_mode;
                bool direct_hit = false;
                int best_w    = 640;
                int best_h    = 480;
                int best_bpp  = 16;
                int best_zbuf = 16;
                int min_dist  = 1<<30;
                for (act_mode = mode_dir->GetHead();
                     act_mode;
                     act_mode = act_mode->GetSucc())
                {
                    nEnv *env;
                    int mw    = 0;
                    int mh    = 0;
                    int mbpp  = 0;
                    int mzbuf = 0;
                    int dist;
                    if ((env = (nEnv *) act_mode->Find("w")))    mw = env->GetI();
                    if ((env = (nEnv *) act_mode->Find("h")))    mh = env->GetI();
                    if ((env = (nEnv *) act_mode->Find("bpp")))  mbpp = env->GetI();
                    if ((env = (nEnv *) act_mode->Find("zbuf"))) mzbuf = env->GetI();
                    if ((mw == w) && (mh == h) && (mbpp == bpp) && (mzbuf == zbuf)) {
                        direct_hit = true;
                        this->refSelMode = act_mode;
                        break;
                    }
                    dist = abs(mw-w) + abs(mh-h) + abs(mbpp-bpp) + abs(mzbuf-zbuf);
                    if (dist < min_dist) 
                    {
                        min_dist  = dist;
                        best_w    = mw;
                        best_h    = mh;
                        best_bpp  = mbpp;
                        best_zbuf = zbuf;
                        this->refSelMode = act_mode;
                    }
                }
                if (!direct_hit) 
                {
                    w    = best_w;
                    h    = best_h;
                    bpp  = best_bpp;
                    zbuf = best_zbuf; 
                    needs_change = true;
                }
            }
        }

        if (needs_change)
        {
            // adjust the display mode parameters in case any adjustments 
            // had to be made in this routine
            char mode_str[N_MAXPATH];
            sprintf(mode_str,"dev(%d)-type(%s)-w(%d)-h(%d)-bpp(%d)-zbuf(%d)",
                    adapter, this->disp_type, w, h, bpp, zbuf);
            retval = nGfxServer::SetDisplayMode(mode_str);
        }
    }
    return retval;
}

//-----------------------------------------------------------------------------
/**
    Open the display as defined by SetDisplayMode(). This will restore
    the app window and create a new d3d device.

    history:
    - 29-Jan-01 floh    created
*/
bool
nD3D8Server::OpenDisplay()
{
    n_assert(!this->displayOpen);
    
    this->devOpen();
    return true;
}

//-----------------------------------------------------------------------------
/**
    Close the display open by OpenDisplay(). This will minimize the
    app window and destroy the d3d device.

    history:
    - 29-Jan-01 floh    created
*/
void
nD3D8Server::CloseDisplay()
{
    if (this->displayOpen)
    {
        this->devClose();
        this->displayOpen = false;
    }
}

//-----------------------------------------------------------------------------
/**
    Select the next fullscreen display mode and return the display
    mode descriptor string. If there is no next fullscreen mode,
    return the current mode.

    @param  buf         char buffer to copy next display mode pointer to
    @param  bufSize     sizeof(buf)
    @return             pointer to buf
*/
char*
nD3D8Server::nextDisplayMode(char* buf, int bufSize)
{
    n_assert(buf);
    n_assert(bufSize > 0);

    buf[0] = 0;
    if (!this->windowFullscreen)
    {
        // if we are currently in windowed mode, return first fullscreen mode
        nRoot* adapDir = this->refSelAdapter.get();
        kernelServer->PushCwd(adapDir);
        nEnv* firstModeName = (nEnv*) kernelServer->Lookup("modes/0/name");
        kernelServer->PopCwd();
        n_assert(firstModeName);
        n_strncpy2(buf, firstModeName->GetS(), bufSize);
    }
    else
    {
        // if we are in full screen mode, return next fullscreen mode
        nRoot* curMode = this->refSelMode.get();
        nRoot* nextMode = curMode->GetSucc();
        if (!nextMode)
        {
            nextMode = curMode;
        }
        kernelServer->PushCwd(nextMode);
        nEnv* modeName = (nEnv*) kernelServer->Lookup("name");
        kernelServer->PopCwd();
        n_assert(modeName);
        n_strncpy2(buf, modeName->GetS(), bufSize);
    }
    n_printf("nextDisplayMode: %s\n", buf);
    return buf;
}

//-----------------------------------------------------------------------------
/**
    Select the previous fullscreen display mode and return the display
    mode descriptor string. If there is no previous fullscreen mode,
    return the current mode.

    @param  buf         char buffer to copy previous display mode pointer to
    @param  bufSize     sizeof(buf)
    @return             pointer to buf
*/
char*
nD3D8Server::prevDisplayMode(char* buf, int bufSize)
{
    n_assert(buf);
    n_assert(bufSize > 0);

    buf[0] = 0;
    if (!this->windowFullscreen)
    {
        // if we are currently in windowed mode, return first fullscreen mode
        nRoot* adapDir = this->refSelAdapter.get();
        kernelServer->PushCwd(adapDir);
        nEnv* firstModeName = (nEnv*) kernelServer->Lookup("modes/0/name");
        kernelServer->PopCwd();
        n_assert(firstModeName);
        n_strncpy2(buf, firstModeName->GetS(), bufSize);
    }
    else
    {
        // if we are in full screen mode, return previous fullscreen mode
        nRoot* curMode = this->refSelMode.get();
        nRoot* prevMode = curMode->GetPred();
        if (!prevMode)
        {
            prevMode = curMode;
        }
        kernelServer->PushCwd(prevMode);
        nEnv* modeName = (nEnv*) kernelServer->Lookup("name");
        kernelServer->PopCwd();
        n_assert(modeName);
        n_strncpy2(buf, modeName->GetS(), bufSize);
    }
    n_printf("prevDisplayMode: %s\n", buf);
    return buf;
}

//-----------------------------------------------------------------------------
