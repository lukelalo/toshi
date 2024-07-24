#ifndef N_MATHSERVER_H
#define N_MATHSERVER_H
//--------------------------------------------------------------------
/**
    @class  nMathServer

    @brief Optimizes operations on vector arrays.

    The nMathServer implements a software device, which controls
    operations on stencils and vector arrays. The server defines
    a special register set and operations on it, emulating a sort
    of highlevel FPU. The idea is to concentrate complex operations
    on vector arrays in a single place to handle special CPU
    instruction sets (Katmai SSE, 3DNow) with subclassed nMathServers.
    
    Register <br>
    ======== <br>
     - 1 4x4 Matrix accumulator (m0)
     - 3 Vector arrays (v0,v1,v2)

    Operations <br>
    =========== <br>
     - MatrixIdentity()    -- load identity matrix
     - MatrixSet()         -- load matrix
     - MatrixGet()         -- read matrix
     - MatrixMult()        -- multiply matrix
     - MatrixMultSimple()  -- multiply under the assumption that m14,m24,m34=0,m44=1
     - MatrixTranslate()   -- Translation
     - MatrixScale()       -- Scaling
     - MatrixRotateX()        -- Rotation
     - MatrixRotateY()
     - MatrixRotateZ()

     - VecSet()            -- define vector array
     - VecTransform()
     - VecScale()
     - VecAdd()
     - VecNorm()
*/  
//--------------------------------------------------------------------
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#undef N_DEFINES
#define N_DEFINES nMathServer
#include "kernel/ndefdllclass.h"

//--------------------------------------------------------------------
//  Vertex-Array-Definition
//--------------------------------------------------------------------
struct nVArray {
    float *ptr;
    int stride;
};

//--------------------------------------------------------------------
class N_DLLCLASS nMathServer : public nRoot {
private:
    enum {
        N_VREG_NUM = 4,         // 4 vector array registers
        N_RAND_NUM = 16*1024,   // random number buffer (64KB)
    };

    matrix44 m;
    nVArray va[N_VREG_NUM];     // vector array registers
    int vnum;                   // number of vectors in array
    int vdim;                   // dimension of array vectors
    float *rand_array;          // random number array (size is N_RAND_NUM)

public:
    static nClass *local_cl;
    static nKernelServer *ks;

    nMathServer();
    virtual ~nMathServer();

    // pseudo random numbers
    float PseudoRand(int key) {
        // force key into valid range
        key &= (N_RAND_NUM-1);
        n_assert(key>=0);
        return this->rand_array[key];
    };
    void PseudoRandVec3D(int key, vector3& v) {
        key *= 4;              // align to start of random normalized 3d vector
        int k0 = key & (N_RAND_NUM-1);
        n_assert((k0>=0) && ((k0+2)<N_RAND_NUM));
        v.set(this->rand_array[k0],this->rand_array[k0+1],this->rand_array[k0+2]);
    };

    // Matrix Operations
    virtual void MatrixIdentity(void);
    virtual void MatrixSet(matrix44&);
    virtual void MatrixGet(matrix44&);
    virtual void MatrixMult(matrix44&);
    virtual void MatrixMultSimple(matrix44&);
    virtual void MatrixTranslate(float, float, float);
    virtual void MatrixScale(float, float, float);
    virtual void MatrixRotateX(float);
    virtual void MatrixRotateY(float);
    virtual void MatrixRotateZ(float);

    // Vektor-Array-Operationen
    virtual void VecSetCommon(int num, int dim);
    virtual void VecSet(int r, float *ptr, int stride);

    //--- r1[i] = m * r0[i]
    virtual void VecTransform(int r1, int r0);

    //--- r1[i] = r0[i] * s
    virtual void VecScale(int r1, int r0, float s);

    //--- r1[i] += r0[i] * s
    virtual void VecScaleAdd(int r1, int r0, float s);

    //--- r1[i] = norm(r0[i])
    virtual void VecNorm(int r1, int r0);

    //--- r2[i] = r0[i] + ((r1[i]-r0[i])*s)
    virtual void VecIpolConst(int r2, int r1, int r0, float s);

    //--- r3[i] = r0[i] + ((r1[i]-r0[i])*r2[i])
    virtual void VecIpol(int r3, int r2, int r1, int r0); 
};
//--------------------------------------------------------------------
#endif
