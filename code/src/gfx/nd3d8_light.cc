#define N_IMPLEMENTS nD3D8Server
//-----------------------------------------------------------------------------
//  nd3d8_light.cc
//  (C) 2001 A.Weissflog
//-----------------------------------------------------------------------------
#include "gfx/nd3d8server.h"

#define N_RANGEMAX (1000000.0f)

//-----------------------------------------------------------------------------
/**
    Write diffuse color from Nebula light object into D3D8LIGHT8
    structure.

    @param d3dLight     pointer to D3D8LIGHT structure
    @param nebLight     pointer to Nebula light object
*/
void 
nD3D8Server::lightGetColor(D3DLIGHT8 *d3dLight, nLight* nebLight)
{
    memcpy(&(d3dLight->Diffuse), &(nebLight->color), 4*sizeof(float));
}

//-----------------------------------------------------------------------------
/**
    Write light position from Nebula modelview matrix44 into D3D8LIGHT8
    structure.

    @param d3dLight     pointer to D3D8LIGHT structure
    @param modelview    pointer to modelview matrix44 object
*/
void 
nD3D8Server::lightGetPos(D3DLIGHT8 *d3dLight, const matrix44& modelView)
{
    d3dLight->Position.x = modelView.M41;
    d3dLight->Position.y = modelView.M42;
    d3dLight->Position.z = modelView.M43;
}

//-----------------------------------------------------------------------------
/**
    Write light direction from Nebula modelview matrix44 into D3D8LIGHT8
    structure.

    @param d3dLight     pointer to D3D8LIGHT structure
    @param modelview    pointer to modelview matrix44 object
*/
void 
nD3D8Server::lightGetDir(D3DLIGHT8 *d3dLight, const matrix44& modelView)
{
    d3dLight->Direction.x = -modelview.M31;
    d3dLight->Direction.y = -modelview.M32;
    d3dLight->Direction.z = -modelview.M33;
}

//-----------------------------------------------------------------------------
/**
    Write light attenuation from Nebula light object into D3D8LIGHT8
    structure.

    @param d3dLight     pointer to D3D8LIGHT structure
    @param nebLight     pointer to Nebula light object
*/
void 
nD3D8Server::lightGetAtt(D3DLIGHT8 *d3dLight, nLight *nebLight)
{
    d3dLight->Range = N_RANGEMAX;
    d3dLight->Attenuation0 = nebLight->att[0];
    d3dLight->Attenuation1 = nebLight->att[1];
    d3dLight->Attenuation2 = nebLight->att[2];
}

//-----------------------------------------------------------------------------
/**
    @param  index   the light slot index (0..N_MAXLIGHTS)
    @param  light   pointer to Nebula nLight object (can be NULL)
*/
bool
nD3D8Server::SetLight(nLight& light)
{
    n_assert(this->d3d8Dev);

    // get current light index
    int index = this->GetNumLights();
    if (nGfxServer::SetLight(light))
    {
        D3DLIGHT8 d;
        IDirect3DDevice8 *dev = this->d3d8Dev;
        memset(&d,0,sizeof(d));
        switch(light.type) {
            case N_LIGHT_AMBIENT:
                {
                    float r = light.color[0];
                    float g = light.color[1];
                    float b = light.color[2];
                    float a = light.color[3];
                    DWORD amb = D3DCOLOR_COLORVALUE(r,g,b,a);
                    dev->SetRenderState(D3DRS_AMBIENT, amb);
                    dev->LightEnable(index, FALSE);
                    return true;
                }
                break;

            case N_LIGHT_POINT:
                d.Type = D3DLIGHT_POINT;
                lightGetColor(&d, &light);
                lightGetPos(&d, this->modelview);
                lightGetAtt(&d, &light);
                break;

            case N_LIGHT_SPOT:
                d.Type = D3DLIGHT_SPOT;
                lightGetColor(&d, &light);
                lightGetPos(&d, this->modelview);
                lightGetDir(&d, this->modelview);
                lightGetAtt(&d, &light);
                d.Theta   = 0.0f;
                d.Phi     = n_deg2rad(light.cutoff) * 2.0f;
                d.Falloff = 1.0f;
                break;

            case N_LIGHT_DIRECTIONAL:
                d.Type = D3DLIGHT_DIRECTIONAL;
                lightGetColor(&d, &light);
                lightGetDir(&d, this->modelview);
                d.Range = N_RANGEMAX;
                break;
        }
        this->d3d8Dev->SetLight(index, &d);
        this->d3d8Dev->LightEnable(index, TRUE);
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
