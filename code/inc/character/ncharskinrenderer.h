#ifndef N_CHARSKINRENDERER_H
#define N_CHARSKINRENDERER_H
//------------------------------------------------------------------------------
/**
    @class nCharSkinRenderer
    
    @brief Render a weighted character skin to a dynamic vertex buffer.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#ifndef N_DYNVERTEXBUFFER_H
#include "gfx/ndynvertexbuffer.h"
#endif

//-----------------------------------------------------------------------------
class nShadowServer;
class nVertexBuffer;
class nShadowCaster;
class nCharSkeleton;
//-----------------------------------------------------------------------------
class nCharSkinRenderer
{
public:
    /// constructor
    nCharSkinRenderer(nKernelServer* kernelServer, nRoot* owner);
    /// destructor
    ~nCharSkinRenderer();
    /// initialize object
    void Initialize(nShadowServer* shadowServer,
                    nVertexBuffer* srcVertexBuffer,
                    nShadowCaster* srcShadowCaster,
                    bool readOnly);
    /// return false if Initialize() needs to be called
    bool IsValid();
    /// render skin
    void Render(const nCharSkeleton* charSkeleton, 
                nIndexBuffer* indexBuffer, 
                nVertexBuffer* srcSkin,
                nPixelShader* pixelShader,
                nTextureArray* textureArray);
    /// get pointer to internal shadow caster object
    nShadowCaster* GetShadowCaster() const;

private:
    nRef<nShadowCaster> refShadowCaster;
    nDynVertexBuffer dynVBuf;
};

//------------------------------------------------------------------------------
#endif
