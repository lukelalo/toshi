#ifndef N_BLOB_H
#define N_BLOB_H
//--------------------------------------------------------------------
/**
    @class nBlob

    @brief Container for strange datastructures.

    An nBlob embeds any block of data into itself.  It was
    originally used for embedding the obscure Description
    datastructures of DirectDraw/Direct3D, without putting
    each individual bit of information into an nEnv.  It
    is here in the hope that it might be useful to others.
*/
//--------------------------------------------------------------------
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif
//--------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nBlob
#include "kernel/ndefdllclass.h"
//--------------------------------------------------------------------
class N_DLLCLASS nBlob : public nRoot {
public:
    static nClass *local_cl;
    static nKernelServer *ks;
private:
    void *blob;
    int size;
public:
    nBlob();
    ~nBlob();
    /// Copy the data from the address given.
    virtual bool Set(void *, int);
    /// Return a reference to the copy of the data.
    virtual void *Get(void);
};
//--------------------------------------------------------------------
#endif
