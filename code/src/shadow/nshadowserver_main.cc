#define N_IMPLEMENTS nShadowServer
//------------------------------------------------------------------------------
//  nshadowserver_main.cc
//  (C) 2001 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "shadow/nshadowserver.h"
#include "gfx/nindexbuffer.h"
#include "gfx/nvertexbuffer.h"

nNebulaScriptClass(nShadowServer, "nroot");

//------------------------------------------------------------------------------
/**
*/
nShadowServer::nShadowServer() :
    castShadows(false),
    uniqueId(0),
    shadowColor(0.0f, 0.0f, 0.0f, 0.5f)
{
    // initialize shadow casters directory
    this->refShadowCasters = kernelServer->New("nroot", "/sys/share/shadowcasters");
}

//------------------------------------------------------------------------------
/**
*/
nShadowServer::~nShadowServer()
{
    // kill shadow casters
    if (this->refShadowCasters.isvalid())
    {
        this->refShadowCasters->Release();
    }
}

//------------------------------------------------------------------------------
/**
    @param  b   shadow casting toggle
*/
void
nShadowServer::SetCastShadows(bool b)
{
    this->castShadows = b;
}

//------------------------------------------------------------------------------
/**
    @return     shadow casting toggle
*/
bool
nShadowServer::GetCastShadows()
{
    return this->castShadows;
}

//------------------------------------------------------------------------------
/**
    @param  r   red component of shadow color
    @param  g   red component of shadow color
    @param  b   red component of shadow color
    @param  a   red component of shadow color
*/
void
nShadowServer::SetShadowColor(float r, float g, float b, float a)
{
    this->shadowColor.set(r, g, b, a);
}

//------------------------------------------------------------------------------
/**
    @param  r   [out] red component of shadow color
    @param  g   [out] red component of shadow color
    @param  b   [out] red component of shadow color
    @param  a   [out] red component of shadow color
*/
void
nShadowServer::GetShadowColor(float& r, float& g, float& b, float& a)
{
    r = this->shadowColor.x;
    g = this->shadowColor.y;
    b = this->shadowColor.z;
    a = this->shadowColor.w;
}

//------------------------------------------------------------------------------
/**
    This starts a shadow casting scene. Inside BeginScene()/EndScene() light
    sources and shadow casters should be attached. Rendering the shadows 
    happend inside EndScene().
*/
void
nShadowServer::BeginScene()
{
    n_printf("nShadowServer::BeginScene() called\n");
}

//------------------------------------------------------------------------------
/**
    Add a light source which contributes to shadow generation. Please 
    note that not all shadow server implementations have to consider all
    light sources. Please attach the most important (global) light sources
    first (e.g. the sun should be the very first light object). The shadow
    server is also free to reinterpret or ignore light attributes (it could
    treat all lights as directional light for instance).
    The position of the light is defined by the current modelview matrix
    in the gfx server.

    @param  light   the light object
*/
void
nShadowServer::AttachLight(const matrix44& modelview, nLight* light)
{
    n_assert(light);
    const vector3& pos = modelview.pos_component();
    n_printf("nShadowServer::AttachLight(pos=(%f,%f,%f)) called\n", pos.x, pos.y, pos.z);
}

//------------------------------------------------------------------------------
/**
    Add a shadow caster to the scene. The position of the shadow caster
    is defined by the current modelview matrix in the gfx server.
    
    @param  caster      a static caster object
*/
void
nShadowServer::AttachCaster(const matrix44& modelview, nShadowCaster* caster)
{
    n_assert(caster);
    const vector3& pos = modelview.pos_component();
    n_printf("nShadowServer::AttachCaster(pos=(%f,%f,%f)) called\n", pos.x, pos.y, pos.z);
}

//------------------------------------------------------------------------------
/**
    Finish a shadow casting scene. This is where rendering the shadows usually
    happens.
*/
void
nShadowServer::EndScene()
{
    n_printf("nShadowServer::EndScene()\n");
}

//------------------------------------------------------------------------------
/**
    Create a new shadow caster object. Subclasses can override this method
    and return objects of a different class.

    @param  name        a name (used to create resource id, can be 0)
    @return             a pointer to an nShadowCaster object
*/
nShadowCaster*
nShadowServer::NewCaster(const char* name)
{
    nShadowCaster* obj;
    char resid[N_MAXNAMELEN];
    this->getResourceId(name, resid, sizeof(resid));
    kernelServer->PushCwd(this->refShadowCasters.get());
    obj = (nShadowCaster*) kernelServer->New("nshadowcaster", resid);
    kernelServer->PopCwd();
    return obj;
}

//------------------------------------------------------------------------------
/**
    Find a shadow caster object. Used for resource sharing.

    @param  name        a name (used to create resource id)
    @return             a pointer to an nShadowCaster object or 0
*/
nShadowCaster*
nShadowServer::FindCaster(const char* name)
{
    char resid[N_MAXNAMELEN];
    this->getResourceId(name, resid, sizeof(resid));
    return (nShadowCaster*) this->refShadowCasters->Find(resid);
}

//------------------------------------------------------------------------------
/**
    Private routine which returns a valid resource id for either a shared
    or a non-shared resource.
    
    @param  name        an optional name (0 for non-shared resource)
    @param  buf         char buffer where resource id is written to
    @param  bufSize     size of buffer in bytes
*/
char *
nShadowServer::getResourceId(const char *name, char *buf, int bufSize)
{
    if (!name) 
    {
        // a non-shared resource
        sprintf(buf, "res%d", this->uniqueId++);
    }
    else 
    {
        // a shared resource, use the last 32 characters of name for resource id
        char c;
        char *str;

        int len = strlen(name)+1;
        int off = len - bufSize;
        if (off < 0) off = 0;
        len -= off;
        strcpy(buf,&(name[off]));

        // replace any illegal characters
        str = buf;
        while ((c = *str)) 
        {
            if ((c=='.')||(c=='/')||(c=='\\')||(c==':')) *str='_';
            str++;
        }
    }
    return buf;
}

//------------------------------------------------------------------------------
