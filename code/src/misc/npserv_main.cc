#define N_IMPLEMENTS nParticleServer
//-------------------------------------------------------------------
//  npserv_main.cc
//  (C) 2000 A.Weissflog
//-------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "misc/nparticleserver.h"

#include <stdlib.h>

nNebulaClass(nParticleServer, "nroot");

//-------------------------------------------------------------------
/**
   - 24-Jan-00   floh    created
*/
//-------------------------------------------------------------------
nParticleServer::nParticleServer()
{
    this->psystem_key = 1;
    this->ctx_array   = NULL;
}

//-------------------------------------------------------------------
/**
    - 24-Jan-00   floh    created
*/
//-------------------------------------------------------------------
nParticleServer::~nParticleServer()
{
    this->EvictContexts();
}

//-------------------------------------------------------------------
/**
    @brief Create new context. 

    - 24-Jan-00   floh    created
*/
//-------------------------------------------------------------------
nParticleContext *nParticleServer::new_context(int key)
{   
    // if there's no key array yet, allocate one
    if (!this->ctx_array) {
        this->ctx_array = new nKeyArray<nParticleContext *>(128,128);
    }

    // create and initialize context
    nParticleContext *ctx = new nParticleContext(key);
    this->ctx_list.AddTail(ctx);
    this->ctx_array->Add(key,ctx);
    return ctx;
}

//-------------------------------------------------------------------
/**
    @brief Return the particle context defined by 'key'. Allocates new
    context on demand.

    - 17-Feb-00   floh    created
    - 11-Apr-00   floh    oops, even if the context already existed,
                          a new one had be created!!!
    - 12-May-00   floh    + removed support for current context
*/
//-------------------------------------------------------------------
nParticleContext *nParticleServer::get_context(int key)
{
    if (this->ctx_array) {
        nParticleContext *ctx = NULL;
        if (this->ctx_array->Find(key,ctx)) return ctx;
    }

    // context doesn't exist, or no contexts yet at all.
    return this->new_context(key);
}

//-------------------------------------------------------------------
/**
    @brief Throw away all allocated contexts.
    
    Should be done at the end of a "game session", so that contexts
    don't pile up.

    - 17-Feb-00   floh    created
*/
//-------------------------------------------------------------------
void nParticleServer::EvictContexts(void)
{
    nParticleContext *ctx;
    while ((ctx = (nParticleContext *) this->ctx_list.RemHead())) delete ctx;
    if (this->ctx_array) {
        delete this->ctx_array;
        this->ctx_array = NULL;
    }
}

//-------------------------------------------------------------------
/**
    @brief Add a new particle system to particle context.

    - 17-Feb-00   floh    created
    - 01-Mar-00   floh    + speed and accel vectors
    - 14-Mar-00   floh    + speed and acceleration removed...
    - 11-Apr-00   floh    + added zombietime parameter
                          + returns pointer to new psystem
    - 12-May-00   floh    + removed 'current context'
*/
//-------------------------------------------------------------------
nPSystem *nParticleServer::AddSystem(int ctx_key, int emit_key, float tstamp, int num_particles)
{
    nParticleContext *ctx = this->get_context(ctx_key);
    n_assert(ctx);
    return ctx->AddPSystem(emit_key,tstamp,num_particles);
}

//-------------------------------------------------------------------
/**
    @brief Return particle system identified by context and emitter key.

    Return NULL if particle system doesn't exist.

    - 17-Feb-00   floh    created
    - 12-May-00   floh    + removed 'current context'
*/
//-------------------------------------------------------------------
nPSystem *nParticleServer::GetSystem(int ctx_key, int emit_key)
{
    nParticleContext *ctx = this->get_context(ctx_key);
    n_assert(ctx);
    return ctx->GetPSystem(emit_key);
}

//-------------------------------------------------------------------
/**
    @brief Remove the particle system from context, keep context
    even if it's empty.

    - 18-Jan-01   floh    created
*/
//-------------------------------------------------------------------
void
nParticleServer::RemSystem(int ctx_key, int emit_key)
{
    nParticleContext *ctx = this->get_context(ctx_key);
    n_assert(ctx);
    ctx->RemPSystem(emit_key);
}

//-------------------------------------------------------------------
/**
    - 25-Jan-00   floh    created
*/
//-------------------------------------------------------------------
int nParticleServer::GetUniqueSystemKey(void)
{
    return this->psystem_key++;
}

//-------------------------------------------------------------------
/**
    @brief Update all existing particle systems.

    - 24-Jan-00   floh    created
    - 11-Jan-00   floh    kill all contexts which dont contain
                          any living particle systems at all...
*/
//-------------------------------------------------------------------
void nParticleServer::Trigger(void)
{
    // evict all particle systems (and contexts) which have not been
    // updated last frame...
    nParticleContext *ctx;
    nParticleContext *next_ctx;
    for (ctx = (nParticleContext *) this->ctx_list.GetHead();
         ctx;
         ctx = next_ctx)
    {
        next_ctx = (nParticleContext *) ctx->GetSucc();
        ctx->EvictOrphanedPSystems();
        
        // kill context if it no longer contains any particle systems
        if (ctx->GetNumPSystems()==0) {
            // this context is empty, kill it!
            this->ctx_array->Rem(ctx->GetKey());
            ctx->Remove();
            delete ctx;
        }
    }
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------

