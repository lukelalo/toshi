#define N_IMPLEMENTS nAudioServer2
//------------------------------------------------------------------------------
//  naudioserver2_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "audio/naudioserver2.h"
#include "audio/nstream.h"

nNebulaScriptClass(nAudioServer2, "nroot");

//------------------------------------------------------------------------------
/**
*/
nAudioServer2::nAudioServer2() :
    modeDevice(0),
    modeBitsPerSample(16),
    modeSamplesPerSec(22050),
    modeChannels(16),
    modeSpeakerConfig(STEREO),
    refStreams(this),
    audioOpen(false),
    inBeginScene(false),
    uniqueId(0),
    muteModulator(1.0f),
    masterVolume(1.0f),
    timeStamp(0.0),
    flushStarted(0.0),
    flushActive(false),
    hasBeenFlushed(false),
    fadeoutTimeStamp(0.0),
    fadeinTimeStamp(0.0)
{
    this->refStreams = kernelServer->New("nroot", "/sys/share/audio");
}

//------------------------------------------------------------------------------
/**
*/
nAudioServer2::~nAudioServer2()
{
    n_assert(!this->inBeginScene);
    n_assert(!this->audioOpen);
    if (this->refStreams.isvalid())
    {
        this->refStreams->Release();
        this->refStreams.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
*/
const char*
nAudioServer2::GetModeTag(const char *desc, const char *key, char *buf, int buf_size)
{
    char tmp[N_MAXPATH];
    char *p = tmp;
    char *frag;
    
    n_strncpy2(tmp,desc,sizeof(tmp));
    while ((frag = strtok(p,"-"))) 
    {
        char *open_brace = strchr(frag,'(');
        char *close_brace = strchr(frag,')');
        char *arg = NULL;
        if (p) 
        {
            p = NULL;
        }
        if (open_brace) 
        {
            *open_brace = 0;
            arg = open_brace+1;
        }
        if (close_brace) 
        {
            *close_brace = 0;
        }
        if (strcmp(key,frag) == 0) 
        {
            // Treffer...
            if (arg) n_strncpy2(buf,arg,buf_size);
            else     buf[0] = 0;
            return buf;
        }
    }
    return NULL;
}                        

//------------------------------------------------------------------------------
/**
*/
void
nAudioServer2::SetMode(const char* modeStr)
{
    // default settings
    this->modeDevice = 0;
    this->modeBitsPerSample = 16;
    this->modeSamplesPerSec = 22050;
    this->modeChannels = 16;
    this->modeSpeakerConfig = STEREO;

    // audio device...
    char buf[N_MAXNAMELEN];
    const char* str;
    str = this->GetModeTag(modeStr, "dev", buf, sizeof(buf));
    if (str)
    {
        this->modeDevice = atoi(str);
    }

    // bits per sample...
    str = this->GetModeTag(modeStr, "bps", buf, sizeof(buf));
    if (str)
    {
        this->modeBitsPerSample = atoi(str);
    }

    // samples per second...
    str = this->GetModeTag(modeStr, "hz", buf, sizeof(buf));
    if (str)
    {
        this->modeSamplesPerSec = atoi(str);
    }

    // number of channels...
    str = this->GetModeTag(modeStr, "chn", buf, sizeof(buf));
    if (str)
    {
        this->modeChannels = atoi(str);
    }

    // speaker config
    str = this->GetModeTag(modeStr, "speakers", buf, sizeof(buf));
    if (str)
    {
        if (0 == strcmp("5.1", str))
        {
            this->modeSpeakerConfig = FIVEPOINTONE;
        }
        else if (0 == strcmp("head", str))
        {
            this->modeSpeakerConfig = HEADPHONE;
        }
        else if (0 == strcmp("mono", str))
        {
            this->modeSpeakerConfig = MONO;
        }
        else if (0 == strcmp("quad", str))
        {
            this->modeSpeakerConfig = QUAD;
        }
        else if (0 == strcmp("stereo", str))
        {
            this->modeSpeakerConfig = STEREO;
        }
        else if (0 == strcmp("surround", str))
        {
            this->modeSpeakerConfig = SURROUND;
        }
        else
        {
            this->modeSpeakerConfig = STEREO;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
const char*
nAudioServer2::GetMode()
{
    // TODO!
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nAudioServer2::SetMasterVolume(float v)
{
    this->masterVolume = v;
}

//------------------------------------------------------------------------------
/**
*/
float
nAudioServer2::GetMasterVolume()
{
    return this->masterVolume;
}

//------------------------------------------------------------------------------
/**
*/
bool
nAudioServer2::OpenAudio()
{
    n_assert(!this->audioOpen);    
    this->audioOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nAudioServer2::CloseAudio()
{
    n_assert(this->audioOpen);
    this->audioOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
const char *
nAudioServer2::GetResourceId(const char* name, char* buf, int bufSize)
{
    n_assert(name);
    n_assert(buf);
    n_assert(bufSize > 0);

    // copy the last 32 chars
    char c;
    char *str;
    int len = strlen(name) + 1;
    int off = len - bufSize;
    if (off < 0) 
    {
        off = 0;
    }
    len -= off;
    strcpy(buf, &(name[off]));

    // replace all evil chars with underscores
    str = buf;
    while ((c = *str)) 
    {
        if ((c=='.')||(c=='/')||(c=='\\')||(c==':')) *str='_';
        str++;
    }
    return buf;
}


//------------------------------------------------------------------------------
/**
*/
nStream*
nAudioServer2::NewStream(const char* streamName, bool looping)
{
    n_assert(streamName);

    // check file extension
    const char* streamClass = 0;
    if (strstr(streamName, ".wav"))
    {
        streamClass = "nwavstream";
    }
    else if (strstr(streamName, ".ogg"))
    {
        streamClass = "noggstream";
    }
    if (!streamClass)
    {
        n_printf("nAudioServer2:NewStream(): unrecognized file extension: '%s' must be '.wav' or '.ogg'!\n", streamName);
        return 0;
    }

    // create a new possibly shared stream object
    nStream *stream;
    char resid[N_MAXNAMELEN];
    this->GetResourceId(streamName, resid, sizeof(resid));

    stream = (nStream *) this->refStreams->Find(resid);
    if (stream) 
    {
        stream->AddRef();
    } 
    else 
    {
        // stream doesn't exist yet, create a new one
        kernelServer->PushCwd(this->refStreams.get());
        stream = (nStream *) kernelServer->New(streamClass, resid);
        if (!stream->Open(streamName,looping ? nStream::LOOPING : 0)) 
        {
            n_printf("nAudioServer2: Could not open stream file '%s'!\n",streamName);
            stream->Release();
            stream = 0;
        }
        kernelServer->PopCwd();
    }
    return stream;
}

//------------------------------------------------------------------------------
/**
*/
void
nAudioServer2::UpdateListener(const nListener2& l)
{
    this->listener = l;
}

//------------------------------------------------------------------------------
/**
*/
void
nAudioServer2::BeginScene(double time)
{
    n_assert(this->audioOpen);
    n_assert(!this->inBeginScene);
    this->inBeginScene = true;
    this->timeStamp = time;

    // FlushSound() Exception?
    if ((this->flushActive) && (this->timeStamp < this->flushStarted))
    {
        this->flushStarted = timeStamp;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nAudioServer2::StartSound(const nSound2& /*s*/)
{
    // actual functionality must be added in subclass
    n_assert(this->inBeginScene);
}

//------------------------------------------------------------------------------
/**
*/
void
nAudioServer2::UpdateSound(const nSound2& /*s*/)
{
    // actual functionality must be added in subclass
    n_assert(this->inBeginScene);
}

//------------------------------------------------------------------------------
/**
    Stop a currently playing sound.
*/
void
nAudioServer2::StopSound(const nSound2& /*s*/)
{
    // actual functionality must be added in subclass
    n_assert(this->inBeginScene);
}

//------------------------------------------------------------------------------
/**
*/
void
nAudioServer2::EndScene()
{
    // actual functionality must be added in subclass
    n_assert(this->inBeginScene);
    this->inBeginScene = false;
}

//------------------------------------------------------------------------------
/**
    Initiate an audio queue flush.
*/
void
nAudioServer2::FlushAudio(float fadeoutTime, float fadeinTime)
{
    this->flushStarted     = this->timeStamp;
    this->flushActive      = true;
    this->hasBeenFlushed   = false;
    this->fadeoutTimeStamp = this->flushStarted + fadeoutTime;
    this->fadeinTimeStamp  = this->fadeoutTimeStamp + fadeinTime;
}

//------------------------------------------------------------------------------
/**
*/
void
nAudioServer2::MuteAudio(bool b)
{
    if (b)
    {
        this->muteModulator = 0.0f;
    }
    else
    {
        this->muteModulator = 1.0f;
    }
}

//------------------------------------------------------------------------------
/**
*/
int
nAudioServer2::GetUniqueId()
{
    return ++this->uniqueId;
}

//------------------------------------------------------------------------------
