#define N_IMPLEMENTS nGlTexture
//------------------------------------------------------------------------------
/**
    ngltex_openil.cc
    
    Texture load function using OpenIL library functions.

    (C) A.Weissflog 2001
*/
#include "gfx/ngltexture.h"
#include "il/il.h"
#include "il/ilu.h"

//------------------------------------------------------------------------------
/**
    Load an image file into an OpenGL texture. Allocate a new OpenGL texture
    and write its name to this->tex_id.

    @param  fileName    filename of the texture file
    @return             true if texture successfully loaded and bound
*/
bool
nGlTexture::loadFileIntoTextureIL(const char* fileName)
{
    n_assert(0 == this->tex_id);

    // load the image
    ILuint imgId;
    ilGenImages(1, &imgId);
    ilBindImage(imgId);
    if (!ilLoadImage((char*) fileName)) {
        n_printf("nGlTexture: Failed loading image file '%s' as texture!\n", fileName);
        n_printf("OpenIL error: '%s'\n", iluErrorString(ilGetError()));
        ilDeleteImages(1, &imgId);
        return false;
    }

    // if this is a paletted image, convert it to RGB format
    if (ilGetInteger(IL_IMAGE_FORMAT) == IL_COLOR_INDEX)
    {
        if (!ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE))
        {
            n_printf("nGlTexture: ilConvertImage() failed with '%s'\n", iluErrorString(ilGetError()));
            ilDeleteImages(1, &imgId);
            return false;
        }
    }

    // generate mipmaps if necessary
    //if (this->gen_mipmaps)
    {
        if (!iluBuildMipmaps())
        {
            n_printf("nGlTexture: iluBuildMipmaps() failed with '%s'\n", iluErrorString(ilGetError()));
            ilDeleteImages(1, &imgId);
            return false;
        }
    }

    // bind the images to OpenGL
    glGenTextures(1, &this->tex_id);
    glBindTexture(GL_TEXTURE_2D, this->tex_id);

    // for each mipmap level...
    ILint  numMipmaps = ilGetInteger(IL_NUM_MIPMAPS);
    GLint  components = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
    GLenum type       = (GLenum) ilGetInteger(IL_IMAGE_TYPE);
    GLenum format     = (GLenum) ilGetInteger(IL_IMAGE_FORMAT);
    int imgWidth  = ilGetInteger(IL_IMAGE_WIDTH);
    int imgHeight = ilGetInteger(IL_IMAGE_HEIGHT);
    this->SetWidth(imgWidth);
    this->SetHeight(imgHeight);
    this->SetBytesPerPixel(components);
    this->SetNumMipLevels(numMipmaps + 1);
    
    GLint curLevel;
    for (curLevel=0; curLevel<=numMipmaps; curLevel++)
    {
        // need to rebind original image before selecting 
        // mipmap level (is this a design bug in OpenIL?)
        ilBindImage(imgId);
        if (!ilActiveMipmap(curLevel))
        {
            n_printf("nGlTexture: ilActiveMipmap() failed with '%s'\n", iluErrorString(ilGetError()));
            ilDeleteImages(1, &imgId);
            return false;
        }

        GLsizei curWidth        = ilGetInteger(IL_IMAGE_WIDTH);
        GLsizei curHeight       = ilGetInteger(IL_IMAGE_HEIGHT);
        const GLvoid* curData   = (GLvoid*) ilGetData();

        glTexImage2D(GL_TEXTURE_2D,     // target
                     curLevel,          // mipmap level-of-detail
                     components,        // components (internal format)
                     curWidth,          // width
                     curHeight,         // height
                     0,                 // border
                     format,            // format (GL_RGB, GL_RGBA...)
                     type,              // type (GL_UNSIGNED_BYTE...)
                     curData);          // pointer to image data
    }

    // kill temporary IL image data
    ilDeleteImages(1, &imgId);
    return true;
}
