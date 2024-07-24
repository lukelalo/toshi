#define N_IMPLEMENTS nMathServer
//-------------------------------------------------------------------
//  nmath_matrix.cc
//  (C) 1999 A.Weissflog
//-------------------------------------------------------------------
#include <stdlib.h>
#include <string.h>
#include "misc/nmathserver.h"

//-------------------------------------------------------------------
//  MatrixIdentity()
//  21-Jun-99   floh    created
//-------------------------------------------------------------------
void nMathServer::MatrixIdentity(void)
{
    this->m.ident();
}

//-------------------------------------------------------------------
//  MatrixSet()
//  21-Jun-99   floh    created
//-------------------------------------------------------------------
void nMathServer::MatrixSet(matrix44& m0)
{
    this->m = m0;
}

//-------------------------------------------------------------------
//  MatrixGet()
//  21-Jun-99   floh    created
//-------------------------------------------------------------------
void nMathServer::MatrixGet(matrix44& m0)
{
    m0 = this->m;
}

//-------------------------------------------------------------------
//  MatrixMult()
//  Genereller 4x4-Matrix-Multiplizierer,
//  Benoetigt 64 Multiplikationen. 
//  21-Jun-99   floh    created
//-------------------------------------------------------------------
void nMathServer::MatrixMult(matrix44& _m)
{
    this->m *= _m;
}

//-------------------------------------------------------------------
//  MatrixMultSimple()
//  Multipliziert unter der Annahme, dass beide Matrizen folgende
//  Bedingung erfuellen:
//  m(0,3) = m(1,3) = m(2,3) = 0.0, m(3,3) = 1.0
//  Benoetigt 36 Multiplikationen.
//  21-Jun-99   floh    created
//-------------------------------------------------------------------
void nMathServer::MatrixMultSimple(matrix44& _m)
{
    this->m.mult_simple(_m);
}

//-------------------------------------------------------------------
//  MatrixTranslate()
//  Geht von einer normalen Model-Matrix mit
//  m(0,3) = m(1,3) = m(2,3) = 0.0, m(3,3) = 1.0
//  aus!
//  21-Jun-99   floh    created
//-------------------------------------------------------------------
void nMathServer::MatrixTranslate(float x, float y, float z)
{
    vector3 v(x,y,z);
    this->m.translate(v);
}

//-------------------------------------------------------------------
//  MatrixRotateX()
//  16 Multiplikationen.
//  21-Jun-99   floh    created
//-------------------------------------------------------------------
void nMathServer::MatrixRotateX(float a)
{
    this->m.rotate_x(a);
}    

//-------------------------------------------------------------------
//  MatrixRotateY()
//  16 Multiplikationen
//  21-Jun-99   floh    created
//-------------------------------------------------------------------
void nMathServer::MatrixRotateY(float a)
{
    this->m.rotate_y(a);
}

//-------------------------------------------------------------------
//  MatrixRotateZ()
//  16 Multiplikationen
//  21-Jun-99   floh    created
//-------------------------------------------------------------------
void nMathServer::MatrixRotateZ(float a)
{
    this->m.rotate_z(a);
}

//-------------------------------------------------------------------
//  MatrixScale()
//  12 Multiplikationen
//  21-Jun-99   floh    created
//-------------------------------------------------------------------
void nMathServer::MatrixScale(float x, float y, float z)
{
    vector3 s(x,y,z);
    this->m.scale(s);
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------

