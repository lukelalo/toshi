//------------------------------------------------------------------------------
//  nbmp2ntx.cc
//
//  Convert bmp files to Nebula's ntx texture files. Merges alpha map,
//  generates mipmaps and stores image in 16 and 32 bit formats.
//  If pname texture has alpha bits then these will be used also.
//
//  Usage:
//
//  nbmp2ntx [-pname pixelname] 
//           [-aname alphaname] 
//           [-mipmap] 
//           [-detailmap] 
//           [-out filename] 
//           [-w x]
//           [-h y]
//           [-smalleronly]
//           [-flip]
//           [-tga]
//           [-32bit]
//
//  -help           show help
//  -pname          name of bmp file containing color information (def texture.bmp)
//  -aname          name of bmp file containing alpha information (def no alpha)
//  -oname          filename of output file (def texture.ntx)
//  -mipmap         generate mipmaps
//  -detailmap      generate special detailmap mipmaps (mipmap levels are black)
//  -w              target width
//  -h              target height
//  -smalleronly    only scale if image larger then target size
//  -flip           flip vertically
//  -tga            save result as tga file.
//  -normalmap      the destination file is a normal map
//  -normcubemap    generate a normalization cube map
//
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include <assert.h>
#include "tools/wftools.h"
#include "il/il.h"
#include "il/ilu.h"
#include "gfx/nntxfile.h"
#include "mathlib/vector.h"

// imported from dither.cc
void gen565Image(int w, int h, uchar* srcBuffer, ushort* dstBuffer);
void gen4444Image(int w, int h, uchar* srcBuffer, ushort* dstBuffer);

//------------------------------------------------------------------------------
//  Return next power of 2.
//------------------------------------------------------------------------------
unsigned int
nextPOW2(unsigned int val)
{
    unsigned int i;
    for (i = 1; i < (1<<30); i <<= 1)
    {
        if (val <= i) 
        {
            return i;
        }
    }
    return i;
}

//------------------------------------------------------------------------------
//  Load source image using DevIL, if a alpha channel file is given, merge
//  it into a new DevIL image. Returns the image name.
//------------------------------------------------------------------------------
bool
loadSourceImage(const char* pname, 
                const char* aname, 
                ILuint& outImage, 
                int targetWidth, 
                int targetHeight, 
                bool smallerOnly,
                bool flip)
{
    ILuint pixelImage;
    ILuint alphaImage;

    pixelImage = iluGenImage();
    alphaImage = iluGenImage();

    // generate and load pixel channel
    ilBindImage(pixelImage);
    if (!ilLoadImage((char*) pname))
    {
        printf("DevIL failed loading image '%s' with '%s'.\n", pname, iluErrorString(ilGetError()));
        iluDeleteImage(pixelImage);
        iluDeleteImage(alphaImage);
        iluDeleteImage(outImage);
        return false;
    }

    // convert to bgr byte order
    if (ilGetInteger(IL_IMAGE_FORMAT) == IL_RGB)
    {    
        ilConvertImage(IL_BGR, IL_UNSIGNED_BYTE);
    }
    else
    {
        ilConvertImage(IL_BGRA, IL_UNSIGNED_BYTE);
    }

    // generate and load optional alpha channel
    if (aname)
    {
        ilBindImage(alphaImage);
        if (!ilLoadImage((char *)aname))
        {
            printf("DevIL failed loading image '%s' with '%s'.\n", aname, iluErrorString(ilGetError()));
            iluDeleteImage(pixelImage);
            iluDeleteImage(alphaImage);
            iluDeleteImage(outImage);
            return false;
        }
        ilConvertImage(IL_BGR, IL_UNSIGNED_BYTE);
    }

    // adjust image size
    ilBindImage(pixelImage);
    ILint pixWidth  = ilGetInteger(IL_IMAGE_WIDTH);
    ILint pixHeight = ilGetInteger(IL_IMAGE_HEIGHT);
    bool forceScale = false;
    if (targetWidth > 0)
    {
        if (smallerOnly)
        {
            if (pixWidth > targetWidth)
            {
                pixWidth = targetWidth;
                forceScale = true;
            }
        }
        else
        {
            pixWidth = targetWidth;
            forceScale = true;
        }
    }
    if (targetHeight > 0)
    {
        if (smallerOnly)
        {
            if (pixHeight > targetHeight)
            {
                pixHeight = targetHeight;
                forceScale = true;
            }
        }
        else
        {
            pixHeight = targetHeight;
            forceScale = true;
        }
    }
    ILint powWidth  = nextPOW2(pixWidth);
    ILint powHeight = nextPOW2(pixHeight);
    if ((pixWidth != powWidth) || (pixHeight != powHeight) || forceScale)
    {
        iluScale(powWidth, powHeight, 1);
    }

    // generate target image
    ilBindImage(outImage);
    ilCopyImage(pixelImage);
    
    // merge alpha channel?
    if (aname)
    {
        // force alpha size to be identical with image size
        ilBindImage(alphaImage);
        ILint alphaWidth  = ilGetInteger(IL_IMAGE_WIDTH);
        ILint alphaHeight = ilGetInteger(IL_IMAGE_HEIGHT);
        if ((powWidth != alphaWidth) || (powHeight != alphaHeight))
        {
            iluScale(powWidth, powHeight, 1);
        }
        ilBindImage(outImage);

        // add an alpha channel to outImage
        ilConvertImage(IL_BGRA, IL_UNSIGNED_BYTE);

        // get image attributes 
        ilBindImage(alphaImage);
        alphaWidth  = ilGetInteger(IL_IMAGE_WIDTH);
        alphaHeight = ilGetInteger(IL_IMAGE_HEIGHT);
        ILint    alphaBpp    = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
        ILubyte* alphaData   = ilGetData();

        ilBindImage(outImage);
        ILint    outWidth  = ilGetInteger(IL_IMAGE_WIDTH);
        ILint    outHeight = ilGetInteger(IL_IMAGE_HEIGHT);
        ILint    outBpp    = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
        ILubyte* outData   = ilGetData();

        assert((alphaWidth == outWidth) && (alphaHeight == outHeight));

        // merge alpha values
        int numPixels = outWidth * outHeight;
        int i;
        for (i = 0; i < numPixels; i++)
        {
            outData[3] = alphaData[0];
            alphaData += alphaBpp;
            outData   += outBpp;
        }
    }

    // flip image if necessary
    if (flip)
    {
        iluFlipImage();
    }

    // delete source images, and return image name of out image
    iluDeleteImage(pixelImage);
    iluDeleteImage(alphaImage);
    return true;
}

//------------------------------------------------------------------------------
//  save image as tga file
//------------------------------------------------------------------------------
bool
saveDestImageTGA(nKernelServer* /* ks */, ILuint srcImage, const char* oname)
{
    ilEnable(IL_FILE_OVERWRITE);
    ilBindImage(srcImage);
    return (0 != ilSave(IL_TGA, (char*) oname));
}

//------------------------------------------------------------------------------
//  Create an NTX file, fill it with the relevant data and write it. Generate
//  mipmaps and 16 bit formats on the fly.
//------------------------------------------------------------------------------
bool
saveDestImageNTX(nKernelServer* ks, ILuint srcImage, const char* oname, bool /* detailmap */, bool nosharpen, bool bpp32)
{
    // create a ntx file object
    nNtxFile ntxFile(ks);

    // get some image params
    ilBindImage(srcImage);
    ILint imgWidth  = ilGetInteger(IL_IMAGE_WIDTH);
    ILint imgHeight = ilGetInteger(IL_IMAGE_HEIGHT);
    ILint imgFormat = ilGetInteger(IL_IMAGE_FORMAT);

    // sharpen the source image
    if ((!nosharpen) && (imgWidth > 16) && (imgHeight > 16))
    {
        iluSharpen(2.0f, 1);
    }

    nNtxFile::Format ntxFormat32 = (imgFormat == IL_BGRA) ? nNtxFile::FORMAT_A8R8G8B8 : nNtxFile::FORMAT_R8G8B8;
    nNtxFile::Format ntxFormat16 = (imgFormat == IL_BGRA) ? nNtxFile::FORMAT_A4R4G4B4 : nNtxFile::FORMAT_R5G6B5;

    bool write16 = !bpp32;
    bool write32 = bpp32;

    // compute number of mipmaps
    int w = imgWidth;
    int h = imgHeight;
    int numMipMaps = 0;
    while ((w > 0) || (h > 0))
    {
        numMipMaps++;
        w >>= 1;
        h >>= 1;
    }

    int numWrite = 0;
    int offset = 0;
    if (write16) numWrite++;
    if (write32) 
    {
        numWrite++;
        offset = numMipMaps;
    }

    // configure ntxFile object
    ntxFile.SetNumBlocks(numWrite * numMipMaps);
    
    // configure 32 and 16 bit block headers
    int i;
    for (i = 0; i < numMipMaps; i++)
    {
        w = imgWidth >> i;
        h = imgHeight >> i;
        if (w == 0) w = 1;
        if (h == 0) h = 1;

        // 32 bit block header
        if (write32)
        {
            ntxFile.SetCurrentBlock(i);
            ntxFile.SetWidth(w);
            ntxFile.SetHeight(h);
            ntxFile.SetDepth(1);
            ntxFile.SetMipLevel(i);
            ntxFile.SetType(nNtxFile::TYPE_TEXTURE2D);
            ntxFile.SetFormat(ntxFormat32);
        }

        // 16 bit block header
        if (write16)
        {
            ntxFile.SetCurrentBlock(offset + i);
            ntxFile.SetWidth(w);
            ntxFile.SetHeight(h);
            ntxFile.SetDepth(1);
            ntxFile.SetMipLevel(i);
            ntxFile.SetType(nNtxFile::TYPE_TEXTURE2D);
            ntxFile.SetFormat(ntxFormat16);
        }
    }

    // open ntx file for writing
    if (!ntxFile.OpenWrite(oname))
    {
        ntxFile.FreeBlocks();
        return false;
    }

    ILint mipImage = iluGenImage();
    ilBindImage(mipImage);
    ilClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    for (i = 0; i < numMipMaps; i++)
    {
        int numPixels;

        ntxFile.SetCurrentBlock(i);
        w = ntxFile.GetWidth();
        h = ntxFile.GetHeight();
        numPixels = w * h;

        // copy the image to the current mipmap
        ilCopyImage(srcImage);
        iluScale(w, h, 1);

        // if detailmapping enabled, overwrite mipmaps with white
/*
        if (detailmap && (i > (numMipMaps - 3)))
        {
            ilClearImage();
        }
*/

        ILubyte* data32 = ilGetData(); 
        ILint bpp32  = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);

        // write 32 bit data block
        if (write32)
        {
            int data32Size = numPixels * bpp32;
            ntxFile.WriteBlock(data32, data32Size);
        }

        // write 16 bit data block, DevIL doesn't support conversion
        // to 16 bit formats, thus we do this on our own...
        if (write16)
        {
            ntxFile.SetCurrentBlock(i + offset);
            w = ntxFile.GetWidth();
            h = ntxFile.GetHeight();
            numPixels = w * h;

            ushort* data16 = new ushort[numPixels];
            int data16Size = numPixels * sizeof(ushort);

            if (ntxFormat16 == nNtxFile::FORMAT_A4R4G4B4)
            {
                // convert to A4R4G4B4
                gen4444Image(w, h, data32, data16);
            }
            else
            {
                // convert to R5G6B5
                gen565Image(w, h, data32, data16);
            }
            ntxFile.WriteBlock(data16, data16Size);
        }
    }

    // close ntxFile
    ntxFile.CloseWrite();
    ntxFile.FreeBlocks();
    return true;
}

//------------------------------------------------------------------------------
/**
    Convert a vector3 to rgb and write to cube map face
*/
void
writeCubePixel(uchar* data, int width, int x, int y, float vx, float vy, float vz)
{
    int r = int(((vx * 0.5f) + 0.5f) * 255.0f);
    int g = int(((vy * 0.5f) + 0.5f) * 255.0f);
    int b = int(((vz * 0.5f) + 0.5f) * 255.0f);

    uchar* ptr = data + (((width * y) + x) * 3);
    ptr[2] = r;
    ptr[1] = g;
    ptr[0] = b;
}

//------------------------------------------------------------------------------
/**
    Create and save a normalization cube map.
*/
bool
createAndSaveCubeMap(nKernelServer* ks, const char* oname, int size)
{
    if (size <= 0)
    {
        printf("Invalid size %d for normalization cube map!\n", size);
        return false;
    }

    const int numImages = 6;

    // create 6 IL images
    ILuint images[numImages];
    ilEnable(IL_FORMAT_SET);
    ilEnable(IL_TYPE_SET);
    ilSetInteger(IL_FORMAT_MODE, IL_RGB);
    ilSetInteger(IL_TYPE_MODE, IL_UNSIGNED_BYTE);
    ilGenImages(numImages, &images[0]);
    
    // scale to required size
    int i;
    for (i = 0; i < numImages; i++)
    {
        ilBindImage(images[i]);
        iluImageParameter(ILU_FILTER, ILU_NEAREST);
        ilDefaultImage();
        iluScale(size, size, 1);
    }

    ilBindImage(images[0]);
    uchar* posX = ilGetData();
    ilBindImage(images[1]);
    uchar* negX = ilGetData();
    ilBindImage(images[2]);
    uchar* posY = ilGetData();
    ilBindImage(images[3]);
    uchar* negY = ilGetData();
    ilBindImage(images[4]);
    uchar* posZ = ilGetData();
    ilBindImage(images[5]);
    uchar* negZ = ilGetData();

    int ix, iy;
    vector3 vec;
    float d = 2.0f / float(size);

    for (iy = 0; iy < size; iy++)
    {
        for (ix = 0; ix < size; ix++)
        {
            float y = 1.0f - float(iy) * d;
            float z = 1.0f - float(ix) * d;
            
            vec.set(1.0f, y, z);
            vec.norm();

            writeCubePixel(posX, size, ix, iy, +vec.x, +vec.y, +vec.z);
            writeCubePixel(negX, size, ix, iy, -vec.x, +vec.y, -vec.z);

            writeCubePixel(posY, size, ix, iy, -vec.z, +vec.x, -vec.y);
            writeCubePixel(negY, size, ix, iy, -vec.z, -vec.x, +vec.y);

            writeCubePixel(posZ, size, ix, iy, -vec.z, +vec.y, +vec.x);
            writeCubePixel(negZ, size, ix, iy, +vec.z, +vec.y, -vec.x);
        }
    }

    // write the ntx file
    nNtxFile ntxFile(ks);
    ntxFile.SetNumBlocks(1);
    ntxFile.SetCurrentBlock(0);
    ntxFile.SetWidth(size);
    ntxFile.SetHeight(size);
    ntxFile.SetDepth(1);
    ntxFile.SetMipLevel(0);
    ntxFile.SetType(nNtxFile::TYPE_TEXTURECUBE);
    ntxFile.SetFormat(nNtxFile::FORMAT_R8G8B8);
    
    // open ntx file for writing
    if (!ntxFile.OpenWrite(oname))
    {
        ntxFile.FreeBlocks();
        return false;
    }

    // copy pixel data into continous block and write data block
    int imageSize = size * size * 3;
    int dataSize = 6 * imageSize;
    uchar* data = (uchar*) n_malloc(dataSize);
    memcpy(data + 0 * imageSize, posX, imageSize);
    memcpy(data + 1 * imageSize, negX, imageSize);
    memcpy(data + 2 * imageSize, posY, imageSize);
    memcpy(data + 3 * imageSize, negY, imageSize);
    memcpy(data + 4 * imageSize, posZ, imageSize);
    memcpy(data + 5 * imageSize, negZ, imageSize);

    // also save the cube maps as tga
    for (i = 0; i < 6; i++)
    {
        char tgaName[N_MAXPATH];
        sprintf(tgaName, "cube%d.tga", i);
        saveDestImageTGA(ks, images[i], tgaName);
    }

    ntxFile.WriteBlock(data, dataSize);
    ntxFile.CloseWrite();
    ntxFile.FreeBlocks();

    ilDeleteImages(numImages, &images[0]);

    return true;
}

//------------------------------------------------------------------------------
/**
    Creates a specular lookup table which resolves the formula

    c = x ^ y

    with (0 <= x < 1)
    and  (0 <= y < height)
*/
bool
createAndSaveSpecularMap(nKernelServer* ks, const char* oname, int width, int height)
{
    if ((width <= 0) || (height <= 0))
    {
        printf("Invalid size (%d, %d) for specular map!\n", width, height);
        return false;
    }

    // create IL image
    ILuint image;
    ilEnable(IL_FORMAT_SET);
    ilEnable(IL_TYPE_SET);
    ilSetInteger(IL_FORMAT_MODE, IL_RGB);
    ilSetInteger(IL_TYPE_MODE, IL_UNSIGNED_BYTE);
    ilGenImages(1, &image);
    
    ilBindImage(image);
    iluImageParameter(ILU_FILTER, ILU_NEAREST);
    ilDefaultImage();
    iluScale(width, height, 1);

    uchar* data = ilGetData();

    int x, y;
    float d = 1.0f / float(width);
    for (y = 0; y < height; y++)
    {
        double fx = 0.0;
        for (x = 0; x < width; x++, fx += d)
        {
            uchar* ptr = data + (((width * y) + x) * 3);
            double val = pow(fx, double(y));
            int c = int(val * 255.0);
            ptr[2] = c;
            ptr[1] = c;
            ptr[0] = c;
        }
    }

    // write ntx file
    nNtxFile ntxFile(ks);
    ntxFile.SetNumBlocks(1);
    ntxFile.SetCurrentBlock(0);
    ntxFile.SetWidth(width);
    ntxFile.SetHeight(height);
    ntxFile.SetDepth(1);
    ntxFile.SetMipLevel(0);
    ntxFile.SetType(nNtxFile::TYPE_TEXTURE2D);
    ntxFile.SetFormat(nNtxFile::FORMAT_R8G8B8);
    
    // open ntx file for writing
    if (!ntxFile.OpenWrite(oname))
    {
        ntxFile.FreeBlocks();
        return false;
    }

    int dataSize = width * height * 3;
    ntxFile.WriteBlock(data, dataSize);
    ntxFile.CloseWrite();
    ntxFile.FreeBlocks();

    // for debugging: also save as tga file
    saveDestImageTGA(ks, image, "specularmap.tga");

    ilDeleteImages(1, &image);

    return true;
}

//------------------------------------------------------------------------------
int 
main(int argc, char* argv[])
{
    bool help;
    const char* pname;
    const char* aname;
    bool detailmap;
    const char* oname;
    const char* filter;
    int width;
    int height;
    bool smallerOnly;
    bool tga;
    bool flip;
    bool bpp32;
    bool nosharpen;
    bool normalmap;
    bool normcubemap;
    bool specularmap;
    int retval = 0;

    // get args
    help        = wf_getboolarg(argc, argv, "-help");
    pname       = wf_getstrarg(argc, argv, "-pname", "texture.bmp");
    aname       = wf_getstrarg(argc, argv, "-aname", 0);
    oname       = wf_getstrarg(argc, argv, "-oname", "texture.ntx");
    detailmap   = wf_getboolarg(argc, argv, "-detailmap");
    filter      = wf_getstrarg(argc, argv, "-filter", "bilinear");
    width       = wf_getintarg(argc, argv, "-w", 0);
    height      = wf_getintarg(argc, argv, "-h", 0);
    smallerOnly = wf_getboolarg(argc, argv, "-smalleronly");
    tga         = wf_getboolarg(argc, argv, "-tga");
    flip        = wf_getboolarg(argc, argv, "-flip");
    bpp32       = wf_getboolarg(argc, argv, "-bpp32");
    nosharpen   = wf_getboolarg(argc, argv, "-nosharpen");
    normalmap   = wf_getboolarg(argc, argv, "-normalmap");
    normcubemap = wf_getboolarg(argc, argv, "-normcubemap");
    specularmap = wf_getboolarg(argc, argv, "-specularmap");
    assert(pname);

    // startup IL and ILU
    ilInit();
    iluInit();
    ilEnable(IL_CONV_PAL);
    iluImageParameter(ILU_FILTER, (filter[0] == 'b') ? ILU_BILINEAR : ILU_SCALE_MITCHELL);
    ILuint srcImage = iluGenImage();

    // show help?
    if (help)
    {
        printf("nbmp2ntx [-help] [-pname] [-aname] [-oname] [-mipmap] [-detailmap] [-w x] [-h y]\n"
                "(C) 2001 A.Weissflog\n"
                "-help:         show help\n"
                "-pname:        input bmp file with color information (def texture.bmp)\n"
                "-aname:        optional input bmp file with alpha information (def no alpha)\n"
                "-oname:        output filename (def texture.ntx)\n"
                "-mipmap:       generate mipmaps\n"
                "-detailmap:    generate special detailmap mipmaps\n"
                "-w:            target width\n"
                "-h:            target height\n"
                "-smalleronly   only scale image if target size smaller original size\n"
                "-tga           save result as tga file\n"
                "-flip          flip image vertically\n"
                "-bpp32         save destination as 32 bit data\n"
                "-nosharpen     do not sharpen the destination image\n"
                "-normalmap     the file is a normalmap (== -bpp32 -nosharpen)\n"
                "-normcubemap   generate a normalization cube map from scratch\n"
                "-specularmap   generate a specular lookup table\n");
        return 0;
    }

    // create a Nebula kernel server and nFileServer2
    nKernelServer* ks = new nKernelServer;
    nFileServer2* fs2 = (nFileServer2*) ks->New("nfileserver2", "/sys/servers/file2");
	n_assert(fs2);

    if (specularmap)
    {
        // generate a specular lookup map
        if (!createAndSaveSpecularMap(ks, oname, width, height))
        {
            printf("Failed to generate or save specular lookup map!\n");
            retval = 5;
            goto ende;
        }
    }
    else if (normcubemap)
    {
        // generate a normalization cube map
        if (!createAndSaveCubeMap(ks, oname, width))
        {
            printf("Failed to generate or save normalization cube map!\n");
            retval = 5;
            goto ende;
        }
    }
    else
    {
        // load source image
        if (normalmap)
        {
            nosharpen = true;
            bpp32 = true;
        }
        if (!loadSourceImage(pname, aname, srcImage, width, height, smallerOnly, flip))
        {
            printf("Failed to load source image!\n");
            retval = 5;
            goto ende;
        }

        if (tga)
        {
            // save as tga file
            if (!saveDestImageTGA(ks, srcImage, oname))
            {
                printf("Failed to save dest image!\n");
                retval = 5;
                goto ende;
            }
        }
        else
        {
            // generate and save NTX File
            if (!saveDestImageNTX(ks, srcImage, oname, detailmap, nosharpen, bpp32))
            {
                printf("Failed to save dest image!\n");
                retval = 5;
                goto ende;
            }
        }
    }

ende:
    // done
    delete ks;
    iluDeleteImage(srcImage);
    return retval;
}
//------------------------------------------------------------------------------
