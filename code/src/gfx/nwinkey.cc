//-------------------------------------------------------------------
//  nwinkey.cc
//
//  Gemeinsames Modul fuer OpenGL- und Direct3D-Server, welches
//  Win32-Keys nach Nebula-Keys uebersetzt.
//
//  (C) 1999 A.Weissflog
//-------------------------------------------------------------------
#include "kernel/ntypes.h"
#ifdef __WIN32__

#include <windows.h>
#include "input/ninputserver.h"

//-------------------------------------------------------------------
//  nwin32_TranslateKey()
//
//  Uebersetzt Windows-Virtual-Key nach N_KEY.
//
//  01-Dec-98   floh    created
//-------------------------------------------------------------------
nKey nwin32_TranslateKey(int vkey)
{
    nKey nk;
    switch(vkey) {
        case VK_LBUTTON:    nk=N_KEY_LBUTTON; break;
        case VK_RBUTTON:    nk=N_KEY_RBUTTON; break;
        case VK_MBUTTON:    nk=N_KEY_MBUTTON; break;
        case VK_BACK:       nk=N_KEY_BACK; break;
        case VK_TAB:        nk=N_KEY_TAB; break;
        case VK_RETURN:     nk=N_KEY_RETURN; break;
        case VK_SHIFT:      nk=N_KEY_SHIFT; break;
        case VK_CONTROL:    nk=N_KEY_CONTROL; break;
        case VK_MENU:       nk=N_KEY_MENU; break;
        case VK_PAUSE:      nk=N_KEY_PAUSE; break;
        case VK_CAPITAL:    nk=N_KEY_CAPITAL; break;
        case VK_ESCAPE:     nk=N_KEY_ESCAPE; break;
        case VK_SPACE:      nk=N_KEY_SPACE; break;
        case VK_PRIOR:      nk=N_KEY_PRIOR; break;
        case VK_NEXT:       nk=N_KEY_NEXT; break;
        case VK_END:        nk=N_KEY_END; break;
        case VK_HOME:       nk=N_KEY_HOME; break;
        case VK_LEFT:       nk=N_KEY_LEFT; break;
        case VK_UP:         nk=N_KEY_UP; break;
        case VK_RIGHT:      nk=N_KEY_RIGHT; break;
        case VK_DOWN:       nk=N_KEY_DOWN; break;
        case VK_SELECT:     nk=N_KEY_SELECT; break;
        case VK_PRINT:      nk=N_KEY_PRINT; break;
        case VK_EXECUTE:    nk=N_KEY_EXECUTE; break;
        case VK_SNAPSHOT:   nk=N_KEY_SNAPSHOT; break;
        case VK_INSERT:     nk=N_KEY_INSERT; break;
        case VK_DELETE:     nk=N_KEY_DELETE; break;
        case VK_HELP:       nk=N_KEY_HELP; break;
        case '0':           nk=N_KEY_0; break;
        case '1':           nk=N_KEY_1; break;
        case '2':           nk=N_KEY_2; break;
        case '3':           nk=N_KEY_3; break;
        case '4':           nk=N_KEY_4; break;
        case '5':           nk=N_KEY_5; break;
        case '6':           nk=N_KEY_6; break;
        case '7':           nk=N_KEY_7; break;
        case '8':           nk=N_KEY_8; break;
        case '9':           nk=N_KEY_9; break;
        case 'A':           nk=N_KEY_A; break;
        case 'B':           nk=N_KEY_B; break;
        case 'C':           nk=N_KEY_C; break;
        case 'D':           nk=N_KEY_D; break;
        case 'E':           nk=N_KEY_E; break;
        case 'F':           nk=N_KEY_F; break;
        case 'G':           nk=N_KEY_G; break;
        case 'H':           nk=N_KEY_H; break;
        case 'I':           nk=N_KEY_I; break;
        case 'J':           nk=N_KEY_J; break;
        case 'K':           nk=N_KEY_K; break;
        case 'L':           nk=N_KEY_L; break;
        case 'M':           nk=N_KEY_M; break;
        case 'N':           nk=N_KEY_N; break;
        case 'O':           nk=N_KEY_O; break;
        case 'P':           nk=N_KEY_P; break;
        case 'Q':           nk=N_KEY_Q; break;
        case 'R':           nk=N_KEY_R; break;
        case 'S':           nk=N_KEY_S; break;
        case 'T':           nk=N_KEY_T; break;
        case 'U':           nk=N_KEY_U; break;
        case 'V':           nk=N_KEY_V; break;
        case 'W':           nk=N_KEY_W; break;
        case 'X':           nk=N_KEY_X; break;
        case 'Y':           nk=N_KEY_Y; break;
        case 'Z':           nk=N_KEY_Z; break;
        case VK_LWIN:       nk=N_KEY_LWIN; break;
        case VK_RWIN:       nk=N_KEY_RWIN; break;
        case VK_APPS:       nk=N_KEY_APPS; break;
        case VK_NUMPAD0:    nk=N_KEY_NUMPAD0; break;
        case VK_NUMPAD1:    nk=N_KEY_NUMPAD1; break;
        case VK_NUMPAD2:    nk=N_KEY_NUMPAD2; break;
        case VK_NUMPAD3:    nk=N_KEY_NUMPAD3; break;
        case VK_NUMPAD4:    nk=N_KEY_NUMPAD4; break;
        case VK_NUMPAD5:    nk=N_KEY_NUMPAD5; break;
        case VK_NUMPAD6:    nk=N_KEY_NUMPAD6; break;
        case VK_NUMPAD7:    nk=N_KEY_NUMPAD7; break;
        case VK_NUMPAD8:    nk=N_KEY_NUMPAD8; break;
        case VK_NUMPAD9:    nk=N_KEY_NUMPAD9; break;
        case VK_MULTIPLY:   nk=N_KEY_MULTIPLY; break;
        case VK_ADD:        nk=N_KEY_ADD; break;
        case VK_SEPARATOR:  nk=N_KEY_SEPARATOR; break;
        case VK_SUBTRACT:   nk=N_KEY_SUBTRACT; break;
        case VK_DECIMAL:    nk=N_KEY_DECIMAL; break;
        case VK_DIVIDE:     nk=N_KEY_DIVIDE; break;
        case VK_F1:         nk=N_KEY_F1; break;
        case VK_F2:         nk=N_KEY_F2; break;
        case VK_F3:         nk=N_KEY_F3; break;
        case VK_F4:         nk=N_KEY_F4; break;
        case VK_F5:         nk=N_KEY_F5; break;
        case VK_F6:         nk=N_KEY_F6; break;
        case VK_F7:         nk=N_KEY_F7; break;
        case VK_F8:         nk=N_KEY_F8; break;
        case VK_F9:         nk=N_KEY_F9; break;
        case VK_F10:        nk=N_KEY_F10; break;
        case VK_F11:        nk=N_KEY_F11; break;
        case VK_F12:        nk=N_KEY_F12; break;
        case VK_F13:        nk=N_KEY_F13; break;
        case VK_F14:        nk=N_KEY_F14; break;
        case VK_F15:        nk=N_KEY_F15; break;
        case VK_F16:        nk=N_KEY_F16; break;
        case VK_F17:        nk=N_KEY_F17; break;
        case VK_F18:        nk=N_KEY_F18; break;
        case VK_F19:        nk=N_KEY_F19; break;
        case VK_F20:        nk=N_KEY_F20; break;
        case VK_F21:        nk=N_KEY_F21; break;
        case VK_F22:        nk=N_KEY_F22; break;
        case VK_F23:        nk=N_KEY_F23; break;
        case VK_F24:        nk=N_KEY_F24; break;
        case VK_NUMLOCK:    nk=N_KEY_NUMLOCK; break;
        case VK_SCROLL:     nk=N_KEY_SCROLL; break;
        default:            nk=N_KEY_NONE; break;
    }
    return nk;
}
#endif
//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------

