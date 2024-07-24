//------------------------------------------------------------------------------
/* Copyright (c) 2003 Leaf Garland.
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
//-----------------------------------------------------------------------------
#ifndef N_AREAEMITTER_H
#define N_AREAEMITTER_H
//--------------------------------------------------------------------
/**
    @class nAreaEmitter

    @brief emit particles from an area
*/
//--------------------------------------------------------------------

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#ifndef N_PSERVEMITTER_H
#include "particle/npservemitter.h" 
#endif

//--------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nAreaEmitter
#include "kernel/ndefdllclass.h"
//--------------------------------------------------------------------
class nGfxServer;
class nMathServer;
class N_DLLCLASS nAreaEmitter : public nPServEmitter {
    nAutoRef<nGfxServer>      ref_gs;
    nAutoRef<nMathServer>     ref_ms;
    int rand_key;   // random number key

    // holds area size * 0.5 for convenience.
    float areaWidth;
    float areaHeight;
    float areaDepth;
    
public:
    static nKernelServer *kernelServer;

    nAreaEmitter();
    virtual ~nAreaEmitter();
    virtual void Compute(nSceneGraph2* sceneGraph);
    virtual bool SaveCmds(nPersistServer* fileServer);
    
    /// Set the area that particles can be emitted from.
    void SetAreaSize(float w, float h, float d);
    /// Get the area that particles can be emitted from.
    void GetAreaSize(float& w, float& h, float& d);
};

//------------------------------------------------------------------------------
/**
*/
inline void nAreaEmitter::SetAreaSize(float w, float h, float d) {
    this->areaWidth = w * 0.5f;
    this->areaHeight = h * 0.5f;
    this->areaDepth = d * 0.5f;
}

//------------------------------------------------------------------------------
/**
*/
inline void nAreaEmitter::GetAreaSize(float& w, float& h, float& d) {
    w = this->areaWidth * 2.0f;
    h = this->areaHeight * 2.0f;
    d = this->areaDepth * 2.0f;
}

//------------------------------------------------------------------------------
#endif
