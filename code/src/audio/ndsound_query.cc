#define N_IMPLEMENTS nDSoundServer
//-------------------------------------------------------------------
//  ndsound_query.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "kernel/nenv.h"
#include "misc/nblob.h"
#include "audio/ndsoundserver.h"

extern const char *ndsound_Error(HRESULT hr);

//-------------------------------------------------------------------
/**
    Callback function for DirectSound device enumeration.
    Each device gets a numbered entry under @c '/sys/share/audio'.

    15-May-00   floh    created
    28-Sep-00   floh    PushCwd()/PopCwd()
*/
//-------------------------------------------------------------------
BOOL CALLBACK ndsound_EnumDevicesCB(LPGUID lpGuid,
                                    LPCSTR lpcstrDescription,
                                    LPCSTR lpcstrModule,
                                    LPVOID lpContext)
{
    nDSoundServer *dsound = (nDSoundServer *) lpContext;
    nKernelServer *ks = nDSoundServer::ks;
    HRESULT hr;
    nRoot *root;
    nEnv  *env;
    nBlob *blob;
    char buf[N_MAXPATH];

    // generate a new device database entry
    sprintf(buf,"%d",dsound->act_device++);
    root = ks->New("nroot",buf);
    ks->PushCwd(root);

    // export device data into database
    env = (nEnv *) ks->New("nenv","desc");
    env->SetS(lpcstrDescription);
    env = (nEnv *) ks->New("nenv","module");
    env->SetS(lpcstrModule);
    blob = (nBlob *) ks->New("nblob","guid");
    blob->Set(lpGuid,sizeof(GUID));
    
    // open device and ask for caps bits, close device
    IDirectSound *tmp_ds;
    hr = DirectSoundCreate(lpGuid,&tmp_ds,NULL);
    if (FAILED(hr)) {
        n_printf("ndsound_EnumDevicesCB(): DirectSoundCreate() failed with '%s'\n",ndsound_Error(hr));
        dsound->enum_failed = true;
        return FALSE;
    }
    tmp_ds->SetCooperativeLevel(dsound->hwnd,DSSCL_EXCLUSIVE);
    DSCAPS ds_caps;
    memset(&ds_caps,0,sizeof(ds_caps));
    ds_caps.dwSize = sizeof(ds_caps);
    hr = tmp_ds->GetCaps(&ds_caps);
    if (FAILED(hr)) {
        n_printf("ndsound_EnumDevicesCB(): GetCaps() failed with '%s'\n",ndsound_Error(hr));
        dsound->enum_failed = true;
        return FALSE;
    }
    tmp_ds->Release();

    // export caps into filesystem
    blob = (nBlob *) ks->New("nblob","caps_blob");
    blob->Set(&ds_caps,sizeof(ds_caps));
    env = (nEnv *) ks->New("nenv","flags");
    DWORD f = ds_caps.dwFlags;
    buf[0] = 0;
    if (f & DSCAPS_CERTIFIED)       n_strcat(buf,"certified ",sizeof(buf));
    if (f & DSCAPS_CONTINUOUSRATE)  n_strcat(buf,"continuousrate ",sizeof(buf));
    if (f & DSCAPS_EMULDRIVER)      n_strcat(buf,"emuldriver ",sizeof(buf));
    if (f & DSCAPS_PRIMARY16BIT)    n_strcat(buf,"primary16bit ",sizeof(buf));
    if (f & DSCAPS_PRIMARY8BIT)     n_strcat(buf,"primary8bit ",sizeof(buf));
    if (f & DSCAPS_PRIMARYMONO)     n_strcat(buf,"primarymono ",sizeof(buf));
    if (f & DSCAPS_PRIMARYSTEREO)   n_strcat(buf,"primarystereo ",sizeof(buf));
    if (f & DSCAPS_SECONDARY16BIT)  n_strcat(buf,"secondary16bit ",sizeof(buf));
    if (f & DSCAPS_SECONDARY8BIT)   n_strcat(buf,"secondary8bit ",sizeof(buf));
    if (f & DSCAPS_SECONDARYMONO)   n_strcat(buf,"secondarymono ",sizeof(buf));
    if (f & DSCAPS_SECONDARYSTEREO) n_strcat(buf,"secondarystereo ",sizeof(buf));
    env->SetS(buf);

    env = (nEnv *) ks->New("nenv","min_sec_rate");
    env->SetI(ds_caps.dwMinSecondarySampleRate);
    env = (nEnv *) ks->New("nenv","max_sec_rate");
    env->SetI(ds_caps.dwMaxSecondarySampleRate);
    env = (nEnv *) ks->New("nenv","max_hwmixing_all_buffers");
    env->SetI(ds_caps.dwMaxHwMixingAllBuffers);
    env = (nEnv *) ks->New("nenv","max_hwmixing_static_buffers");
    env->SetI(ds_caps.dwMaxHwMixingStaticBuffers);
    env = (nEnv *) ks->New("nenv","max_hwmixing_streaming_buffers");
    env->SetI(ds_caps.dwMaxHwMixingStreamingBuffers);
    env = (nEnv *) ks->New("nenv","max_hw3d_all_buffers");
    env->SetI(ds_caps.dwMaxHw3DAllBuffers);
    env = (nEnv *) ks->New("nenv","max_hw3d_static_buffers");
    env->SetI(ds_caps.dwMaxHw3DStaticBuffers);
    env = (nEnv *) ks->New("nenv","max_hw3d_streaming_buffers");
    env->SetI(ds_caps.dwMaxHw3DStreamingBuffers);
    env = (nEnv *) ks->New("nenv","total_hwmem_bytes");
    env->SetI(ds_caps.dwTotalHwMemBytes);
    env = (nEnv *) ks->New("nenv","unlock_transferrate_hwbuffers");
    env->SetI(ds_caps.dwUnlockTransferRateHwBuffers);
    env = (nEnv *) ks->New("nenv","play_cpuoverhead_swbuffers");
    env->SetI(ds_caps.dwPlayCpuOverheadSwBuffers);

    // everything done, next please...
    ks->PopCwd();
    return TRUE;
}
 
//-------------------------------------------------------------------
/**
    Enumerate all DirectSound devices in the system and create
    a database under @c '/sys/share/audio'.

    15-May-00   floh    created
    28-Sep-00   floh    PushCwd()/PopCwd()
*/
//-------------------------------------------------------------------
bool nDSoundServer::queryDevices(void)
{
    HRESULT hr;
    this->query_called = true;
    this->act_device = 0;
    this->enum_failed = false;

    // throw away old database and allocate new one
    this->ref_devdir->Release();
    this->ref_devdir = ks->New("nroot","/sys/share/audio");

    // enumerate dsound devices...
    ks->PushCwd(ref_devdir.get());
    hr = DirectSoundEnumerate(ndsound_EnumDevicesCB,this);
    ks->PopCwd();
    if (hr != DD_OK) {
        n_printf("DirectSoundEnumerate() failed with '%s'!\n",ndsound_Error(hr));
        return false;
    }
    return !(this->enum_failed);
}

//-------------------------------------------------------------------
/**
    After queryDevice() has been called to build the database,
    call selectDevice() to select one from the database. This
    device will then be used with the next call to OpenAudio().

    The following members will be initialized:

    @verbatim
    this->ref_seldevice
    this->sel_guid
    this->sel_caps
    @endverbatim

    15-May-00   floh    created
*/
//-------------------------------------------------------------------
bool nDSoundServer::selectDevice(int n)
{
    n_assert(this->query_called);
    return true;
}


