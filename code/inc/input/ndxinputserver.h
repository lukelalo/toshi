#ifndef N_DXINPUTSERVER_H
#define N_DXINPUTSERVER_H
//-------------------------------------------------------------------
/**
    @class nDXInputServer
    
    @brief DirectInput InputServer

    The DirectInput input server makes all input devices visible under
    DirectInput available as input devices within Nebula.
*/
//-------------------------------------------------------------------
#ifndef N_INPUTSERVER_H
#include "input/ninputserver.h"
#endif

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#define DIRECTINPUT_VERSION 0x0700

#include <dinput.h>
//--------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nDXInputServer
#include "kernel/ndefdllclass.h"
//--------------------------------------------------------------------
class N_INLINECLASS nDIDevice : public nNode {
public:
    IDirectInputDevice7 *dev7;
    DIDEVICEINSTANCE    inst;
    DIDEVCAPS           caps;
    int nebula_dev_id;
    float min_range;
    float max_range;
private:
    int num_bufvals;
    float *valbuf;
public:
    nDIDevice() {
        dev7        = NULL;
        num_bufvals = 0;
        valbuf      = NULL;
        memset(&inst,0,sizeof(inst));
        memset(&caps,0,sizeof(caps));
    };
    ~nDIDevice() {
        if (valbuf) n_free(valbuf);
        if (dev7) { 
            dev7->Unacquire();
            dev7->Release();
        }
    };
    void SetNumAxes(int n) {
        n_assert(NULL==valbuf);
        num_bufvals = n;
        if (n>0) {
            int bsize = n*sizeof(float);
            valbuf = (float *) n_malloc(bsize);
            memset(valbuf,0,bsize);
        }
    };
    void SetAxVal(int n, float f) {
        n_assert(valbuf);
        n_assert(n < num_bufvals);
        valbuf[n] = f;
    };
    float GetAxVal(int n) {
        n_assert(valbuf);
        n_assert(n < num_bufvals);
        return valbuf[n];
    };
};
//--------------------------------------------------------------------
class nEnv;
class N_DLLCLASS nDXInputServer : nInputServer {
public:
    static nClass *local_cl;
    static nKernelServer *ks;

    enum {
        N_DINPUT_BUFSIZE   = 32,
    };

    nAutoRef<nEnv> ref_hwnd;      // ref auf '/sys/env/hwnd'
    HWND hwnd;
    IDirectInput7 *di;
    nList dev_list;    
    int act_joystick;
    int act_mouse;

    nDXInputServer();
    virtual ~nDXInputServer();
    virtual void Trigger(double time);

private:
    bool dxInit(void);
    void dxKill(void);
    void dxHwndChanged(void);
    bool exportDevices(void);
    void genAxisButtonEvents(nDIDevice* did, nInputEvent* axisIe, int btnNum);
};
//--------------------------------------------------------------------
#endif
