#define N_IMPLEMENTS nGlServer
//-------------------------------------------------------------------
//  ngl_macosxdisp.cc -- MacOS X display initialization
//
//  (C) 1998 Andre Weissflog
//-------------------------------------------------------------------
#include "kernel/ntypes.h"
#if defined(__MACOSX__)

#include "kernel/nkernelserver.h"
#include "kernel/nenv.h"
#include "gfx/nglserver.h"
#include "input/ninputserver.h"

#include <Carbon/carbon.h>
#include <AGL/agl.h>

//-------------------------------------------------------------------
//  doCloseWindow()
//  26-Sep-02   daniel_t    created
//-------------------------------------------------------------------
pascal OSStatus doCloseWindow( EventHandlerCallRef /* nextHandler */, EventRef /* event */, void* data )
{
	nGlServer* gls = (nGlServer*)data;
	gls->invalidate_context();
	gls->quit_requested = true;
	return eventNotHandledErr;
}

pascal OSStatus doResizeWindow( EventHandlerCallRef /* nextHandler */, EventRef event, void* data )
{
	nGlServer* gls = (nGlServer*)data;
	Rect windowSize;
	GetEventParameter( event, kEventParamCurrentBounds, typeQDRectangle, NULL, sizeof(Rect), NULL, &windowSize );
	gls->_reshapeDisplay( windowSize.right - windowSize.left, windowSize.bottom - windowSize.top );
	aglUpdateContext( gls->ctx );
	return eventNotHandledErr;
}

int whichButton( EventRef event )
{
	int result = 0;
	UInt16 whichButton;
	GetEventParameter( event, kEventParamMouseButton, typeMouseButton, NULL, sizeof(whichButton), NULL, &whichButton );
	if ( whichButton == kEventMouseButtonPrimary )
	{
		UInt32 modiferKeys;
		GetEventParameter( event, kEventParamKeyModifiers, typeUInt32, NULL, sizeof(modiferKeys), NULL, &modiferKeys );
		if ( modiferKeys & cmdKey )
		{
			result = 1;
		}
	}
	else if ( whichButton == kEventMouseButtonSecondary )
	{
		result = 1;
	}
	else if ( whichButton == kEventMouseButtonTertiary )
	{
		result = 2;
	}
	return result;
}

void doMouse( nInputEvent* ie, Point where, int render_width, int render_height )
{
	ie->SetDeviceId( N_INPUT_MOUSE( 0 ) );
	ie->SetAbsPos( where.h, where.v );
	float relX = float( where.h ) / render_width;
	float relY = float( where.v ) / render_height;
	ie->SetRelPos( relX, relY );
}

pascal OSStatus doMouseDown( EventHandlerCallRef nextHandler, EventRef event, void* data )
{
	OSStatus result = CallNextEventHandler( nextHandler, event );
	if ( result != noErr )
	{
		nGlServer* gls = (nGlServer*)data;
		if ( gls->ref_is.isvalid() )
		{
			nInputEvent* ie = gls->ref_is->NewEvent();
			if ( ie )
			{
				Point where;
				GetEventParameter( event, kEventParamMouseLocation, typeQDPoint, NULL, sizeof(where), NULL, &where );
				UInt32 clickCount;
				GetEventParameter( event, kEventParamClickCount, typeUInt32, NULL, sizeof(clickCount), NULL, &clickCount );
				if ( clickCount > 1 )
				{
					ie->SetType( N_INPUT_BUTTON_DBLCLCK );
				}
				else
				{
					ie->SetType( N_INPUT_BUTTON_DOWN );
				}
				ie->SetButton( whichButton( event ) );
				doMouse( ie, where, gls->render_width, gls->render_height );
				gls->ref_is->LinkEvent( ie );
			}
		}
	}
	return result;
}

pascal OSStatus doMouseUp( EventHandlerCallRef /* nextHandler */, EventRef event, void* data )
{
	nGlServer* gls = (nGlServer*)data;
	if ( gls->ref_is.isvalid() )
	{
		nInputEvent* ie = gls->ref_is->NewEvent();
		if ( ie )
		{
			ie->SetType( N_INPUT_BUTTON_UP );
			ie->SetButton( whichButton( event ) );
			Point where;
			GetEventParameter( event, kEventParamMouseLocation, typeQDPoint, NULL, sizeof(where), NULL, &where );
			doMouse( ie, where, gls->render_width, gls->render_height );
			gls->ref_is->LinkEvent( ie );
		}
	}
	return eventNotHandledErr;
}

pascal OSStatus doMouseMove( EventHandlerCallRef /* nextHandler */, EventRef event, void* data )
{
	nGlServer* gls = (nGlServer*)data;
	if ( gls->ref_is.isvalid() )
	{
		nInputEvent* ie = gls->ref_is->NewEvent();
		if ( ie )
		{
			Point where;
			GetEventParameter( event, kEventParamMouseLocation, typeQDPoint, NULL, sizeof(where), NULL, &where );
			ie->SetType( N_INPUT_MOUSE_MOVE );
			
			doMouse( ie, where, gls->render_width, gls->render_height );
			gls->ref_is->LinkEvent( ie );
		}
	}
	return eventNotHandledErr;
}

extern nKey nmac_TranslateKey( UInt32 key );

pascal OSStatus doKeyDown( EventHandlerCallRef nextHandler, EventRef event, void* data )
{
	OSStatus result = CallNextEventHandler( nextHandler, event );
	if ( result != noErr )
	{
		nGlServer* gls = (nGlServer*)data;
		if ( gls->ref_is.isvalid() )
		{
			nInputEvent* ie = gls->ref_is->NewEvent();
			if ( ie )
			{
				ie->SetType( N_INPUT_KEY_DOWN );
				ie->SetDeviceId( N_INPUT_KEYBOARD( 0 ) );
				UInt32 key;
				GetEventParameter( event, kEventParamKeyCode, typeUInt32, NULL, sizeof(key), NULL, &key );
				nKey nk = nmac_TranslateKey( key );
				ie->SetKey( nk );
				gls->ref_is->LinkEvent( ie );
			}
			char c;
			GetEventParameter( event, kEventParamKeyMacCharCodes, typeChar, NULL, sizeof(c), NULL, &c );
			if ( isprint( c ) )
			{
				ie = gls->ref_is->NewEvent();
				if ( ie )
				{
					ie->SetType( N_INPUT_KEY_CHAR );
					ie->SetDeviceId( N_INPUT_KEYBOARD( 0 ) );
					ie->SetChar( c );
					gls->ref_is->LinkEvent( ie );
				}
			}
		}
	}
	return result;
}

pascal OSStatus doKeyUp( EventHandlerCallRef nextHandler, EventRef event, void* data )
{
	OSStatus result = CallNextEventHandler( nextHandler, event );
	if ( result != noErr )
	{
		nGlServer* gls = (nGlServer*)data;
		if ( gls->ref_is.isvalid() )
		{
			nInputEvent* ie = gls->ref_is->NewEvent();
			if ( ie )
			{
				ie->SetType( N_INPUT_KEY_UP );
				ie->SetDeviceId( N_INPUT_KEYBOARD( 0 ) );
				UInt32 key;
				GetEventParameter( event, kEventParamKeyCode, typeUInt32, NULL, sizeof(key), NULL, &key );
				nKey nk = nmac_TranslateKey( key );
				ie->SetKey( nk );
				gls->ref_is->LinkEvent( ie );
			}
		}
	}
	return result;
}

//-------------------------------------------------------------------
//  winOpen()
//  14-Mar-99   floh    created
//-------------------------------------------------------------------
bool nGlServer::winOpen(void)
{
	quit_requested = false;
	window = 0;
	IBNibRef  nibRef = 0;
	OSStatus err = CreateNibReference( CFSTR( "gfxserv" ), &nibRef );
	if ( err == noErr )
	{
		err = CreateWindowFromNib( nibRef, CFSTR( "MainWindow" ), &window );
		DisposeNibReference( nibRef );
	}
	if ( err == noErr )
	{
		SetPortWindowPort( window );
		
		GLint attrib[] = { AGL_RGBA, AGL_DOUBLEBUFFER, AGL_NONE };
		AGLPixelFormat fmt = aglChoosePixelFormat( NULL, 0, attrib ); /* Choose pixel format */
		ctx = aglCreateContext( fmt, NULL ); 	/* Create an AGL context */	
		aglDestroyPixelFormat( fmt ); // pixel format is no longer needed
	
		aglSetDrawable( ctx, GetWindowPort( window ) ); /* Attach the context to the window */

		aglSetCurrentContext(ctx);

		// install events
		// window events
		{
			EventTypeSpec  event;
			EventTargetRef eventTarget = GetWindowEventTarget( window );
			event.eventClass = kEventClassWindow;
			event.eventKind = kEventWindowClose;
			InstallEventHandler( eventTarget, NewEventHandlerUPP( doCloseWindow ), 1, &event, this, NULL );
		
			event.eventKind = kEventWindowBoundsChanged;
			InstallEventHandler( eventTarget, NewEventHandlerUPP( doResizeWindow ), 1, &event, this, NULL );
			
			event.eventClass = kEventClassMouse;
			event.eventKind = kEventMouseDown;
			InstallEventHandler( eventTarget, NewEventHandlerUPP( doMouseDown ), 1, &event, this, NULL );
		}
		// application events
		{
			EventTargetRef eventTarget = GetApplicationEventTarget();
			EventTypeSpec  event[2];
			event[0].eventClass = kEventClassMouse;
			event[0].eventKind = kEventMouseUp;
			InstallEventHandler( eventTarget, NewEventHandlerUPP( doMouseUp ), 1, &event[0], this, NULL );
	
			event[0].eventKind = kEventMouseMoved;
			event[1].eventClass = kEventClassMouse;
			event[1].eventKind = kEventMouseDragged;
			InstallEventHandler( eventTarget, NewEventHandlerUPP( doMouseMove ), 2, &event[0], this, NULL );
	
			event[0].eventClass = kEventClassKeyboard;
			event[0].eventKind = kEventRawKeyDown;
			InstallEventHandler( eventTarget, NewEventHandlerUPP( doKeyDown ), 1, &event[0], this, NULL );
			
			event[0].eventKind = kEventRawKeyUp;
			InstallEventHandler( eventTarget, NewEventHandlerUPP( doKeyUp ), 1, &event[0], this, NULL );
			
		}
	}
	
	return err == noErr;
}

//-------------------------------------------------------------------
//  invalidate_context()
//  Invalidate (close) the OpenGL context, unload all resources
//  bound to the OpenGL context.
//  24-Nov-00   floh    created
//-------------------------------------------------------------------
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
	
	if ( ctx )
	{
		aglDestroyContext( ctx );
		ctx = 0;
	}
}

//-------------------------------------------------------------------
//  winClose()
//  14-Mar-99   floh    created
//-------------------------------------------------------------------
bool nGlServer::winClose(void)
{
    this->invalidate_context();
	if ( window )
	{
		DisposeWindow( window );
		window = 0;
	}
    return true;
}

static bool ngl_initText( nGlServer* gls )
{
	n_assert( gls );
	FontInfo finfo;
	FetchFontInfo( 1, GetPortTextSize( GetWindowPort( gls->window ) ), 0, &finfo );
	gls->text_height = finfo.ascent + finfo.descent ;
	gls->text_listbase = glGenLists( 256 );
	aglUseFont( gls->ctx, GetPortTextFont( GetWindowPort( gls->window ) ), 1, GetPortTextSize( GetWindowPort( gls->window ) ), 0, 256, gls->text_listbase );
	gls->text_initialized = true;
	return true;
}

//-------------------------------------------------------------------
//  dispOpen()
//  14-Mar-99   floh    created
//  24-Dec-99   floh    + rewritten for fullscreen support
//  22-Jan-00   floh    + in fullscreen mode, mouse pointer is hidden
//-------------------------------------------------------------------
bool nGlServer::dispOpen(bool fullscreen, int w, int h, int /* bpp */)
{
    n_assert(!this->display_open);

    this->is_fullscreen = fullscreen; 
	n_warn( "Must finish implementation of nGLServer::dispOpen()" );
    if (this->is_fullscreen) {
		n_warn( "nGLServer::dispOpen() fullscreen not yet implemented" );
		Rect bounds = { 50, 5, h + 5, w + 50 };
		SetWindowBounds( window, kWindowContentRgn, &bounds );
		ShowWindow( window );
    } else {
		Rect bounds = { 50, 5, h + 5, w + 50 };
		SetWindowBounds( window, kWindowContentRgn, &bounds );
		ShowWindow( window );
    }
	ngl_initText( this );

    if (this->cursorShown)
    {
		InitCursor();
    }
    else
    {
		HideCursor();
    }

    this->display_open = true;

    return true;
}

//-------------------------------------------------------------------
/**
    14-Mar-99   floh    created
    24-Dec-99   floh    fullscreen support
    22-Jan-00   floh    restore mouse pointer when coming from
                        fullscreen mode
*/
//-------------------------------------------------------------------
bool nGlServer::dispClose(void)
{
	n_assert( this->display_open );
	this->display_open = false;
	if ( window )
	{
		HideWindow( window );
	}
    return false;
}

//------------------------------------------------------------------------------
/**
    @brief Show the mouse cursor.
*/
void
nGlServer::ShowCursor()
{
	ShowCursor(); // maybe should call InitCursor()?
    nGfxServer::ShowCursor();
}

//------------------------------------------------------------------------------
/**
    @brief Hide the mouse cursor.
*/
void
nGlServer::HideCursor()
{
	HideCursor();
    nGfxServer::HideCursor();
}

#endif /* __MACOSX__ */
//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------

