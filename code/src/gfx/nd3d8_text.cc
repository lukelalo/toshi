#define N_IMPLEMENTS nD3D8Server
//-----------------------------------------------------------------------------
//  nd3d8_text.cc
//  (C) 2001 A.Weissflog
//-----------------------------------------------------------------------------
#include "gfx/nd3d8server.h"
#include "gfx/nd3dcommon.h"

//-----------------------------------------------------------------------------
/**
    Initialize the text renderer, should be called from devOpen() after
    the d3d device has been created. Will validate
    this->hFont 
    this->d3dxFont

    @return     true if text renderer could be initialized

    11-Jul-01   floh    rewritten to CD3DFont class, ID3DXFonts have been
                        too slow on some cards
*/
bool
nD3D8Server::initTextRenderer()
{
    n_assert(this->d3d8Dev);
    n_assert(!this->d3dFont);
    
    // create the CD3DFont object
    this->d3dFont = n_new CD3DFont("Arial", 10, D3DFONT_BOLD);
    n_assert(this->d3dFont);

    // initialize the font object
    this->d3dFont->InitDeviceObjects(this->d3d8Dev);
    this->d3dFont->RestoreDeviceObjects();

    return true;
}

//-----------------------------------------------------------------------------
/**
    Shutdown the text renderer.
*/
void
nD3D8Server::killTextRenderer()
{
    // kill any leftover text node objects
    nD3DTextNode* curTextNode;
    while (curTextNode = (nD3DTextNode*) this->textList.RemHead())
    {
        n_delete curTextNode;
    }

    // kill the CD3DFont object
    if (this->d3dFont)
    {
        n_delete this->d3dFont;
        this->d3dFont = 0;
    }
}

//-----------------------------------------------------------------------------
/**
    Start a text rendering. Can be called multiple times per frame.

    @return     true if it is valid to render text
*/
bool
nD3D8Server::BeginText()
{
    return true;
}

//-----------------------------------------------------------------------------
/**
    Finish rendering text. Each BeginText() requires a corresponding
    call to EndText().

    @return     true if everything ok
*/
bool
nD3D8Server::EndText()
{
    return true;
}

//-----------------------------------------------------------------------------
/**
    Set text rendering cursor to a new position.

    @param  x   new x coord (-1.0 .. +1.0)
    @param  y   new y coord (-1.0 .. +1.0)
*/
void
nD3D8Server::TextPos(float x, float y)
{
    this->textXPos = x;
    this->textYPos = y;
}

//-----------------------------------------------------------------------------
/**
    Define a string to be rendered, can contain newlines.

    @param  text    a const char* pointing to the string to be rendered
    @return         true if the text was accepted
*/
bool
nD3D8Server::Text(const char* text)
{
    nD3DTextNode *tn = n_new nD3DTextNode(this->textXPos, this->textYPos, text);
    this->textList.AddTail((nNode *)tn);
    return true;
}

//-----------------------------------------------------------------------------
/**
    Render (and delete) the text nodes objects that have been defined during 
    the frame. Should be called from nD3D8Server::EndScene().

    07-Mar-01   floh    turn off fogging, don't restore fogging state, since
                        text rendering happens at the end of the frame anyway
    11-Jul-01   floh    using CD3DFont
*/
void
nD3D8Server::renderTextNodes()
{
    n_assert(this->d3d8Dev);
    n_assert(this->d3dFont);

    // do nothing if there are no text nodes to be rendered
    if (this->textList.IsEmpty())
    {
        return;
    }

    // turn off the fogenable rendering state, this is a known
    // bug in the D3D8 runtime
    this->d3d8Dev->SetRenderState(D3DRS_FOGENABLE, FALSE);

    // render the text nodes
    nD3DTextNode* textNode;
    while (textNode = (nD3DTextNode*) this->textList.RemHead())
    {
        this->d3dFont->DrawText(((textNode->x + 1.0f) * 0.5f * this->renderWidth) + 1, 
                                ((textNode->y + 1.0f) * 0.5f * this->renderHeight) + 1,
                                D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, 1.0f),
                                (char*) textNode->text, D3DFONT_FILTERED);
        this->d3dFont->DrawText((textNode->x + 1.0f) * 0.5f * this->renderWidth, 
                                (textNode->y + 1.0f) * 0.5f * this->renderHeight,
                                D3DCOLOR_COLORVALUE(1.0f, 1.0f, 0.0f, 1.0f),
                                (char*) textNode->text, 0);
        n_delete textNode;
    }
}

//-----------------------------------------------------------------------------



