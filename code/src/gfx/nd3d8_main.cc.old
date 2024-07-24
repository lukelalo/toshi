#define N_IMPLEMENTS nD3D8Server
//-----------------------------------------------------------------------------
//  nd3d8_main.cc
//  (C) 2001 Andre Weissflog
//-----------------------------------------------------------------------------
#include "gfx/nd3d8server.h"
#include "il/il.h"
#include "il/ilu.h"

nNebulaClass(nD3D8Server, "ngfxserver");

extern const char *nd3d8_Error(HRESULT hr);

//-----------------------------------------------------------------------------
/**
    Initializes nD3D8Server object, registers window class, opens
    app window, registers window accelerator keys, opens d3d,
    enumerates d3d capable adapters.

    history:
    - 23-Jan-2001   floh    created
    - 03-Mar-2001   floh    didn't write window handle to /sys/env/hwnd
    - 12-May-2001   floh    initialize OpenIL and OpenILU
*/
nD3D8Server::nD3D8Server()
:   ref_is(kernelServer,this),
    hInst(0),
    hWnd(0),
    hAccel(0),
    d3d8(0),
    d3d8Dev(0),
    d3dFont(0),
    numAdapters(0),
    selAdapter(0),
    refSelAdapter(this),
    refSelMode(this),
    renderWidth(0),
    renderHeight(0),
    classRegistered(false),
    windowOpen(false),
    displayOpen(false),
    windowMinimized(false),
    windowFullscreen(false),
    inBeginScene(false),
    quitRequested(false),
    queryAdaptersCalled(false),
    selectAdapterCalled(false),
    useWbuffer(false),
    d3dClearColor(0),
    globalFogEnable(false),
    clipPlaneMask(0),
    stencilClear(0),
    textXPos(0.0f),
    textYPos(0.0f),
    systemCursorShown(true),
    d3dCursorShown(false),
    fullscreenStyle(WS_POPUP | WS_SYSMENU | WS_VISIBLE),
    windowedStyle(WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VISIBLE),
    childStyle(WS_CHILD | WS_TABSTOP),
    parent_hWnd(0)
{
    // initialize refs to other Nebula objects
    this->ref_is = "/sys/servers/input";

    // initialize instance handle
    this->hInst = GetModuleHandle(0);

    memset(&this->presentParams,    0, sizeof(this->presentParams));
    memset(&this->devCaps,          0, sizeof(this->devCaps));
    memset(&this->vpoolManager,     0, sizeof(this->vpoolManager));
    memset(&this->d3dWorldMatrix,   0, sizeof(this->d3dWorldMatrix));
    memset(&this->d3dProjMatrix,    0, sizeof(this->d3dProjMatrix));

    // initialize accelerator keys
    ACCEL acc[3];
    acc[0].fVirt = FALT|FNOINVERT|FVIRTKEY;
    acc[0].key   = VK_RETURN;
    acc[0].cmd   = N_D3DCMD_TOGGLEFULLSCREEN;
    acc[1].fVirt = FALT|FNOINVERT|FVIRTKEY;
    acc[1].key   = VK_ADD;
    acc[1].cmd   = N_D3DCMD_NEXTMODE;
    acc[2].fVirt = FALT|FNOINVERT|FVIRTKEY;
    acc[2].key   = VK_SUBTRACT;
    acc[2].cmd   = N_D3DCMD_PREVMODE;
    this->hAccel = CreateAcceleratorTable(acc,3);

    // register window class and open window
    this->classRegister();
    this->winOpen();
    this->d3dOpen();

    // create device database
    this->queryAdapters();

    // set the default adapter and display mode
    nD3D8Server::SetDisplayMode("dev(0)-w(320)-h(200)-zbuf(16)");

    // publish window handle
    nEnv *env;
    if ((env = (nEnv *) kernelServer->New("nenv","/sys/env/hwnd"))) 
    {
        env->SetI((int)this->hWnd);
    }

    // initialize IL and ILU
    ilInit();
    iluInit();
}

//-----------------------------------------------------------------------------
/**
    Shut down everything and destroy object.

    history:
    - 23-Jan-2001   floh    created
*/
nD3D8Server::~nD3D8Server()
{
    this->CloseDisplay();
    this->d3dClose();
    this->winClose();
    this->classUnregister();
    
    if (this->hAccel) 
    {
        DestroyAcceleratorTable(this->hAccel);
        this->hAccel = 0;
    }
    n_assert(NULL == this->vpoolManager[N_VBTYPE_STATIC]);
    n_assert(NULL == this->vpoolManager[N_VBTYPE_READONLY]);
    n_assert(NULL == this->vpoolManager[N_VBTYPE_WRITEONLY]);
}

//-----------------------------------------------------------------------------
/**
    Get a locked surface as a copy of the front buffer.

    @param  surfDesc    [out] will be filled with surface description
    @param  surfRect    [out] will be filled with pointer to surf data and pitch
    @return             pointer to surface object

    09-Aug-01   floh    created
*/
IDirect3DSurface8*
nD3D8Server::LockFrontBuffer(D3DSURFACE_DESC& surfDesc, D3DLOCKED_RECT& surfRect)
{
    n_assert(this->d3d8Dev);
    HRESULT hr;

    // get the current display mode description
    D3DDISPLAYMODE dispMode;
    hr = this->d3d8Dev->GetDisplayMode(&dispMode);
    if (FAILED(hr))
    {
        n_printf("nD3D8Server::Screenshot(): GetDisplayMode() failed with '%s'!\n", nd3d8_Error(hr));
        return 0;
    }

    // create a system memory surface as a clone of the front buffer
    IDirect3DSurface8* surf;
    hr = this->d3d8Dev->CreateImageSurface(dispMode.Width, dispMode.Height, D3DFMT_A8R8G8B8, &surf);
    if (FAILED(hr))
    {
        n_printf("nD3D8Server::Screenshot(): CreateImageSurface() failed with '%s'!\n", nd3d8_Error(hr));
        return 0;
    }
    n_assert(surf);

    // copy front buffer to sysmem surface
    hr = this->d3d8Dev->GetFrontBuffer(surf);
    if (FAILED(hr))
    {
        n_printf("nD3D8Server::Screenshot(): GetFrontBuffer() failed with '%s'!\n", nd3d8_Error(hr));
        surf->Release();
        return 0;
    }

    // get surface description of sysmem buffer
    hr = surf->GetDesc(&surfDesc);
    if (FAILED(hr))
    {
        n_printf("nD3D8Server::Screenshot(): GetDesc() failed with '%s'!\n", nd3d8_Error(hr));
        surf->Release();
        return 0;
    }

    // lock the sysmem surface
    hr = surf->LockRect(&surfRect, NULL, D3DLOCK_READONLY);
    if (FAILED(hr))
    {
        n_printf("nD3D8Server::Screenshot(): LockRect() failed with '%s'!\n", nd3d8_Error(hr));
        surf->Release();
        return 0;
    }
    return surf;
}

//-----------------------------------------------------------------------------
/**
    Unlock and release the front buffer copy obtained by LockFrontBuffer().

    @param  surf    pointer to surface object as returned by LockFrontBuffer()

    09-Aug-01   floh    created
*/
void
nD3D8Server::UnlockFrontBuffer(IDirect3DSurface8* surf)
{
    n_assert(surf);
    surf->UnlockRect();
    surf->Release();
}

//-----------------------------------------------------------------------------
/**
    Create a screen shot.
    
    09-Aug-01   floh    created
*/
bool
nD3D8Server::Screenshot(const char* fileName)
{
    n_assert(fileName);

    IDirect3DSurface8* surf;
    D3DSURFACE_DESC surfDesc;
    D3DLOCKED_RECT surfRect;
    if (surf = this->LockFrontBuffer(surfDesc, surfRect))
    {   
        // create a bmp file and write contents of sysmem surface
        nBmpFile bmpFile;
        int width  = surfDesc.Width;
        int height = surfDesc.Height;
        bmpFile.SetWidth(width);
        bmpFile.SetHeight(height);
        if (bmpFile.Open(fileName, "wb"))
        {
            // define the pixel format conversion
            nPixelFormat srcFormat(32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
            nPixelFormat *dstFormat = bmpFile.GetPixelFormat();
            if (!dstFormat->BeginConv(&srcFormat))
            {
                n_printf("nD3D8Server::Screenshot(): invalid pixel format conversion!\n");
                bmpFile.Close();
                this->UnlockFrontBuffer(surf);
                return false;
            }

            // get src/dst pointers and pitches
            uchar* srcPtr  = (uchar*) surfRect.pBits;
            int srcPitch   = surfRect.Pitch;
            int lineSize   = ((dstFormat->bpp / 8) * width) + 4;
            uchar* linePtr = (uchar*) n_malloc(lineSize);
            int y;
            srcPtr += height * srcPitch;
            for (y = 0; y < height; y++)
            {
                srcPtr -= srcPitch;
                dstFormat->Conv(srcPtr, linePtr, width);
                bmpFile.WriteLine(linePtr);
            }

            // cleanup
            n_free(linePtr);
            linePtr = 0;
            dstFormat->EndConv();        
            bmpFile.Close();
        }
        else
        {
            n_printf("nD3D8Server::Screenshot(): Failed to open file '%s'!\n", fileName);
            this->UnlockFrontBuffer(surf);
            return false;
        }
    }
    else
    {
        n_printf("nD3D8Server::LockFrontBuffer() failed!\n");
        return false;
    }
    this->UnlockFrontBuffer(surf);
    return true;
}

//-----------------------------------------------------------------------------
/**
    Create a scaled screen shot.
    
    09-Aug-01   floh    created
*/
bool
nD3D8Server::ScaledScreenshot(const char *fileName, int dstWidth, int dstHeight)
{
    n_assert(fileName);

    IDirect3DSurface8* surf;
    D3DSURFACE_DESC surfDesc;
    D3DLOCKED_RECT surfRect;
    if (surf = this->LockFrontBuffer(surfDesc, surfRect))
    {   
        // create a bmp file and write contents of sysmem surface
        nBmpFile bmpFile;
        int srcWidth  = surfDesc.Width;
        int srcHeight = surfDesc.Height;
        bmpFile.SetWidth(dstWidth);
        bmpFile.SetHeight(dstHeight);
        if (bmpFile.Open(fileName, "wb"))
        {
            // define the pixel format conversion
            nPixelFormat srcFormat(32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
            nPixelFormat *dstFormat = bmpFile.GetPixelFormat();
            if (!dstFormat->BeginConv(&srcFormat))
            {
                n_printf("nD3D8Server::ScaledScreenshot(): invalid pixel format conversion!\n");
                bmpFile.Close();
                this->UnlockFrontBuffer(surf);
                return false;
            }

            // create a temp buffer in the source pixel format
            int srcBytesPerPixel = 4;
            uchar* tmpBuf = (uchar*) n_malloc((srcBytesPerPixel * dstWidth) + 8);

            // initialize src and dst pointers 
            uchar* srcPtr  = (uchar*) surfRect.pBits;
            int srcPitch   = surfRect.Pitch;
            int lineSize   = ((dstFormat->bpp / 8) * dstWidth) + 4;
            uchar* linePtr = (uchar*) n_malloc(lineSize);
            int yShift = (srcHeight<<16);
            int yAdd   = ((1<<16)*srcHeight) / dstHeight;
            int xAdd   = ((1<<16)*srcWidth) / dstWidth;
            int y;
            for (y = 0; y < dstHeight; y++) 
            {
                yShift -= yAdd;
                uchar* srcLine = srcPtr + (yShift>>16) * srcPitch;
                int xShift = 0;
                int x;
                for (x = 0; x < dstWidth; x++)
                {
                    memcpy(&(tmpBuf[x * srcBytesPerPixel]),
                           &(srcLine[(xShift>>16) * srcBytesPerPixel]),
                           srcBytesPerPixel);
                    xShift += xAdd;
                }

                // pixel-convert into target buffer and write to bmp file
                dstFormat->Conv(tmpBuf, linePtr, dstWidth);
                bmpFile.WriteLine(linePtr);
            }

            // cleaup
            n_free(linePtr);
            n_free(tmpBuf);
            linePtr = 0;
            tmpBuf = 0;
            dstFormat->EndConv();
            bmpFile.Close();

        }
        else
        {
            n_printf("nD3D8Server::ScaledScreenshot(): Failed to open file '%s'!\n", fileName);
            this->UnlockFrontBuffer(surf);
            return false;
        }
    }
    else
    {
        n_printf("nD3D8Server::LockFrontBuffer() failed!\n");
        return false;
    }
    this->UnlockFrontBuffer(surf);
    return true;
}

//-----------------------------------------------------------------------------

