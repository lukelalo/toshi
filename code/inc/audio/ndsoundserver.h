#ifndef N_DSOUNDSERVER_H
#define N_DSOUNDSERVER_H
//------------------------------------------------------------------------------
/**
	@class nDSoundChannel
    @ingroup NebulaAudioModule
	@brief A shared sound stream

	The nDSoundServer allocates a fixed number of nDSoundChannel 
	objects. nDSoundChannel objects stream data from a wave file
	into a secondary DirectSound buffer. Normally, nDSoundChannel
	objects continue to play between frames, however, a new
	sound may intercept the old one at any time, which
	leads to a stream exception. The data from the new file
	will be read immediately at the next possible buffer position
	(DirectSounds write cursor).

	The wave file object may be shared among several channels,
	thats why channels cache the last seek position and will
	update the wave file objects seek position any time new
	data needs to be fed from the file.

*/

#ifndef N_AUDIOSERVER_H
#include "audio/naudioserver.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#ifndef N_SOUND_H
#include "audio/nsound.h"
#endif

#ifndef N_LISTENER_H
#include "audio/nlistener.h"
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <mmreg.h>
#include <dsound.h>

#undef N_DEFINES
#define N_DEFINES nDSoundServer
#include "kernel/ndefdllclass.h"

//-------------------------------------------------------------------
class nDSoundServer;
class nDSoundChannel {
    bool is_initialized;
    bool is_active;
    IDirectSoundBuffer*     dsbuf7;
    IDirectSoundBuffer8*    dsbuf8;
    IDirectSound3DBuffer8*  ds3dbuf8;
    
	/// the sound currently attached to this channel
    nSound sound;
	/// position in wave file (because SoundSource object may be shared)
    int filepos;                

	/// start of valid data
    int valid_start_offset;
	/// if write cursor is ahead this, stream next chunk
    int trigger_write_offset;
	/// end of valid data
    int next_write_offset;              
    enum {
        BUFFERBYTES = 65536,            /// sound buffer size in bytes
        CHUNKBYTES  = (BUFFERBYTES/4),  /// chunk size in bytes
    };
    
public:
    nDSoundChannel() 
        : is_initialized(false),
          is_active(false),
          dsbuf7(NULL),
          dsbuf8(NULL),
          ds3dbuf8(NULL),
          filepos(0),
          valid_start_offset(0),
          trigger_write_offset(0),
          next_write_offset(0)
    { };
    ~nDSoundChannel();
    bool Initialize(nDSoundServer *dsound);
	/// call once per frame
    void Update(void);
	/// attach new sound source
    void AttachSound(nSound *);
	/// refresh with new values
    void UpdateSound(nSound *);
	/// check if attached sound source is identical
    bool IsIdenticalSound(nSound *);
	/// check if the given source has a smaller timestamp
    bool IsOlder(nSound *);                     
    bool IsActive(void) { return this->is_active; };
	/// clear channel, stop playback and remove sound source
    void Clear(void);                           

private:
    void read_ahead(int at_position);
    bool inside(int pos, int start, int end);
    void update_dsbuffer(void);
};

/**
	@class nDSoundServer
	@brief nDSoundServer -- audio server on top of DirectSound3D derived from nAudioSever
	
    @deprecated nDSoundServer is deprecated in favor of nDSoundServer2 which works with nAudioServer2.
*/
//-------------------------------------------------------------------
class N_DLLCLASS nDSoundServer : public nAudioServer {
public:

    /// various pointers to Nebula objects...
    nAutoRef<nEnv>  ref_hwnd;   // '/sys/env/hwnd'

    // dsound pointers
    HWND hwnd;                                  // HWND of app window (comes from gfx server)
    IDirectSound8 *ds;                          // pointer to dsound object
    IDirectSoundBuffer *ds_prim;                // pointer to primary sound buffer
    IDirectSound3DListener8 *ds_listener;       // pointer to 3d listener interface of primary buffer
    DSCAPS caps;                                // the DirectSound object's caps bits

    int num_channels;
    nDSoundChannel *channels;
    int cur_candidates;
    nSound *candidates;
    nListener listener;                 

public:
    nDSoundServer();
    virtual ~nDSoundServer();

    virtual bool OpenAudio(void);
    virtual void CloseAudio(void);
    virtual void SetMasterVolume(float);

    virtual void BeginScene(void);
    virtual void SetListener(nListener *);
    virtual void SetSound(nSound *);
    virtual void StopSound(nSound *);
    virtual void EndScene(void);
private:
    void insert_candidate(nSound *snd);
    nSound *get_lowpri_candidate(void);
    void update_listener(void);
};
//-------------------------------------------------------------------
#endif
    
