#ifndef N_ANIMCURVEARRAY_H
#define N_ANIMCURVEARRAY_H
//------------------------------------------------------------------------------
/**
    @class nAnimCurveArray

    @brief Holds a number of animation curves, and can load animation curves
    from .nanim and .nax files.

    .nanim file format (ascii):

    <pre class="code>
    curves [number of curves]        
    curve [name] [startkey] [numkeys] [keysPerSecond] [oneshot|loop] [step|linear|quaternion]
    key x y z w
 
    .nax file format (binary):

    HEADER {
        int32 NAX0          -> magic number
        int32 blockLen      -> 4
        int32 numCurves     -> number of curves in file
    }

    FOR EACH CURVE {

        CURVEHEADER {
            int32 CHDR              -> magic number
            int32 blockLen          -> num of following bytes in block
            int32 startKey          -> number of first key
            int32 numKeys           -> number of keys
            float keysPerSecond     -> frame rate
            char  ipolType;         -> 0 = STEP, 1 = LINEAR, 2 = QUATERNION
            char  repType           -> 0 = LOOP, 1 = ONESHOT
            char  keyType           -> 0 = VANILLA, 1 = PACKED
            char  pad0              -> padding
            int16 nameLen           -> length of following string
            char[] name             -> curve name (no terminating 0!)
        }

        CURVEDATA_VANILLA {
            int32 CDTV              -> magic number (== VANILLA CURVE DATA)
            int32 blockLen          -> number of following bytes
            float[4] data           -> curve data
            ...
        }

        CURVEDATA_PACKED {
            int32 CDTP              -> magic number (== PACKED CURVE DATA)
            int32 blockLen          -> number of following bytes
            ushort[4] data          -> curve data, can 
            ...
        }
    }
    </pre>

    (C) 2001 RadonLabs GmbH
*/
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_ANIMCURVE_H
#include "anim/nanimcurve.h"
#endif

//------------------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nAnimCurveArray
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nFileServer2;
class N_PUBLIC nAnimCurveArray : public nRoot
{
public:
    /// constructor
    nAnimCurveArray();
    /// destructor
    virtual ~nAnimCurveArray();
    /// set number of curves, reallocates curve objects
    void SetNumCurves(int num);
    /// get number of curves
    int GetNumCurves() const;
    /// get nAnimCurve object by index
    nAnimCurve& GetCurve(int index) const;
    /// save curve array to .anim file
    bool SaveAnim(nFileServer2* fs, const char* fileName);
    /// save curve array to .nax file
    bool SaveNax(nFileServer2* fs, const char* fileName);
    /// load curve array from .anim file
    bool LoadAnim(nFileServer2* fs, const char* fileName);
    /// load curve data from .nax file
    bool LoadNax(nFileServer2* fs, const char* fileName);
    /// find anim curve by name
    nAnimCurve* FindCurveByName(const char* name) const;

    static nKernelServer* kernelServer;

private:
    /// free internal anim curve array
    void Clear();

    int numCurves;
    nAnimCurve* curveArray;
    nHashList hashList;         // additional hashlist for fast find by name
};

//------------------------------------------------------------------------------
#endif
