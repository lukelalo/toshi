#define N_IMPLEMENTS nMathServer
//-------------------------------------------------------------------
//  nmath_varray.cc
//  (C) 1999 A.Weissflog
//-------------------------------------------------------------------
#include <stdlib.h>
#include <math.h>
#include "misc/nmathserver.h"

#define X (0)
#define Y (1)
#define Z (2)
#define W (3)

//-------------------------------------------------------------------
//  VecSetCommon()
//  Definiere gemeinsame Parameter der Vector-Arrays
//-------------------------------------------------------------------
void nMathServer::VecSetCommon(int num, int dim)
{
    this->vnum = num;
    this->vdim = dim;
}

//-------------------------------------------------------------------
//  VecSet()
//  Definiert eines der Vector-Arrays.
//  21-Jun-99   floh    created
//-------------------------------------------------------------------
void nMathServer::VecSet(int r, float *ptr, int stride)
{
    n_assert((r >= 0) && (r < N_VREG_NUM));
    n_assert(ptr);
    // stride muss durch 4 teilbar sein (==sizeof(float))
    n_assert((stride & 3) == 0);

    this->va[r].ptr    = ptr;
    this->va[r].stride = (stride>>2);
}

//-------------------------------------------------------------------
//  v_transform_3d()
//  Transformiert 3-dimensionale Vektoren von v_from nach v_to.
//  Erlaubt: (v_from == v_to)
//  21-Jun-99   floh    created
//-------------------------------------------------------------------
static void v_transform_3d(matrix44& m,          // 4x4 Matrix
                           float *v0,            // Source-Daten
                           float *v1,            // Ergebnis
                           int num,              // Anzahl Vektoren
                           int stride0,          // Stride in Quads
                           int stride1)
{
    int i;
    for (i=0; i<num; i++) {
        float x = v0[X];
        float y = v0[Y];
        float z = v0[Z];
        v1[X] = m.M11*x + m.M21*y + m.M31*z + m.M41;
        v1[Y] = m.M12*x + m.M22*y + m.M32*z + m.M42;
        v1[Z] = m.M13*x + m.M23*y + m.M33*z + m.M43;
        v0 += stride0;
        v1 += stride1;
    }
}

//-------------------------------------------------------------------
//  v_transform_4d()
//  Transformiert 4-dimensionale Vektoren von v_from nach v_to.
//  Erlaubt: (v_from == v_to)
//  21-Jun-99   floh    created
//-------------------------------------------------------------------
/*
static void v_transform_4d(float *m,         // 4x4 Matrix
                           float *v0,        // Source-Daten
                           float *v1,        // Ergebnis
                           int num,          // Anzahl Vektoren
                           int stride0,      // Stride in Quads
                           int stride1)
{
    int i;
    for (i=0; i<num; i++) {
        float x = v0[X];
        float y = v0[Y];
        float z = v0[Z];
        float w = v0[W];
        v1[X] = m[M11]*x + m[M21]*y + m[M31]*z + m[M41]*w;
        v1[Y] = m[M12]*x + m[M22]*y + m[M32]*z + m[M42]*w;
        v1[Z] = m[M13]*x + m[M23]*y + m[M33]*z + m[M43]*w;
        v1[W] = m[M14]*x + m[M24]*y + m[M34]*z + m[M44]*w;
        v0 += stride0;
        v1 += stride1;
    }
}
*/

//-------------------------------------------------------------------
//  v_scale_xd()
//  Skaliert 1,2,3,4-dimensionales Vektor-Array.
//  Erlaubt: (v0 == v1)
//  21-Jun-99   floh    created
//-------------------------------------------------------------------
static void v_scale_1d(float s,          // Scale
                       float *v0,        // Source
                       float *v1,        // Target
                       int num,          // Anzahl
                       int stride0,      // Source-Stride (Quads!)
                       int stride1)      // Target-Stride (Quads!)
{
    int i;
    for (i=0; i<num; i++) {
        v1[0]=v0[0]*s;
        v0 += stride0;
        v1 += stride1;
    }
}

static void v_scale_2d(float s, float *v0, float *v1, int num, int stride0, int stride1)
{
    int i;
    for (i=0; i<num; i++) {
        v1[0]=v0[0]*s; v1[1]=v0[1]*s;
        v0 += stride0;
        v1 += stride1;
    }
}

static void v_scale_3d(float s, float *v0, float *v1, int num, int stride0, int stride1)
{
    int i;
    for (i=0; i<num; i++) {
        v1[0]=v0[0]*s; v1[1]=v0[1]*s; v1[2]=v0[2]*s;
        v0 += stride0;
        v1 += stride1;
    }
}

static void v_scale_4d(float s, float *v0, float *v1, int num, int stride0, int stride1)
{
    int i;
    for (i=0; i<num; i++) {
        v1[0]=v0[0]*s; v1[1]=v0[1]*s; v1[2]=v0[2]*s; v1[3]=v0[3]*s;
        v0 += stride0;
        v1 += stride1;
    }
}

//-------------------------------------------------------------------
//  v_scaledadd_xd()
//  Skalier-Addiert 1,2,3,4-dimensionales Vektor-Array.
//  Erlaubt: (v0 == v1)
//  21-Jun-99   floh    created
//-------------------------------------------------------------------
static void v_scaleadd_1d(float s,          // Scale
                          float *v0,        // Source
                          float *v1,        // Target
                          int num,          // Anzahl
                          int stride0,      // Source-Stride (Quads!)
                          int stride1)      // Target-Stride (Quads!)
{
    int i;
    for (i=0; i<num; i++) {
        v1[0]+=v0[0]*s;
        v0 += stride0;
        v1 += stride1;
    }
}

static void v_scaleadd_2d(float s, float *v0, float *v1, int num, int stride0, int stride1)
{
    int i;
    for (i=0; i<num; i++) {
        v1[0]+=v0[0]*s; v1[1]+=v0[1]*s;
        v0 += stride0;
        v1 += stride1;
    }
}

static void v_scaleadd_3d(float s, float *v0, float *v1, int num, int stride0, int stride1)
{
    int i;
    for (i=0; i<num; i++) {
        v1[0]+=v0[0]*s; v1[1]+=v0[1]*s; v1[2]+=v0[2]*s;
        v0 += stride0;
        v1 += stride1;
    }
}

static void v_scaleadd_4d(float s, float *v0, float *v1, int num, int stride0, int stride1)
{
    int i;
    for (i=0; i<num; i++) {
        v1[0]+=v0[0]*s; v1[1]+=v0[1]*s; v1[2]+=v0[2]*s; v1[3]+=v0[3]*s;
        v0 += stride0;
        v1 += stride1;
    }
}

//-------------------------------------------------------------------
//  v_norm_3d()
//  Normiert 3-dimensionale Vektor-Arrays.
//  FIXME:
//  Die Wurzel sollte mal durch eine Naeherung ersetzt werden.
//  21-Jun-99   floh    created
//-------------------------------------------------------------------
static void v_norm_3d(float *v0,        // Source
                      float *v1,        // Target
                      int num,          // Anzahl
                      int stride0,      // Source-Stride (Quads!)
                      int stride1)      // Target-Stride (Quads!)
{
    int i;
    for (i=0; i<num; i++) {
        float x=v0[0], y=v0[1], z=v0[2];
        float l = (float) n_sqrt(x*x + y*y + z*z);
        if (l > 0.0f) {
            v1[0]=x/l; v1[1]=y/l; v1[2]=z/l;
        } else {
            v1[0]=0.0f; v1[1]=0.0f; v1[2]=1.0f;
        }
        v0 += stride0;
        v1 += stride1;
    }
}

//-------------------------------------------------------------------
//  v_ipolconst_Xd()
//  Interpoliert zwischen 2 1,2,3,4-dimensionalen Array.
//  21-Jun-99   floh    created
//-------------------------------------------------------------------
static void v_ipolconst_1d(float l,     // Lerp-Value
                           float *v0,   // Source0
                           float *v1,   // Source1
                           float *v2,   // Target
                           int num,     // Anzahl
                           int stride0, // Source0-Stride (Quads!)
                           int stride1, // Source1-Stride (Quads!)
                           int stride2) // Target-Stride (Quads!)
{
    int i;
    for (i=0; i<num; i++) {
        v2[0] = v0[0] + ((v1[0]-v0[0])*l);
        v0 += stride0;
        v1 += stride1;
        v2 += stride2;
    }
}

static void v_ipolconst_2d(float l,     // Lerp-Value
                           float *v0,   // Source0
                           float *v1,   // Source1
                           float *v2,   // Target
                           int num,     // Anzahl
                           int stride0, // Source0-Stride (Quads!)
                           int stride1, // Source1-Stride (Quads!)
                           int stride2) // Target-Stride (Quads!)
{
    int i;
    for (i=0; i<num; i++) {
        v2[0] = v0[0] + ((v1[0]-v0[0])*l);
        v2[1] = v0[1] + ((v1[1]-v0[1])*l);
        v0 += stride0;
        v1 += stride1;
        v2 += stride2;
    }
}

static void v_ipolconst_3d(float l,     // Lerp-Value
                           float *v0,   // Source0
                           float *v1,   // Source1
                           float *v2,   // Target
                           int num,     // Anzahl
                           int stride0, // Source0-Stride (Quads!)
                           int stride1, // Source1-Stride (Quads!)
                           int stride2) // Target-Stride (Quads!)
{
    int i;
    for (i=0; i<num; i++) {
        v2[0] = v0[0] + ((v1[0]-v0[0])*l);
        v2[1] = v0[1] + ((v1[1]-v0[1])*l);
        v2[2] = v0[2] + ((v1[2]-v0[2])*l);
        v0 += stride0;
        v1 += stride1;
        v2 += stride2;
    }
}

static void v_ipolconst_4d(float l,     // Lerp-Value
                           float *v0,   // Source0
                           float *v1,   // Source1
                           float *v2,   // Target
                           int num,     // Anzahl
                           int stride0, // Source0-Stride (Quads!)
                           int stride1, // Source1-Stride (Quads!)
                           int stride2) // Target-Stride (Quads!)
{
    int i;
    for (i=0; i<num; i++) {
        v2[0] = v0[0] + ((v1[0]-v0[0])*l);
        v2[1] = v0[1] + ((v1[1]-v0[1])*l);
        v2[2] = v0[2] + ((v1[2]-v0[2])*l);
        v2[3] = v0[3] + ((v1[3]-v0[3])*l);
        v0 += stride0;
        v1 += stride1;
        v2 += stride2;
    }
}

//-------------------------------------------------------------------
//  v_ipol_Xd()
//  Interpoliert zwischen 2 1,2,3,4-dimensionalen Arrays
//  mit einem 1-dimensionalen Array mit den Wichtungen.
//  22-Jun-99   floh    created
//-------------------------------------------------------------------
static void v_ipol_1d(float *v0,   // Source0
                      float *v1,   // Source1
                      float *v2,   // Wichtung
                      float *v3,   // Target
                      int num,     // Anzahl
                      int stride0, // Source0-Stride (Quads!)
                      int stride1, // Source1-Stride (Quads!)
                      int stride2, // Wichtungs-Stride (Quads!)
                      int stride3) // Target-Stride (Quads!)
{
    int i;
    for (i=0; i<num; i++) {
        v3[0] = v0[0] + ((v1[0]+v0[0])*v2[0]);
        v0 += stride0;
        v1 += stride1;
        v2 += stride2;
        v3 += stride3;
    }
}

static void v_ipol_2d(float *v0,   // Source0
                      float *v1,   // Source1
                      float *v2,   // Wichtung
                      float *v3,   // Target
                      int num,     // Anzahl
                      int stride0, // Source0-Stride (Quads!)
                      int stride1, // Source1-Stride (Quads!)
                      int stride2, // Wichtungs-Stride (Quads!)
                      int stride3) // Target-Stride (Quads!)
{
    int i;
    for (i=0; i<num; i++) {
        v3[0] = v0[0] + ((v1[0]+v0[0])*v2[0]);
        v3[1] = v0[1] + ((v1[1]+v0[1])*v2[1]);
        v0 += stride0;
        v1 += stride1;
        v2 += stride2;
        v3 += stride3;
    }
}

static void v_ipol_3d(float *v0,   // Source0
                      float *v1,   // Source1
                      float *v2,   // Wichtung
                      float *v3,   // Target
                      int num,     // Anzahl
                      int stride0, // Source0-Stride (Quads!)
                      int stride1, // Source1-Stride (Quads!)
                      int stride2, // Wichtungs-Stride (Quads!)
                      int stride3) // Target-Stride (Quads!)
{
    int i;
    for (i=0; i<num; i++) {
        v3[0] = v0[0] + ((v1[0]+v0[0])*v2[0]);
        v3[1] = v0[1] + ((v1[1]+v0[1])*v2[1]);
        v3[2] = v0[2] + ((v1[2]+v0[2])*v2[2]);
        v0 += stride0;
        v1 += stride1;
        v2 += stride2;
        v3 += stride3;
    }
}

static void v_ipol_4d(float *v0,   // Source0
                      float *v1,   // Source1
                      float *v2,   // Wichtung
                      float *v3,   // Target
                      int num,     // Anzahl
                      int stride0, // Source0-Stride (Quads!)
                      int stride1, // Source1-Stride (Quads!)
                      int stride2, // Wichtungs-Stride (Quads!)
                      int stride3) // Target-Stride (Quads!)
{
    int i;
    for (i=0; i<num; i++) {
        v3[0] = v0[0] + ((v1[0]+v0[0])*v2[0]);
        v3[1] = v0[1] + ((v1[1]+v0[1])*v2[1]);
        v3[2] = v0[2] + ((v1[2]+v0[2])*v2[2]);
        v3[3] = v0[3] + ((v1[3]+v0[3])*v2[3]);
        v0 += stride0;
        v1 += stride1;
        v2 += stride2;
        v3 += stride3;
    }
}

//-------------------------------------------------------------------
//  VecTransform()
//  r1 = m * r0
//  21-Jun-99   floh    created
//-------------------------------------------------------------------
void nMathServer::VecTransform(int r1, int r0)
{
    n_assert((r0 >= 0) && (r0 < N_VREG_NUM));
    n_assert((r1 >= 0) && (r1 < N_VREG_NUM));

    float *p0 = this->va[r0].ptr;
    float *p1 = this->va[r1].ptr;
    int s0 = this->va[r0].stride;
    int s1 = this->va[r1].stride;

    switch(this->vdim) {
        case 3:
            // 3-dimensionaler Fall
            v_transform_3d(this->m,p0,p1,this->vnum,s0,s1);
            break;
        default:
            n_error("operation not supported\n");
            break;
    }
}

//-------------------------------------------------------------------
//  VecScale()
//  r1 = r0 * s
//  21-Jun-99   floh    created
//-------------------------------------------------------------------
void nMathServer::VecScale(int r1, int r0, float s)
{
    n_assert((r0 >= 0) && (r0 < N_VREG_NUM));
    n_assert((r1 >= 0) && (r1 < N_VREG_NUM));

    float *p0 = this->va[r0].ptr;
    float *p1 = this->va[r1].ptr;
    int s0 = this->va[r0].stride;
    int s1 = this->va[r1].stride;

    switch (this->vdim) {
        case 1: v_scale_1d(s,p0,p1,this->vnum,s0,s1); break;
        case 2: v_scale_2d(s,p0,p1,this->vnum,s0,s1); break;
        case 3: v_scale_3d(s,p0,p1,this->vnum,s0,s1); break;
        case 4: v_scale_4d(s,p0,p1,this->vnum,s0,s1); break;
    }
}

//-------------------------------------------------------------------
//  VecScaleAdd()
//  r1 += r0 * s
//  21-Jun-99   floh    created
//-------------------------------------------------------------------
void nMathServer::VecScaleAdd(int r1, int r0, float s)
{
    n_assert((r0 >= 0) && (r0 < N_VREG_NUM));
    n_assert((r1 >= 0) && (r1 < N_VREG_NUM));

    float *p0 = this->va[r0].ptr;
    float *p1 = this->va[r1].ptr;
    int s0 = this->va[r0].stride;
    int s1 = this->va[r1].stride;

    switch (this->vdim) {
        case 1: v_scaleadd_1d(s,p0,p1,this->vnum,s0,s1); break;
        case 2: v_scaleadd_2d(s,p0,p1,this->vnum,s0,s1); break;
        case 3: v_scaleadd_3d(s,p0,p1,this->vnum,s0,s1); break;
        case 4: v_scaleadd_4d(s,p0,p1,this->vnum,s0,s1); break;
    }
}

//-------------------------------------------------------------------
//  VecNorm()
//  r1 = norm(r0)
//  21-Jun-99   floh    created
//-------------------------------------------------------------------
void nMathServer::VecNorm(int r1, int r0)
{
    n_assert((r0 >= 0) && (r0 < N_VREG_NUM));
    n_assert((r1 >= 0) && (r1 < N_VREG_NUM));

    float *p0 = this->va[r0].ptr;
    float *p1 = this->va[r1].ptr;
    int s0 = this->va[r0].stride;
    int s1 = this->va[r1].stride;

    switch(this->vdim) {
        case 3: v_norm_3d(p0,p1,this->vnum,s0,s1); break;
        default:
            n_error("operation not supported\n");
            break;
    }
}

//-------------------------------------------------------------------
//  VecIpolConst()
//  r2[i] = r0[i] + ((r1[i]-r0[i])*s)
//  23-Jun-99   floh    created
//-------------------------------------------------------------------
void nMathServer::VecIpolConst(int r2, int r1, int r0, float s)
{
    n_assert((r0 >= 0) && (r0 < N_VREG_NUM));
    n_assert((r1 >= 0) && (r1 < N_VREG_NUM));
    n_assert((r2 >= 0) && (r2 < N_VREG_NUM));

    float *p0 = this->va[r0].ptr;
    float *p1 = this->va[r1].ptr;
    float *p2 = this->va[r2].ptr;
    int s0 = this->va[r0].stride;
    int s1 = this->va[r1].stride;
    int s2 = this->va[r2].stride;
    switch (this->vdim) {
        case 1: v_ipolconst_1d(s,p0,p1,p2,this->vnum,s0,s1,s2); break;
        case 2: v_ipolconst_2d(s,p0,p1,p2,this->vnum,s0,s1,s2); break;
        case 3: v_ipolconst_3d(s,p0,p1,p2,this->vnum,s0,s1,s2); break;
        case 4: v_ipolconst_4d(s,p0,p1,p2,this->vnum,s0,s1,s2); break;
    }
}

//-------------------------------------------------------------------
//  VecIpol()
//  r3[i] = r0[i] + ((r1[i]-r0[i])*r2[i])
//  In r2 wird nur die 1.Komponente ausgewertet.
//  23-Jun-99   floh    created
//-------------------------------------------------------------------
void nMathServer::VecIpol(int r3, int r2, int r1, int r0)
{
    n_assert((r0 >= 0) && (r0 < N_VREG_NUM));
    n_assert((r1 >= 0) && (r1 < N_VREG_NUM));
    n_assert((r2 >= 0) && (r2 < N_VREG_NUM));
    n_assert((r3 >= 0) && (r2 < N_VREG_NUM));
    float *p0 = this->va[r0].ptr;
    float *p1 = this->va[r1].ptr;
    float *p2 = this->va[r2].ptr;
    float *p3 = this->va[r3].ptr;
    int s0 = this->va[r0].stride;
    int s1 = this->va[r1].stride;
    int s2 = this->va[r2].stride;
    int s3 = this->va[r2].stride;
    switch (this->vdim) {
        case 1: v_ipol_1d(p0,p1,p2,p3,this->vnum,s0,s1,s2,s3); break;
        case 2: v_ipol_2d(p0,p1,p2,p3,this->vnum,s0,s1,s2,s3); break;
        case 3: v_ipol_3d(p0,p1,p2,p3,this->vnum,s0,s1,s2,s3); break;
        case 4: v_ipol_4d(p0,p1,p2,p3,this->vnum,s0,s1,s2,s3); break;
    }
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------

