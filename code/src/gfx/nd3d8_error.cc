#define N_IMPLEMENTS nD3D8Server
//-----------------------------------------------------------------------------
//  nd3d8_error.cc
//  (C) 2001 A.Weissflog
//-----------------------------------------------------------------------------
#include "kernel/ntypes.h"

#include <d3d8.h>
#include <d3dx8.h>

/**.
    Convert D3D error enums into string.

    @param  hr  a HRESULT from D3D
    @return     the HRESULT as a string

    history:
    - 23-Jan-2001   floh    created
*/
const char *nd3d8_Error(HRESULT hr)
{
    const char *s;
    switch (hr)
    {
        case D3D_OK:
            s = "D3D_OK";
            break;

        case D3DERR_WRONGTEXTUREFORMAT:
            s = "D3DERR_WRONGTEXTUREFORMAT";
            break;

        case D3DERR_UNSUPPORTEDCOLOROPERATION:
            s = "D3DERR_UNSUPPORTEDCOLOROPERATION";
            break;

        case D3DERR_UNSUPPORTEDCOLORARG:
            s = "D3DERR_UNSUPPORTEDCOLORARG";
            break;

        case D3DERR_UNSUPPORTEDALPHAOPERATION:
            s = "D3DERR_UNSUPPORTEDALPHAOPERATION";
            break;

        case D3DERR_UNSUPPORTEDALPHAARG:
            s = "D3DERR_UNSUPPORTEDALPHAARG";
            break;

        case D3DERR_TOOMANYOPERATIONS:
            s = "D3DERR_TOOMANYOPERATIONS";
            break;

        case D3DERR_CONFLICTINGTEXTUREFILTER:
            s = "D3DERR_CONFLICTINGTEXTUREFILTER";
            break;

        case D3DERR_UNSUPPORTEDFACTORVALUE:
            s = "D3DERR_UNSUPPORTEDFACTORVALUE";
            break;

        case D3DERR_CONFLICTINGRENDERSTATE:
            s = "D3DERR_CONFLICTINGRENDERSTATE";
            break;

        case D3DERR_UNSUPPORTEDTEXTUREFILTER:
            s = "D3DERR_UNSUPPORTEDTEXTUREFILTER";
            break;

        case D3DERR_CONFLICTINGTEXTUREPALETTE:
            s = "D3DERR_CONFLICTINGTEXTUREPALETTE";
            break;

        case D3DERR_DRIVERINTERNALERROR:
            s = "D3DERR_DRIVERINTERNALERROR";
            break;

        case D3DERR_NOTFOUND:
            s = "D3DERR_NOTFOUND";
            break;

        case D3DERR_MOREDATA:
            s = "D3DERR_MOREDATA";
            break;

        case D3DERR_DEVICELOST:
            s = "D3DERR_DEVICELOST";
            break;

        case D3DERR_DEVICENOTRESET:
            s = "D3DERR_DEVICENOTRESET";
            break;

        case D3DERR_NOTAVAILABLE:
            s = "D3DERR_NOTAVAILABLE";
            break;

        case D3DERR_OUTOFVIDEOMEMORY:
            s = "D3DERR_OUTOFVIDEOMEMORY";
            break;

        case D3DERR_INVALIDDEVICE:
            s = "D3DERR_INVALIDDEVICE";
            break;

        case D3DERR_INVALIDCALL:
            s = "D3DERR_INVALIDCALL";
            break;

        default:
            s = "*** UNDEFINED ***";
            break;
    }
    return s;
}
//-----------------------------------------------------------------------------
