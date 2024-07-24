#ifndef N_MOUSECURSOR_H
#define N_MOUSECURSOR_H
//------------------------------------------------------------------------------
/**
    @class nMouseCursor

    @brief Hold mouse cursor attributes.

    (C) 2002 RadonLabs GmbH
*/

#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_STRING_H
#include "util/nstring.h"
#endif

//------------------------------------------------------------------------------
class nMouseCursor
{
public:
    /// constructor
    nMouseCursor();
    /// destructor
    ~nMouseCursor();
    /// set mouse cursor image path
    void SetImagePath(const char* path);
    /// get mouse cursor image path
    const char* GetImagePath();
    /// set hotspot
    void SetHotspot(int x, int y);
    /// get hotspot
    void GetHotspot(int& x, int& y);
    /// set texture object
    void SetTexture(nTexture* tex);
    /// get texture object
    nTexture* GetTexture();
    /// clear everything
    void Clear();

private:
    nString imagePath;
    nRef<nTexture> refTexture;
    int xHotspot;
    int yHotspot;
};

//------------------------------------------------------------------------------
/**
*/
inline
nMouseCursor::nMouseCursor() :
    xHotspot(0),
    yHotspot(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nMouseCursor::~nMouseCursor()
{
    if (this->refTexture.isvalid())
    {
        this->refTexture->Release();
        this->refTexture.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMouseCursor::SetImagePath(const char* path)
{
    n_assert(path);
    this->imagePath = path;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nMouseCursor::GetImagePath()
{
    if (this->imagePath.IsEmpty())
    {
        return 0;
    }
    else
    {
        return this->imagePath.Get();
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMouseCursor::SetHotspot(int x, int y)
{
    this->xHotspot = x;
    this->yHotspot = y;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMouseCursor::GetHotspot(int& x, int& y)
{
    x = this->xHotspot;
    y = this->yHotspot;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMouseCursor::SetTexture(nTexture* tex)
{
    if (this->refTexture.isvalid())
    {
        this->refTexture->Release();
        this->refTexture.invalidate();
    }
    this->refTexture = tex;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTexture*
nMouseCursor::GetTexture()
{
    if (this->refTexture.isvalid())
    {
        return this->refTexture.get();
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMouseCursor::Clear()
{
    this->imagePath = "";
    this->xHotspot = 0;
    this->yHotspot = 0;
    if (this->refTexture.isvalid())
    {
        this->refTexture->Release();
        this->refTexture.invalidate();
    }
}

//------------------------------------------------------------------------------
#endif
