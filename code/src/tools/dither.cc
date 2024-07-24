//------------------------------------------------------------------------------
//  dither.cc
//  
//  Simple Floyd-Steinberg style error distribution dithering.
//
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/ntypes.h"

//------------------------------------------------------------------------------
//  Take rgb component error values and add them to the pixel defined by
//  curIndex.
//------------------------------------------------------------------------------
inline
static 
void
addErrorRGB(uchar* srcBuffer, 
            int pixelIndex, 
            int numPixels, 
            uint rError,
            uint gError,
            uint bError)
{
    // would overflow buffer?
    if (pixelIndex < numPixels)
    {
        int charIndex = pixelIndex * 3;
        uint b8 = srcBuffer[charIndex + 0] + bError;
        uint g8 = srcBuffer[charIndex + 1] + gError;
        uint r8 = srcBuffer[charIndex + 2] + rError;
        
        // saturate
        if (b8 > 255) b8 = 255;
        if (g8 > 255) g8 = 255;
        if (r8 > 255) r8 = 255;

        // write back
        srcBuffer[charIndex + 0] = (uchar) b8;
        srcBuffer[charIndex + 1] = (uchar) g8;
        srcBuffer[charIndex + 2] = (uchar) r8;
    }
}

//------------------------------------------------------------------------------
//  Take rgb component error values and add them to the pixel defined by
//  curIndex.
//------------------------------------------------------------------------------
inline
static
void
addErrorRGBA(uchar* srcBuffer, 
             int pixelIndex, 
             int numPixels, 
             uint rError,
             uint gError,
             uint bError,
             uint aError)
{
    // would overflow buffer?
    if (pixelIndex < numPixels)
    {
        int charIndex = pixelIndex * 4;
        uint b8 = srcBuffer[charIndex + 0] + bError;
        uint g8 = srcBuffer[charIndex + 1] + gError;
        uint r8 = srcBuffer[charIndex + 2] + rError;
        uint a8 = srcBuffer[charIndex + 3] + aError;
        
        // saturate
        if (b8 > 255) b8 = 255;        
        if (g8 > 255) g8 = 255;
        if (r8 > 255) r8 = 255;
        if (a8 > 255) a8 = 255;

        // write back
        srcBuffer[charIndex + 0] = (uchar) b8;
        srcBuffer[charIndex + 1] = (uchar) g8;
        srcBuffer[charIndex + 2] = (uchar) r8;
        srcBuffer[charIndex + 3] = (uchar) a8;
    }
}

//------------------------------------------------------------------------------
//  Generate a 565 bit image with kinda-Floyd-Steinberg-Dithering.
//------------------------------------------------------------------------------
void
gen565Image(int w, int h, uchar* srcBuffer, ushort* dstBuffer)
{
    int numPixels = w * h;
    int shortIndex = 0;
    int charIndex  = 0;
    for (; shortIndex < numPixels; shortIndex++, charIndex += 3)
    {
        // get original color components
        uint b8 = srcBuffer[charIndex + 0];
        uint g8 = srcBuffer[charIndex + 1];
        uint r8 = srcBuffer[charIndex + 2];

        // get error
        uint rError = r8 & 7;
        uint gError = g8 & 3;
        uint bError = b8 & 7;

        // get reduced color components
        uint r5 = r8 >> 3;
        uint g6 = g8 >> 2;
        uint b5 = b8 >> 3;
        
        ushort c = (r5 << 11) | (g6 << 5) | (b5);
        dstBuffer[shortIndex] = c;

        // distribute error to neighboring pixels
        addErrorRGB(srcBuffer, shortIndex + 1,     numPixels, rError >> 1, gError >> 1, bError >> 1);
        addErrorRGB(srcBuffer, shortIndex + w,     numPixels, rError >> 1, gError >> 1, bError >> 1);
//        addErrorRGB(srcBuffer, shortIndex + w + 1, numPixels, rError >> 2, gError >> 2, bError >> 2);
    }
}

//------------------------------------------------------------------------------
//  Generate a 4444 bit image with kinda-Floyd-Steinberg-Dithering.
//------------------------------------------------------------------------------
void
gen4444Image(int w, int h, uchar* srcBuffer, ushort* dstBuffer)
{
    int numPixels = w * h;
    int shortIndex = 0;
    int charIndex  = 0;
    for (; shortIndex < numPixels; shortIndex++, charIndex += 4)
    {
        // get original color components
        uint b8 = srcBuffer[charIndex + 0];
        uint g8 = srcBuffer[charIndex + 1];
        uint r8 = srcBuffer[charIndex + 2];
        uint a8 = srcBuffer[charIndex + 3];

        // get error
        uint aError = a8 & 15;
        uint rError = r8 & 15;
        uint gError = g8 & 15;
        uint bError = b8 & 15;

        // get reduced color components
        uint a4 = a8 >> 4;
        uint r4 = r8 >> 4;
        uint g4 = g8 >> 4;
        uint b4 = b8 >> 4;
        
        ushort c = (a4 << 12) | (r4 << 8) | (g4 << 4) | (b4 << 0);
        dstBuffer[shortIndex] = c;

        // distribute error to neighboring pixels
        addErrorRGBA(srcBuffer, shortIndex + 1,     numPixels, rError >> 1, gError >> 1, bError >> 1, aError >> 1);
        addErrorRGBA(srcBuffer, shortIndex + w,     numPixels, rError >> 1, gError >> 1, bError >> 1, aError >> 1);
//        addErrorRGBA(srcBuffer, shortIndex + w + 1, numPixels, rError >> 2, gError >> 2, bError >> 2, aError >> 2);
    }
}

