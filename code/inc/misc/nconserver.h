#ifndef N_CONSERVER_H
#define N_CONSERVER_H
//--------------------------------------------------------------------
/**
    @class nConServer

    @brief Nebula console server

    The console server can be found at "/sys/servers/console", it 
    implements an interactive console. 
    For the console server to be functioning, the following objects 
    have to exist: 
     - /sys/servers/input 
     - /sys/servers/gfx 
     - /sys/servers/script 
       
    When the console is open, each line of key input is passed to the
    script server.
       
    The console server should be triggered before all other
    input-processing objects, so that it can get all key
    input events when the console is open.
*/
//--------------------------------------------------------------------
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_LINEBUFFER_H
#include "util/nlinebuffer.h"
#endif

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#ifndef N_INPUTEVENT_H
#include "input/ninputevent.h"
#endif

//--------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nConServer
#include "kernel/ndefdllclass.h"
//--------------------------------------------------------------------
class nInputServer;
class nGfxServer;
class nScriptServer;
class N_DLLCLASS nConServer : public nRoot {
public:
    static nClass *local_cl;
    static nKernelServer *ks;
private:
    nAutoRef<nInputServer>  ref_is;
    nAutoRef<nGfxServer>    ref_gs;
    nAutoRef<nScriptServer> ref_ss;
    nClass *cl_env;
    
    bool con_open;              // true, wenn Konsole offen
    bool watch_open;            // true, wenn Watcher aktiv
    int max_screen_lines;       // Anzahl Zeilen, die max. auf den Screen passen
    int render_buf_size;        // Groesse des Renderbuffers
    char *render_buf;           // Pointer auf Renderbuffer

    int act_hist;               // aktueller History-Level
    nLineBuffer hist_buf;       // Command-History

    int  cursor_pos;            // aktuelle Cursorpos
    bool overstrike;            // Overstrike an
    bool ctrl_down;             // Ctrl-Taste gedrueckt
    int scroll_offset;          // Scroll-Offset fuer Line-History
    char input_buffer[N_MAXPATH];   // Eingabezeile, die gerade editiert wird
    char watch_pattern[N_MAXPATH];  // Variablen-Watcher-Pattern
public:
    nConServer();
    virtual ~nConServer();

    virtual void Trigger(void);
    virtual void Render(void);
    virtual bool Open(void);                // oeffne interaktive Konsole
    virtual bool Close(void);               // schliesse interaktive Konsole
    virtual void Toggle(void);              // Toggle Konsole
    virtual void Watch(const char *);       // Watcher-Anzeige, wenn Konsole zu
    virtual void Unwatch(void);             // Watcher-Anzeige aus
        
    virtual bool EditLine(nInputEvent *);
    virtual void AddCmdToHistory(const char *);
    virtual void RecallPrevCmd(void);
    virtual void RecallNextCmd(void);
    virtual void ResetHistory(void);
private:
    void renderConsole(int, int, int, int, int);
    void renderWatcher(int, int, int, int, int);
};
//--------------------------------------------------------------------
#endif
