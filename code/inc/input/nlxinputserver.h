#ifndef N_LXINPUTSERVER_H
#define N_LXINPUTSERVER_H
//--------------------------------------------------------------------
/**
    @class  nLXInputServer
    
    @brief Linux Joystick Device Server

    Interface to the Linux joystick devices /dev/js0 and /dev/js1.
*/
//--------------------------------------------------------------------
#ifndef N_INPUTSERVER_H
#include "input/ninputserver.h"
#endif

//--------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nLXInputServer
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
class N_DLLCLASS nLXInputServer : nInputServer {
public:
    static nClass *local_cl;
    static nKernelServer *ks;

    nJoystick js0;
    nJoystick js1;
        
    nLXInputServer();
    virtual ~nLXInputServer();
    virtual void Trigger(double time);
};
//--------------------------------------------------------------------
#endif
