#ifndef N_D3D8SERVER_H
#define N_D3D8SERVER_H
/**
    Nebula gfx server on top of Direct3D8.

    Implements a gfx server on Direct3D8, this is a straight
    (and slimmified) port, no vertex or pixel shaders are supported 
    yet.

    history:
    - 23-Jan-2001   floh    created
*/

#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#ifndef N_GFXSERVER_H
#include "gfx/ngfxserver.h"
#endif

#ifndef N_DX8LINES_H
#include "gfx/nd3d8_lines.h"
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <d3d8.h>
#include <d3dx8.h>
#include "gfx/d3dfont.h"

#undef N_DEFINES
#define N_DEFINES nD3D8Server
#include "kernel/ndefdllclass.h"

//-----------------------------------------------------------------------------
class nInputServer;

class nD3D8Server : public nGfxServer
{
public:
    /// da constructor, baby
    nD3D8Server();
    /// the destructor
    virtual ~nD3D8Server();

    /// set display mode descriptor string
    virtual bool SetDisplayMode(const char* modeDesc);
    /// open display as defined by SetDisplayMode()
    virtual bool OpenDisplay();
    /// close display
    virtual void CloseDisplay();
    /// define the view volume
    virtual void SetViewVolume(float minx, float maxx, float miny, float maxy, float minz, float maxz);
    /// define the background color
    virtual void SetClearColor(float r, float g, float b, float a);
    /// return current background color
    virtual void GetClearColor(float& r, float& g, float& b, float& a);
    /// get the colorformat (N_COLOR_RGBA or N_COLOR_BGRA)
    virtual nColorFormat GetColorFormat(void);
    /// take a screenshot
    virtual bool Screenshot(const char* fileName);
    /// take a screenshot scaled to a given size
    virtual bool ScaledScreenshot(const char *fileName, int w, int h);

    /// begin rendering a frame
    virtual bool BeginScene();
    /// finish and present frame
    virtual void EndScene();

    /// set a renderstate
    virtual void SetState(nRState& rState);
    /// set one of the matrices
    virtual void SetMatrix(nMatrixMode matrixMode, matrix44& matrix);
    /// push current matrix on matrix stack
    virtual void PushMatrix(nMatrixMode matrixMode);
    /// pop matrix from stack and make current
    virtual void PopMatrix(nMatrixMode matrixMode);
    /// define a new light
    virtual bool SetLight(nLight& lightObj);
    /// define a new clip plane
    virtual bool SetClipPlane(int planeIndex, vector4& plane);

    /// create a new texture object
    virtual nTexture *NewTexture(const char* identifier);
    /// create a new pixelshader object
    virtual nPixelShader *NewPixelShader(const char* identifier);
    /// create a new index buffer
    virtual nIndexBuffer *NewIndexBuffer(const char* identifier);
    /// create a new vertex buffer object
    virtual nVertexBuffer* NewVertexBuffer(
        const char *identifier,
        nVBufType vbufType, 
        int vertexType, 
        int numVertices);
    /// create a new vertex pool object
    virtual nVertexPool *NewVertexPool(
        nVertexPoolManager* vpManager, 
        nVBufType vbType, 
        int vertexType,
        int numVertices);
    
    /// start writing text
    virtual bool BeginText();
    /// define current text position
    virtual void TextPos(float x, float y);
    /// define current text string
    virtual bool Text(const char *text);
    /// finish writing text
    virtual bool EndText();
    /// return display geometry
    virtual bool GetDisplayDesc(int& x0, int& y0, int& x1, int& y1);
    /// return current font height in pixel
    virtual bool GetFontDesc(int& height);

    /// make a mouse cursor visible
    virtual void ShowCursor();
    /// hide mouse cursor
    virtual void HideCursor();
	// Primitive drawing
    virtual void Begin(nPrimType t);
	// Primitive drawing
    virtual void Coord(float x, float y, float z);
	// Primitive drawing
    virtual void Norm(float x, float y, float z);
	// Primitive drawing
    virtual void Rgba(float r, float g, float b, float a);
	// Primitive drawing
    virtual void Uv(ulong layer, float u, float v);
	// Primitive drawing
    virtual void End(void);
	// Muestra la pantalla
    virtual void ShowDisplay();

    /// encapsulate the Windows message pump
    virtual bool Trigger();

    static nClass *local_cl;
    static nKernelServer *ks;

protected:
    int  bppFromFormat(D3DFORMAT format);
    bool checkZBufferFormat(int adapter, D3DFORMAT mode, D3DFORMAT zbuf);
    int  enumDisplayModes(int adapter);
    bool queryAdapters();
    void selectAdapter(int adapter);
    bool classRegister();
    bool classUnregister();
    bool winOpen();
    bool winClose();
    bool d3dOpen();
    bool d3dClose();
    void dispOpen();
    void dispClose();
    bool devOpen();
    bool devClose();
    void adjustWindowForChange();
    void minimizeWindow();
    void initDeviceStates();
    bool testResetDevice();
    void setView(float clip_left, float clip_right, float clip_bottom, float clip_top, float clip_near, float clip_far);
    void lightGetColor(D3DLIGHT8 *d3dLight, nLight *nebLight);
    void lightGetPos(D3DLIGHT8 *d3dLight, const matrix44& modelView);
    void lightGetDir(D3DLIGHT8 *d3dLight, const matrix44& modelView);
    void lightGetAtt(D3DLIGHT8 *d3dLight, nLight *nebLight);
    bool initTextRenderer();
    void killTextRenderer();
    void renderTextNodes();
    IDirect3DSurface8* LockFrontBuffer(D3DSURFACE_DESC& surfDesc, D3DLOCKED_RECT& surfRect);
    void UnlockFrontBuffer(IDirect3DSurface8* surf);

public:
    // NOTE: those members are only public because WinProc
    // must access some of them. Keep your filthy fingers off 
    // them or DIE :)
    void present();
    char* nextDisplayMode(char* buf, int bufSize);
    char* prevDisplayMode(char* buf, int bufSize);

    bool adjustViewport(int w, int h);
    HWND parent_hWnd; 

    nAutoRef<nInputServer> ref_is;

    HINSTANCE hInst;
    HWND      hWnd;
    HACCEL    hAccel;
    DWORD windowedStyle;            // WS_* flags for windowed mode
    DWORD fullscreenStyle;          // WS_* flags for fullscreen mode
    DWORD childStyle;               // WS_* flags for child mode 

    IDirect3D8*         d3d8;       // pointer to D3D8 object
    IDirect3DDevice8*   d3d8Dev;    // pointer to device object
    CD3DFont*           d3dFont;    // pointer to CD3DFont object

    int numAdapters;                // number of adapters in the system
    int selAdapter;                 // the adapter that has been selected
    nRef<nRoot> refSelAdapter;      // points to /sys/share/display/X
    nRef<nRoot> refSelMode;         // the display mode selected by SetDisplayMode()
    D3DCAPS8 devCaps;               // the device caps

    D3DPRESENT_PARAMETERS presentParams;    // the presentation parameters
    int renderWidth;                // the current backbuffer width
    int renderHeight;               // the current backbuffer height

    bool classRegistered;           // window class has been registered
    bool windowOpen;                // window has been opened
    bool windowMinimized;           // window is currently minimized
    bool windowFullscreen;          // if currently in fullscreen mode
    bool displayOpen;               // display currently open
    bool inBeginScene;              // currently inside BeginScene() / EndScene()
    bool quitRequested;             // quit requested by WinProc()
    bool queryAdaptersCalled;       // queryAdapters() has been called
    bool selectAdapterCalled;       // selectedAdapter() has been called

    bool useWbuffer;                // W buffer is present and must be used

    // d3d8 line drawing helper
    nD3D8Lines *myLines;

    D3DCOLOR d3dClearColor;
    vector4 vecClearColor;
    bool globalFogEnable;
    DWORD clipPlaneMask;
    nMatrixStack<D3DMATRIX,N_MXSTACKDEPTH> d3dModelviewStack;
    nMatrixStack<D3DMATRIX,N_MXSTACKDEPTH> d3dProjStack;
    D3DMATRIX d3dWorldMatrix;
    D3DMATRIX d3dProjMatrix;
    DWORD stencilClear;

    nList textList;                 // list of text nD3DTextNode objects
    float textXPos;
    float textYPos;

    bool systemCursorShown;         // system cursor on/off
    bool d3dCursorShown;            // d3d8's custom cursor on/off

    nVertexPoolManager *vpoolManager[N_NUM_VBTYPES];

    enum {                          // accelerator key enums
        N_D3DCMD_TOGGLEFULLSCREEN = 1001,
        N_D3DCMD_NEXTMODE         = 1002,
        N_D3DCMD_PREVMODE         = 1003,
    };

    enum {
        N_STATICPOOLSIZE    = 8000,
        N_READONLYPOOLSIZE  = 8000,
        N_WRITEONLYPOOLSIZE = 8000,
    };
};
//-----------------------------------------------------------------------------
#endif

