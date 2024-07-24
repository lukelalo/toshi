#define N_IMPLEMENTS nLXInputServer
//--------------------------------------------------------------------
//  nlxinp_main.cc
//  (C) 1999 A.Weissflog
//--------------------------------------------------------------------
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/joystick.h>

#include "kernel/ntypes.h"
#include "kernel/nkernelserver.h"
#include "kernel/nenv.h"
#include "input/nlxinputserver.h"

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
    n_assert(this->fp < 0);
    
    char fs_name[N_MAXPATH];
    char neb_name[N_MAXPATH];
    sprintf(fs_name,"/dev/js%d",joy_num);
    sprintf(neb_name,"/sys/share/input/devs/joy%d",joy_num);
    
    this->fp = open(fs_name,(O_RDONLY|O_NONBLOCK));
    if (fp >= 0) {
        // request properties
        char c_num_axis;
        char c_num_buttons;
        int version;
        char buf[N_MAXPATH];
        
        // number of axis...
        if ((ioctl(this->fp,JSIOCGAXES,&c_num_axis)<0)) {
            n_printf("ioctl(JSIOCGAXES) failed!\n");
            goto fail;
        }
        // number of buttons
        if ((ioctl(this->fp,JSIOCGBUTTONS,&c_num_buttons)<0)) {
            n_printf("ioctl(JSIOCGBUTTONS) failed!\n");
            goto fail;
        }
        // version...
        if ((ioctl(this->fp,JSIOCGVERSION,&version)<0)) {
            n_printf("ioctl(JSIOCGVERSION) failed!\n");
            goto fail;
        }
        // device name...
        if ((ioctl(this->fp,JSIOCGNAME(sizeof(buf)),buf)<0)) {
            n_strncpy2(buf,"Unknown",sizeof(buf));
        }

        // initialize object...
        this->num_axis   = c_num_axis;
        this->num_btns   = c_num_buttons;
        this->neb_dev_id = neb_devid;
        this->buf_size   = this->num_axis * 2;
        this->val_buf    = (float *) n_malloc(this->buf_size * sizeof(float));
        memset(this->val_buf,0,this->buf_size * sizeof(float));

        n_printf("found joystick device '%s':\n",fs_name);
        n_printf("  name:    %s\n",buf);
        n_printf("  version: %d\n",version);
        n_printf("  buttons: %d\n",this->num_btns);
        n_printf("  axes:    %d\n",this->num_axis);
        
        // export properties into Nebula filesystem
        nRoot *o;
        nEnv *env;
        
        o = ks->New("nroot",neb_name);
        ks->PushCwd(o);
        env = (nEnv *) ks->New("nenv","name");
        env->SetS(buf);
        env = (nEnv *) ks->New("nenv","version");
        env->SetI(version);
        o = ks->New("nroot","channels");
        ks->PushCwd(o);
        
        // each joystick axis generates 2 Nebula axis events
        int i;
        for (i=0; i<this->num_axis; i++) {
            char *ax_name;
            switch(i) {
                case 0: ax_name="x"; break;
                case 1: ax_name="y"; break;
                case 2: ax_name="z"; break;
                case 3: ax_name="r"; break;
                case 4: ax_name="s"; break;
                case 5: ax_name="t"; break;
                case 6: ax_name="u"; break;
                case 7: ax_name="v"; break;
                case 8: ax_name="w"; break;
                default: ax_name="a"; break; 
            }
            
            // negative axis...
            sprintf(buf,"-%s",ax_name);
            env = (nEnv *) ks->New("nenv",buf);
            sprintf(buf,"devid=%d type=%d axis=%d",neb_devid,(int)N_INPUT_AXIS_MOVE,(2*i)+0);
            env->SetS(buf);
            
            // positive axis...
            sprintf(buf,"+%s",ax_name);
            env = (nEnv *) ks->New("nenv",buf);
            sprintf(buf,"devid=%d type=%d axis=%d",neb_devid,(int)N_INPUT_AXIS_MOVE,(2*i)+1);
            env->SetS(buf);
            
            // negativer Achsen-Button
            sprintf(buf,"-%sbtn",ax_name);
            env = (nEnv *) ks->New("nenv",buf);
            sprintf(buf,"devid=%d type=%d btn=%d",
                    neb_dev_id,(int)N_INPUT_BUTTON_DOWN,this->num_btns+(2*i)+0);
            env->SetS(buf);

            // positiver Achsen-Button
            sprintf(buf,"+%sbtn",ax_name);
            env = (nEnv *) ks->New("nenv",buf);
            sprintf(buf,"devid=%d type=%d btn=%d",
                    neb_dev_id,(int)N_INPUT_BUTTON_DOWN,this->num_btns+(2*i)+1);
            env->SetS(buf);
        }
        
        // export buttons...
        for (i=0; i<this->num_btns; i++) {
            sprintf(buf,"btn%d",i);
            env = (nEnv *) ks->New("nenv",buf);
            sprintf(buf,"devid=%d type=%d btn=%d",neb_devid,(int)N_INPUT_BUTTON_DOWN,i);
            env->SetS(buf);
        }
        ks->PopCwd();
        ks->PopCwd();
        return true;
    }                        
               
fail:
    if (this->fp >= 0) {
        close(this->fp);
        this->fp = 0;
    }
    return false;
} 

//--------------------------------------------------------------------
//  nJoystick::Read()
//  16-Dec-99   floh    created
//--------------------------------------------------------------------
void nJoystick::Read(nInputServer *is)
{
    n_assert(this->fp >= 0);
    struct js_event e;
    while (read(this->fp, &e, sizeof(e)) > 0) {
        if (e.type & JS_EVENT_AXIS) {
            // axis has been moved
            float val = ((float)e.value) / 32767.0;
            if (val < -1.0f)      val=-1.0f;
            else if (val > +1.0f) val=+1.0f;
            int neg_axis_num = (e.number*2)+0;
            int pos_axis_num = (e.number*2)+1;
            nInputEvent *neg_axis_ie = is->NewEvent();
            nInputEvent *pos_axis_ie = is->NewEvent();
            if (neg_axis_ie) {
               neg_axis_ie->SetType(N_INPUT_AXIS_MOVE);
               neg_axis_ie->SetDeviceId(this->neb_dev_id);
               neg_axis_ie->SetAxis(neg_axis_num);
               neg_axis_ie->SetAxisValue((val < 0.0f) ? -val : 0.0f);
               is->LinkEvent(neg_axis_ie);
            }
            if (pos_axis_ie) {
                pos_axis_ie->SetType(N_INPUT_AXIS_MOVE);
                pos_axis_ie->SetDeviceId(this->neb_dev_id);
                pos_axis_ie->SetAxis(pos_axis_num);
                pos_axis_ie->SetAxisValue((val > 0.0f) ? +val : 0.0f);
                is->LinkEvent(pos_axis_ie);
            }

            // generate axis button events
            if (neg_axis_ie->GetAxisValue() > 0.0f) {
                if (this->GetBufVal(neg_axis_num)==0.0f) {
                    nInputEvent *ie = is->NewEvent();
                    if (ie) {
                        ie->SetType(N_INPUT_BUTTON_DOWN);
                        ie->SetDeviceId(this->neb_dev_id);
                        ie->SetButton(neg_axis_num + this->num_btns);
                        is->LinkEvent(ie);
                    }
                }
            } else {
                if (this->GetBufVal(neg_axis_num)>0.0f) {
                    nInputEvent *ie = is->NewEvent();
                    if (ie) {
                        ie->SetType(N_INPUT_BUTTON_UP);
                        ie->SetDeviceId(this->neb_dev_id);
                        ie->SetButton(neg_axis_num + this->num_btns);
                        is->LinkEvent(ie);
                    }
                }
            }
            if (pos_axis_ie->GetAxisValue() > 0.0f) {
                if (this->GetBufVal(pos_axis_num)==0.0f) {
                    nInputEvent *ie = is->NewEvent();
                    if (ie) {
                        ie->SetType(N_INPUT_BUTTON_DOWN);
                        ie->SetDeviceId(this->neb_dev_id);
                        ie->SetButton(pos_axis_num + this->num_btns);
                        is->LinkEvent(ie);
                    }
                }
            } else {
                if (this->GetBufVal(pos_axis_num)>0.0f) {
                    nInputEvent *ie = is->NewEvent();
                    if (ie) {
                        ie->SetType(N_INPUT_BUTTON_UP);
                        ie->SetDeviceId(this->neb_dev_id);
                        ie->SetButton(pos_axis_num + this->num_btns);
                        is->LinkEvent(ie);
                    }
                }
            }
            this->SetBufVal(neg_axis_num, neg_axis_ie->GetAxisValue());
            this->SetBufVal(pos_axis_num, pos_axis_ie->GetAxisValue());

        } else if (e.type & JS_EVENT_BUTTON) {
            // button pressed or released
            nInputEvent *ie = is->NewEvent();
            if (ie) {
                if (e.value == 0) ie->SetType(N_INPUT_BUTTON_UP);
                else              ie->SetType(N_INPUT_BUTTON_DOWN);
                ie->SetDeviceId(this->neb_dev_id);
                ie->SetButton(e.number);
                is->LinkEvent(ie);
            }
        }
    }
}

//--------------------------------------------------------------------
//  nLXInputServer()
//  05-Jun-99   floh    created
//--------------------------------------------------------------------
nLXInputServer::nLXInputServer()
{
    this->js0.Init(ks,0,N_INPUT_JOYSTICK(0));
    this->js1.Init(ks,1,N_INPUT_JOYSTICK(1));
}

//--------------------------------------------------------------------
//  ~nLXInputServer()
//  05-Jun-99   floh    created
//--------------------------------------------------------------------
nLXInputServer::~nLXInputServer()
{ }

//--------------------------------------------------------------------
//  Trigger()
//  Liest alle Events aus den Joystick-Devices und generiert
//  Nebula-Input-Events.
//  05-Jun-99   floh    created
//--------------------------------------------------------------------
void nLXInputServer::Trigger(double time)
{
    if (this->js0.IsValid()) this->js0.Read(this);
    if (this->js1.IsValid()) this->js1.Read(this);
    nInputServer::Trigger(time);
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
