#define N_IMPLEMENTS nDSoundServer2
//-------------------------------------------------------------------
//  ndsound_error.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "audio/ndsoundserver2.h"

//-------------------------------------------------------------------
/**
    @brief  Resolve DirectSound error code into string.

    15-May-00   floh    created
*/
//-------------------------------------------------------------------
const char *ndsound_Error(HRESULT hr)
{
    char *s;
    switch (hr) {
        case DS_NO_VIRTUALIZATION:      s="DS_NO_VIRTUALIZATION"; break;    
        case DSERR_ALLOCATED:           s="DSERR_ALLOCATED"; break;                
        case DSERR_CONTROLUNAVAIL:      s="DSERR_CONTROLUNAVAIL"; break;      
        case DSERR_INVALIDPARAM:        s="DSERR_INVALIDPARAM"; break;
        case DSERR_INVALIDCALL:         s="DSERR_INVALIDCALL"; break;
        case DSERR_GENERIC:             s="DSERR_GENERIC"; break;
        case DSERR_PRIOLEVELNEEDED:     s="DSERR_PRIOLEVELNEEDED"; break;
        case DSERR_OUTOFMEMORY:         s="DSERR_OUTOFMEMORY"; break;
        case DSERR_BADFORMAT:           s="DSERR_BADFORMAT"; break;
        case DSERR_UNSUPPORTED:         s="DSERR_UNSUPPORTED"; break;
        case DSERR_NODRIVER:            s="DSERR_NODRIVER"; break;
        case DSERR_ALREADYINITIALIZED:  s="DSERR_ALREADYINITIALIZED"; break;
        case DSERR_NOAGGREGATION:       s="DSERR_NOAGGREGATION"; break;
        case DSERR_BUFFERLOST:          s="DSERR_BUFFERLOST"; break;
        case DSERR_OTHERAPPHASPRIO:     s="DSERR_OTHERAPPHASPRIO"; break;
        case DSERR_UNINITIALIZED:       s="DSERR_UNINITIALIZED"; break;
        case DSERR_NOINTERFACE:         s="DSERR_NOINTERFACE"; break;
        case DSERR_ACCESSDENIED:        s="DSERR_ACCESSDENIED"; break;
        default: s="<UNKNOWN ERROR>"; break;
    };
    return s;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
