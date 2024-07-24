#define N_IMPLEMENTS nSpriteRender
//--------------------------------------------------------------------
//  nsprender_init.cc
//  (C) 2000 A.Weissflog
//--------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "particle/nspriterender.h"

nClass *nSpriteRender::local_cl  = NULL;
nKernelServer *nSpriteRender::ks = NULL;
vector3 nSpriteRender::refVertices[nSpriteRender::REFQUADS * 4];
vector3 nSpriteRender::tformedVertices[nSpriteRender::REFQUADS * 4];

extern char *nSpriteRender_version;
extern "C" bool N_EXPORT n_init(nClass *, nKernelServer *);
extern "C" void N_EXPORT n_fini(void);
extern "C" N_EXPORT void *n_create(void);
extern "C" N_EXPORT char *n_version(void);

//--------------------------------------------------------------------
//  n_init()
//--------------------------------------------------------------------
bool N_EXPORT n_init(nClass *cl, nKernelServer *ks)
{
    nSpriteRender::local_cl = cl;
    nSpriteRender::ks       = ks;
    ks->AddClass("nprender", cl);

    // initialize untransformed reference quads
    vector3 v0(-0.5f, -0.5f, 0.0f);
    vector3 v1(+0.5f, -0.5f, 0.0f);
    vector3 v2(+0.5f,  0.5f, 0.0f);
    vector3 v3(-0.5f,  0.5f, 0.0f);
    matrix33 tform;
    int i;
    for (i = 0; i < nSpriteRender::REFQUADS; i++)
    {
        float rad = (float(i) / float(nSpriteRender::REFQUADS)) * 2.0f * N_PI;
        tform.ident();
        tform.rotate_z(rad);
        nSpriteRender::refVertices[i*4 + 0] = tform * v0;
        nSpriteRender::refVertices[i*4 + 1] = tform * v1;
        nSpriteRender::refVertices[i*4 + 2] = tform * v2;
        nSpriteRender::refVertices[i*4 + 3] = tform * v3;
    }
    return true;
}

//--------------------------------------------------------------------
//  n_fini()
//--------------------------------------------------------------------
void N_EXPORT n_fini(void)
{
    nSpriteRender::ks->RemClass(nSpriteRender::local_cl);
}

//--------------------------------------------------------------------
//  n_create()
//--------------------------------------------------------------------
N_EXPORT void *n_create(void)
{
    return new nSpriteRender;
}

//--------------------------------------------------------------------
//  n_version()
//--------------------------------------------------------------------
N_EXPORT char *n_version(void)
{
    return nSpriteRender_version;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
