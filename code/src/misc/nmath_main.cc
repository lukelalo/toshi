#define N_IMPLEMENTS nMathServer
//-------------------------------------------------------------------
//  nmath_main.cc
//  (C) 1999 A.Weissflog
//-------------------------------------------------------------------
#include "misc/nmathserver.h"
#include "math/nmath.h"

#include <stdlib.h>
#include <time.h>

//-------------------------------------------------------------------
/**
    - 21-Jun-99   floh    created
    - 26-Jan-00   floh    + random number table
*/
//-------------------------------------------------------------------
nMathServer::nMathServer()
{
    int i;
    this->MatrixIdentity();
    memset(this->va,0,sizeof(this->va));
    this->vnum = 0;
    this->vdim = 0;
    this->rand_array = (float *) n_malloc(N_RAND_NUM*sizeof(float));
    srand((unsigned int)time(NULL));
    for (i=0; i<N_RAND_NUM;) {
        // get a random number between -1.0f and 1.0f
        float f0 = (2.0f*((float)rand())/((float)RAND_MAX))-1.0f;
        float f1 = (2.0f*((float)rand())/((float)RAND_MAX))-1.0f;
        float f2 = (2.0f*((float)rand())/((float)RAND_MAX))-1.0f;
        float f3 = (2.0f*((float)rand())/((float)RAND_MAX))-1.0f;

        // treat f0,f1,f2 as a 3d vector and normalize it
        float l = n_sqrt(f0*f0 + f1*f1 + f2*f2);
        if (l > 0.0f) {
            f0/=l; f1/=l; f2/=l;
        }
        this->rand_array[i++] = f0;
        this->rand_array[i++] = f1;
        this->rand_array[i++] = f2;
        this->rand_array[i++] = f3;
    }
}

//-------------------------------------------------------------------
/**
    - 21-Jun-99   floh    created
    - 26-Jan-00   floh    + random number table
*/
//-------------------------------------------------------------------
nMathServer::~nMathServer()
{
    if (this->rand_array) n_free(this->rand_array);
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
