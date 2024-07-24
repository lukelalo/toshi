#define N_IMPLEMENTS nAnimServer
//------------------------------------------------------------------------------
//  nanimserver_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "anim/nanimserver.h"

nNebulaClass(nAnimServer, "nroot");

//------------------------------------------------------------------------------
/**
*/
nAnimServer::nAnimServer()
{
    // create resource directories
    this->refCurveArrays  = kernelServer->New("nroot", "/sys/share/animcurves");
}

//------------------------------------------------------------------------------
/**
*/
nAnimServer::~nAnimServer()
{
    if (this->refCurveArrays.isvalid())
    {
        this->refCurveArrays->Release();
    }
}

//------------------------------------------------------------------------------
/**
*/
const char*
nAnimServer::GetResourceId(const char* rsrcId, char* buf, int bufSize)
{
    n_assert(rsrcId);
    n_assert(buf);
    n_assert(bufSize > 0);

    // copy the last 32 chars to buf
    char c;
    char *str;

    int len = strlen(rsrcId) + 1;
    int off = len - bufSize;
    if (off < 0) off = 0;
    len -= off;
    strcpy(buf, &(rsrcId[off]));

    // replace illegal characters
    str = buf;
    while ((c = *str)) {
        if ((c=='.')||(c=='/')||(c=='\\')||(c==':')) *str='_';
        str++;
    }
    return buf;
}

//------------------------------------------------------------------------------
/**
    Find an anim curve array object by its resource id. This will
    call AddRef() on the found object.

    @return     pointer to nAnimCurveArray object or 0
*/
nAnimCurveArray*
nAnimServer::FindAnimCurveArray(const char* rsrcId)
{
    char resId[N_MAXPATH];
    this->GetResourceId(rsrcId, resId, sizeof(resId));
    
    nRoot* obj = this->refCurveArrays->Find(resId);
    if (obj)
    {
        obj->AddRef();
    }

    return (nAnimCurveArray*) obj;
}

//------------------------------------------------------------------------------
/**
*/
nAnimCurveArray*
nAnimServer::NewAnimCurveArray(const char* rsrcId)
{
    char resId[N_MAXPATH];
    this->GetResourceId(rsrcId, resId, sizeof(resId));

    kernelServer->PushCwd(this->refCurveArrays.get());
    nRoot* obj = kernelServer->New("nanimcurvearray", resId);
    kernelServer->PopCwd();

    return (nAnimCurveArray*) obj;
}

//------------------------------------------------------------------------------
