#define N_IMPLEMENTS nConServer
//-------------------------------------------------------------------
//  ncsrv_main.cc
//  (C) 1999 A.Weissflog
//-------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/nenv.h"
#include "gfx/ngfxserver.h"
#include "input/ninputevent.h"
#include "input/ninputserver.h"
#include "misc/nconserver.h"
#include "kernel/nscriptserver.h"
#include "kernel/nloghandler.h"

// alle Tasten, die fuer die Konsole eine Bedeutung
// haben und nicht weitergegeben werden duerfen...
int key_killset[] = {
    N_KEY_BACK,
    N_KEY_RETURN,
    N_KEY_SHIFT,
    N_KEY_CONTROL,
    N_KEY_SPACE,
    N_KEY_END,
    N_KEY_HOME,
    N_KEY_LEFT,
    N_KEY_UP,
    N_KEY_RIGHT,
    N_KEY_DOWN,
    N_KEY_INSERT,
    N_KEY_DELETE,
    N_KEY_1,
    N_KEY_2,
    N_KEY_3,
    N_KEY_4,
    N_KEY_5,
    N_KEY_6,
    N_KEY_7,
    N_KEY_8,
    N_KEY_9,
    N_KEY_0,
    N_KEY_A,
    N_KEY_B,
    N_KEY_C,
    N_KEY_D,
    N_KEY_E,
    N_KEY_F,
    N_KEY_G,
    N_KEY_H,
    N_KEY_I,
    N_KEY_J,
    N_KEY_K,
    N_KEY_L,
    N_KEY_M,
    N_KEY_N,
    N_KEY_O,
    N_KEY_P,
    N_KEY_Q,
    N_KEY_R,
    N_KEY_S,
    N_KEY_T,
    N_KEY_U,
    N_KEY_V,
    N_KEY_W,
    N_KEY_X,
    N_KEY_Y,
    N_KEY_Z,
    N_KEY_NUMPAD0,
    N_KEY_NUMPAD1,
    N_KEY_NUMPAD2,
    N_KEY_NUMPAD3,
    N_KEY_NUMPAD4,
    N_KEY_NUMPAD5,
    N_KEY_NUMPAD6,
    N_KEY_NUMPAD7,
    N_KEY_NUMPAD8,
    N_KEY_NUMPAD9,
    N_KEY_MULTIPLY,
    N_KEY_ADD,
    N_KEY_SEPARATOR,
    N_KEY_SUBTRACT,
    N_KEY_DECIMAL,
    N_KEY_DIVIDE,
    N_KEY_NONE
};

//-------------------------------------------------------------------
/**
    - 20-Feb-99   floh    created
    - 19-Jul-99   floh    + Objekt-Pointer durch Objekt-Referenzen
                            ersetzt
*/
//-------------------------------------------------------------------
nConServer::nConServer()
          : ref_is(ks,this), ref_gs(ks,this), ref_ss(ks,this)
{
    this->ref_is = "/sys/servers/input";
    this->ref_gs = "/sys/servers/gfx";
    this->ref_ss = "/sys/servers/script";
    this->cl_env = ks->FindClass("nenv");
    this->con_open   = false;
    this->watch_open = false;
    this->max_screen_lines = 32;    // Gfx-Aufloesungs-abhaengig
    this->render_buf_size  = (64 * 1024);
    this->render_buf = (char *) n_malloc(render_buf_size);
    memset(this->input_buffer,0,sizeof(this->input_buffer));
    this->cursor_pos    = 0;
    this->overstrike    = false;
    this->ctrl_down     = false;
    this->scroll_offset = 0;
    this->act_hist      = 0;
    memset(this->watch_pattern,0,sizeof(this->watch_pattern));
}

//-------------------------------------------------------------------
/**
    - 20-Feb-99   floh    created
*/
//-------------------------------------------------------------------
nConServer::~nConServer()
{
    if (this->render_buf) n_free(this->render_buf);
}

//-------------------------------------------------------------------
/**
    - 20-Feb-99   floh    created
    - 20-Dec-99   floh    OpenConsole()->Open()
*/
//-------------------------------------------------------------------
bool nConServer::Open(void)
{
    if (this->con_open) {
        n_printf("Console already open!\n");
        return false;
    }
    this->con_open = true;
    return true;
}

//-------------------------------------------------------------------
/**
    - 20-Feb-99   floh    created
    - 20-Dec-99   floh    CloseConsole()->Close()
*/
//-------------------------------------------------------------------
bool nConServer::Close(void)
{
    if (!this->con_open) {
        n_printf("Console not open!\n");
        return false;
    }
    this->con_open = false;
    return true;
}

//-------------------------------------------------------------------
/**
    - 20-Dec-99   floh    created
*/
//-------------------------------------------------------------------
void nConServer::Toggle(void)
{
    if (this->con_open) this->Close();
    else                this->Open();
}

//-------------------------------------------------------------------
/**
    - 13-May-99   floh    created
*/
//-------------------------------------------------------------------
void nConServer::Watch(const char *pat)
{
    this->watch_open = true;
    n_strncpy2(this->watch_pattern,pat,sizeof(this->watch_pattern));
}

//-------------------------------------------------------------------
/**
    - 13-May-99   floh    created
*/
//-------------------------------------------------------------------
void nConServer::Unwatch(void)
{
    this->watch_open = false;
}

//-------------------------------------------------------------------
/**
    Schicke alle Zeilen im Linebuffer von der "aktuellen" bis
    zur neuesten, bzw. bis zur maximalen Anzahl Zeilen
    an den GfxServer.

    - 13-May-99   floh    created
    - 19-Jul-99   floh    Objekt-Pointer durch Referenzen ersetzt
*/
//-------------------------------------------------------------------
void nConServer::renderConsole(int, int, int, int h, int f_height)
{
    n_assert(this->con_open);
            
    const char *line_array[512];
    int num_lines;
    int first_line, last_line;
    int act_buf_pos;
    int l,i;
    char act_line[1024];
    char *to, *from;

    if (f_height > 0) this->max_screen_lines = ((h*4)/5) / f_height;

    num_lines = ks->GetDefaultLogHandler()->GetLineBuffer()->GetLines(line_array,512);

    first_line = this->max_screen_lines + this->scroll_offset;
    if (first_line >= num_lines) {
        first_line = num_lines-1;
        this->scroll_offset = first_line - this->max_screen_lines;
    }
    last_line  = first_line - max_screen_lines;
    if (last_line < 0) {
        scroll_offset = 0;
        last_line = 0;
    }

    act_buf_pos = 0;
    for (i=first_line; i>=last_line; i--) {
        const char *s = line_array[i];
        if (s) {
            int l = strlen(s);
            if ((act_buf_pos+l+1) < this->render_buf_size) {
                strcpy(&(this->render_buf[act_buf_pos]),s);
                act_buf_pos += l;
                this->render_buf[act_buf_pos++] = '\n';
            } else break;
        }
    }
    // hintenran muss noch die aktuelle Eingabezeile mit Cursor...
    l = strlen(this->input_buffer);
    this->ref_ss->Prompt(act_line,sizeof(act_line));
    to   = act_line + strlen(act_line);
    from = this->input_buffer;
    for (i=0; i<this->cursor_pos; i++) *to++ = *from++;
    *to++ = '_';    // das ist der Cursor
    while ((*to++ = *from++));
    if (((int)(act_buf_pos + strlen(act_line) + 1)) < this->render_buf_size) {
        strcpy(&(this->render_buf[act_buf_pos]),act_line);
    }
}

//-------------------------------------------------------------------
/**
    Macht Pattern-Matching auf alle Env-Objekte unter
    /sys/share/vars und rendert die Treffer.

    - 13-May-99   floh    created
*/
//-------------------------------------------------------------------
void nConServer::renderWatcher(int, int, int, int h, int f_height)
{
    n_assert(this->watch_open);
    n_assert(this->cl_env);
    
    nRoot *vars = ks->Lookup("/sys/var");
    if (vars) {
        char line[N_MAXPATH];
        int act_buf_pos = 0;
        nEnv *env;
                
        if (f_height > 0) this->max_screen_lines = ((h*4)/5) / f_height;

        this->render_buf[0] = 0;
        for (env = (nEnv *) vars->GetHead();
             env;
             env = (nEnv *) env->GetSucc())
        {
            if (env->IsA(this->cl_env)) {
                const char *n = env->GetName();
                if (n_strmatch(n,this->watch_pattern)) {
                    // ein Treffer...
                    switch(env->GetType()) {
                        case nArg::ARGTYPE_INT:
                            sprintf(line,"%s: %d\n",n,env->GetI());
                            break;
                        case nArg::ARGTYPE_FLOAT:
                            sprintf(line,"%s: %f\n",n,env->GetF());
                            break;
                        case nArg::ARGTYPE_STRING:
                            sprintf(line,"%s: %s\n",n,env->GetS());
                            break;
                        case nArg::ARGTYPE_BOOL:
                            sprintf(line,"%s: %s\n",n,env->GetB()?"true":"false");
                            break;
                        default:
                            sprintf(line,"%s: <unknown data type>\n",n);
                            break;
                    }
                    int l = strlen(line);
                    if ((act_buf_pos+l+1) < this->render_buf_size) {
                        strcpy(&(this->render_buf[act_buf_pos]),line);
                        act_buf_pos += l;
                    }
                }
            }
        }
    }
}

//-------------------------------------------------------------------
/*
    Je nach Mode wird renderConsole() oder renderWatcher()
    aufgerufen (oder nix von beiden) und das Ergebnis 
    gerendert.

    - 13-May-99   floh    created
*/
//-------------------------------------------------------------------
void nConServer::Render(void)
{
    n_assert(this->render_buf);
    bool do_render = true;
    int x,y,w,h,f_height;
    
    nGfxServer *gs = this->ref_gs.get();
    gs->GetDisplayDesc(x,y,w,h);
    gs->GetFontDesc(f_height);
    if (this->con_open)        this->renderConsole(x,y,w,h,f_height);
    else if (this->watch_open) this->renderWatcher(x,y,w,h,f_height);
    else do_render = false;
    if (do_render) {
        if (gs->BeginText()) {
            gs->TextPos(-1.0f,-1.0f);
            gs->Text(this->render_buf);
            gs->EndText();
        }
    }
}

//-------------------------------------------------------------------
/**
    Fuege einen neuen Buchstaben an der aktuellen Cursorposition
    ein, je nach Overstrike-Modus wird der Rest des Strings
    verschoben, oder der naechste Buchstabe wird ueberschrieben.

    - 23-Feb-99   floh    created
*/
//-------------------------------------------------------------------
static inline int insertChar(char c, char *buf, int buf_size,
                             int cursor_pos, bool overstrike)
{
    if (overstrike) {
        buf[cursor_pos++] = c;
        if (cursor_pos >= buf_size) cursor_pos = buf_size-1;
    } else {
        char post_cursor_buf[256];
        int l = strlen(buf)+1;
        if (l < buf_size) {
            strcpy(post_cursor_buf,&(buf[cursor_pos]));
            buf[cursor_pos++] = c;
            strcpy(&(buf[cursor_pos]),post_cursor_buf);
        }
    }
    return cursor_pos;
}

//-------------------------------------------------------------------
/**
    @brief  Bewege Cursor 1 Zeichen nach links. Returniert neue
    Cursor-Position.

    - 23-Feb-99   floh    created
*/
//-------------------------------------------------------------------
static inline int cursorLeft(char *, int cursor_pos)
{
    cursor_pos--;
    if (cursor_pos < 0) cursor_pos = 0;
    return cursor_pos;
}

//-------------------------------------------------------------------
/**
    @brief Bewege Cursor 1 Zeichen nach rechts, returniere neue
    Cursor-Position.

    - 23-Feb-99   floh    created
*/
//-------------------------------------------------------------------
static inline int cursorRight(char *buf, int cursor_pos)
{
    int l = strlen(buf);
    cursor_pos++;
    if (cursor_pos > l) cursor_pos = l;
    return cursor_pos;
}

//-------------------------------------------------------------------
/**
    @brief moves cursor one word to the right

    - 06-Jun-01   leaf    created
    - 18-Aug-01   floh    fixed array overrun
*/
//-------------------------------------------------------------------
static inline int wordRight(char *buf, int cursor_pos)
{
    int l = strlen(buf);
	while((++cursor_pos < l) && buf[cursor_pos] == ' ');
	while((++cursor_pos < l) && buf[cursor_pos] != ' ');
    if (cursor_pos > l) cursor_pos = l;
    return cursor_pos;
}

//-------------------------------------------------------------------
/**
    @brief moves cursor one word to the right

    - 06-Jun-01   leaf    created
    - 18-Aug-01   floh    fixed array underrun
*/
//-------------------------------------------------------------------
static inline int wordLeft(char *buf, int cursor_pos)
{
	while((--cursor_pos >= 0) && buf[cursor_pos] == ' ');
	while((--cursor_pos >= 0) && buf[cursor_pos] != ' ');
    if (cursor_pos < 0) cursor_pos = 0;
    return cursor_pos;
}

//-------------------------------------------------------------------
/**
    @brief Delete character left to cursor, shift cursor one position
    to left, return new cursor_pos.

    - 23-Feb-99   floh    created
*/
//-------------------------------------------------------------------
static inline int delLeft(char *buf, int cursor_pos)
{
    if (cursor_pos == 0) return cursor_pos;
    cursor_pos--;
    int l = strlen(buf);
    int i;
    for (i=cursor_pos; i<=l; i++) buf[i] = buf[i+1];
    return cursor_pos;
}

//-------------------------------------------------------------------
/**
    @brief Delete character under cursor, move rest of line one to the left.

    - 23-Feb-99   floh    created
*/
//-------------------------------------------------------------------
static inline int delRight(char *buf, int cursor_pos)
{
    int l = strlen(buf);
    int i;
    for (i=cursor_pos; i<=l; i++) buf[i] = buf[i+1];
    return cursor_pos;
}

//-------------------------------------------------------------------
/**
    Teste, ob der uebergebene Key-Wert im Killset ist.

    - 20-Dec-99   floh    created
*/
//-------------------------------------------------------------------
static inline bool inKillSet(int key)
{
    int i=0;
    while (key_killset[i] != N_KEY_NONE) {
        if (key == key_killset[i]) return true;
        i++;
    }
    return false;
}

//-------------------------------------------------------------------
/**
    Bekommt ein Input-Event rein, und editiert damit die
    Kommandozeile. Wenn das Event verwertet wurde, kommt TRUE
    zurueck, dieses Event sollte dann aus der Input-Event-Liste
    geloescht werden.
    Wenn die Eingabezeile mit Return abgeschlossen wurde,
    wird das Kommando sofort an den Scriptserver gegeben,
    das Kommando sowie dessen Resultat per n_printf() an den
    Stdout-Puffer gegeben, sowie das Kommando selbst an die
    Command-History angeghaengt. 

    - 23-Feb-99   floh    created
    - 10-Mar-99   floh    auf neue Inputevents umgeschrieben
    - 06-Jun-01   leaf    ctrl left & right - move cursor one word
                          home and end - move cursor to start/end of line
*/
//-------------------------------------------------------------------
bool nConServer::EditLine(nInputEvent *ie)
{
    bool kill_me = false;
    if ((ie->GetType() == N_INPUT_KEY_CHAR) && (ie->GetChar() >= ' ')) {
        // ein normaler Buchstabe...
        this->cursor_pos = insertChar(ie->GetChar(),
                                      this->input_buffer,
                                      sizeof(input_buffer),
                                      this->cursor_pos,
                                      overstrike);
        kill_me = true;
    }
    if (ie->GetType() == N_INPUT_KEY_DOWN) {
        if (inKillSet(ie->GetKey())) kill_me = true;

        // koennte ein Control-Character sein...
        switch(ie->GetKey()) {
            case N_KEY_LEFT:
                if (this->ctrl_down)
                    this->cursor_pos = wordLeft(this->input_buffer, this->cursor_pos);
                else
					this->cursor_pos = cursorLeft(this->input_buffer, this->cursor_pos);
                break;
            case N_KEY_RIGHT:
                if (this->ctrl_down)
					this->cursor_pos = wordRight(this->input_buffer, this->cursor_pos);
                else
					this->cursor_pos = cursorRight(this->input_buffer, this->cursor_pos);
                break;
            case N_KEY_BACK:
                this->cursor_pos = delLeft(this->input_buffer, this->cursor_pos);
                break;
            case N_KEY_DELETE:
                this->cursor_pos = delRight(this->input_buffer, this->cursor_pos);
                break;
            case N_KEY_INSERT:
                this->overstrike = this->overstrike ? false : true;
                break;
            case N_KEY_UP:
                if (this->ctrl_down) this->scroll_offset++;
                else                 this->RecallPrevCmd();
                break;
            case N_KEY_DOWN:
                if (this->ctrl_down) this->scroll_offset--;
                else                 this->RecallNextCmd();
                break;
            case N_KEY_RETURN:
                {
                    // fertiges Command an den Script-Server uebergeben...
                    char act_line[1024];
                    this->AddCmdToHistory(this->input_buffer);
                    this->ref_ss->Prompt(act_line,sizeof(act_line));
                    strcat(act_line,this->input_buffer);
                    strcat(act_line,"\n");
                    n_printf(act_line);
                    const char *res;

                    // temporarily turn off the fail on error flag in the script server
                    nScriptServer* scriptServer = this->ref_ss.get();
                    bool failOnError = scriptServer->GetFailOnError();
                    scriptServer->SetFailOnError(false);
                    this->ref_ss->Run(this->input_buffer, res);
                    scriptServer->SetFailOnError(failOnError);
                    if (res && (*res)) {
                        n_printf(res);
                        n_printf("\n");
                    }
                    memset(this->input_buffer,0,sizeof(this->input_buffer));
                    this->cursor_pos    = 0;
                    this->scroll_offset = 0;
                }
                break;
            case N_KEY_CONTROL:
                this->ctrl_down = true;
                break;
			case N_KEY_HOME:
                this->cursor_pos = 0;
                break;
			case N_KEY_END:
                this->cursor_pos = strlen(this->input_buffer);
                break;
            default:    break;
        }
    }
    if (ie->GetType() == N_INPUT_KEY_UP) {
        if (inKillSet(ie->GetKey())) kill_me = true;
        switch (ie->GetKey()) {
            case N_KEY_CONTROL:
                this->ctrl_down = false;
                break;
            default:    break;
        }
    }
    return kill_me;
}

//-------------------------------------------------------------------
/**
    - 23-Feb-99   floh    created
*/
//-------------------------------------------------------------------
void nConServer::AddCmdToHistory(const char *cmd)
{
    this->ResetHistory();
    if (strlen(cmd) > 0) {
        this->hist_buf.Put(cmd);
        this->hist_buf.Put("\n");
    }
}

//-------------------------------------------------------------------
/**
    - 23-Feb-99   floh    created
*/
//-------------------------------------------------------------------
void nConServer::ResetHistory(void)
{
    this->act_hist = 0;
}

//-------------------------------------------------------------------
/**
    - 23-Feb-99   floh    created
*/
//-------------------------------------------------------------------
void nConServer::RecallPrevCmd(void)
{
    int i;
    int act_line = this->hist_buf.GetHeadLine();
    int prev_line;
    const char *hist_cmd;
    this->act_hist++;
    for (i=0; i<this->act_hist; i++) {
        prev_line = this->hist_buf.GetPrevLine(act_line);
        if (prev_line == -1) {
            this->act_hist--;
            break;
        }
        act_line = prev_line;
    }
    hist_cmd = this->hist_buf.GetLine(act_line);
    if (hist_cmd) {
        strcpy(this->input_buffer,hist_cmd);
        this->cursor_pos = strlen(hist_cmd);
    }
}

//-------------------------------------------------------------------
/**
    - 23-Feb-99   floh    created
*/
//-------------------------------------------------------------------
void nConServer::RecallNextCmd(void)
{
    int i;
    int act_line = this->hist_buf.GetHeadLine();
    int prev_line;
    const char *hist_cmd;

    this->act_hist--;
    if (this->act_hist < 0) this->act_hist = 0;
    for (i=0; i<this->act_hist; i++) {
        prev_line = this->hist_buf.GetPrevLine(act_line);
        if (prev_line == -1) {
            this->act_hist--;
            break;
        }
        act_line = prev_line;
    }
    hist_cmd = this->hist_buf.GetLine(act_line);
    if (hist_cmd) {
        strcpy(this->input_buffer,hist_cmd);
        this->cursor_pos = strlen(hist_cmd);
    }
}

//-------------------------------------------------------------------
/**
    Wenn die Konsole offen ist, zuerst alle gueltigen
    Chars aus dem Inputstrom filtern, und in die Inputzeile
    einfuegen. Wenn die Inputzeile komplettiert ist,
    diese als Kommando ausfuehren und per PutS()
    "darstellen".
    An den Grafik-Server geht der Inhalt des Zeilenbuffers
    ab der "act_line" bis zur Headline, dann ein Newline,
    das Prompt, und die aktuelle Input-Zeile inklusive
    Cursor.
    An Stdout geht nur die aktuelle Input-Zeile inklusive
    Prompt.
  
    - 20-Feb-99   floh    created
    - 10-Dec-99   floh    + killt Tasten-Events nicht mehr, sondern
                            disabled sie, damit das Input-Recording auch
                            bei offener Konsole klappt
    - 20-Dec-99   floh    + hockt jetzt nicht mehr auf der Esc-Taste,
                            kann also nur noch von extern geoffnet werden
*/
//-------------------------------------------------------------------
void nConServer::Trigger(void)
{
    if (this->con_open) {
        // Input-Ereignisse parsen
        nInputServer *is = this->ref_is.get();
        nInputEvent *ie;
        if ((ie = is->FirstEvent())) {
            nInputEvent *next_ie;
            do {
                bool kill_event = false;
                next_ie = is->NextEvent(ie);
                kill_event = this->EditLine(ie);
                if (kill_event) ie->SetDisabled(true);
            } while ((ie = next_ie));
        }
    }
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------

