#ifndef N_AUDIOSERVER_H
#define N_AUDIOSERVER_H

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

//-------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nAudioServer
#include "kernel/ndefdllclass.h"

//-------------------------------------------------------------------
class nSound;
class nListener;
class nStream;
/**
  @class nAudioServer
  @ingroup NebulaAudioModule
  @brief Baseclass for management of 3d audio playback

  @deprecated This has been deprecated in favor of nAudioServer2.
*/
class N_PUBLIC nAudioServer : public nRoot {
protected:
    bool audio_open;
    bool in_begin_scene;
    int unique_id;
	/// device number
    int mode_dev;
	/// bits per sample
    int mode_bps;
	/// sample rate in Hz
    int mode_hz;
	/// number channels
    int mode_channels;
    float master_volume;
    char mode_speaker_config[64];
    char mode_string[N_MAXPATH];
    nRef<nRoot> ref_rsrc;

    enum {
        N_MAXCHANNELS = 32,
    };

public:
    static nClass *local_cl;
    static nKernelServer *ks;

    nAudioServer();
    virtual ~nAudioServer();

	/// Set audio mode (bps, sample rate, etc.)
    virtual void SetMode(const char *);
	/// Return audio mode (bps, sample rate, etc.)
    virtual const char *GetMode(void);
	/// Set the master volume
    virtual void SetMasterVolume(float);
	/// Return the set master volume
    virtual float GetMasterVolume(void);
    virtual bool OpenAudio(void);
    virtual void CloseAudio(void);
	/// Return a unique id value as an integer
    virtual int GetUniqueId(void);

    virtual void BeginScene(void);
    virtual void SetListener(nListener *);
    virtual void SetSound(nSound *);
    virtual void StopSound(nSound *);
    virtual void EndScene(void);

	/// Create a new, possibly shared, stream object .
    virtual nStream *NewStream(const char *stream_name, bool looping);

protected:
	/// Isolate a mode tag string and value string from a given string
    const char *getModeTag(const char *, const char *, char *, int);
	/// Get a resource id string for a file path.
    const char *getResourceId(const char *, char *, ulong);
};
//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
#endif
