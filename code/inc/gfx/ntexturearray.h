#ifndef N_TEXTUREARRAY_H
#define N_TEXTUREARRAY_H
//-------------------------------------------------------------------
/**
    @class nTextureArray

    @brief hold textures for a multitexture stage

    Provides a single place where all textures for a multitexture
    stage are defined. Must be handed to nPixelShader::Begin().
*/
//-------------------------------------------------------------------

#ifndef N_TEXTURE_H
#include "gfx/ntexture.h"
#endif

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#ifndef N_GFXTYPES_H
#include "gfx/ngfxtypes.h"
#endif

#ifndef N_GFXSERVER_H
#include "gfx/ngfxserver.h"
#endif

//-------------------------------------------------------------------
class nGfxServer;
class nTextureArray {
    nRef<nTexture> ref_texarray[N_MAXNUM_TEXSTAGES];

public:
    nTextureArray(nRoot *owner) {
        int i;
        for (i=0; i<N_MAXNUM_TEXSTAGES; i++) {
            ref_texarray[i].initialize(owner);
        }
    };

    ~nTextureArray() {
        int i;
        for (i=0; i<N_MAXNUM_TEXSTAGES; i++) {
            if (ref_texarray[i].isvalid()) {
                ref_texarray[i]->Release();
                ref_texarray[i].invalidate();
            }
        }
    };

    //---------------------------------------------------------------
    //  SetTexture()
    //  Called to create a new texture in the texture array. A
    //  new (possibly shared) texture object will be requested from
    //  the gfx server and initialized from the given pixel and
    //  alpha filenames. Alpha filename may be NULL if the texture
    //  has no alpha channel. Pixel filename may be NULL if the
    //  texture stage should contain no texture.
    //  The textures will NOT reload automatically if they become
    //  invalid.
    //  Both pixel_name and alpha_name must be complete absolute
    //  paths, because they are also used as unique resource ids
    //  for texture sharing.
    //
    //  Also note that if you supply an alpha channel file both the
    //  pixel and alpha file have to be BMPs.
    //
    //  16-Nov-00   floh    created
    //  13-Aug-01   floh    added high quality flag
    //---------------------------------------------------------------
    bool SetTexture(nGfxServer *gs, 
                    int stage, 
                    const char *pixel_name, 
                    const char *alpha_name,
                    bool gen_mipmaps,
                    bool highQuality)
    {
        n_assert(gs);
        n_assert((stage>=0) && (stage<N_MAXNUM_TEXSTAGES));

        // kill old texture
        if (this->ref_texarray[stage].isvalid()) 
        {
            this->ref_texarray[stage]->Release();
            this->ref_texarray[stage].invalidate();
        }

        if (pixel_name) 
        {
            // create a resource identifier that takes the alpha
            // channel name into account
            char buf[N_MAXPATH + 2];
            sprintf(buf, "%s", pixel_name);
            if (alpha_name)
            {
                strcat(buf, "_a");
            }

            // see if texture already exists and can be shared
            nTexture *tex = gs->FindTexture(buf);
        
            // if not, create a new one
            if (!tex) 
            {
                tex = gs->NewTexture(buf);
                n_assert(tex);
                tex->SetTexture(pixel_name, alpha_name);
                tex->SetGenMipMaps(gen_mipmaps);
                tex->SetHighQuality(highQuality);
                if (!tex->Load()) 
                {
                    n_printf("nTextureArray: Could not load '%s' and/or '%s'!\n",pixel_name,alpha_name);
                    tex->Release();
                    return false;
                }
            }
            this->ref_texarray[stage] = tex;
        }
        return true;
    };

    //---------------------------------------------------------------
    //  GetTexture()
    //  Get the texture defined for given stage, may return NULL
    //  if stage empty.
    //  16-Nov-00   floh    created
    //---------------------------------------------------------------
    nTexture *GetTexture(int stage) {
        n_assert((stage>=0) && (stage<N_MAXNUM_TEXSTAGES));
        if (this->ref_texarray[stage].isvalid()) return this->ref_texarray[stage].get();
        else                                     return NULL;
    };

    //---------------------------------------------------------------
    //  Compare()
    //  Compare this object with another and return value suitable
    //  for qsort().
    //---------------------------------------------------------------
    int Compare(nTextureArray *other) {
        n_assert(other);
        int i;
        for (i=0; i<N_MAXNUM_TEXSTAGES; i++) {
            int t0,t1;
            if (this->ref_texarray[i].isvalid())  t0 = int(this->ref_texarray[i].get());
            else                                  t0 = 0;
            if (other->ref_texarray[i].isvalid()) t1 = int(other->ref_texarray[i].get());
            else                                  t1 = 0;
            int d = t1-t0;
            if (d != 0) return d;
        }
        // objects are identical
        return 0;
    };
};

//-------------------------------------------------------------------
#endif
