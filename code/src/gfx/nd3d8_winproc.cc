#define N_IMPLEMENTS nD3D8Server
//-----------------------------------------------------------------------------
//  nd3d8_winproc.cc
//  (C) 2001 RadonLabs GmbH -- A.Weissflog
//-----------------------------------------------------------------------------
#include "gfx/nd3d8server.h"
#include "input/ninputserver.h"
#include <zmouse.h>

extern nKey nwin32_TranslateKey(int vkey);

//-----------------------------------------------------------------------------
/**.
    nD3D8Server's WinProc, besides the usual window cleaning it
    also generates Key- und Mouse-Events for Nebula's input server.
    The WinProc gets the pointer to the nD3D8Server object from
    the window's user data field (initialized in winOpen()).

    history:
    - 23-Jan-2001   floh    created
*/
LONG 
WINAPI 
nd3d8_WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LONG retval = 1;

    // user data of windows contains 'this' pointer
    nD3D8Server *d3d8 = (nD3D8Server *) GetWindowLong(hWnd,0);

    switch(uMsg) 
    {
        case WM_SYSCOMMAND:
            // prevent moving/sizing and power loss in fullscreen mode
            if (d3d8 && d3d8->windowFullscreen) 
            {
                switch (wParam)
                {
                    case SC_MOVE:
                    case SC_SIZE:
                    case SC_MAXIMIZE:
                    case SC_KEYMENU:
                    case SC_MONITORPOWER:
                        return 1;
                        break;
                }
            }
            break;

        case WM_ERASEBKGND:
            // prevent windows from erasing 
            return 1;

        case WM_SIZE:
            // see if we become minimized
            if (d3d8)
            {
                if ((SIZE_MAXHIDE==wParam) || (SIZE_MINIMIZED==wParam))
                    d3d8->windowMinimized = true;
                else 
                    d3d8->windowMinimized = false;
            }
            break;

        case WM_SETCURSOR:
            // show/hide the system and/or custom d3d cursor
            if (d3d8)
            {
                if (d3d8->d3dCursorShown)
                {
                    SetCursor(NULL);
                    d3d8->d3d8Dev->ShowCursor(TRUE);
                    return TRUE;
                }
                else if (d3d8->systemCursorShown)
                {
                    d3d8->d3d8Dev->ShowCursor(FALSE);
                }
                else
                {
                    SetCursor(NULL);
                    return TRUE;
                }
            }

            break;

        case WM_PAINT:
            // flip the buffers out of order, to fill window
            // with something useful, but only in windowed mode
            if (d3d8 && (!d3d8->windowFullscreen)) 
            {
                // when in fullscreen, turn off the system cursor,
                // and optionally set the d3d cursor
                d3d8->present();
            }
            break;

        case WM_SETFOCUS:
            // tell input server that we have obtained the input focus
            if (d3d8 && d3d8->ref_is.isvalid())
            {
                d3d8->ref_is->ObtainFocus();
            }
            break;

        case WM_KILLFOCUS:
            // tell input server that we have lost the input focus
            if (d3d8 && d3d8->ref_is.isvalid())
            {
                d3d8->ref_is->LoseFocus();
            }
            break;

        case WM_CLOSE:
            // ask Nebula to quit, everything else should happen in ~nD3D8Server()
            d3d8->quitRequested = true;
            return 0;
            break;

        case WM_COMMAND:
            if (d3d8) {
                switch (LOWORD(wParam)) {
                    case nD3D8Server::N_D3DCMD_TOGGLEFULLSCREEN:
                        {
                            // toggle fullscreen/windowed
                            char mode_str[64];
                            if (d3d8->windowFullscreen) {
                                sprintf(mode_str,"type(win)-w(%d)-h(%d)",d3d8->disp_w,d3d8->disp_h);
                            } else {
                                sprintf(mode_str,"type(full)-w(%d)-h(%d)-bpp(%d)",d3d8->disp_w,d3d8->disp_h,d3d8->disp_bpp);
                            }
                            d3d8->CloseDisplay();
                            d3d8->SetDisplayMode(mode_str);
                            d3d8->OpenDisplay();
                        }
                        break;

                    case nD3D8Server::N_D3DCMD_NEXTMODE:
                        {
                            char mode_str[128];
                            if (d3d8->nextDisplayMode(mode_str,sizeof(mode_str))) {
                                d3d8->CloseDisplay();
                                d3d8->SetDisplayMode(mode_str);
                                d3d8->OpenDisplay();
                            }
                        }
                        break;
                    case nD3D8Server::N_D3DCMD_PREVMODE:
                        {
                            char mode_str[128];
                            if (d3d8->prevDisplayMode(mode_str,sizeof(mode_str))) {
                                d3d8->CloseDisplay();
                                d3d8->SetDisplayMode(mode_str);
                                d3d8->OpenDisplay();
                            }
                        }
                        break;
                }
            }
            break;

        case WM_KEYDOWN:
            if (d3d8 && d3d8->ref_is.isvalid()) {
                nKey nk = nwin32_TranslateKey((int)wParam);
                nInputEvent *ie = d3d8->ref_is->NewEvent();
                if (ie) {
                    ie->SetType(N_INPUT_KEY_DOWN);
                    ie->SetDeviceId(N_INPUT_KEYBOARD(0));
                    ie->SetKey(nk);
                    d3d8->ref_is->LinkEvent(ie);
                }
            }
            break;

        case WM_KEYUP:
            if (d3d8 && d3d8->ref_is.isvalid()) {
                nKey nk = nwin32_TranslateKey((int)wParam);
                nInputEvent *ie = d3d8->ref_is->NewEvent();
                if (ie) {
                    ie->SetType(N_INPUT_KEY_UP);
                    ie->SetDeviceId(N_INPUT_KEYBOARD(0));
                    ie->SetKey(nk);
                    d3d8->ref_is->LinkEvent(ie);
                }
            }
            break;

        case WM_CHAR:
            if (d3d8 && d3d8->ref_is.isvalid()) {
                nInputEvent *ie = d3d8->ref_is->NewEvent();
                if (ie) {
                    ie->SetType(N_INPUT_KEY_CHAR);
                    ie->SetDeviceId(N_INPUT_KEYBOARD(0));
                    ie->SetChar((int) wParam);
                    d3d8->ref_is->LinkEvent(ie);
                }    
            }
            break;

        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDBLCLK:
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
            /// @TODO check if window already has focus
            // this is hack to target input from keyboard to nebula window
            if (d3d8->parent_hWnd) 
                SetFocus(hWnd);
            if (d3d8 && d3d8->ref_is.isvalid()) {
                nInputEvent *ie = d3d8->ref_is->NewEvent();
                if (ie) {
                    short x = LOWORD(lParam);
                    short y = HIWORD(lParam);
                    switch(uMsg) {
                        case WM_LBUTTONDBLCLK:
                        case WM_RBUTTONDBLCLK:
                        case WM_MBUTTONDBLCLK:
                            ie->SetType(N_INPUT_BUTTON_DBLCLCK);
                            break;
                        case WM_LBUTTONDOWN:
                        case WM_RBUTTONDOWN:
                        case WM_MBUTTONDOWN:
                            SetCapture(hWnd);
                            ie->SetType(N_INPUT_BUTTON_DOWN);
                            break;
                        case WM_LBUTTONUP:
                        case WM_RBUTTONUP:
                        case WM_MBUTTONUP:
                            ReleaseCapture();
                            ie->SetType(N_INPUT_BUTTON_UP);
                            break;
                    }
                    switch(uMsg) {
                        case WM_LBUTTONDBLCLK:
                        case WM_LBUTTONDOWN:
                        case WM_LBUTTONUP:
                            ie->SetButton(0);
                            break;
                        case WM_RBUTTONDBLCLK:
                        case WM_RBUTTONDOWN:
                        case WM_RBUTTONUP:
                            ie->SetButton(1);
                            break;
                        case WM_MBUTTONDBLCLK:
                        case WM_MBUTTONDOWN:
                        case WM_MBUTTONUP:
                            ie->SetButton(2);
                            break;
                    }
                    ie->SetDeviceId(N_INPUT_MOUSE(0));
                    ie->SetAbsPos(x, y);
                    float relX = float(x) / d3d8->renderWidth;
                    float relY = float(y) / d3d8->renderHeight;
                    ie->SetRelPos(relX, relY);
                    d3d8->ref_is->LinkEvent(ie);
                }
            }
            break;

        case WM_MOUSEMOVE:
            if (d3d8 && d3d8->ref_is.isvalid()) {
                short x = LOWORD(lParam);
                short y = HIWORD(lParam);
                nInputEvent *ie = d3d8->ref_is->NewEvent();
                if (ie) {
                    ie->SetType(N_INPUT_MOUSE_MOVE);
                    ie->SetDeviceId(N_INPUT_MOUSE(0));
                    ie->SetAbsPos(x, y);
                    float relX = float(x) / d3d8->renderWidth;
                    float relY = float(y) / d3d8->renderHeight;
                    ie->SetRelPos(relX, relY);
                    d3d8->ref_is->LinkEvent(ie);
					if (d3d8->windowFullscreen && d3d8->d3dCursorShown)
					{
						d3d8->d3d8Dev->SetCursorPosition(x,y,0);
					}
                }
            }
            break;
        case WM_MOUSEWHEEL:
            if (d3d8 && d3d8->ref_is.isvalid()) {
                long axis = HIWORD(wParam);
                nInputEvent *ie = d3d8->ref_is->NewEvent();
                if (ie) {
                    ie->SetType(N_INPUT_MOUSE_WHEEL);
                    ie->SetDeviceId(N_INPUT_MOUSE(0));
					ie->SetAxisValue(axis);
                    d3d8->ref_is->LinkEvent(ie);
                }
            }
            break;
    }
    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}
//-----------------------------------------------------------------------------










       
