#define N_IMPLEMENTS nSkyDome

// (C) 2000 Forged Reality

#include "nemesis/nskydome.h"

static void n_setsunangle(void*, nCmd*);
static void n_setcloudfactor(void*, nCmd*);

void n_initcmds(nClass *cl)
{
	cl->BeginCmds();
    cl->AddCmd("v_setsunangle_f", 'SANG', n_setsunangle);
    cl->AddCmd("v_setcloudfactor_f", 'CFAC', n_setcloudfactor);
	cl->EndCmds();
}

static void n_setsunangle(void* vpObj, nCmd* pCmd)
{
    nSkyDome* pSelf = static_cast<nSkyDome*>(vpObj);
    pSelf->SetSunAngle(pCmd->In()->GetF());
}

static void n_setcloudfactor(void* vpObj, nCmd* pCmd)
{
    nSkyDome* pSelf = static_cast<nSkyDome*>(vpObj);
    pSelf->SetCloudFactor(pCmd->In()->GetF());
}
