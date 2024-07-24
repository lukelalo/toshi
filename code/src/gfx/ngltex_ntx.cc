#define N_IMPLEMENTS nGlTexture
//------------------------------------------------------------------------------
//  ngltex_ntx.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx/ngltexture.h"
#include "gfx/nntxfile.h"

//------------------------------------------------------------------------------
/**
    Load an image in Nebula's ntx file format and create an OpenGL 
    texture from it.
*/
bool
nGlTexture::loadFileIntoTextureNTX(const char* fileName)
{
    n_assert(0 == this->tex_id);
    n_assert(fileName);
    nGlServer* gfx = this->ref_gs.get();

// n_printf("nGlTexture: Using ntx texture loading code.\n");

    // create a nNtxFile object
    nNtxFile ntxFile(kernelServer);

    // open ntx file for reading
    if (!ntxFile.OpenRead(fileName))
    {
        n_printf("nGlTexture: failed to open ntx file '%s'!\n", fileName);
        return false;
    }

    // OpenGL doesn't support the 565 format as offered by the ntx
    // file format, thus we always read the high quality textures
    // from the ntx file and let OpenGL reduce the bit depth
    ntxFile.SetCurrentBlock(0);
    nNtxFile::Format ntxFormat = ntxFile.GetFormat();
    GLenum glFormat;
    GLint  glInternalFormat;
    if ((ntxFormat == nNtxFile::FORMAT_R8G8B8) || (ntxFormat == nNtxFile::FORMAT_R5G6B5))
    {
        // ntx file has no alpha channel
        if (this->GetHighQuality())
        {
            glInternalFormat = GL_RGB8;
        }
        else
        {
            glInternalFormat = GL_RGB5;
        }

        if (gfx->ext_bgra)
        {
            glFormat = (GLenum)GL_BGR_EXT;
        }
        else
        {
            glFormat = GL_RGB;
        }

        ntxFormat = nNtxFile::FORMAT_R8G8B8;
    }
    else
    {
        // ntx file has alpha channel
        if (this->GetHighQuality())
        {
            glInternalFormat = GL_RGBA8;
        }
        else
        {
            glInternalFormat = GL_RGBA4;
        }

        if (gfx->ext_bgra)
        {
            glFormat = (GLenum)GL_BGRA_EXT;
        }
        else
        {
            glFormat = GL_RGBA;
        }
        ntxFormat = nNtxFile::FORMAT_A8R8G8B8;
    }

    // create an OpenGL texture
    glGenTextures(1, &(this->tex_id));
    glBindTexture(GL_TEXTURE_2D, this->tex_id);

    // allocate temporary image buffer
    int firstBlock = ntxFile.FindBlock(nNtxFile::TYPE_TEXTURE2D, ntxFormat, 0);
    ntxFile.SetCurrentBlock(firstBlock);
    int dataSize    = ntxFile.GetSize();
    void* imageData = n_malloc(dataSize);
    this->SetWidth(ntxFile.GetWidth());
    this->SetHeight(ntxFile.GetHeight());
    this->SetBytesPerPixel(ntxFile.GetBytesPerPixel());

    // for each mipmap level, read image data and write to texture
    int curLevel = 0;
    int curBlock;
    while (-1 != (curBlock = ntxFile.FindBlock(nNtxFile::TYPE_TEXTURE2D, ntxFormat, curLevel))) 
    {
        ntxFile.SetCurrentBlock(curBlock);
        GLint curWidth  = ntxFile.GetWidth();
        GLint curHeight = ntxFile.GetHeight();

        // read image data of current block
        ntxFile.ReadBlock(imageData, dataSize);

        // if BGR pixel formats not support, chew the loaded image data
        if (!gfx->ext_bgra)
        {
            int i;
            uchar* ptr = (uchar*) imageData;
            int numPixels = curWidth * curHeight;
            int bytesPerPixel = ntxFile.GetBytesPerPixel(); 
            for (i = 0; i < numPixels; ptr += bytesPerPixel, i++)
            {
                // swap b and r components
                uchar tmp = ptr[0];
                ptr[0] = ptr[2];
                ptr[2] = tmp;
            }
        }

        // transfer image data to gl texture
        glTexImage2D(GL_TEXTURE_2D,     // target
                     curLevel,          // mipmap level
                     glInternalFormat,  // internal pixel format
                     curWidth,          // width
                     curHeight,         // height
                     0,                 // border
                     glFormat,          // format of image data
                     GL_UNSIGNED_BYTE,  // type
                     imageData);        // data
        curLevel++;
    }
    this->SetNumMipLevels(curLevel);

    // success
    n_free(imageData);
    ntxFile.CloseRead();
    ntxFile.FreeBlocks();
    return true;
}
                        
        

