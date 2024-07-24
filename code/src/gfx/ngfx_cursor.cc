#define N_IMPLEMENTS nGfxServer
//------------------------------------------------------------------------------
//  ngfx_cursor.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx/ngfxserver.h"
#include "kernel/nfileserver2.h"

//------------------------------------------------------------------------------
/**
    Begin defining the mouse cursors. A mouse cursor is defined by a
    path to a texture and the hotspot of the mouse cursor.

    @param  num     number of cursors to be defined
*/
void
nGfxServer::BeginCursors(int num)
{
    n_assert((num > 0) && (num < N_MAXCURSORS));

    // clear previous names and textures
    int i;
    for (i = 0; i < this->numCursors; i++)
    {
        this->cursor[i].Clear();
    }
    this->numCursors = num;
}

//------------------------------------------------------------------------------
/**
    Set the texture path and hotspot for a mouse cursor.

    @param  index       mouse cursor index
    @param  texPath     path to texture image
    @param  xHotspot    cursor hotspot in x direction
    @param  yHotspot    cursor hotspot in y direction
*/
void
nGfxServer::SetCursor(int index, const char* texPath, int xHotspot, int yHotspot)
{
    n_assert((index >= 0) && (index < this->numCursors));
    n_assert(texPath);

    char buf[N_MAXPATH];
    nFileServer2* file2 = (nFileServer2*)kernelServer->Lookup("/sys/servers/file2");
    this->cursor[index].SetImagePath(file2->ManglePath(texPath, buf, sizeof(buf)));
    this->cursor[index].SetHotspot(xHotspot, yHotspot);
}

//------------------------------------------------------------------------------
/**
    Finish defining the textures, this will also load all textures.
*/
void
nGfxServer::EndCursors()
{
    this->LoadCursors();
    if (this->cursorShown)
    {
        this->ShowCursor();
    }
    else
    {
        this->HideCursor();
    }
}

//------------------------------------------------------------------------------
/**
    (Re)-Loads the cursor images.
*/
void
nGfxServer::LoadCursors()
{
    int i;
    for (i = 0; i < this->numCursors; i++)
    {
        // first clear previous texture
        this->cursor[i].SetTexture(0);

        // create a texture object
        char rsrcName[N_MAXNAMELEN];
        sprintf(rsrcName, "cursor%d", i);
        nTexture* tex = this->NewTexture(rsrcName);
        n_assert(tex);

        // configure texture
        tex->SetTexture(this->cursor[i].GetImagePath(), 0);
        tex->SetGenMipMaps(false);
        tex->SetHighQuality(true);
        if (!tex->Load())
        {
            n_error("Failed to load '%s' as cursor image!\n", this->cursor[i].GetImagePath());
        }

        // set texture object
        this->cursor[i].SetTexture(tex);
    }
}

//------------------------------------------------------------------------------
/**
    Select the current mouse cursor
*/
void
nGfxServer::SetCurrentCursor(int index, bool show)
{
    n_assert((index >= 0) && (index < this->numCursors));
    this->currentCursor = index;
    if(show == true) this->ShowCursor();
}

//------------------------------------------------------------------------------
/**
    Get the currently selected mouse cursor.
*/
int
nGfxServer::GetCurrentCursor()
{
    return this->currentCursor;
}

//------------------------------------------------------------------------------
/**
    Activate and show the current mouse cursor. Should be implemented
    by subclass.
*/
void
nGfxServer::ShowCursor()
{
    this->cursorShown = true;
}

//------------------------------------------------------------------------------
/**
    Hide the current mouse cursor. Should be implemented by a specific subclass.
*/
void
nGfxServer::HideCursor()
{
    this->cursorShown = false;
}

//------------------------------------------------------------------------------
