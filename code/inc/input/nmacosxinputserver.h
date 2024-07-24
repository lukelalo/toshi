#ifndef N_MACOSXINPUTSERVER_H
#define N_MACOSXINPUTSERVER_H
//--------------------------------------------------------------------
/**
    @class  nMacOSXInputServer
    
    @brief MacOS X Joystick Device Server

    Interface to the MacOS X joystick devices.
*/
//--------------------------------------------------------------------
#ifndef N_INPUTSERVER_H
#include "input/ninputserver.h"
#endif

//--------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nMacOSXInputServer
#include "kernel/ndefdllclass.h"
//--------------------------------------------------------------------
class nJoystick {
    int fp;
    int num_axis;
    int num_btns;
    int neb_dev_id;
    int buf_size;
    float *val_buf;
public:
    nJoystick();
    ~nJoystick();
    bool Init(nKernelServer *, int, int);
    bool IsValid(void);
    void Read(nInputServer *);    
    void SetBufVal(int, float);
    float GetBufVal(int);    
};

//--------------------------------------------------------------------
class N_DLLCLASS nMacOSXInputServer : nInputServer {
public:
    static nClass *local_cl;
    static nKernelServer *ks;

    nJoystick js0;
    nJoystick js1;
        
    nMacOSXInputServer();
    virtual ~nMacOSXInputServer();
    virtual void Trigger(double time);
};
//--------------------------------------------------------------------
#endif
