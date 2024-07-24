#ifndef N_IMAGEFILE_H
#define N_IMAGEFILE_H
//-------------------------------------------------------------------
/**
    @class nImageFile

    @brief Superclass for image file loader/saver

    17-Dec-99   floh    interface for saving files
*/
//-------------------------------------------------------------------
#include <stdio.h>

#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_PIXELFORMAT_H
#include "gfx/npixelformat.h"
#endif

//-------------------------------------------------------------------
//  Code belongs to ngfxserver
//-------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nImageFile
#include "kernel/ndefdllclass.h"

//-------------------------------------------------------------------
class N_PUBLIC nImageFile {
protected:
    enum nAccessMode {
        N_NONE,
        N_READ,
        N_WRITE,
    };
    FILE *fp;
    int width;
    int height;
    nPixelFormat *pf;
    nAccessMode acc_mode;    
    bool open;

public:
    nImageFile();
    virtual ~nImageFile();

    // opening/closing file
    virtual bool Open(const char *, const char *mode);
    virtual void Close(void);
    virtual bool IsOpen(void);

    // read interface
    virtual int GetWidth(void);
    virtual int GetHeight(void);
    virtual nPixelFormat *GetPixelFormat(void);
    virtual uchar *ReadLine(int&);

    // write interface
    virtual void SetWidth(int);
    virtual void SetHeight(int);
    virtual void SetPixelFormat(int bpp, nPalEntry *);
    virtual void SetPixelFormat(int bpp, int r_mask, int g_mask, int b_mask, int a_mask);
    virtual int WriteLine(uchar *);
};
//-------------------------------------------------------------------
#endif
