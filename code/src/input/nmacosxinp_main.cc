#define N_IMPLEMENTS nMacOSXInputServer
//--------------------------------------------------------------------
//  nmacosxinp_main.cc
//  (C) 1999 A.Weissflog
//--------------------------------------------------------------------
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "kernel/ntypes.h"
#include "kernel/nkernelserver.h"
#include "kernel/nenv.h"
#include "input/nmacosxinputserver.h"

//--------------------------------------------------------------------
//  nJoystick::nJoystick()
//  16-Dec-99   floh    created
//--------------------------------------------------------------------
nJoystick::nJoystick()
{
    this->fp       = -1;
    this->num_axis = 0;
    this->num_btns = 0;
    this->buf_size = 0;
    this->val_buf  = NULL;
}

//--------------------------------------------------------------------
//  nJoystick::~nJoystick()
//  16-Dec-99   floh    created
//--------------------------------------------------------------------
nJoystick::~nJoystick()
{
    if (this->fp >= 0) close(this->fp);
    if (this->val_buf) n_free(this->val_buf);
}

//--------------------------------------------------------------------
//  nJoystick::IsValid()
//  16-Dec-99   floh    created
//--------------------------------------------------------------------
bool nJoystick::IsValid(void)
{
    return (this->fp >= 0) ? true : false;
} 

//--------------------------------------------------------------------
//  SetBufVal()
//  16-Dec-99   floh    created
//--------------------------------------------------------------------
void nJoystick::SetBufVal(int i, float v)
{
    n_assert(i < this->buf_size);
    n_assert(this->val_buf);
    this->val_buf[i] = v;
}

//--------------------------------------------------------------------
//  GetBufVal()
//  16-Dec-99   floh    created
//--------------------------------------------------------------------
float nJoystick::GetBufVal(int i)
{
    n_assert(i < this->buf_size);
    n_assert(this->val_buf);
    return this->val_buf[i];
}

//--------------------------------------------------------------------
//  nJoystick::Init()
//  16-Dec-99   floh    created
//  28-Sep-00   floh    + PushCwd()/PopCwd()
//--------------------------------------------------------------------
bool nJoystick::Init(nKernelServer *ks, int joy_num, int neb_devid)
{
#   warning nJoystick::Read() not implemented.
    return false;
} 

//--------------------------------------------------------------------
//  nJoystick::Read()
//  16-Dec-99   floh    created
//--------------------------------------------------------------------
void nJoystick::Read(nInputServer *is)
{
#   warning nJoystick::Read() not implemented.
}

//--------------------------------------------------------------------
//  nMacOSXInputServer()
//  05-Jun-99   floh    created
//--------------------------------------------------------------------
nMacOSXInputServer::nMacOSXInputServer()
{
#   warning nMacOSXInputServer::nMacOSXInputServer() not implemented.
}

//--------------------------------------------------------------------
//  ~nMacOSXInputServer()
//  05-Jun-99   floh    created
//--------------------------------------------------------------------
nMacOSXInputServer::~nMacOSXInputServer()
{ }

//--------------------------------------------------------------------
//  Trigger()
//  Liest alle Events aus den Joystick-Devices und generiert
//  Nebula-Input-Events.
//  05-Jun-99   floh    created
//--------------------------------------------------------------------
void nMacOSXInputServer::Trigger(double time)
{
#   warning nMacOSXInputServer::Trigger() not implemented.
    nInputServer::Trigger(time);
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
