#define N_IMPLEMENTS nAudioServer
//-------------------------------------------------------------------
//  naudio_main.cc
//  (C) 2000 RadonLabs -- A.Weissflog
//-------------------------------------------------------------------
#include "audio/naudioserver.h"
#include "audio/nstream.h"

//-------------------------------------------------------------------
/**
    15-May-00   floh    created
*/
//-------------------------------------------------------------------
nAudioServer::nAudioServer()
: ref_rsrc(this)
{
    this->unique_id = 0;
    this->audio_open = false;
    this->in_begin_scene = false;
    this->master_volume = 1.0f;
    this->ref_rsrc = ks->New("nroot","/sys/share/sound");
    this->SetMode("dev(0)-bps(16)-hz(22050)-chn(8)-speakers(stereo)");
}

//-------------------------------------------------------------------
/**
    15-May-00   floh    created
*/
//-------------------------------------------------------------------
nAudioServer::~nAudioServer()
{
    if (this->ref_rsrc.isvalid()) this->ref_rsrc->Release();
}

//-------------------------------------------------------------------
/**
    23-Jun-00   floh    created
*/
//-------------------------------------------------------------------
void nAudioServer::SetMasterVolume(float f)
{
    if (f > 1.0f)       f = 1.0f;
    else if (f < 0.0f)  f = 0.0f;
    this->master_volume = f;
}

//-------------------------------------------------------------------
/**
    23-Jun-00   floh    created
*/
//-------------------------------------------------------------------
float nAudioServer::GetMasterVolume(void)
{
    return this->master_volume;
}

//-------------------------------------------------------------------
/**
    Isolate a mode tag string and value string from a string of the form:

       tag(value)-tag(value)-...
*/
//-------------------------------------------------------------------
const char *nAudioServer::getModeTag(const char *desc,
                                     const char *key,
                                     char *buf,
                                     int buf_size)
{
    char tmp[N_MAXPATH];
    char *p = tmp;
    char *frag;
    n_strncpy2(tmp,desc,sizeof(tmp));
    while ((frag = strtok(p,"-"))) {
        char *open_brace = strchr(frag,'(');
        char *close_brace = strchr(frag,')');
        char *arg = NULL;
        if (p) p=NULL;
        if (open_brace) {
            *open_brace=0;
            arg = open_brace+1;
        }
        if (close_brace) *close_brace=0;
        if (strcmp(key,frag)==0) {
            if (arg) n_strncpy2(buf,arg,buf_size);
            else     buf[0] = 0;
            return buf;
        }
    }
    return NULL;
}                        

//-------------------------------------------------------------------
/**
    The following mode descriptors are valid:
       - dev(int)        - device number
       - bps(int)        - bits per sample
       - chn(int)        - num channels
       - hz(int)         - sampling rate in Hz
       - speakers(head|mono|stereo|quad|surround)   - speaker config

    15-May-00   floh    created
*/
//-------------------------------------------------------------------
void nAudioServer::SetMode(const char *mode)
{
    n_assert(mode);
    const char *s;
    char buf[N_MAXPATH];

    // analyze mode string
    s = this->getModeTag(mode,"dev",buf,sizeof(buf));
    if (s) this->mode_dev = atoi(buf);
    s = this->getModeTag(mode,"bps",buf,sizeof(buf));
    if (s) this->mode_bps = atoi(buf);
    s = this->getModeTag(mode,"hz",buf,sizeof(buf));
    if (s) this->mode_hz = atoi(buf);
    s = this->getModeTag(mode,"chn",buf,sizeof(buf));
    if (s) this->mode_channels = atoi(buf);
    s = this->getModeTag(mode,"speakers",buf,sizeof(buf));
    if (s) n_strncpy2(this->mode_speaker_config,buf,sizeof(this->mode_speaker_config));

    // adjust frequency to valid values
    if      (this->mode_hz < 11025+((22050-11025)>>1)) this->mode_hz = 11025;
    else if (this->mode_hz < 22050+((44100-22050)>>1)) this->mode_hz = 22050;
    else this->mode_hz = 44100;

    // adjust bps to valid values
    if      (this->mode_bps < 12) this->mode_bps = 8;
    else this->mode_bps = 16;

    sprintf(this->mode_string,"dev(%d)-bps(%d)-hz(%d)-chn(%d)-speakers(%s)",
            this->mode_dev,this->mode_bps,this->mode_hz,this->mode_channels,
            this->mode_speaker_config);
}

//-------------------------------------------------------------------
/**
    15-May-00   floh    created
*/
//-------------------------------------------------------------------
const char *nAudioServer::GetMode(void)
{
    return this->mode_string;
}

//-------------------------------------------------------------------
/**
    15-May-00   floh    created
*/
//-------------------------------------------------------------------
bool nAudioServer::OpenAudio(void)
{
    n_assert(!this->audio_open);
    this->audio_open = true;
    return true;
}

//-------------------------------------------------------------------
/**
    15-May-00   floh    created
*/
//-------------------------------------------------------------------
void nAudioServer::CloseAudio(void)
{
    n_assert(this->audio_open);
    this->audio_open = false;
}

//-------------------------------------------------------------------
/**
    15-May-00   floh    created
*/
//-------------------------------------------------------------------
void nAudioServer::BeginScene(void)
{
    n_assert(!this->in_begin_scene);
    this->in_begin_scene = true;
}

//-------------------------------------------------------------------
/**
    26-May-00   floh    created
*/
//-------------------------------------------------------------------
void nAudioServer::SetSound(nSound *)
{ }

//-------------------------------------------------------------------
/**
    26-May-00   floh    created
*/
//-------------------------------------------------------------------
void nAudioServer::StopSound(nSound *)
{ }

//-------------------------------------------------------------------
/**
    24-May-00   floh    created
*/
//-------------------------------------------------------------------
void nAudioServer::SetListener(nListener *)
{ }

//-------------------------------------------------------------------
/**
    15-May-00   floh    created
*/
//-------------------------------------------------------------------
void nAudioServer::EndScene(void)
{
    n_assert(this->in_begin_scene);
    this->in_begin_scene = false;
}

//-------------------------------------------------------------------
/**
    19-May-00   floh    created
*/
//-------------------------------------------------------------------
int nAudioServer::GetUniqueId(void)
{
    return ++this->unique_id;
}

//-------------------------------------------------------------------
/**
    @brief Get a resource id string for a file path.

    This is simply the last 32 chars of the path.  

    26-May-00   floh    created
*/
//-------------------------------------------------------------------
const char *nAudioServer::getResourceId(const char *name, char *buf, ulong buf_size)
{
    n_assert(name);

    // copy the last 32 chars
    char c;
    char *str;
    int len = strlen(name)+1;
    int off = len - buf_size;
    if (off < 0) off = 0;
    len -= off;
    strcpy(buf,&(name[off]));

    // replace all evil chars with underscores
    str = buf;
    while ((c = *str)) {
        if ((c=='.')||(c=='/')||(c=='\\')||(c==':')) *str='_';
        str++;
    }
    return buf;
}

//-------------------------------------------------------------------
/**
    @brief Create a new, possibly shared, stream object .

    FIXME: currently hardcoded to nWavStream objects.

    26-May-00   floh    created
    28-Sep-00   floh    PushCwd()/PopCwd()
*/
//-------------------------------------------------------------------
nStream *nAudioServer::NewStream(const char *stream_name, bool looping)
{
    n_assert(stream_name);

    // create a new possibly shared stream object
    nStream *stream;
    char resid[N_MAXNAMELEN];
    this->getResourceId(stream_name,resid,sizeof(resid));
    ks->PushCwd(this->ref_rsrc.get());
    stream = (nStream *) ks->Lookup(resid);
    if (stream) {
        stream->AddRef();
    } else {
        // stream doesn't exist yet, create a new one
        stream = (nStream *) ks->New("nwavstream",resid);
        if (!stream->Open(stream_name,looping ? nStream::N_STREAM_LOOPING : 0)) {
            n_printf("Could not open stream file '%s'!\n",stream_name);
            stream->Release();
            stream = NULL;
        }
    }
    ks->PopCwd();
    return stream;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
