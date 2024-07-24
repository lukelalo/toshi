#define N_IMPLEMENTS nGlServer
//--------------------------------------------------------------------
//  ngl_xdisp.cc -- X Window Display Initialisierung
//
// The full screen and window handling code represents a collaboration 
// between Samuel Humphreys and Jorrit Tyberghein (www.crystalspace.org)
// Portions of which are under the copyright of Jorrit, but are included 
// here with permission, and subject to the same license as TND.
//
//  (C) 1998 Andre Weissflog
//--------------------------------------------------------------------
#include "kernel/ntypes.h"
#ifdef __LINUX__

#include "kernel/nkernelserver.h"
#include "kernel/nenv.h"
#include "gfx/nglserver.h"
#include "input/ninputserver.h"

// Comment out the define in order to over-ride the incorrect default behaviour
// supplied by the window manager for the full screen hot key combination
// ALT-Enter. This causes the keyboard to be grabbed while in focus.
// When uncommented the hot-key combination for switching to and from 
// full screen mode is ALT-Multiply and the keyboard isnt grabbed between
// focus events.
#define DEBUG_FULL_SCREEN

//--------------------------------------------------------------------
//  ngl_TranslateKey()
//  Uebersetzt X-Keys nach Nebula-Keys.
//  08-Dec-98   floh    created
//  02-Jul-98   samuel  added further keycodes
//--------------------------------------------------------------------
static nKey ngl_TranslateKey(KeySym xkey)
{
    nKey nk;
    switch(xkey) {
        case XK_BackSpace:      nk=N_KEY_BACK; break;
        case XK_Tab:            nk=N_KEY_TAB; break;
        case XK_Linefeed:       nk=N_KEY_RETURN; break;
        case XK_Return:         nk=N_KEY_RETURN; break;
        case XK_Pause:          nk=N_KEY_PAUSE; break;
        case XK_Scroll_Lock:    nk=N_KEY_SCROLL; break;
        case XK_Sys_Req:        nk=N_KEY_PRINT; break;
        case XK_Escape:         nk=N_KEY_ESCAPE; break;
        case XK_Delete:         nk=N_KEY_DELETE; break;
        case XK_Home:           nk=N_KEY_HOME; break;
        case XK_Left:           nk=N_KEY_LEFT; break;
        case XK_Up:             nk=N_KEY_UP; break;
        case XK_Right:          nk=N_KEY_RIGHT; break;
        case XK_Down:           nk=N_KEY_DOWN; break;
        case XK_Page_Up:        nk=N_KEY_PRIOR; break;
        case XK_Page_Down:      nk=N_KEY_NEXT; break;
        case XK_End:            nk=N_KEY_END; break;
        case XK_Begin:          nk=N_KEY_BEGIN; break;
        case XK_Select:         nk=N_KEY_SELECT; break;
        case XK_Print:          nk=N_KEY_PRINT; break;
        case XK_Execute:        nk=N_KEY_EXECUTE; break;
        case XK_Insert:         nk=N_KEY_INSERT; break;
        case XK_Menu:           nk=N_KEY_MENU; break;
        case XK_Help:           nk=N_KEY_HELP; break;
        case XK_Num_Lock:       nk=N_KEY_NUMLOCK; break;
        case XK_KP_Multiply:    nk=N_KEY_MULTIPLY; break;
        case XK_KP_Add:         nk=N_KEY_ADD; break;
        case XK_KP_Separator:   nk=N_KEY_SEPARATOR; break;
        case XK_KP_Subtract:    nk=N_KEY_SUBTRACT; break;
        case XK_KP_Decimal:     nk=N_KEY_DECIMAL; break;
        case XK_KP_Divide:      nk=N_KEY_DIVIDE; break;
        case XK_KP_0:           nk=N_KEY_NUMPAD0; break;
        case XK_KP_1:           nk=N_KEY_NUMPAD1; break;
        case XK_KP_2:           nk=N_KEY_NUMPAD2; break;
        case XK_KP_3:           nk=N_KEY_NUMPAD3; break;
        case XK_KP_4:           nk=N_KEY_NUMPAD4; break;
        case XK_KP_5:           nk=N_KEY_NUMPAD5; break;
        case XK_KP_6:           nk=N_KEY_NUMPAD6; break;
        case XK_KP_7:           nk=N_KEY_NUMPAD7; break;
        case XK_KP_8:           nk=N_KEY_NUMPAD8; break;
        case XK_KP_9:           nk=N_KEY_NUMPAD9; break;

        case XK_KP_Space:	nk=N_KEY_SPACE; break;
        case XK_KP_Tab:		nk=N_KEY_TAB; break;
        case XK_KP_Enter:	nk=N_KEY_RETURN; break;
        case XK_KP_F1:		nk=N_KEY_F1; break;
        case XK_KP_F2:		nk=N_KEY_F2; break;
        case XK_KP_F3:		nk=N_KEY_F3; break;
        case XK_KP_F4:		nk=N_KEY_F4; break;
        case XK_KP_Home:	nk=N_KEY_HOME; break;
        case XK_KP_Left:	nk=N_KEY_LEFT; break;
        case XK_KP_Up:		nk=N_KEY_UP; break;
        case XK_KP_Right:	nk=N_KEY_RIGHT; break;
        case XK_KP_Down:	nk=N_KEY_DOWN; break;
        case XK_KP_End:		nk=N_KEY_END; break;
        case XK_KP_Begin:	nk=N_KEY_BEGIN; break;
        case XK_KP_Insert:	nk=N_KEY_INSERT; break;
        case XK_KP_Delete:	nk=N_KEY_DELETE; break;
        case XK_KP_Page_Up:     nk=N_KEY_PRIOR; break;
        case XK_KP_Page_Down:   nk=N_KEY_NEXT; break;

        case XK_F1:             nk=N_KEY_F1; break;
        case XK_F2:             nk=N_KEY_F2; break;
        case XK_F3:             nk=N_KEY_F3; break;
        case XK_F4:             nk=N_KEY_F4; break;
        case XK_F5:             nk=N_KEY_F5; break;
        case XK_F6:             nk=N_KEY_F6; break;
        case XK_F7:             nk=N_KEY_F7; break;
        case XK_F8:             nk=N_KEY_F8; break;
        case XK_F9:             nk=N_KEY_F9; break;
        case XK_F10:            nk=N_KEY_F10; break;
        case XK_F11:            nk=N_KEY_F11; break;
        case XK_F12:            nk=N_KEY_F12; break;
        case XK_F13:            nk=N_KEY_F13; break;
        case XK_F14:            nk=N_KEY_F14; break;
        case XK_F15:            nk=N_KEY_F15; break;
        case XK_F16:            nk=N_KEY_F16; break;
        case XK_F17:            nk=N_KEY_F17; break;
        case XK_F18:            nk=N_KEY_F18; break;
        case XK_F19:            nk=N_KEY_F19; break;
        case XK_F20:            nk=N_KEY_F20; break;
        case XK_F21:            nk=N_KEY_F21; break;
        case XK_F22:            nk=N_KEY_F22; break;
        case XK_F23:            nk=N_KEY_F23; break;
        case XK_F24:            nk=N_KEY_F24; break;
        case XK_Shift_L:        nk=N_KEY_SHIFT; break;
        case XK_Shift_R:        nk=N_KEY_SHIFT; break;
        case XK_Control_L:      nk=N_KEY_CONTROL; break;
        case XK_Control_R:      nk=N_KEY_CONTROL; break;
        case XK_space:          nk=N_KEY_SPACE; break;
        case XK_0:              nk=N_KEY_0; break;
        case XK_1:              nk=N_KEY_1; break;
        case XK_2:              nk=N_KEY_2; break;
        case XK_3:              nk=N_KEY_3; break;
        case XK_4:              nk=N_KEY_4; break;
        case XK_5:              nk=N_KEY_5; break;
        case XK_6:              nk=N_KEY_6; break;
        case XK_7:              nk=N_KEY_7; break;
        case XK_8:              nk=N_KEY_8; break;
        case XK_9:              nk=N_KEY_9; break;
        case XK_A:              nk=N_KEY_A; break;
        case XK_B:              nk=N_KEY_B; break;
        case XK_C:              nk=N_KEY_C; break;
        case XK_D:              nk=N_KEY_D; break;
        case XK_E:              nk=N_KEY_E; break;
        case XK_F:              nk=N_KEY_F; break;
        case XK_G:              nk=N_KEY_G; break;
        case XK_H:              nk=N_KEY_H; break;
        case XK_I:              nk=N_KEY_I; break;
        case XK_J:              nk=N_KEY_J; break;
        case XK_K:              nk=N_KEY_K; break;
        case XK_L:              nk=N_KEY_L; break;
        case XK_M:              nk=N_KEY_M; break;
        case XK_N:              nk=N_KEY_N; break;
        case XK_O:              nk=N_KEY_O; break;
        case XK_P:              nk=N_KEY_P; break;
        case XK_Q:              nk=N_KEY_Q; break;
        case XK_R:              nk=N_KEY_R; break;
        case XK_S:              nk=N_KEY_S; break;
        case XK_T:              nk=N_KEY_T; break;
        case XK_U:              nk=N_KEY_U; break;
        case XK_V:              nk=N_KEY_V; break;
        case XK_W:              nk=N_KEY_W; break;
        case XK_X:              nk=N_KEY_X; break;
        case XK_Y:              nk=N_KEY_Y; break;
        case XK_Z:              nk=N_KEY_Z; break;

        case XK_a:              nk=N_KEY_A; break;
        case XK_b:              nk=N_KEY_B; break;
        case XK_c:              nk=N_KEY_C; break;
        case XK_d:              nk=N_KEY_D; break;
        case XK_e:              nk=N_KEY_E; break;
        case XK_f:              nk=N_KEY_F; break;
        case XK_g:              nk=N_KEY_G; break;
        case XK_h:              nk=N_KEY_H; break;
        case XK_i:              nk=N_KEY_I; break;
        case XK_j:              nk=N_KEY_J; break;
        case XK_k:              nk=N_KEY_K; break;
        case XK_l:              nk=N_KEY_L; break;
        case XK_m:              nk=N_KEY_M; break;
        case XK_n:              nk=N_KEY_N; break;
        case XK_o:              nk=N_KEY_O; break;
        case XK_p:              nk=N_KEY_P; break;
        case XK_q:              nk=N_KEY_Q; break;
        case XK_r:              nk=N_KEY_R; break;
        case XK_s:              nk=N_KEY_S; break;
        case XK_t:              nk=N_KEY_T; break;
        case XK_u:              nk=N_KEY_U; break;
        case XK_v:              nk=N_KEY_V; break;
        case XK_w:              nk=N_KEY_W; break;
        case XK_x:              nk=N_KEY_X; break;
        case XK_y:              nk=N_KEY_Y; break;
        case XK_z:              nk=N_KEY_Z; break;
        default:    nk=N_KEY_NONE; break;
    }
    return nk;
}


//--------------------------------------------------------------------
//  ngl_initText()
//  24-Feb-99   floh    created
//--------------------------------------------------------------------
static bool ngl_initText(nGlServer *gls)
{
    n_assert(gls);
    n_assert(gls->dsp);
    char *font_name = "-adobe-courier-bold-r-normal--12-*-*-*-*-*-*-*";
    
    gls->text_initialized = false;
    gls->in_begin_text    = false;
    gls->text_listbase    = 1000;
    gls->text_numglyphs   = 256;
    gls->text_height      = 0.0f;
    gls->text_x           = 0.0f;
    gls->text_y           = 0.0f;
    
    gls->fs = XLoadQueryFont(gls->dsp, font_name);
    if (!gls->fs) {
        n_printf("Could not load X font '%s'\n",font_name);
        return false;
    }
    gls->text_height = gls->fs->ascent + gls->fs->descent;
    glXUseXFont(gls->fs->fid, 0, gls->text_numglyphs, gls->text_listbase);
    gls->text_initialized = true;
    return true;
}

//--------------------------------------------------------------------
//  ngl_killText()
//  24-Feb-99   floh    created
//--------------------------------------------------------------------
static void ngl_killText(nGlServer *gls)
{
    n_assert(gls);
    n_assert(gls->dsp);
    if (gls->text_initialized) {
        glDeleteLists(gls->text_listbase,gls->text_numglyphs);
    }    
    if (gls->fs) {
        XFreeFont(gls->dsp, gls->fs);
        gls->fs = NULL;
    }
}

//--------------------------------------------------------------------
//  xwin_proc()
//
//  16-Nov-98   floh    created
//  08-Dec-98   floh    + Input-Events
//  30-Jul-01   samuel  + added window management and resizing
//  02-Aug-01   samuel  + added full screen extension support
//  02-Aug-01   samuel  + fixed mouse button support
//--------------------------------------------------------------------
bool nGlServer::xwin_proc(void)
{
    // Processes everything in the XEventQueue, when nothing is left
    // to be done, the routine returns TRUE. In case wmDeleteWindow
    // comes down the pipe, the routine returns FALSE, so that the
    // app knows, that it should stop.
    bool retval = true;
    while (retval && XPending(this->dsp)) {
        XNextEvent(this->dsp,&(this->event));
        switch(this->event.type) {
            case ConfigureNotify:
                if (event.xconfigure.window == this->wm_win)
                {
                    if ((this->disp_w != this->event.xconfigure.width) ||
                       (this->disp_h != this->event.xconfigure.height))
                    {
                        // Deal with resizing as close to the context swap
                        // as possible to minimize glX and X lag visual artifacts
                        this->resize = true;
                        this->disp_w = this->event.xconfigure.width;
                        this->disp_h = this->event.xconfigure.height;
                    }
                }
                break;
            case ClientMessage:
                if ((Atom)this->event.xclient.data.l[0] == this->wmDeleteWindow) {
                    // Window was killed externally
                    if (this->display_open) this->CloseDisplay();
                    // the window itself stays alive and is only finally
                    // destroyed in the destructor!
                }
                retval = false;
                break;

            case KeyPress:
                if (this->ref_is.isvalid())
                {
                    XKeyEvent *ke = (XKeyEvent *) &(this->event);
                    KeySym ks = XLookupKeysym(ke,0);
                    if (this->ext_XF86VIDMODE && event.xkey.state & Mod1Mask)
                    {
                        if (HotKeyVideoMode(ks))
                        {
                            break;
                        }
                    }

                    nKey nk   = ngl_TranslateKey(ks);
                    nInputEvent *ie = this->ref_is->NewEvent();
                    if (ie)
                    {
                        ie->SetType(N_INPUT_KEY_DOWN);
                        ie->SetDeviceId(N_INPUT_KEYBOARD(0));
                        ie->SetKey(nk);
                        this->ref_is->LinkEvent(ie);
                    }
                }
                break;

            case KeyRelease:
                if (this->ref_is.isvalid())
                {
                    // can generate 2 events, a KeyUp and possibly a Char
                    XKeyEvent *ke = (XKeyEvent *) &(this->event);
                    unsigned char buf[32];
                    int l;
                    KeySym ks = XLookupKeysym(ke,0);
                    nKey nk   = ngl_TranslateKey(ks);
                    nInputEvent *ie = this->ref_is->NewEvent();
                    if (ie)
                    {
                        ie->SetType(N_INPUT_KEY_UP);
                        ie->SetDeviceId(N_INPUT_KEYBOARD(0));
                        ie->SetKey(nk);
                        this->ref_is->LinkEvent(ie);
                    }
                    l = XLookupString((XKeyEvent *)&(this->event), (char *) buf, sizeof(buf), NULL, NULL);
                    if (l==1)
                    {
                        ie = this->ref_is->NewEvent();
                        if (ie)
                        {
                            ie->SetType(N_INPUT_KEY_CHAR);
                            ie->SetDeviceId(N_INPUT_KEYBOARD(0));
                            ie->SetChar((int)(buf[0]));
                            this->ref_is->LinkEvent(ie);
                        }
                    }
                }
                break;

            case ButtonPress:
                if (this->ref_is.isvalid())
                {
                    nInputEvent *ie = this->ref_is->NewEvent();
                    if (ie)
                    {
                        ie->SetType(N_INPUT_BUTTON_DOWN);
                        ie->SetDeviceId(N_INPUT_MOUSE(0));
                        ie->SetAbsPos(this->event.xbutton.x, this->event.xbutton.y);
                        switch(this->event.xbutton.button)
                        {
                            case Button1: ie->SetButton(0); break;   // LMB
                            case Button2: ie->SetButton(1); break;   // MMB
                            case Button3: ie->SetButton(2); break;   // RMB
                            default:      ie->SetButton(2); break;   // ???
                        }
                        float relX = float(this->event.xbutton.x) / this->render_width;
                        float relY = float(this->event.xbutton.y) / this->render_height;
                        ie->SetRelPos(relX, relY);
                        this->ref_is->LinkEvent(ie);
                    }
                }
                break;

            case ButtonRelease:
                if (this->ref_is.isvalid())
                {
                    nInputEvent *ie = this->ref_is->NewEvent();
                    if (ie)
                    {
                        ie->SetType(N_INPUT_BUTTON_UP);
                        ie->SetDeviceId(N_INPUT_MOUSE(0));
                        ie->SetAbsPos(this->event.xbutton.x, this->event.xbutton.y);
                        switch(this->event.xbutton.button) {
                            case Button1: ie->SetButton(0); break;   // LMB
                            case Button2: ie->SetButton(1); break;   // MMB
                            case Button3: ie->SetButton(2); break;   // RMB
                            default:      ie->SetButton(2); break;   // ???
                        }
                        float relX = float(this->event.xbutton.x) / this->render_width;
                        float relY = float(this->event.xbutton.y) / this->render_height;
                        ie->SetRelPos(relX, relY);
                        this->ref_is->LinkEvent(ie);
                    }
                }
                break;

            case MotionNotify:
                if (this->ref_is.isvalid())
                {
                    short x = this->event.xmotion.x;
                    short y = this->event.xmotion.y;
                    nInputEvent *ie = this->ref_is->NewEvent();
                    if (ie)
                    {
                        ie->SetType(N_INPUT_MOUSE_MOVE);
                        ie->SetDeviceId(N_INPUT_MOUSE(0));
                        ie->SetAbsPos(x, y);
                        float relX = float(this->event.xmotion.x) / this->render_width;
                        float relY = float(this->event.xmotion.y) / this->render_height;
                        ie->SetRelPos(relX, relY);
                        this->ref_is->LinkEvent(ie);
                    }
                }
                break;
   	    case FocusIn:
#ifndef DEBUG_FULL_SCREEN
		if (this->ext_XF86VIDMODE &&
		    !this->keyboard_grabbed && event.xfocus.window == wm_win) {
		    XGrabKeyboard (this->dsp,
				   this->ctx_win,
				   false,
				   GrabModeAsync,
				   GrabModeAsync,
				   CurrentTime);
		    this->focused = true;
		    this->keyboard_grabbed = 2;
		}
#endif
		break;
	    case FocusOut:
#ifndef DEBUG_FULL_SCREEN
		if (this->ext_XF86VIDMODE && 
		    this->keyboard_grabbed && !--(this->keyboard_grabbed)) {
		    XUngrabKeyboard (this->dsp,
				     CurrentTime);
		    this->focused = false;
		}
#endif
		break;

	    case MappingNotify:   // Automatic message, standard response
	        XRefreshKeyboardMapping (&(this->event.xmapping));
		break;

	    case SelectionClear:  // Automatic message
	        n_printf ("SelectionClear Received\n");
		break;
	    case SelectionNotify:  // Automatic message
	        n_printf ("SelectionNotify Received\n");
		break;
	    case SelectionRequest:  // Automatic message
	        n_printf ("SelectionRequest Received\n");
		break;
	  
	    case MapNotify:
	    case UnmapNotify:
	    case ReparentNotify:
	        // Not needed
	        break;

	    default:
	        // Make sure we know exactly what events are received
	        n_assert (false);
		break;
        }
    }
    return retval;  
}

//--------------------------------------------------------------------
//  HotKeyVideoMode()
//
//  02-Aug-01   samuel  create   Hot keys for full screen extension support
//--------------------------------------------------------------------
bool nGlServer::HotKeyVideoMode(KeySym &ks)
{
    //  Alt-Multiply -> toggle fullscreen/windowed (debug)
    //  Alt-Return -> toggle fullscreen/windowed (release)
    //  Alt-Plus  -> naechst hoehere Aufloesung
    //  Alt-Minus -> naechst niedrigere Aufloesung
    switch (ks) {
#ifdef DEBUG_FULL_SCREEN
        case XK_KP_Multiply: 
#else
	case  XK_Return:
#endif
	    char mode_str[64];
	    if (this->is_fullscreen) {
	        // switch to windowed
	        sprintf(mode_str,
			"type(win)-w(%d)-h(%d)",
			this->disp_w,this->disp_h);
	    } else {
	        // switch to fullscreen
	        sprintf(mode_str,
			"type(full)-w(%d)-h(%d)-bpp(%d)",
			this->disp_w,this->disp_h,
			this->disp_bpp);
	    }
	    this->CloseDisplay();
	    this->SetDisplayMode(mode_str);
	    this->OpenDisplay();
	    return true;

        case XK_KP_Add:
	    // switch to next fullscreen mode
	    if (this->is_fullscreen && 
		this->nextDisplayMode(mode_str,sizeof(mode_str))) {
	        n_printf ("Up To New Mode: %s\n", mode_str);
		this->CloseDisplay();
		this->SetDisplayMode(mode_str);
		if (!this->OpenDisplay()) {
		  this->SetDisplayMode("type(full)-w(640)-h(480)");
		  this->OpenDisplay();
		}
	    }
	    return true;

        case XK_KP_Subtract:
	    // switch to previous fullscreen mode
	    if (this->is_fullscreen && 
		this->prevDisplayMode(mode_str,sizeof(mode_str))) {
	      n_printf ("Down To New Mode: %s\n", mode_str);
	      this->CloseDisplay();
	      this->SetDisplayMode(mode_str);
	      if (!this->OpenDisplay()) {
		this->SetDisplayMode("type(full)-w(640)-h(480)");
		this->OpenDisplay();
	      }
	    }
	    return true;
    }
    return false;
}
//-------------------------------------------------------------------
//  enumDisplayModes()
//  Enumerate available display modes and build database under
//  /sys/share/display/0/modes.
//
//  Currently it is not possible with a vanilla Xserver to swap bpp
//  depths without restarting the Xserver itself (as far as I know :)
//
//  01-Aug-01   samuel  created based on windows counterpart
//-------------------------------------------------------------------
static int cmp_modes (const void *va, const void *vb)
{
    XF86VidModeModeInfo *a = *(XF86VidModeModeInfo **) va;
    XF86VidModeModeInfo *b = *(XF86VidModeModeInfo **) vb;

    if (a->hdisplay > b->hdisplay) return -1;
    else return b->vdisplay - a->vdisplay;
}


void nGlServer::enumDisplayModes(void)
{
    int act_mode = 0;
    int last_w   = 0;
    int last_h   = 0;
    int w, h;
    XF86VidModeModeInfo **modes;
    int i, nModes;


    if (this->ext_XF86VIDMODE) {
        ks->PushCwd(this->ref_devdir.get());    

	nRoot *mode_root = ks->New("nroot","modes");
	ks->PushCwd(mode_root);

	if (XF86VidModeGetAllModeLines (this->dsp, xvis->screen, 
					&nModes, &modes)) {
	    n_printf ("\nXFree86-VidModeExtension detected\n");
	    n_printf ("Available Video Modes:\n");
	    qsort (modes, nModes, sizeof (*modes), cmp_modes);

	    for (i = nModes - 1; i >= 0; --i) {
	        w = modes[i]->hdisplay;
	        h = modes[i]->vdisplay;
		nRoot *cur_mode_root;
		nEnv  *env;
		char buf[N_MAXPATH];
	  
		sprintf(buf,"%d",act_mode++);
		cur_mode_root = ks->New("nroot",buf);
		ks->PushCwd(cur_mode_root);
		sprintf(buf,"type(full)-w(%d)-h(%d)-bpp(%d)", 
			w, h, this->disp_bpp);
		n_printf("-w(%d)-h(%d)-bpp(%d)\n", 
			 w, h, this->disp_bpp);
		env = (nEnv *) ks->New("nenv","name");
		env->SetS(buf);
		env = (nEnv *) ks->New("nenv","w");
		env->SetI(w);
		env = (nEnv *) ks->New("nenv","h");
		env->SetI(h);
		env = (nEnv *) ks->New("nenv","bpp");
		env->SetI(this->disp_bpp);
		ks->PopCwd();
		last_w   = w;
		last_h   = h;
	    }

	    XFree (modes);
	} else {
	    n_printf ("Warning: XFree86-VidModeExtension detected,\n"
		      "but unable to enumerate available modes.\n"
		      "Disabling XFree86-VidModeExtension!\n");
	    this->ext_XF86VIDMODE = false;
	    n_assert (false);
	}
	n_printf ("\n");
	ks->PopCwd();
	ks->PopCwd();
    }
}


//--------------------------------------------------------------------
//  ngl_getColormap()
//  18-Nov-98   created (from glxdino example)
//--------------------------------------------------------------------
static Colormap ngl_getColormap(nGlServer *gls, XVisualInfo * vi)
{
    Status status;
    XStandardColormap *standardCmaps;
    Colormap cmap;
    int i, numCmaps;
    
    if (vi->c_class!=TrueColor) {
        n_error("nGlServer: no support for non-TrueColor visual");
    }
    status = XmuLookupStandardColormap(gls->dsp, vi->screen, vi->visualid,
        vi->depth, XA_RGB_DEFAULT_MAP, /* replace */ False, /* retain */ True);
    if (status == 1) {
        status = XGetRGBColormaps(gls->dsp, RootWindow(gls->dsp, vi->screen),
                             &standardCmaps, &numCmaps, XA_RGB_DEFAULT_MAP);
        if (status == 1)
            for (i = 0; i < numCmaps; i++)
                if (standardCmaps[i].visualid == vi->visualid) {
                    cmap = standardCmaps[i].colormap;
                    XFree(standardCmaps);
                    return cmap;
                }
    }
    cmap = XCreateColormap(gls->dsp, RootWindow(gls->dsp, vi->screen),
        vi->visual, AllocNone);
    return cmap;
}

//--------------------------------------------------------------------
//  winOpen()
//  Initializes X, opens windows, generates OpenGL context, but
//  does not map the window, so that for the time being it remains
//  invisible. The important thing is that the OpenGL context exists
//  for the lifetime of the nGlServer.
//
//  Initializes:
//      this->dsp    
//      this->ctx_win
//      this->cx
//
//  Calls:
//      this->initGl()      
//      ngl_initText
//
//  Exports:
//      /sys/env/xwinproc
//      /sys/env/xwinproc_arg
//
//  13-May-99   floh    created
//  06-Jun-99   floh    der dev()-Teil des DisplayMode-Descriptors
//                      wird jetzt als XDisplay-Descriptor
//                      aufgefasst
//  20-Jul-99   floh    exportiert keine Daten mehr in Filesystem
//  11-Jan-01   floh    try to get 32 bit zbuffer
//  30-Jul-01   samuel  added window management and resizing code
//  02-Aug-01   samuel  added full screen extension support
//--------------------------------------------------------------------
bool nGlServer::winOpen(void)
{
    n_assert(!this->dsp);
    n_assert(!this->ctx_win);
    n_assert(!this->cx); 
            
    Colormap cmap;
    XSetWindowAttributes swa;
    const char *disp_id;
    char win_title [] = "nGlServer glx";

//--------------------------------------------------------------------
    unsigned long cw_ctx_mask = (CWOverrideRedirect | 
				 CWBorderPixel | 
				 CWColormap | 
				 CWEventMask );

    unsigned long cw_wm_mask  = (CWBorderPixel | 
				 CWColormap | 
				 CWEventMask );

    unsigned long cw_fs_mask  = (CWOverrideRedirect | 
				 CWBorderPixel | 
				 CWColormap);

//--------------------------------------------------------------------
    unsigned long swa_ctx_mask = (KeyPressMask | 
				 KeyReleaseMask |
				 ButtonPressMask | 
				 ButtonReleaseMask | 
				 PointerMotionMask );

    unsigned long swa_wm_mask = (StructureNotifyMask |
				 FocusChangeMask |
				 KeyPressMask | 
				 KeyReleaseMask |
				 ButtonPressMask | 
				 ButtonReleaseMask | 
				 PointerMotionMask );

    unsigned long swa_fs_mask = 0;

//--------------------------------------------------------------------
    unsigned long si_ctx_mask  = (KeyPressMask | 
				 KeyReleaseMask |
				 ButtonPressMask | 
				 ButtonReleaseMask | 
				 PointerMotionMask );

    unsigned long si_wm_mask  = (StructureNotifyMask | 
				 FocusChangeMask |
				 KeyPressMask | 
				 KeyReleaseMask |
				 ButtonPressMask | 
				 ButtonReleaseMask | 
				 PointerMotionMask );

    unsigned long si_fs_mask  = 0;
//--------------------------------------------------------------------

    if (this->disp_dev[0]) disp_id = this->disp_dev;
    else                   disp_id = NULL;
            
    this->dsp = XOpenDisplay(disp_id);
    if (!this->dsp) {
        n_printf("nGlServer::OpenDisplay(): XOpenDisplay(%s) failed!",
                 disp_id ? disp_id : "NULL");
        return false;
    }
    int opcode, evbase, everror;
    this->ext_XF86VIDMODE = XQueryExtension (this->dsp, 
					     "XFree86-VidModeExtension",
					     &opcode, &evbase, &everror);

    // test, if X Server supports glX 
    if (!glXQueryExtension(this->dsp,NULL,NULL)) {
        n_warn("nGlServer::OpenDisplay(): glXQueryExtension() failed!");
        return false;
    }

    // find Visual ...    
    static int config[] = {
        GLX_RGBA,
        GLX_DOUBLEBUFFER,
        GLX_DEPTH_SIZE, 16,
        None,
    };
    xvis = glXChooseVisual(this->dsp,DefaultScreen(this->dsp),config);
    if (xvis == NULL) {
        n_warn("nGlServer::OpenDisplay(): no appropriate RGB visual!");
        return false;
    }
    cmap = ngl_getColormap(this,xvis);
    this->disp_bpp = xvis->depth;
    
    // create OpenGL Context
    this->cx = glXCreateContext(this->dsp, xvis, NULL, GL_TRUE);
    if (this->cx == NULL) {
       n_warn("nGlServer::OpenDisplay(): glXCreateContext() failed!");
       return false;
    }

    // create Window
    memset(&swa,0,sizeof(swa));
    swa.colormap = cmap;        
    swa.border_pixel = 0; 
    swa.override_redirect = true;
    swa.event_mask = swa_fs_mask;

    if (this->ext_XF86VIDMODE) {
        this->fs_win = XCreateWindow (this->dsp, 
				      RootWindow(this->dsp, xvis->screen), 
				      0, 0, 1, 1,
				      0 /*border*/, 
				      xvis->depth, 
				      InputOutput, 
				      xvis->visual,
				      cw_fs_mask, 
				      &swa);
	XStoreName (this->dsp, this->fs_win, win_title);
	XSetWindowBackground (this->dsp, this->fs_win, 
			      BlackPixel (this->dsp, xvis->screen));
	XSelectInput (this->dsp, this->fs_win,  si_fs_mask);
    }

    // Technique inspired by GTK to ensure correct window behaviour
    // under some more pedantic window managers.
    // Create a group leading window for the window manager:

    swa.event_mask = swa_wm_mask;
    wm_win = XCreateWindow (this->dsp, 
			    RootWindow(this->dsp,xvis->screen), // parent
			    16, 16, 256, 256, // arbitary location, dimensions
			    4,                // resizable border width
			    xvis->depth, 
			    InputOutput, 
			    xvis->visual,
			    cw_wm_mask, 
			    &swa);

    XStoreName (this->dsp, this->wm_win, win_title);
    XSelectInput (this->dsp, this->wm_win, si_wm_mask);
    // FocusChangeMask |

    wmDeleteWindow = XInternAtom(this->dsp, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(this->dsp, this->wm_win, &wmDeleteWindow, 1);

    XClassHint *class_hint = XAllocClassHint();
    class_hint->res_name = win_title;
    class_hint->res_class = win_title;
    XmbSetWMProperties (this->dsp, this->wm_win,
			NULL, NULL, NULL, 0, 
			NULL, NULL, class_hint);

    XFree (class_hint);

    swa.event_mask = swa_ctx_mask;
    this->ctx_win = XCreateWindow(
		    this->dsp,      // display
                    this->wm_win,   // make child of the window manager window
                    0, 0, 32, 32,   // x, y, w, h
                    0,              // border_width
                    xvis->depth,      // depth
                    InputOutput,    // class
                    xvis->visual,     // visual
                    cw_ctx_mask,           // valuemask
                    &swa);          // attributes

    XSelectInput (this->dsp, this->ctx_win, si_ctx_mask );

    // Resiziing hints
    XSizeHints normal_hints;
    normal_hints.flags = PMinSize | PMaxSize | PSize | PResizeInc;
    normal_hints.width = 256;    // arbitary init dimensions
    normal_hints.height = 256;
    normal_hints.width_inc = 1;  // step size of resize
    normal_hints.height_inc = 1;
    normal_hints.min_width = 32; 
    normal_hints.min_height = 32;
    normal_hints.max_width = DisplayWidth (this->dsp, 
					   DefaultScreen (this->dsp));
    normal_hints.max_height = DisplayHeight (this->dsp, 
					     DefaultScreen (this->dsp));
    XSetWMNormalHints (this->dsp, this->wm_win, &normal_hints);
    
    // Window manager hints
    XWMHints wm_hints;
    wm_hints.flags = InputHint | StateHint | WindowGroupHint;
    wm_hints.input = True;
    wm_hints.window_group = wm_win;
    wm_hints.initial_state = NormalState;
    XSetWMHints (this->dsp, this->wm_win, &wm_hints);

    Atom wm_client_leader = XInternAtom (this->dsp, "WM_CLIENT_LEADER", False);
    XChangeProperty (this->dsp, this->ctx_win, wm_client_leader, XA_WINDOW, 32, 
		     PropModeReplace, (const unsigned char*)&wm_win, 1);
    XmbSetWMProperties (this->dsp, this->ctx_win, win_title, win_title,
			NULL, 0, NULL, NULL, NULL);
    XmbSetWMProperties (this->dsp, this->wm_win, win_title, win_title,
			NULL, 0, NULL, NULL, NULL);

    glXMakeCurrent(this->dsp, this->ctx_win, this->cx);

    ngl_initText(this);    
    this->initGl();

    return true;
}         

//--------------------------------------------------------------------
//  invalidate_context()
//  Invalidate OpenGL context and all Nebula resources bound to it.
//  24-Nov-00   floh    created
//  02-Apr-01   leaf    removed ref_vbufdir (obsolete)
//--------------------------------------------------------------------
void nGlServer::invalidate_context(void)
{
    // release all textures, vbuffers, pixelshader, meshes
    nRoot *o;
    if (this->ref_texdir.isvalid()) {
        while ((o = this->ref_texdir->GetHead()))  o->Release();
    }
    if (this->ref_pshaderdir.isvalid()) {
        while ((o = this->ref_pshaderdir->GetHead())) o->Release();
    }
    if (this->ref_vxbufdir.isvalid()) {
        while ((o = this->ref_vxbufdir->GetHead())) o->Release();
    }
    if (this->ref_ixbufdir.isvalid()) {
        while ((o = this->ref_ixbufdir->GetHead())) o->Release();
    }

    if (this->cx) {
        n_assert(this->ctx_win);
        n_assert(this->cx);
        ngl_killText(this);
        glXDestroyContext(this->dsp,this->cx);
        this->cx  = NULL;
    }
}

//--------------------------------------------------------------------
//  winClose()
//  Gegenstueck zu winOpen()
//  13-May-99   floh    created
//  30-Jul-01   samuel  + added linux window resizing code
//  02-Aug-01   samuel  added full screen extension support
//--------------------------------------------------------------------
bool nGlServer::winClose(void)
{
    if (this->dsp) {
        n_assert(this->ctx_win);
        n_assert(this->cx);
        
        this->invalidate_context();

	if (this->is_fullscreen) {
	    n_assert (this->ext_XF86VIDMODE);
	    // Give up monopoly on input
	    XUngrabPointer (this->dsp, CurrentTime);
	    XUngrabKeyboard (this->dsp, CurrentTime);
	    if (!XF86VidModeSwitchToMode (this->dsp, 
					  this->xvis->screen, 
					  &this->wm_mode)) {
	        n_printf ("***PANIC*** Unable to restore windowed mode: %d, %d\n",
			  this->wm_mode.hdisplay, this->wm_mode.vdisplay);
	    }
	    XF86VidModeLockModeSwitch (this->dsp, this->xvis->screen, False);
	    XUnmapWindow(this->dsp,this->fs_win);
	    XDestroyWindow(this->dsp,this->fs_win);
	    this->fs_win = 0;	  
	}

        XDestroyWindow(this->dsp,this->ctx_win);
        this->ctx_win = 0;

	XDestroyWindow (this->dsp, this->wm_win);
	this->wm_win = 0;

        XCloseDisplay(this->dsp);
        this->dsp = NULL;

        return true;
    }
    return false;
}            


//--------------------------------------------------------------------
//  GetVideoMode()
//  02-Aug-01   samuel  added full screen extension support
//--------------------------------------------------------------------
bool nGlServer::GetVideoMode (XF86VidModeModeInfo &vid_mode, int w, int h)
{
  XF86VidModeModeInfo **modes;
  int i, nModes;

  if (XF86VidModeGetAllModeLines (this->dsp, xvis->screen, &nModes, &modes)) {
      for (i = nModes - 1; i >= 0; --i) {
	  if (modes[i]->hdisplay == w && 
	      modes[i]->vdisplay == h) {
	      vid_mode = *modes[i];
	      break;
	  }
      }
      XFree (modes);
      return true;
  }
  return false;
}

//--------------------------------------------------------------------
//  GrabInputs()
//  
//  02-Aug-01   samuel  create
//--------------------------------------------------------------------
bool nGlServer::GrabInputs ()
{
    // Grab mouse and keyboard focus
    if ((XGrabPointer (this->dsp, 
		       this->fs_win, 
		       True, 
		       0, 
		       GrabModeAsync, 
		       GrabModeAsync,
		       this->fs_win, 
		       None, 
		       CurrentTime)  !=  GrabSuccess)) {
        XUnmapWindow (this->dsp, this->fs_win);
	n_printf ("Unable to grab Mouse focus");
	return false;
    }
	  
    int error = XGrabKeyboard (this->dsp, 
			       this->fs_win, 
			       True, 
			       GrabModeAsync, 
			       GrabModeAsync, 
			       CurrentTime); 
    if (error != GrabSuccess)  {
        XUnmapWindow (this->dsp, this->fs_win);
	n_printf ("Unable to grab keyboard focus: %s\n",
		  error == AlreadyGrabbed ? "AlreadyGrabbed" : 
		  error == GrabNotViewable ? "GrabNotViewable" :
		  error == GrabInvalidTime ? "GrabInvalidTime" : 
		  "GrabFrozen");
	return false;
    }
    return true;
}

//--------------------------------------------------------------------
//  UngrabInputs()
//  
//  02-Aug-01   samuel  create
//--------------------------------------------------------------------
void nGlServer::UngrabInputs ()
{
    // Give up monopoly on input
    XUngrabPointer (this->dsp, CurrentTime);
    XUngrabKeyboard (this->dsp, CurrentTime);
}

//--------------------------------------------------------------------
//  SwitchVideoMode()
//  
//  Switches video modes
//
//  02-Aug-01   samuel  create  
//--------------------------------------------------------------------
bool nGlServer::SwitchVideoMode (XF86VidModeModeInfo *mode, Window win, 
				 int ox, int oy, 
				 int width, int height, 
				 bool lock)
{
    XReparentWindow (this->dsp, this->ctx_win, win, 0, 0);
    XSync (this->dsp, false);
    if (!XF86VidModeSwitchToMode (this->dsp,this->xvis->screen, mode)) {
        n_printf ("Unable to restore mode: %d, %d\n",
		  mode->hdisplay, mode->vdisplay);
	return false;
    }
    // If one is running X with a virtual screen the following function call
    // should restore the original viewport. On X4.0.1a it doesnt do anything.
    XF86VidModeSetViewPort(this->dsp, 
			   this->xvis->screen, 
			   ox, oy);
    XF86VidModeLockModeSwitch (this->dsp, this->xvis->screen, lock);
    XWarpPointer (this->dsp, 
		  None, 
		  this->ctx_win, 
		  0, 0, 0, 0, 
		  width >> 1,
		  height >> 1);
    return true;
}

//--------------------------------------------------------------------
//  dispOpen()
//  Resized und mappt das Window, so dass es sichtbar wird.
//  13-May-99   floh    created
//  24-Dec-99   floh    + new arguments for fullscreen support
//                        (only under Win32 right now)
//  30-Jul-01   samuel  + added linux window resizing code
//  02-Aug-01   samuel  added full screen extension support
//--------------------------------------------------------------------

static Bool GetModeInfo (Display *dpy, int scr, XF86VidModeModeInfo *info)
{
    XF86VidModeModeLine *l;

    l = (XF86VidModeModeLine *) ((char *)info + sizeof(info->dotclock));
    return XF86VidModeGetModeLine (dpy, scr, (int *)&info->dotclock, l);
}

bool nGlServer::dispOpen(bool fullscreen, int w, int h, int)
{
    // ignore bpp argument
    n_assert(this->dsp);
    n_assert(this->ctx_win);
    n_assert(this->wm_win);
    n_assert(!this->display_open);

    if (fullscreen && !this->ext_XF86VIDMODE)
        return false;

    XResizeWindow(this->dsp,this->ctx_win,w,h);
    XResizeWindow(this->dsp,this->wm_win,w,h);

    XMapWindow(this->dsp,this->ctx_win);

    if (!fullscreen) {
        XMapWindow(this->dsp,this->wm_win);
	XSync (this->dsp, false);
	if (this->is_fullscreen) {
	    XF86VidModeModeInfo mode;
	    GetModeInfo (this->dsp, this->xvis->screen, &mode);

	    if (!((mode.hdisplay == this->wm_mode.hdisplay) && 
		  (mode.vdisplay == this->wm_mode.vdisplay))) {
	        if (!this->SwitchVideoMode (&this->wm_mode, this->wm_win, 
					    this->wm_mode_x, this->wm_mode_y,
					    w, h, false))
		    return false;
	    } else {
	        XReparentWindow (this->dsp, this->ctx_win, this->wm_win, 0, 0);
		XWarpPointer (this->dsp, 
			      None, 
			      this->ctx_win, 
			      0, 0, 0, 0, 
			      w >> 1,
			      h >> 1);
	    }
	    this->UngrabInputs ();
	    XUnmapWindow (this->dsp, this->fs_win);
	    this->is_fullscreen = false;
	}
    } else {
        bool to_fullscreen = fullscreen && !this->is_fullscreen;
	bool ch_fullscreen = this->is_fullscreen && fullscreen;

	if (to_fullscreen) {
	    n_printf ("FullScreen Mode: %d, %d\n", 
		      this->fs_mode.hdisplay, this->fs_mode.vdisplay);

	    XResizeWindow(this->dsp,this->fs_win,w,h);
	    // Remember current windowed mode configuration
	    GetModeInfo (this->dsp, this->xvis->screen, &this->wm_mode);
	    XF86VidModeGetViewPort (this->dsp, this->xvis->screen, 
				    &this->wm_mode_x, &this->wm_mode_y);
	    XClearWindow (this->dsp, this->fs_win);
	    XMapRaised (this->dsp, this->fs_win);
	 
	} else if (ch_fullscreen) {
	    if ((this->fs_mode.hdisplay == w) && 
		(this->fs_mode.vdisplay == h)) {
	        if (!this->GrabInputs ())
		  return false;
		ch_fullscreen = false;
	    }
	}

	if (ch_fullscreen || to_fullscreen) {
	    int old_w = this->fs_mode.hdisplay;
	    if (!GetVideoMode (this->fs_mode, w, h))
	        return false;
	    if (!this->GrabInputs ()) {
	        XUnmapWindow(this->dsp,this->fs_win);
		return false;
	    }
	    if (ch_fullscreen && (old_w < w))
	        XResizeWindow(this->dsp,this->fs_win,w,h);

	    if (!this->SwitchVideoMode (&this->fs_mode, this->fs_win, 
					0, 0, w, h, true))
	        return false;

	    if (ch_fullscreen && (old_w > w))
	        XResizeWindow(this->dsp,this->fs_win,w,h);

	    this->is_fullscreen = true;
	}
    }

    this->disp_w = w;
    this->disp_h = h;
    XSync (this->dsp, false);
    // ensure GLX resizes context to new window size
    glXWaitX ();
    this->_reshapeDisplay(this->disp_w, this->disp_h);
    this->display_open = true;
    return true;
}


//--------------------------------------------------------------------
//  dispClose()
//  Unmapped das Window, so dass es unsichtbar wird.
//  02-Aug-01   samuel  added full screen extension support
//--------------------------------------------------------------------
bool nGlServer::dispClose(void)
{
    n_assert(this->dsp);
    n_assert(this->ctx_win);
    n_assert(this->display_open);
    XUnmapWindow(this->dsp,this->ctx_win);
    if (this->is_fullscreen) {
      n_assert (this->ext_XF86VIDMODE);
      this->UngrabInputs ();
    } else {
      XUnmapWindow(this->dsp,this->wm_win);
    }
    this->display_open = false;
    return true;
}    

//------------------------------------------------------------------------------
/**
    Show the mouse cursor (no effect yet under Linux).
*/
void
nGlServer::ShowCursor()
{
    nGfxServer::ShowCursor();
}

//------------------------------------------------------------------------------
/**
    Hide the mouse cursor (no effect yet under Linux).
*/
void
nGlServer::HideCursor()
{
    nGfxServer::HideCursor();
}

#endif /* __LINUX__ */
//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
