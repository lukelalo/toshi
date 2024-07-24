//------------------------------------------------------------------------------
//  nnanim2nax.cc
//
//  Read ascii nanim file, optimize curves and save as 
//  binary nax file.
//
//  Useage:
//
//  nnanim2nax [-help] [-in nanimfile] [-out naxfile]
//
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "tools/wftools.h"
#include "kernel/nkernelserver.h"
#include "anim/nanimcurvearray.h"

//------------------------------------------------------------------------------
int
main(int argc, char* argv[])
{
    bool help;
    const char* inFileName;
    const char* outFileName;
    float resample;
    int i;

    // get args
    help        = wf_getboolarg(argc, argv, "-help");
    inFileName  = wf_getstrarg(argc, argv, "-in", "");
    outFileName = wf_getstrarg(argc, argv, "-out", "");
    resample    = wf_getfloatarg(argc, argv, "-resample", 0.0f);

    if (0 == strlen(inFileName) && 0 == strlen(outFileName))
        help = true;

    // show help?
    if (help)
    {
        printf("nnanim2nax [-help] [-in] [-out]\n"
               "(C) 2002 RadonLabs Gmbh\n"
               "-help       show help\n"
               "-in         filename of nanim input file\n"
               "-out        filename of nax output file\n");
        return 0;
    }

    // create a Nebula kernel server and nFileServer2
    nKernelServer* ks = new nKernelServer;
    nFileServer2* fs2 = (nFileServer2*) ks->New("nfileserver2", "/sys/servers/file2");
	n_assert(fs2);

    // create a nAnimCurveArray object
    nAnimCurveArray* animCurveArray = (nAnimCurveArray*) ks->New("nanimcurvearray", "/obj");

    // load input file
    if (!animCurveArray->LoadAnim(fs2, inFileName))
    {
        printf("Failed to load input file '%s'!\n", inFileName);
        delete ks;
        return 10;
    }

    // resample???
    if (resample > 0.0f)
    {
        n_printf("  resampling curve to %f keys per second\n", resample);

        nAnimCurveArray* newCurveArray = (nAnimCurveArray*) ks->New("nanimcurvearray", "/obj1");
        n_assert(newCurveArray);

        int numCurves = animCurveArray->GetNumCurves();
        newCurveArray->SetNumCurves(numCurves);
        
        for (i = 0; i < numCurves; i++)
        {
            nAnimCurve& srcCurve = animCurveArray->GetCurve(i);
            nAnimCurve& dstCurve = newCurveArray->GetCurve(i);

            dstCurve.SetName(srcCurve.GetName());
            dstCurve.SetIpolType(srcCurve.GetIpolType());
            dstCurve.SetRepeatType(srcCurve.GetRepeatType());

            int srcNumKeys = srcCurve.GetNumKeys();
            float srcKeysPerSecond = srcCurve.GetKeysPerSecond();

            float duration = srcNumKeys * (1.0f / srcKeysPerSecond);
            float dstKeyTime = 1.0f / resample;
            int dstNumKeys = int(duration / dstKeyTime);
            if (dstNumKeys == 0)
            {
                dstNumKeys = 1;
            }

            dstCurve.BeginKeys(resample, srcCurve.GetStartKey(), dstNumKeys, srcCurve.GetKeyType());
            int j;
            float dstTime = 0.0f;
            vector4 val;
            for (j = 0; j < dstNumKeys; j++)
            {
                srcCurve.Sample(dstTime, val);     
                dstCurve.SetKey(j, val);
                dstTime += dstKeyTime;
            }
            dstCurve.EndKeys();
        }
        animCurveArray->Release();
        animCurveArray = newCurveArray;
    }

    // optimize curves...
    printf("-> Optimizing curves...\n");
    int numCurves = animCurveArray->GetNumCurves();
    for (i = 0; i < numCurves; i++)
    {
        nAnimCurve& curCurve = animCurveArray->GetCurve(i);
        if (curCurve.Optimize())
        {
            printf("  optimized curve '%s'\n", curCurve.GetName());
        }
    }

    // save target file
    if (!animCurveArray->SaveNax(fs2, outFileName))
    {
        printf("Failed to save output file '%s'!\n", outFileName);
        delete ks;
        return 10;
    }

    // clean up and exit
    printf("-> done.\n");
    delete ks;
    return 0;
}
