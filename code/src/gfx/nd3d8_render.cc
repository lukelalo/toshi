#define N_IMPLEMENTS nD3D8Server
//-----------------------------------------------------------------------------
//  nd3d8_render.cc
//  (C) 2001 A.Weissflog
//-----------------------------------------------------------------------------
#include "gfx/nd3d8server.h"

extern const char *nd3d8_Error(HRESULT hr);

//-----------------------------------------------------------------------------
/**
    Start rendering the scene, this first make sure that the d3d device
    is in a valid state, then clear the color and zbuffer and reset some
    global render states (such as turning off all lights).

    @return if false, you should skip the renderloop, including EndScene()
*/
bool
nD3D8Server::BeginScene()
{
    HRESULT hr;
    if (nGfxServer::BeginScene())
    {

        // if display currently closed, do nothing!
        if (!this->displayOpen) return false;

        n_assert(this->d3d8Dev);
        n_assert(!this->inBeginScene);

        // see if the device has been lost and needs to be reset,
        // since all our d3d resources are managed we do not
        // reload them from disk
        if (!this->testResetDevice())
        {
            // device can not be restored at this time
            return false;
        }

        // tell d3d that a new frame starts
        hr = this->d3d8Dev->BeginScene();
        if (FAILED(hr))
        {
            n_printf("nD3D8Server: BeginScene() failed with '%s'\n", nd3d8_Error(hr));
            return false;
        }

        // clear buffers
        D3DFORMAT f = this->presentParams.AutoDepthStencilFormat;
        if (f == D3DFMT_D24S8 || f == D3DFMT_D24X4S4 || f == D3DFMT_D15S1)
        {
            // clear with stencil buffer
            this->d3d8Dev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
                                 this->d3dClearColor, 1.0f, this->stencilClear);
        }
        else
        {
            // clear without stencil buffer
            this->d3d8Dev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
                                 this->d3dClearColor, 1.0f, 0L);
        }
            
        // kill all lights from previous frame
        int i;
        for (i=0; i<nGfxServer::N_MAXLIGHTS; i++) 
        {
            this->d3d8Dev->LightEnable(i,FALSE);
        }

        // turn off fog
        this->globalFogEnable = false;
        this->d3d8Dev->SetRenderState(D3DRS_FOGENABLE, FALSE);
        this->d3d8Dev->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);

        this->inBeginScene = true;
    }
    return true;
}

//-----------------------------------------------------------------------------
/**
    Finish rendering the frame and swap buffers.
*/
void
nD3D8Server::EndScene()
{
    n_assert(this->inBeginScene);
    n_assert(this->d3d8Dev);

    HRESULT hr;

    // render text nodes
    this->renderTextNodes();

    // tell d3d that the scene is finished
    hr = this->d3d8Dev->EndScene();
    if (FAILED(hr))
    {
        n_printf("nD3D8Server: EndScene() failed with '%s'\n",nd3d8_Error(hr));
        return;
    }

    // flip da buffas
    this->present();

    this->inBeginScene = false;

    nGfxServer::EndScene();
}

//-----------------------------------------------------------------------------
/**
    Set a new view volume. The change takes place immediately. The view
    volume is described as a rectangle on the near plane (the near plane
    is described by minz, with [0,0] in the middle of the screen,
    the rectangle is described as [minx,maxx,miny,maxy] on that plane.
    
    @param  minx    the left x coord where view volume cuts near plane
    @param  maxx    the right x coord where view volume cuts near plane
    @param  miny    the upper y coord where view volume cuts near plane
    @param  maxy    the lower y coord where view volume cuts near plane
    @param  minz    distance from eye to near plane of view volume
    @param  maxz    distance from eye to far plane of view volume
*/
void 
nD3D8Server::SetViewVolume(float minx, float maxx, 
                           float miny, float maxy,
                           float minz, float maxz)
{
    nGfxServer::SetViewVolume(minx, maxx, miny, maxy, minz, maxz);
    if (this->d3d8Dev) this->setView(minx, maxx, miny, maxy, minz, maxz);
}

//-----------------------------------------------------------------------------
/**
    Returns the Nebula color format that this gfx server requires for
    defining vertex colors. This is the only gfx server dependent
    vertex component. This mechanism prevents most on-the-fly color format
    conversions at runtime.

    @return the nColorFormat
*/
nColorFormat 
nD3D8Server::GetColorFormat(void)
{
    return N_COLOR_BGRA;
}

//-----------------------------------------------------------------------------
/**
    Return the current display geometry.

    @param  x0  [out] x coord of top left corner
    @param  y0  [out] y coord of top left corner
    @param  x1  [out] x coord of bottom right corner
    @param  y1  [out] y coord of bottom right corner
    @return     always true
*/
bool
nD3D8Server::GetDisplayDesc(int& x0, int& y0, int& x1, int& y1)
{
    x0 = 0;
    x1 = this->renderWidth;
    y0 = 0;
    y1 = this->renderHeight;
    return true;
}

//-----------------------------------------------------------------------------
/**
    Return the size of the currently used text font.

    @param  height  [out] height of font in pixels
    @return         always true
*/
bool
nD3D8Server::GetFontDesc(int& height)
{
    height = 16; // FIXME!!!
    return true;
}

//-----------------------------------------------------------------------------
/**
    Set the background clear color which is used to fill the back buffer
    at the start of a new frame.

    @param r    red component
    @param g    green component
    @param b    blue component
    @param a    alpha component (usually ignored)
*/
void
nD3D8Server::SetClearColor(float r, float g, float b, float a)
{
    this->d3dClearColor = D3DCOLOR_COLORVALUE(r,g,b,a);
    this->vecClearColor.set(r,g,b,a);
}

//-----------------------------------------------------------------------------
/**
    Returns the current background clear color.

    @param r    [out] red component
    @param g    [out] green component
    @param b    [out] blue component
    @param a    [out] alpha component (usually ignored)
*/
void nD3D8Server::GetClearColor(float& r, float& g, float& b, float& a)
{
    r = this->vecClearColor.x;
    g = this->vecClearColor.y;
    b = this->vecClearColor.z;
    a = this->vecClearColor.w;
}

//-----------------------------------------------------------------------------
/**
    Set one of the modelview or projection matrices. The nD3D8Server
    catches the N_MXM_MODELVIEW and N_MXM_PROJECTION matrix and
    hands them to D3D. Besides that, all matrices are handed
    to the nGfxServer code.
    
    Note that when using the W-buffer the matrix you pass might get
    tinkered with, see code for details.

    @param matrixMode   the nMatrixMode (N_MXM_MODELVIEW or N_MXM_PROJECTION)
    @param matrix       a Nebula matrix44 object   
*/
void 
nD3D8Server::SetMatrix(nMatrixMode matrixMode, matrix44& matrix)
{
    n_assert(this->d3d8Dev);
    switch (matrixMode) {
        case N_MXM_MODELVIEW:
            this->d3d8Dev->SetTransform(D3DTS_WORLD,(D3DMATRIX *)&matrix);
            memcpy(&(this->d3dWorldMatrix), &matrix, sizeof(D3DMATRIX)); 
            break;

        case N_MXM_PROJECTION:
            if (this->useWbuffer && matrix.M34 != 1.0 && matrix.M34 != 0.0) 
            {
                // normalize W to be equivalent to world-space Z
                float e = matrix.M34;
                matrix.M11 /= e; matrix.M22 /= e; matrix.M33 /= e; matrix.M43 /= e;
                matrix.M34 = 1.0;
            }
            this->d3d8Dev->SetTransform(D3DTS_PROJECTION,(D3DMATRIX *)&matrix);
            memcpy(&(this->d3dProjMatrix), &matrix, sizeof(D3DMATRIX)); 
            break;
    }
    nGfxServer::SetMatrix(matrixMode, matrix);
}

//-----------------------------------------------------------------------------
/**
    Push the current modelview or projection matrix onto the internal
    matrix stack.

    @param matrixMode   the nMatrixMode (N_MXM_MODELVIEW or N_MXM_PROJECTION)
*/
void 
nD3D8Server::PushMatrix(nMatrixMode matrixMode)
{
    n_assert(this->d3d8Dev);
    switch (matrixMode) {
        case N_MXM_MODELVIEW: 
            this->d3dModelviewStack.Push(this->d3dWorldMatrix);
            break;

        case N_MXM_PROJECTION:
            this->d3dProjStack.Push(this->d3dProjMatrix);
            break;
    }
    nGfxServer::PushMatrix(matrixMode);
}

//-----------------------------------------------------------------------------
/**
    Pop matrix from internal matrix stack and make it the current matrix.

    @param matrixMode   the nMatrixMode (N_MXM_MODELVIEW or N_MXM_PROJECTION)
*/
void 
nD3D8Server::PopMatrix(nMatrixMode matrixMode)
{
    n_assert(this->d3d8Dev);
    D3DMATRIX m;
    switch (matrixMode) {
        case N_MXM_MODELVIEW:
            m = this->d3dModelviewStack.Pop();
            this->d3d8Dev->SetTransform(D3DTS_WORLD,&m);
            memcpy(&(this->d3dWorldMatrix), &m, sizeof(D3DMATRIX)); 
            break;

        case N_MXM_PROJECTION:
            m = this->d3dProjStack.Pop();
            this->d3d8Dev->SetTransform(D3DTS_PROJECTION,&m);
            memcpy(&(this->d3dProjMatrix), &m, sizeof(D3DMATRIX)); 
            break;
    }
    nGfxServer::PopMatrix(matrixMode);
}

//-----------------------------------------------------------------------------
/**
    Set model space coefficients of a user defined clip plane. The 
    coefficients take the form of the general plane equation
    Ax + By + Cz + Dw = 0. A point [xyzw] is visible if
    Ax + By + Cz + Dw >= 0.

    @param index    index of clip plane (0..N)
    @param plane    ABCD coefficients of clip plane in model space
*/
bool 
nD3D8Server::SetClipPlane(int index, vector4& plane)
{
    n_assert(this->d3d8Dev);

    // multiply incoming model space clip plane by
    // transpose of inverse modelview to yield eye space
    // clip plane
    matrix44 m(this->modelview);
    m.invert();
    m.transpose();
    vector4 v1 = m * plane;
    float d3dv[4];
    d3dv[0] = v1.x;
    d3dv[1] = v1.y;
    d3dv[2] = v1.z;
    d3dv[3] = v1.w;
    this->d3d8Dev->SetClipPlane(index, d3dv);
    return true;
}

//-----------------------------------------------------------------------------
/**
    Set a render state. Please note that most fine grained render states
    are now encapsulated by the nPixelShader object.

    @param rs   the render state object
*/
void 
nD3D8Server::SetState(nRState& rs)
{
    IDirect3DDevice8 *d = this->d3d8Dev;
    switch(rs.rtype) {

        case N_RS_ZBIAS:
            n_assert(d);
            d->SetRenderState(D3DRS_ZBIAS, (DWORD) rs.rstate.f); 
            break;

        case N_RS_FOGMODE:
            {
                D3DFOGMODE fm;
                switch (rs.rstate.r) {
                    case N_FOGMODE_OFF:
                        this->globalFogEnable = false;
                        fm = D3DFOG_NONE;
                        break;
                    case N_FOGMODE_LINEAR:  
                        this->globalFogEnable = true;
                        fm=D3DFOG_LINEAR; 
                        break;
                    case N_FOGMODE_EXP:     
                        this->globalFogEnable = true;
                        fm=D3DFOG_EXP; 
                        break;
                    case N_FOGMODE_EXP2:    
                        this->globalFogEnable = true;
                        fm=D3DFOG_EXP2; 
                        break;
                    default: 
                        this->globalFogEnable = false;
                        fm=D3DFOG_NONE; 
                        break;
                }
                n_assert(d);
                d->SetRenderState(D3DRS_FOGENABLE, this->globalFogEnable);
                d->SetRenderState(D3DRS_FOGVERTEXMODE, fm);
            }
            break;

        case N_RS_FOGSTART:
            n_assert(d);
            d->SetRenderState(D3DRS_FOGSTART, *((LPDWORD)(&(rs.rstate.f))));
            break;

        case N_RS_FOGEND:
            n_assert(d);
            d->SetRenderState(D3DRS_FOGEND, *((LPDWORD)(&(rs.rstate.f))));
            break;

        case N_RS_FOGDENSITY:
            n_assert(d);
            d->SetRenderState(D3DRS_FOGDENSITY, *((LPDWORD)(&(rs.rstate.f))));
            break;

        case N_RS_FOGCOLOR:
            {
                float *p = (float *) rs.rstate.p;
                float r = p[0];
                float g = p[1];
                float b = p[2];
                float a = p[3];
                D3DCOLOR c = D3DCOLOR_COLORVALUE(r,g,b,a);
                n_assert(d);
                d->SetRenderState(D3DRS_FOGCOLOR,c);
            }
            break;

        case N_RS_DONTCLIP:
            n_assert(d);
            d->SetRenderState(D3DRS_CLIPPING, !((DWORD)rs.rstate.r));
            break;

        case N_RS_CLIPPLANEENABLE:
            this->clipPlaneMask |= (1<<rs.rstate.i);
            n_assert(d);
            d->SetRenderState(D3DRS_CLIPPLANEENABLE, this->clipPlaneMask);
            break;

        case N_RS_CLIPPLANEDISABLE:
            this->clipPlaneMask &= ~(1<<rs.rstate.i);
            n_assert(d);
            d->SetRenderState(D3DRS_CLIPPLANEENABLE, this->clipPlaneMask);
            break;

        case N_RS_NORMALIZENORMALS:
            n_assert(d);
            d->SetRenderState(D3DRS_NORMALIZENORMALS,(DWORD)rs.rstate.r);
            break;

        case N_RS_STENCILENABLE:
            n_assert(d);
            d->SetRenderState(D3DRS_STENCILENABLE, (DWORD) rs.rstate.r);
            break;

        case N_RS_STENCILFAIL:
            {
                DWORD r;
                switch (rs.rstate.r)
                {
                    case N_STENCILOP_KEEP:      r = D3DSTENCILOP_KEEP; break;
                    case N_STENCILOP_ZERO:      r = D3DSTENCILOP_ZERO; break;
                    case N_STENCILOP_REPLACE:   r = D3DSTENCILOP_REPLACE; break;
                    case N_STENCILOP_INVERT:    r = D3DSTENCILOP_INVERT; break;
                    case N_STENCILOP_INCR:      r = D3DSTENCILOP_INCRSAT; break;
                    case N_STENCILOP_DECR:      r = D3DSTENCILOP_DECRSAT; break;
                    default:                    r = D3DSTENCILOP_KEEP; break;
                }
                n_assert(d);
                d->SetRenderState(D3DRS_STENCILFAIL, r);
            }
            break;

        case N_RS_STENCILZFAIL:
            {
                DWORD r;
                switch (rs.rstate.r)
                {
                    case N_STENCILOP_KEEP:      r = D3DSTENCILOP_KEEP; break;
                    case N_STENCILOP_ZERO:      r = D3DSTENCILOP_ZERO; break;
                    case N_STENCILOP_REPLACE:   r = D3DSTENCILOP_REPLACE; break;
                    case N_STENCILOP_INVERT:    r = D3DSTENCILOP_INVERT; break;
                    case N_STENCILOP_INCR:      r = D3DSTENCILOP_INCRSAT; break;
                    case N_STENCILOP_DECR:      r = D3DSTENCILOP_DECRSAT; break;
                    default:                    r = D3DSTENCILOP_KEEP; break;
                }
                n_assert(d);
                d->SetRenderState(D3DRS_STENCILZFAIL, r);
            }
            break;

        case N_RS_STENCILPASS:
            {
                DWORD r;
                switch (rs.rstate.r)
                {
                    case N_STENCILOP_KEEP:      r = D3DSTENCILOP_KEEP; break;
                    case N_STENCILOP_ZERO:      r = D3DSTENCILOP_ZERO; break;
                    case N_STENCILOP_REPLACE:   r = D3DSTENCILOP_REPLACE; break;
                    case N_STENCILOP_INVERT:    r = D3DSTENCILOP_INVERT; break;
                    case N_STENCILOP_INCR:      r = D3DSTENCILOP_INCRSAT; break;
                    case N_STENCILOP_DECR:      r = D3DSTENCILOP_DECRSAT; break;
                    default:                    r = D3DSTENCILOP_KEEP; break;
                }
                n_assert(d);
                d->SetRenderState(D3DRS_STENCILPASS, r);
            }
            break;

        case N_RS_STENCILFUNC:
            {
                DWORD r;
                switch (rs.rstate.r)
                {
                    case N_CMP_NEVER:           r = D3DCMP_NEVER; break;
                    case N_CMP_LESS:            r = D3DCMP_LESS; break;
                    case N_CMP_EQUAL:           r = D3DCMP_EQUAL; break;
                    case N_CMP_LESSEQUAL:       r = D3DCMP_LESSEQUAL; break;
                    case N_CMP_GREATER:         r = D3DCMP_GREATER; break;
                    case N_CMP_NOTEQUAL:        r = D3DCMP_NOTEQUAL; break;
                    case N_CMP_GREATEREQUAL:    r = D3DCMP_GREATEREQUAL; break;
                    case N_CMP_ALWAYS:          r = D3DCMP_ALWAYS; break;
                    default:                    r = D3DCMP_ALWAYS; break;
                }
                n_assert(d);
                d->SetRenderState(D3DRS_STENCILFUNC, r);
            }
            break;

        case N_RS_STENCILREF:
            n_assert(d);
            d->SetRenderState(D3DRS_STENCILREF, rs.rstate.i);
            break;

        case N_RS_STENCILMASK:
            n_assert(d);
            d->SetRenderState(D3DRS_STENCILMASK, rs.rstate.i);
            break;

        case N_RS_STENCILCLEAR:
            this->stencilClear = rs.rstate.i;
            break;

        case N_RS_COLORWRITEENABLE:
            n_assert(d);
            if (rs.rstate.r == N_TRUE)
            {
                d->SetRenderState(D3DRS_COLORWRITEENABLE, 
                    D3DCOLORWRITEENABLE_RED |
                    D3DCOLORWRITEENABLE_GREEN |
                    D3DCOLORWRITEENABLE_BLUE |
                    D3DCOLORWRITEENABLE_ALPHA);
            }
            else
            {
                d->SetRenderState(D3DRS_COLORWRITEENABLE, 0);
            }
            break;

        default: break;
    }
}

//-----------------------------------------------------------------------------
