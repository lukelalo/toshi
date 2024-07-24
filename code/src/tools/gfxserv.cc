//-------------------------------------------------------------------
//  gfxserv.cc -- Enduser-GfxServer
//
//  Erzeugt einen nGfxServer, nScriptServer und nSceneGraph.
//  Alle n3DNodes unter /usr/scene werden gerendert, manipulierbar
//  ist der Server unter dem Port "gfxserv".
//
//  03-Dec-98   floh    1 Koordinate == 1 Meter
//  05-Dec-98   floh    jetzt mit optionalem Startup-Script
//  21-Dec-98   floh    + -script Option fuer zu erzeugenden
//                        Script-Server
//  24-Feb-99   floh    + -args Option, uebergibt Argumente
//                        an Script-Server
//  22-Jun-99   floh    + nmathserver
//  29-May-01   floh    + adopted to new SceneGraph2/nChannelServer
//
//  (C) 1998 by A.Weissflog
//-------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "kernel/nkernelserver.h"
#include "kernel/ntimeserver.h"
#include "gfx/ngfxserver.h"
#include "kernel/nfileserver2.h"
#include "input/ninputserver.h"
#include "script/ntclserver.h"
#include "misc/nmathserver.h"
#include "gfx/nscenegraph2.h"
#include "gfx/nchannelserver.h"
#include "misc/nconserver.h"
#include "misc/nparticleserver.h"
#include "node/n3dnode.h"
#include "misc/nspecialfxserver.h"
#include "collide/ncollideserver.h"
#include "shadow/nshadowserver.h"
#include "audio/naudioserver2.h"
#include "anim/nanimserver.h"
#include "kernel/nremoteserver.h"
#include "kernel/nwin32loghandler.h"
#include "kernel/nfileloghandler.h"
#include "gfx/nprimitiveserver.h"

matrix44 V,Rx,Ry;

bool cur_up    = false;
bool cur_down  = false;
bool cur_left  = false;
bool cur_right = false;
bool ctrl      = false;
bool shift     = false;
bool lmb = false;
bool mmb = false;
bool rmb = false;
bool normalize = false;

long old_x = 0;
long old_y = 0;
long act_x = 0;
long act_y = 0;

//-------------------------------------------------------------------
void handleInput(nConServer *con, nInputEvent *ie)
{
    if (ie->IsDisabled()) return;
    switch (ie->GetType()) {
        case N_INPUT_KEY_DOWN:
            {
                switch (ie->GetKey()) {
                    case N_KEY_ESCAPE:  con->Toggle(); break;
                    case N_KEY_LEFT:    cur_left=true; break;
                    case N_KEY_RIGHT:   cur_right=true; break;
                    case N_KEY_UP:      cur_up=true; break;
                    case N_KEY_DOWN:    cur_down=true; break;
                    case N_KEY_CONTROL: ctrl=true; break;
                    case N_KEY_SHIFT:   shift=true; break;
                    case N_KEY_SPACE:   normalize=true; break;
                    default: break;    
                }
            }
            break;
            
        case N_INPUT_KEY_UP:
            {
                switch (ie->GetKey()) {
                    case N_KEY_LEFT:    cur_left=false; break;
                    case N_KEY_RIGHT:   cur_right=false; break;
                    case N_KEY_UP:      cur_up=false; break;
                    case N_KEY_DOWN:    cur_down=false; break;
                    case N_KEY_CONTROL: ctrl=false; break;
                    case N_KEY_SHIFT:   shift=false; break;
                    default: break;    
                }
            }
            break;
            
        case N_INPUT_BUTTON_DOWN:
            {
                old_x = act_x = ie->GetAbsXPos();
                old_y = act_y = ie->GetAbsYPos();
                switch (ie->GetButton()) {
                    case 0:  lmb=true; break;
                    case 1:  rmb=true; break;
                    case 2:  mmb=true; break;
                    default: break;
                }
            }
            break;
        case N_INPUT_BUTTON_UP:
            {
                old_x = act_x = ie->GetAbsXPos();
                old_y = act_y = ie->GetAbsYPos();
                switch (ie->GetButton()) {
                    case 0:  lmb=false; break;
                    case 1:  rmb=false; break;
                    case 2:  mmb=false; break;
                    default: break;
                }
            }
            break;
        case N_INPUT_MOUSE_MOVE:
            {
                act_x = ie->GetAbsXPos();
                act_y = ie->GetAbsYPos();
            }
            break;
        default: break;    
    }
}

//-------------------------------------------------------------------
void handleViewer(void)
{
    matrix44 tm;
    matrix44 tmp;
    float tx,ty,tz;
    float rx,ry;
    tx=0.0f; ty=0.0f; tz=0.0f;
    rx=0.0f; ry=0.0f;
    if (shift) {
        if (cur_up)    rx -= 0.025f;
        if (cur_down)  rx += 0.025f;
        if (cur_left)  ry -= 0.025f;
        if (cur_right) ry += 0.025f;
    } else {
        if (ctrl) {
            if (cur_up)    ty -= 0.1f;
            if (cur_down)  ty += 0.1f;
        } else {
            if (cur_up)    tz -= 0.1f;
            if (cur_down)  tz += 0.1f;
            if (cur_left)  tx -= 0.1f;   
            if (cur_right) tx += 0.1f;
        }
    }
    if (rmb) {
        ry += ((float)(old_x-act_x)) * 0.01f;
        rx += ((float)(old_y-act_y)) * 0.01f;
    }
    if (lmb) {
        tz += ((float)(old_y-act_y) * 0.1f);
        tx += ((float)(old_x-act_x) * 0.1f);
    }

    if (normalize) {
        Rx.ident();
        Ry.ident();
        V.ident();
        vector3 t(0.0f,2.5f,0.0f);
        V.translate(t);
        normalize = false;
    }
        
    Rx.rotate_x(rx);
    Ry.rotate_y(ry);

    // Translation auf existierende Matrix
    tm.ident();
    vector3 t(tx,ty,tz);
    tm.translate(t);
    tmp = tm;
    tmp.mult_simple(V);
    V = tmp;
    vector3 vnul(0.0f,0.0f,0.0f);
    vector3 v = V * vnul;
    V.ident();
    V.translate(v);
    tmp = Ry;
    tmp.mult_simple(V);
    V = tmp;
    tmp = Rx;
    tmp.mult_simple(V);
    V = tmp;

    old_x = act_x;
    old_y = act_y;
}    

//-------------------------------------------------------------------
int main(int argc, char *argv[])
{
    char *port_name = "gfxserv";
    char *server_class = "nglserver";
    char *mode = "w(512)-h(512)";
    char *startup = NULL;
    char *script_server = "ntclserver";
    bool grid = true;
    bool nosleep = false;
    bool argerr;
    long i;
    char arg_cmd[1024];
	arg_cmd[0] = 0;

    // Args auswerten
	argerr=false;
	for (i=1; i<argc && !argerr; i++) {
		char *arg = argv[i];
		if (strcmp(arg,"-port")==0) {
			if (++i < argc) port_name = argv[i];
			else argerr=true;
        } else if (strcmp(arg,"-startup")==0) {
            if (++i < argc) startup = argv[i];
            else argerr=true;
        } else if (strcmp(arg,"-script")==0) {
            if (++i < argc) script_server = argv[i];
            else argerr=true;
        } else if (strcmp(arg,"-nogrid")==0) grid=false;
        else if (strcmp(arg,"-nosleep")==0) nosleep=true;
        else if (strcmp(arg,"-server")==0) {
            if (++i < argc) server_class = argv[i];
            else argerr=true;
        } else if (strcmp(arg,"-mode")==0) {
            if (++i < argc) mode = argv[i];
            else argerr=true;
        } else if (strcmp(arg, "-args")==0) {
            // args are stored for use by scripts etc.
            for (++i; i<argc; i++) {
                strcat(arg_cmd,argv[i]);
                strcat(arg_cmd," ");
            }
        } else printf("unknown arg: %s\n",argv[i]);
	}
	if (argerr) {
		printf("arg error, exiting.\n"); 
		return 5;
	}

    // hier geht's los
    V.ident();
    vector3 t(0.0f,2.5f,0.0f);
    V.translate(t);
    nEvent sleeper;
    nKernelServer* ks       = new nKernelServer;

#ifdef __WIN32__
    nWin32LogHandler* lh = new nWin32LogHandler("gfxserv");
    ks->AddLogHandler(lh);
#endif
    nFileLogHandler* fh = new nFileLogHandler("gfxserv");
    ks->AddLogHandler(fh);

    nFileServer2* fs2       = (nFileServer2*)       ks->New("nfileserver2",         "/sys/servers/file2");
    nGfxServer* gfx         = (nGfxServer *)        ks->New(server_class,           "/sys/servers/gfx");
    nInputServer* inp       = (nInputServer *)      ks->New("ninputserver",         "/sys/servers/input");
    nTclServer* script      = (nTclServer *)        ks->New(script_server,          "/sys/servers/script");
    nSceneGraph2* graph     = (nSceneGraph2 *)      ks->New("nscenegraph2",         "/sys/servers/sgraph2");
    nShadowServer* shadow   = (nShadowServer *)     ks->New("nsbufshadowserver",    "/sys/servers/shadow");
    nChannelServer* chn     = (nChannelServer*)     ks->New("nchannelserver",       "/sys/servers/channel"); 
    nConServer *con         = (nConServer *)        ks->New("nconserver",           "/sys/servers/console");
    nMathServer *math       = (nMathServer *)       ks->New("nmathserver",          "/sys/servers/math");
    nParticleServer *part   = (nParticleServer *)   ks->New("nparticleserver",      "/sys/servers/particle");
    nSpecialFxServer *fx    = (nSpecialFxServer *)  ks->New("nspecialfxserver",     "/sys/servers/specialfx");
    nAnimServer* anim       = (nAnimServer*)        ks->New("nanimserver",          "/sys/servers/anim");
    nPrimitiveServer *prim  = (nPrimitiveServer *)  ks->New("nprimitiveserver",     "/sys/primitive");
    n3DNode *root           = (n3DNode *)           ks->New("n3dnode",              "/usr/scene");

	if (arg_cmd[0]) {
		nEnv *args = (nEnv *) ks->New("nenv","/sys/share/args");
		args->SetS(arg_cmd);
	}

    ks->ts->EnableFrameTime();

    if (ks->GetRemoteServer()->Open("gfxserv")) 
    {
        gfx->SetDisplayMode(mode);
        if (gfx->OpenDisplay()) 
        {
            if (startup) 
            {
                const char* res;
                printf("running startup script '%s'\n",startup);
                script->RunScript(startup, res);
                if (res) printf("%s\n",res);
            }    

            nAudioServer2* as2 = (nAudioServer2*) ks->Lookup("/sys/servers/audio");
            if (as2) as2->OpenAudio();

            nCollideServer *cs = (nCollideServer *) ks->Lookup("/sys/servers/collide");

            nRState rs(N_RS_LIGHTING,N_TRUE);
            gfx->SetState(rs);
            ks->ts->ResetTime();

            root->RenderContextCreated(0);

            bool running = true;
            while (gfx->Trigger() && running) 
            {
                if (!script->Trigger()) running = false;

                ks->Trigger();
                ks->ts->Trigger();
                double t = ks->ts->GetFrameTime();
                inp->Trigger(t);

                ks->GetRemoteServer()->Trigger();
                // Input-Messages auswerten
                nInputEvent *ie;
                if ((ie = inp->FirstEvent())) 
                {
                    do handleInput(con,ie); while ((ie = inp->NextEvent(ie)));
                    inp->FlushEvents();
                }
                handleViewer();

                // rendern
                if (as2)
                {
                    as2->BeginScene(t);
                }
                if (gfx->BeginScene())
                {
                    matrix44 vwr;
                    vwr = V;
                    vwr.invert_simple();

                    // trigger particle server
                    part->Trigger();

                    // Viewer-Matrix setzen
                    gfx->SetMatrix(N_MXM_VIEWER,V);

                    // Scenegraph vollrendern
                    chn->SetChannel1f(chn->GenChannel("time"), (float) t);
                    chn->SetChannel1f(chn->GenChannel("gtime"), (float) t);
                    if (graph->BeginScene(vwr))
                    {
                        fx->Begin();
                        graph->Attach(root, 0);
                        fx->End(graph);
                        graph->EndScene(true);
                    }

                    // render grid
                    if (grid) {
                        gfx->SetMatrix(N_MXM_MODELVIEW,vwr);
                        prim->SetColor(0.5f,1.0f,0.5f,1.0f);
                        prim->WirePlane(20,1);
                    }

                    // trigger collide server?
                    if (cs) {
                        nCollideContext *cc = cs->GetDefaultContext();
                        cc->Collide();
                        cc->Visualize(gfx);
                    }
                    
                    con->Render();                        
                    gfx->EndScene();
                    if (!nosleep) n_sleep(0.02);
                }
                if (as2)
                {
                    as2->EndScene();
                }
            }
            root->RenderContextDestroyed(0);
            if (as2) as2->CloseAudio();
            gfx->CloseDisplay();
        }
        ks->GetRemoteServer()->Close();
    }
    ks->ts->DisableFrameTime();

    if (root)   root->Release();
    if (prim)   prim->Release();
    if (anim)   anim->Release();
    if (part)   part->Release();
    if (math)   math->Release();
    if (con)    con->Release();
    if (chn)    chn->Release();
    if (shadow) shadow->Release();
    if (graph)  graph->Release();
    if (script) script->Release();
    if (gfx)    gfx->Release();
    if (inp)    inp->Release();
    if (fs2)    fs2->Release();    
    
#ifdef __WIN32__
    ks->RemoveLogHandler(lh);
    delete lh;
#endif
    ks->RemoveLogHandler(fh);
    delete fh;

    delete ks;
    return 0;
}

