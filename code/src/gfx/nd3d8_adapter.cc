#define N_IMPLEMENTS nD3D8Server
//-----------------------------------------------------------------------------
//  nd3d8_adapter.cc
//  (C) 2001 Andre Weissflog
//-----------------------------------------------------------------------------
#include "gfx/nd3d8server.h"

namespace {

// valid DepthStencil formats for fullscreen mode (in priority of use)
struct nDepthStencilFormat 
{
    D3DFORMAT   dsFormat;
    int         bpp;
    bool        tryWbuffer;
};

nDepthStencilFormat g_DepthStencilFormats[] = {
    { D3DFMT_D24S8,     32, false },    // try format with stencil buffer first
    { D3DFMT_D24X4S4,   32, false },
    { D3DFMT_D32,       32, false },    // then, others
    { D3DFMT_D24X8,     32, false },
    { D3DFMT_D16,       16, true  },    // in 16/15bit zbuffer try to use Wbuffer (to avoid z buffer artifacts)
    { D3DFMT_D15S1,     16, true },
    { D3DFMT_UNKNOWN,   0,  false }
};

} // namespace

extern const char *nd3d8_Error(HRESULT hr);

//-----------------------------------------------------------------------------
/**.
    Converts D3DFORMAT enum into its corresponding number of bits.
    
    @param format the D3DFORMAT enumerant
    @return       the correponding number of bits

    history:
    - 25-Jan-2001   floh    created
*/
int
nD3D8Server::bppFromFormat(D3DFORMAT format)
{
    int bpp;
    switch (format)
    {
        case D3DFMT_R8G8B8:     bpp = 24; break;
        case D3DFMT_A8R8G8B8:   bpp = 32; break;
        case D3DFMT_X8R8G8B8:   bpp = 32; break;
        case D3DFMT_R5G6B5:     bpp = 16; break;
        case D3DFMT_X1R5G5B5:   bpp = 16; break;
        case D3DFMT_A1R5G5B5:   bpp = 16; break;
        case D3DFMT_A4R4G4B4:   bpp = 16; break;
        case D3DFMT_X4R4G4B4:   bpp = 16; break;
        default:                bpp = 0;  break;
    }
    return bpp;
}

//-----------------------------------------------------------------------------
/**.
    Check if a display format works with the specified zbuffer.

    @param adapter  the adapter to be checked
    @param mode     the display mode to be checked
    @param zbuf     the zbuffer format to be checked

    history:
    - 25-Jan-2001   floh    created
*/
bool
nD3D8Server::checkZBufferFormat(int adapter, D3DFORMAT mode, D3DFORMAT zbuf)
{
    n_assert(this->d3d8);
    HRESULT hr;
    
    // check if the device can the zbuffer format at all
    hr = this->d3d8->CheckDeviceFormat(adapter, 
       D3DDEVTYPE_HAL,
       mode,
       D3DUSAGE_DEPTHSTENCIL,
       D3DRTYPE_SURFACE,
       zbuf);
    if (FAILED(hr)) 
    {
        return false;
    }

    // check if the specific combination is supported
    hr = this->d3d8->CheckDepthStencilMatch(adapter,
        D3DDEVTYPE_HAL,
        mode,
        mode,
        zbuf);
    if (FAILED(hr))
    {
        return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
/**.
    Enumerate all display modes for an adapter and put those modes into 
    the display database that can be hardware accelerated.

    @param adapter  the adapter to be used
    @return         the number of hardware accelerated display modes
    
    history:
    - 24-Jan-2001   floh    created
    - 17-Jul-2001   floh    removed stencil from zbuffer format
    - 04-Sep-2003   valen   added support of range of valid DepthStencil buffer 
                                                        formats (as opposed to only D24S8)
*/
int
nD3D8Server::enumDisplayModes(int adapter)
{
    HRESULT hr;

    // create the mode directory and enumerate modes
    nRoot *dir = kernelServer->New("nroot", "modes");
    kernelServer->PushCwd(dir);

    // enumerate all adapter modes and create a database
    // entry for all modes that can be hardware accelerated
    int cur_mode;
    int accepted_mode = 0;
    int num_modes = this->d3d8->GetAdapterModeCount(adapter);
    D3DDISPLAYMODE prev_dmode;
    D3DDISPLAYMODE dmode;
    memset(&prev_dmode, 0, sizeof(prev_dmode));
    memset(&dmode, 0, sizeof(dmode));
    for (cur_mode = 0; cur_mode < num_modes; cur_mode++)
    {
        // get the current adapter mode description
        hr = this->d3d8->EnumAdapterModes(adapter, cur_mode, &dmode);
        if (FAILED(hr))
        {
            n_error("nD3D8Server: EnumAdapterModes() failed with '%s'\n", nd3d8_Error(hr));
            return 0;
        }

        // ignore the mode if its size is less then 640x480
        if ((dmode.Width >= 640) && (dmode.Height >= 480))
        {
            // ignore the new mode if it only differs in frequency
            if (!((cur_mode > 0) &&
                 (dmode.Width == prev_dmode.Width) &&
                 (dmode.Height == prev_dmode.Height) &&
                 (dmode.Format == prev_dmode.Format)))
            {
                prev_dmode = dmode;

                // check if the mode can be hardware accelerated
                hr = this->d3d8->CheckDeviceType(adapter, 
                                                 D3DDEVTYPE_HAL,
                                                 dmode.Format,
                                                 dmode.Format,
                                                 FALSE);
                if (SUCCEEDED(hr))
                {
                    // generally try to get one of valid DepthStencil buffer formats
                    int i = 0;
                    while (g_DepthStencilFormats[i].dsFormat != D3DFMT_UNKNOWN) 
                    {
                        int zbuf_bpp = g_DepthStencilFormats[i].bpp;
                        D3DFORMAT zbuf_format = g_DepthStencilFormats[i].dsFormat;
                                             
                        if (this->checkZBufferFormat(adapter, dmode.Format, zbuf_format))
                        {
                            nEnv *env;

                            // create a new entry for the mode in the display database
                            char buf[N_MAXPATH];
                            sprintf(buf,"%d",accepted_mode++);
                            dir = kernelServer->New("nroot", buf);
                            kernelServer->PushCwd(dir);

                            // get color depth from format
                            int bpp = this->bppFromFormat(dmode.Format);
    
                            // create the various env vars...
                            sprintf(buf, "type(full)-w(%d)-h(%d)-bpp(%d)-zbuf(%d)",
                                    dmode.Width, dmode.Height, bpp, zbuf_bpp);
                            
                            env = (nEnv *) kernelServer->New("nenv", "name");
                            env->SetS(buf);
        
                            env = (nEnv *) kernelServer->New("nenv", "w");
                            env->SetI(dmode.Width);

                            env = (nEnv *) kernelServer->New("nenv", "h");
                            env->SetI(dmode.Height);

                            env = (nEnv *) kernelServer->New("nenv", "bpp");
                            env->SetI(bpp);

                            env = (nEnv *) kernelServer->New("nenv", "zbuf");
                            env->SetI(zbuf_bpp);
    
                            env = (nEnv *) kernelServer->New("nenv", "d3d8_dsp_format");
                            env->SetI(dmode.Format);

                            env = (nEnv *) kernelServer->New("nenv", "d3d8_zbuf_format");
                            env->SetI(zbuf_format);

                            env = (nEnv *) kernelServer->New("nenv", "tryWbuffer");
                            env->SetB( g_DepthStencilFormats[i].tryWbuffer );

                            kernelServer->PopCwd();
                        } // if
                        ++i;
                    } // while
                }
            }
        }
    }
    kernelServer->PopCwd();

    return accepted_mode;
}

//-----------------------------------------------------------------------------
/**.
    Enumerate all adapters in the system and built a device database under
    /sys/share/display. There is only one device per adapter, the
    HAL device. The numbering is as follows:

    /sys/share/display/0  - HAL device of adapter 0
    /sys/share/display/1  - HAL device of adapter 1
    ...

    Below the device database object, the following objects exist:

    nroot modes             - database of display modes and compatible zbuffer formats
    nenv  drv_name          - name of driver
    nenv  drv_desc          - description of driver
    nenv  drv_version       - version number of driver

    history:
    - 24-Jan-2001   floh    created
*/
bool
nD3D8Server::queryAdapters()
{
    n_assert(this->d3d8);
    n_assert(0 == this->d3d8Dev);

    HRESULT hr;

    this->queryAdaptersCalled = true;

    // get number of adapters
    this->numAdapters = this->d3d8->GetAdapterCount();

    // prepare display database
    nRoot *disp = kernelServer->Lookup("/sys/share/display");
    if (disp) 
    {
        disp->Release();
    }
    disp = kernelServer->New("nroot", "/sys/share/display");
    kernelServer->PushCwd(disp);

    // for each adapter...
    int cur_adapter;
    int cur_accepted_adapter = 0;
    for (cur_adapter = 0; cur_adapter<this->numAdapters; cur_adapter++) 
    {
        // create a new database entry, and make it the current object
        char buf[N_MAXNAMELEN];
        sprintf(buf,"%d", cur_accepted_adapter);
        nRoot *adapt = kernelServer->New("nroot", buf);

        // see if this adapter can accelerate any display modes,
        // if no, we ignore it!
        kernelServer->PushCwd(adapt);
        int num_modes = this->enumDisplayModes(cur_adapter);
        kernelServer->PopCwd();
        
        if (num_modes == 0)
        {
            // this adapter is a useless piece of armadillo poop
            adapt->Release();
        }
        else
        {
            // this adapter is acceptable, query more info about it
            D3DADAPTER_IDENTIFIER8 id;
            hr = this->d3d8->GetAdapterIdentifier(cur_adapter, D3DENUM_NO_WHQL_LEVEL, &id);
            if (FAILED(hr)) {
                n_error("nD3D8Server: GetAdapterIdentifier() failed with '%s'\n",
                         nd3d8_Error(hr));
                return false;
            }

            // write info to device database
            kernelServer->PushCwd(adapt);
            
            nEnv *env;
            env = (nEnv *) kernelServer->New("nenv","drv_name");
            env->SetS(id.Driver);
        
            env = (nEnv *) kernelServer->New("nenv", "drv_desc");
            env->SetS(id.Description);

            env = (nEnv *) kernelServer->New("nenv", "adapter");
            env->SetI(cur_adapter);

            kernelServer->PopCwd();
            
            cur_accepted_adapter++;
        }
    }
    kernelServer->PopCwd();
    return true;
}

//-----------------------------------------------------------------------------
/**.
    Select one of the enumerated adapters. Called by 
    nD3D8Server::SetDisplayMode() with the contents of the
    dev() tag of the display mode descriptor. If the requested number
    is invalid, fall back to adapter 0.

    This method will valid the following members:

    selAdapters
    refSelAdapter

    @param adapter  number of adapter to select

    history:
    - 25-Jan-2001   floh    created
*/
void
nD3D8Server::selectAdapter(int adapter)
{
    n_assert(this->queryAdaptersCalled);

    // create the filename that points to the adapter
    char buf[N_MAXPATH];
    sprintf(buf,"/sys/share/display/%d",adapter);

    nRoot *dir = kernelServer->Lookup(buf);
    if (!dir) {
        n_printf("nD3D8Server: no adapter '%d', falling back to default adapter.\n", adapter);
        adapter = 0;
        dir = kernelServer->Lookup("/sys/share/display/0");
        n_assert(dir);
    }
    this->selAdapter          = adapter;
    this->refSelAdapter       = dir;
    this->selectAdapterCalled = true;
}

//-----------------------------------------------------------------------------
