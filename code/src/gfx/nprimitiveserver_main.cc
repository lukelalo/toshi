#define N_IMPLEMENTS nPrimitiveServer
//------------------------------------------------------------------------------
//  (C) 2003  Leaf Garland & Vadim Macagon
//
//  nPrimitiveServer is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "gfx/nprimitiveserver.h"

nNebulaScriptClass(nPrimitiveServer, "nroot");

#ifndef N_GFXSERVER_H
#include "gfx/ngfxserver.h"
#endif

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

//------------------------------------------------------------------------------
/**
*/
nPrimitiveServer::nPrimitiveServer()
    : ref_GfxServer( kernelServer, this ), 
    psDesc( 0 ),
    dynVB(kernelServer, this),
    VB(0),
    numVerts(0),
    maxVerts(0),
    inBegin(false)
{
    this->ref_GfxServer = "/sys/servers/gfx";
}

//------------------------------------------------------------------------------
/**
*/
nPrimitiveServer::~nPrimitiveServer()
{
    if ( this->ref_ps.isvalid() )
        this->ref_ps->Release();

    if ( this->psDesc )
        n_delete this->psDesc;

    if (this->ref_ibuf.isvalid()) 
    {
        this->ref_ibuf->Release();
        this->ref_ibuf.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
*/
nPixelShader* nPrimitiveServer::GetPixelShader() 
{
    if ( !this->ref_ps.isvalid() )
        this->InitPixelShader();
    return this->ref_ps.get();
}

//------------------------------------------------------------------------------
/**
*/
void nPrimitiveServer::SetPixelShader( nPixelShader* ps )
{
    if ( this->ref_ps.isvalid() ) 
        this->ref_ps->Release();

    if ( ps ) 
    {
        this->ref_ps = ps;
        this->ref_ps->AddRef();
    }
}

//------------------------------------------------------------------------------
/**
*/
void nPrimitiveServer::EnableLighting( bool enable )
{
    this->CheckPixelShader();
    this->ref_ps->GetShaderDesc()->SetLightEnable( enable );
}

//------------------------------------------------------------------------------
/**
*/  
void nPrimitiveServer::SetColor( float r, float g, float b, float a )
{
    ulong c;
    if (N_COLOR_RGBA == this->ref_GfxServer->GetColorFormat()) 
        c = n_f2rgba(r,g,b,a);
    else
        c = n_f2bgra(r,g,b,a);
    this->curColor = c;
}

//------------------------------------------------------------------------------
/**
*/
void nPrimitiveServer::CheckPixelShader()
{
    if ( !this->ref_ps.isvalid() )
        this->InitPixelShader();
    n_assert( this->ref_ps.isvalid() );
}

//------------------------------------------------------------------------------
/**
*/
void nPrimitiveServer::InitPixelShader()
{
    nPixelShader* ps = this->ref_GfxServer->NewPixelShader( NULL );
    if ( ps ) 
    {
        this->ref_ps = ps;
        if ( !this->psDesc )
        {
            this->psDesc = n_new nPixelShaderDesc;
            // Set default renderstate.
            psDesc->SetDiffuse( vector4(1, 1, 1, 1) );
            psDesc->SetLightEnable( false );
            psDesc->SetZWriteEnable( true );
            psDesc->SetZFunc( N_CMP_LESS );
            psDesc->SetCullMode(N_CULL_NONE);
        }
        ps->SetShaderDesc( this->psDesc );
    }
    this->SetColor(1,1,1,1);
}

//------------------------------------------------------------------------------
/**
*/
void nPrimitiveServer::Begin(nPrimType ptype)
{
    n_assert(!this->inBegin);
    
    this->CheckPixelShader();

    // check buffers.
    if (!this->dynVB.IsValid()) {
        this->dynVB.Initialize( N_VT_COORD | N_VT_NORM | N_VT_RGBA | N_VT_UV0, 0);

        n_assert(!this->ref_ibuf.isvalid());

        nIndexBuffer *ibuf = this->ref_GfxServer->FindIndexBuffer("nprimitiveserver_ibuf");
        if (!ibuf) {
            ibuf = this->ref_GfxServer->NewIndexBuffer("nprimitiveserver_ibuf");
            this->maxVerts = (this->dynVB.GetNumVertices() / 3) * 3;
            ibuf->Begin(N_IBTYPE_WRITEONLY, ptype, this->maxVerts);
            int i;
            for (i=0; i< this->maxVerts; i++) {
                ibuf->Index(i, i);
            }
            ibuf->End();
        }

        this->primType = ptype;

        n_assert(ibuf);
        this->ref_ibuf = ibuf;
    }
    else {
        // check for prim type change.
        if (this->primType != ptype) {
            ref_ibuf->SetPrimType(ptype);
            this->primType = ptype;
        }
    }

    this->VB = this->dynVB.Begin(this->ref_ibuf.get(), this->ref_ps.get(), NULL);	

    this->inBegin = true;
}

//------------------------------------------------------------------------------
/**
*/
void nPrimitiveServer::Coord(float x, float y, float z)
{ 
    n_assert(this->inBegin);

    if (this->numVerts >= this->maxVerts) {
        this->VB = this->dynVB.Swap(this->numVerts, this->numVerts);
        this->numVerts = 0;
    }

    n_assert(this->VB);

    this->VB->Coord(this->numVerts, vector3(x, y, z));
    this->VB->Color(this->numVerts, this->curColor);

    this->numVerts++;
}

//------------------------------------------------------------------------------
/**
*/
void nPrimitiveServer::Norm(float x, float y, float z)
{
    n_assert(this->inBegin);  

    this->VB->Norm(this->numVerts, vector3(x, y, z));    
}

//------------------------------------------------------------------------------
/**
*/
void nPrimitiveServer::Rgba(float r, float g, float b, float a)
{
    n_assert(this->inBegin);

    ulong c;
    if (N_COLOR_RGBA == this->ref_GfxServer->GetColorFormat()) 
        c = n_f2rgba(r,g,b,a);
    else
        c = n_f2bgra(r,g,b,a);
    this->VB->Color(this->numVerts, c);
    this->curColor = c;
}

//------------------------------------------------------------------------------
/**
*/
void nPrimitiveServer::Uv(ulong, float u, float v)
{
    n_assert(this->inBegin);  

    this->VB->Uv(this->numVerts, 0, vector2(u, v));
}

//------------------------------------------------------------------------------
/**
*/
void nPrimitiveServer::End(void)
{
    this->dynVB.End(this->numVerts, this->numVerts);    
    this->VB = 0;
    this->numVerts = 0;
    this->inBegin = false;
}


//------------------------------------------------------------------------------
/**
    @brief Render square wireframe grid/plane.
    @param planeExtent Ideal distance from centre of plane to one of the 'edges'.
    @param gridSize Size of each grid square.
*/
void nPrimitiveServer::WirePlane( float planeExtent, float gridSize )
{
    float extent = float((int)(planeExtent / gridSize));
    this->Begin( N_PTYPE_LINE_LIST );
    for ( float p = -extent; p <= +extent; p += gridSize )
    {
        this->Coord(+p, 0.0f, -planeExtent);
        this->Coord(+p, 0.0f, +planeExtent);
        this->Coord(-planeExtent, 0.0f, +p);
        this->Coord(+planeExtent, 0.0f, +p);
    }
    this->End();
}

//------------------------------------------------------------------------------
/**
    @brief Render square plane (really just a square).
    @param planeExtent Distance from centre of plane to one of the 'edges'.
*/
void nPrimitiveServer::SolidPlane( float planeExtent )
{
    this->Begin( N_PTYPE_TRIANGLE_STRIP );
    this->Norm( 0.0f, 1.0f, 0.0f );
    this->Coord( planeExtent, 0.0f, -planeExtent );
    this->Norm( 0.0f, 1.0f, 0.0f );
    this->Coord( -planeExtent, 0.0f, -planeExtent );
    this->Norm( 0.0f, 1.0f, 0.0f );
    this->Coord( planeExtent, 0.0f, planeExtent );
    this->Norm( 0.0f, 1.0f, 0.0f );
    this->Coord( -planeExtent, 0.0f, planeExtent );
    this->End();
}

//------------------------------------------------------------------------------
/**
    @brief Render a wireframe sphere centred at the origin.
    @param radius Sphere radius.
    @param slices Number of slices.
    @param stacks Number of stacks.
*/
void nPrimitiveServer::WireSphere( float sphereRad, int slices, int stacks )
{
    /*
    FIXME: This doesn't actually work properly yet :|, feel free to
    replace with a working version.
    */

    float s, t, sinT, x, y, radius;
    float divS = PI / (float)slices;
    float divT = 2.0f * PI / (float)stacks;
    float extent = PI - divT;

    // draw rings/stacks about the z-axis
    for ( t = -extent; t <= extent; t += divT )
    {
        radius = n_cos(t) * sphereRad;
        sinT = n_sin(t);
        this->Begin( N_PTYPE_LINE_STRIP );
        for ( int i = 0; i <= slices; i++ )
        {
            s = (float)i * divS;
            x = radius * n_cos(s);
            y = radius * n_sin(s);
            this->Coord( x, y, sinT );
        }
        this->End();
    }

    // draw the rest of the rings/slices
    for ( int i = 0; i < slices; i++ )
    {
        s = (float)i * divS;
        this->Begin( N_PTYPE_LINE_STRIP );
        for ( t = -extent; t <= extent; t += divT )
        {
            radius = n_cos(t) * sphereRad;
            x = radius * n_cos(s);
            y = radius * n_sin(s);
            this->Coord( x, y, n_sin(t) );
        }
        this->End();
    }
}

//------------------------------------------------------------------------------
/**
    @brief Render a solid sphere centred at the origin.
*/
void nPrimitiveServer::SolidSphere( float radius, int slices, int stacks )
{
    float t, nextT;
    float divS = 2.0f * PI / (float)slices;
    float divT = 2.0f * PI / (float)stacks;
    float s = 0.0f;
    float nextS = divS;
    float cosS = 1;
    float sinS = 0;
    float cosNextS = n_cos(nextS);
    float sinNextS = n_sin(nextS);
    float cosT, sinT, cosNextT, sinNextT;
    vector2 p0, p1, p2, p3;
    vector3 n0, n1, n2, n3;

    this->Begin( N_PTYPE_TRIANGLE_LIST );
    int i, j;
    for ( i = 0; i < slices; i++ )
    {
        t = -PI;
        nextT = t + divT;
        cosT = 1;
        sinT = 0;
        cosNextT = n_cos(nextT);
        sinNextT = n_sin(nextT);
        for ( j = 0; j < stacks; j++ )
        {
            n0.set( cosT * cosS, cosT * sinS, sinT ); n0.norm();
            p0.set( radius * n0.x , radius * n0.y );
            n1.set( cosT * cosNextS, cosT * sinNextS, sinT ); n1.norm();
            p1.set( radius * n1.x, radius * n1.y );
            n2.set( cosNextT * cosNextS, cosNextT * sinNextS, sinNextT ); n2.norm();
            p2.set( radius * n2.x, radius * n2.y );
            n3.set( cosNextT * cosS, cosNextT * sinS, sinNextT ); n3.norm();
            p3.set( radius * n3.x, radius * n3.y );

            // tri 1
            this->Norm( n0.x, n0.y, n0.z );
            this->Coord( p0.x, p0.y, radius * sinT );
            this->Norm( n1.x, n1.y, n1.z );
            this->Coord( p1.x, p1.y, radius * sinT );
            this->Norm( n2.x, n2.y, n2.z );
            this->Coord( p2.x, p2.y, radius * sinNextT );
            // tri 2
            this->Norm( n2.x, n2.y, n2.z );
            this->Coord( p2.x, p2.y, radius * sinNextT );
            this->Norm( n3.x, n3.y, n3.z );
            this->Coord( p3.x, p3.y, radius * sinNextT );
            this->Norm( n0.x, n0.y, n0.z );
            this->Coord( p0.x, p0.y, radius * sinT );

            t = nextT;
            nextT += divT;
            cosT = cosNextT;
            cosNextT = n_cos(nextT);
            sinT = sinNextT;
            sinNextT = n_sin(nextT);
        }

        s = nextS;
        nextS += divS;
        cosS = cosNextS;
        cosNextS = n_cos(nextS);
        sinS = sinNextS;
        sinNextS = n_sin(nextS);
    }

    this->End();
}

//------------------------------------------------------------------------------
/**
    @brief Render a wireframe cone.
    @param base Radius of the cone base.
    @param height Height of cone.
    @param closed Draw the cone base?
    @param slices Number of slices.
    @param stacks Number of stacks.

    Render a cone oriented along the z-axis with base at z = 0 and the
    top at z = height. The cone is subdivided around the z-axis into
    slices and along the z-axis into stacks.
*/
void nPrimitiveServer::WireCone( float base, float height, bool closed,
                                int slices, int stacks )
{
    float s, t;
    float nextS, nextX, nextY;
    float x = base;
    float y = 0.0f;
    float gradient = -base / height;
    float divS = 2.0f * PI / (float)slices;
    float divT = height / (float)stacks;
    float radius = base;

    this->Begin( N_PTYPE_LINE_LIST );

    // draw vertical side lines & optionally the base
    for ( int i = 0; i < slices; i++ )
    {
        s = (float)i * divS;
        x = base * n_cos(s);
        y = base * n_sin(s);
        this->Coord( x, y, 0 );
        this->Coord( 0, 0, height );
        // draw part of base
        if ( closed )
        {
            this->Coord( x, y, 0 );
            this->Coord( 0, 0, 0 );
        }
    }

    // draw rings
    for ( int j = 0; j < stacks; j++ )
    {
        t = (float)j * divT;
        radius = gradient * t + base;
        x = radius;
        y = 0.0f;
        for ( int i = 0; i < slices; i++ )
        {
            nextS = (float)(i + 1) * divS;
            nextX = radius * n_cos(nextS);
            nextY = radius * n_sin(nextS);
            // draw arc
            this->Coord( x, y, t );
            this->Coord( nextX, nextY, t ); 
            x = nextX;
            y = nextY;
        }
    }

    this->End();
}

//------------------------------------------------------------------------------
/**
    @brief Render a wireframe cone.
*/
void nPrimitiveServer::SolidCone( float base, float height, bool closed,
                                 int slices, int stacks )
{
    float t, nextT;
    float gradient = -base / height;
    float divS = 2.0f * PI / (float)slices;
    float divT = height / (float)stacks;
    float s = 0.0f;
    float nextS = divS;
    float rad, nextRad;
    float cosS = 1;
    float sinS = 0;
    float cosNextS = n_cos(nextS);
    float sinNextS = n_sin(nextS);
    vector2 p0, p1, p2, p3;
    vector3 n0, n1;

    this->Begin( N_PTYPE_TRIANGLE_LIST );
    int i, j;
    for ( i = 0; i < slices; i++ )
    {
        t = 0.0f;
        nextT = divT;
        rad = base;
        nextRad = gradient * divT + base;
        n0.set( divT * cosS, divT * sinS, -gradient ); n0.norm();
        n1.set( divT * cosNextS, divT * sinNextS, -gradient ); n1.norm();
        for ( j = 0; j < stacks - 1; j++ )
        {
            p0.set( rad * cosS, rad * sinS );
            p1.set( rad * cosNextS, rad * sinNextS );
            p2.set( nextRad * cosNextS, nextRad * sinNextS );
            p3.set( nextRad * cosS, nextRad * sinS );

            // tri 1
            this->Norm( n0.x, n0.y, n0.z );
            this->Coord( p0.x, p0.y, t );
            this->Norm( n1.x, n1.y, n1.z );
            this->Coord( p1.x, p1.y, t );
            this->Norm( n1.x, n1.y, n1.z );
            this->Coord( p2.x, p2.y, nextT );
            // tri 2
            this->Norm( n1.x, n1.y, n1.z );
            this->Coord( p2.x, p2.y, nextT );
            this->Norm( n0.x, n0.y, n0.z );
            this->Coord( p3.x, p3.y, nextT );
            this->Norm( n0.x, n0.y, n0.z );
            this->Coord( p0.x, p0.y, t );

            t = nextT;
            nextT += divT;
            rad = nextRad;
            nextRad += gradient * divT;
        }

        // tri on the tip
        p0.set( rad * cosS, rad * sinS );
        p1.set( rad * cosNextS, rad * sinNextS );
        this->Norm( n0.x, n0.y, n0.z );
        this->Coord( p0.x, p0.y, t );
        this->Norm( n1.x, n1.y, n1.z );
        this->Coord( p1.x, p1.y, t );
        this->Norm( 0, 0, 1 ); // what's the normal at the very tip?
        this->Coord( 0, 0, height );

        // draw a 'base' tri
        if ( closed )
        {
            p0.set( base * cosS, base * sinS );
            p1.set( base * cosNextS, base * sinNextS );
            // base tri
            this->Norm( 0, 0, -1 );
            this->Coord( 0, 0, 0 );
            this->Norm( 0, 0, -1 );
            this->Coord( p1.x, p1.y, 0 );
            this->Norm( 0, 0, -1 );
            this->Coord( p0.x, p0.y, 0 );
        }

        s = nextS;
        nextS += divS;
        cosS = cosNextS;
        sinS = sinNextS;
        cosNextS = n_cos(nextS);
        sinNextS = n_sin(nextS);
    }

    this->End();
}

//------------------------------------------------------------------------------
/**
    @brief Render a wireframe capsule.
    @param radius Capsule radius.
    @param length Capsule length.
    @param slices Number of slices.
    @param stacks Number of stacks.

    Render a capsule oriented along the z-axis with one end at z = 0 and
    the other at z = length + 2 * radius.
*/
void nPrimitiveServer::WireCapsule( float /*radius*/, float /*length*/, 
                                   int /*slices*/, int /*stacks*/ )
{
}

//------------------------------------------------------------------------------
/**
    @brief Render a solid capsule.
*/
void nPrimitiveServer::SolidCapsule( float radius, float length, 
                                    int slices, int stacks )
{
    this->SolidCylinder( radius, length, false, slices, 1 );

    float t, nextT;
    float divS = 2.0f * PI / (float)slices;
    float divT = PI / (float)stacks;
    float s = 0.0f;
    float nextS = divS;
    float cosS = 1;
    float sinS = 0;
    float cosNextS = n_cos(nextS);
    float sinNextS = n_sin(nextS);
    float cosT, sinT, cosNextT, sinNextT;
    vector2 p0, p1, p2, p3;
    vector3 n0, n1, n2, n3;
    int i, j;

    this->Begin( N_PTYPE_TRIANGLE_LIST );  

    // render one end hemi-sphere
    for ( i = 0; i < slices; i++ )
    {
        t = -PI;
        nextT = t + divT;
        cosT = -1;
        sinT = 0;
        cosNextT = n_cos(nextT);
        sinNextT = n_sin(nextT);
        for ( j = 0; j < stacks; j++ )
        {
            n0.set( cosT * cosS, cosT * sinS, sinT ); n0.norm();
            p0.set( radius * n0.x , radius * n0.y );
            n1.set( cosT * cosNextS, cosT * sinNextS, sinT ); n1.norm();
            p1.set( radius * n1.x, radius * n1.y );
            n2.set( cosNextT * cosNextS, cosNextT * sinNextS, sinNextT ); n2.norm();
            p2.set( radius * n2.x, radius * n2.y );
            n3.set( cosNextT * cosS, cosNextT * sinS, sinNextT ); n3.norm();
            p3.set( radius * n3.x, radius * n3.y );

            // tri 1
            this->Norm( n0.x, n0.y, n0.z );
            this->Coord( p0.x, p0.y, radius * sinT );
            this->Norm( n1.x, n1.y, n1.z );
            this->Coord( p1.x, p1.y, radius * sinT );
            this->Norm( n2.x, n2.y, n2.z );
            this->Coord( p2.x, p2.y, radius * sinNextT );
            // tri 2
            this->Norm( n2.x, n2.y, n2.z );
            this->Coord( p2.x, p2.y, radius * sinNextT );
            this->Norm( n3.x, n3.y, n3.z );
            this->Coord( p3.x, p3.y, radius * sinNextT );
            this->Norm( n0.x, n0.y, n0.z );
            this->Coord( p0.x, p0.y, radius * sinT );

            t = nextT;
            nextT += divT;
            cosT = cosNextT;
            cosNextT = n_cos(nextT);
            sinT = sinNextT;
            sinNextT = n_sin(nextT);
        }

        s = nextS;
        nextS += divS;
        cosS = cosNextS;
        cosNextS = n_cos(nextS);
        sinS = sinNextS;
        sinNextS = n_sin(nextS);
    }

    s = 0.0f;
    nextS = divS;
    cosS = 1;
    sinS = 0;
    cosNextS = n_cos(nextS);
    sinNextS = n_sin(nextS);

    // render the other hemi-sphere
    for ( i = 0; i < slices; i++ )
    {
        t = 0.0f;
        nextT = t + divT;
        cosT = 1;
        sinT = 0;
        cosNextT = n_cos(nextT);
        sinNextT = n_sin(nextT);
        for ( j = 0; j < stacks; j++ )
        {
            n0.set( cosT * cosS, cosT * sinS, sinT ); n0.norm();
            p0.set( radius * n0.x , radius * n0.y );
            n1.set( cosT * cosNextS, cosT * sinNextS, sinT ); n1.norm();
            p1.set( radius * n1.x, radius * n1.y );
            n2.set( cosNextT * cosNextS, cosNextT * sinNextS, sinNextT ); n2.norm();
            p2.set( radius * n2.x, radius * n2.y );
            n3.set( cosNextT * cosS, cosNextT * sinS, sinNextT ); n3.norm();
            p3.set( radius * n3.x, radius * n3.y );

            // tri 1
            this->Norm( n0.x, n0.y, n0.z );
            this->Coord( p0.x, p0.y, radius * sinT + length );
            this->Norm( n1.x, n1.y, n1.z );
            this->Coord( p1.x, p1.y, radius * sinT + length );
            this->Norm( n2.x, n2.y, n2.z );
            this->Coord( p2.x, p2.y, radius * sinNextT + length );
            // tri 2
            this->Norm( n2.x, n2.y, n2.z );
            this->Coord( p2.x, p2.y, radius * sinNextT + length );
            this->Norm( n3.x, n3.y, n3.z );
            this->Coord( p3.x, p3.y, radius * sinNextT + length );
            this->Norm( n0.x, n0.y, n0.z );
            this->Coord( p0.x, p0.y, radius * sinT + length );

            t = nextT;
            nextT += divT;
            cosT = cosNextT;
            cosNextT = n_cos(nextT);
            sinT = sinNextT;
            sinNextT = n_sin(nextT);
        }

        s = nextS;
        nextS += divS;
        cosS = cosNextS;
        cosNextS = n_cos(nextS);
        sinS = sinNextS;
        sinNextS = n_sin(nextS);
    }

    this->End();
}

//------------------------------------------------------------------------------
/**
    @brief Render a wireframe cylinder.
    @param radius Cylinder radius.
    @param length Cylinder length.
    @param closed Draw the cylinder ends?
    @param slices Number of slices.
    @param stacks Number of stacks.

    Render a capsule oriented along the z-axis with one end at z = 0 and
    the other at z = length. The cylinder is subdivided around the z-axis into
    slices and along the z-axis into stacks.
*/
void nPrimitiveServer::WireCylinder( float radius, float length, 
                                    bool closed, int slices, int stacks )
{
    float s, t;
    float nextS, nextX, nextY;
    float x = radius;
    float y = 0.0f;
    float divS = 2.0f * PI / (float)slices;
    float divT = length / (float)stacks;

    this->Begin( N_PTYPE_LINE_LIST );
    for ( int i = 0; i < slices; i++ )
    {
        nextS = (float)(i + 1) * divS;
        nextX = radius * n_cos(nextS);
        nextY = radius * n_sin(nextS);

        // draw arcs along the cylinder axis
        for ( int j = 0; j <= stacks; j++ )
        {
            t = (float)j * divT;
            this->Coord( x, y, t );
            this->Coord( nextX, nextY, t );
        }

        // draw a side line
        this->Coord( x, y, 0 );
        this->Coord( x, y, length );

        // draw part of the cylinder 'ends'
        if ( closed )
        {
            this->Coord( x, y, 0 );
            this->Coord( 0, 0, 0 );
            this->Coord( x, y, length );
            this->Coord( 0, 0, length );
        }

        s = nextS;
        x = nextX;
        y = nextY;
    }
    this->End();
}

//------------------------------------------------------------------------------
/**
    @brief Render a solid cylinder.
*/
void nPrimitiveServer::SolidCylinder( float radius, float length, 
                                     bool closed, int slices, int stacks )
{
    float t, nextT;
    float divS = 2.0f * PI / (float)slices;
    float divT = length / (float)stacks;
    float s = 0.0f;
    float nextS = divS;
    vector2 p0, p1, n0, n1;

    /*
    Could probably be optimised to use triangle strips, but
    I couldn't be bothered.
    */

    this->Begin( N_PTYPE_TRIANGLE_LIST );
    int i, j;
    for ( i = 0; i < slices; i++ )
    {
        t = 0.0f;
        nextT = divT;
        for ( j = 0; j < stacks; j++ )
        {
            n0.set( n_cos(s), n_sin(s) ); n0.norm();
            p0.set( radius * n0.x , radius * n0.y );
            n1.set( n_cos(nextS), n_sin(nextS) ); n1.norm();
            p1.set( radius * n1.x, radius * n1.y );

            // tri 1
            this->Norm( n0.x, n0.y, 0 );
            this->Coord( p0.x, p0.y, t );
            this->Norm( n1.x, n1.y, 0 );
            this->Coord( p1.x, p1.y, t );
            this->Norm( n1.x, n1.y, 0 );
            this->Coord( p1.x, p1.y, nextT );
            // tri 2
            this->Norm( n1.x, n1.y, 0 );
            this->Coord( p1.x, p1.y, nextT );
            this->Norm( n0.x, n0.y, 0 );
            this->Coord( p0.x, p0.y, nextT );
            this->Norm( n0.x, n0.y, 0 );
            this->Coord( p0.x, p0.y, t );

            t = nextT;
            nextT += divT;
        }

        // draw two 'end' tris
        if ( closed )
        {
            p0.set( radius * n_cos(s), radius * n_sin(s) );
            p1.set( radius * n_cos(nextS), radius * n_sin(nextS) );
            // tri on one end
            this->Norm( 0, 0, -1 );
            this->Coord( 0, 0, 0 );
            this->Norm( 0, 0, -1 );
            this->Coord( p1.x, p1.y, 0 );
            this->Norm( 0, 0, -1 );
            this->Coord( p0.x, p0.y, 0 );
            // tri on the other end
            this->Norm( 0, 0, 1 );
            this->Coord( p0.x, p0.y, length );
            this->Norm( 0, 0, 1 );
            this->Coord( p1.x, p1.y, length );
            this->Norm( 0, 0, 1 );
            this->Coord( 0, 0, length );
        }

        s = nextS;
        nextS += divS;
    }

    this->End();
}

//------------------------------------------------------------------------------
/**
    @brief Render a wireframe cube centred at the origin.
    @param size Length of each of the cube's sides.
*/
void nPrimitiveServer::WireCube( float size )
{
    float half = size * 0.5f;
    // top
    this->Begin( N_PTYPE_LINE_STRIP );
    this->Coord( -half, half, -half );
    this->Coord( half, half, -half );
    this->Coord( half, half, half );
    this->Coord( -half, half, half );
    this->Coord( -half, half, -half );
    this->End();
    // bottom
    this->Begin( N_PTYPE_LINE_STRIP );
    this->Coord( -half, -half, -half );
    this->Coord( half, -half, -half );
    this->Coord( half, -half, half );
    this->Coord( -half, -half, half );
    this->Coord( -half, -half, -half );
    this->End();
    // sides
    this->Begin( N_PTYPE_LINE_LIST );
    this->Coord( -half, -half, -half ); this->Coord( -half, half, -half );
    this->Coord( half, -half, -half );  this->Coord( half, half, -half );
    this->Coord( half, -half, half );   this->Coord( half, half, half );
    this->Coord( -half, -half, half );  this->Coord( -half, half, half );
    this->End();
}

//------------------------------------------------------------------------------
/**
    @brief Render a solid cube.
*/
void nPrimitiveServer::SolidCube( float /*size*/ )
{
}

//------------------------------------------------------------------------------
/**
    @brief Render a wireframe box centred at the origin.
    @param lx Length of box in the x-dimension.
    @param ly Length of box in the y-dimension.
    @param lz Length of box in the z-dimension.
*/
void nPrimitiveServer::WireBox( float lx, float ly, float lz )
{
    float x = lx * 0.5f;
    float y = ly * 0.5f;
    float z = lz * 0.5f;
    // top
    this->Begin( N_PTYPE_LINE_STRIP );
    this->Coord( -x, y, -z );
    this->Coord( x, y, -z );
    this->Coord( x, y, z );
    this->Coord( -x, y, z );
    this->Coord( -x, y, -z );
    this->End();
    // bottom
    this->Begin( N_PTYPE_LINE_STRIP );
    this->Coord( -x, -y, -z );
    this->Coord( x, -y, -z );
    this->Coord( x, -y, z );
    this->Coord( -x, -y, z );
    this->Coord( -x, -y, -z );
    this->End();
    // sides
    this->Begin( N_PTYPE_LINE_LIST );
    this->Coord( -x, -y, -z );  this->Coord( -x, y, -z );
    this->Coord( x, -y, -z );   this->Coord( x, y, -z );
    this->Coord( x, -y, z );    this->Coord( x, y, z );
    this->Coord( -x, -y, z );   this->Coord( -x, y, z );
    this->End();
}

//------------------------------------------------------------------------------
/**
    @brief Render a solid box centred at the origin.
*/
void nPrimitiveServer::SolidBox( float /*lx*/, float /*ly*/, float /*lz*/ )
{
}

//------------------------------------------------------------------------------
/**
    @brief Render a wireframe torus.
    @param innerRadius The inner radius of the torus.
    @param outerRadius The outer radius of the torus.
    @param sides
    @param rings

    Render a torus (doughnut) centred at the origin with its central
    axis aligned with the z-axis. The torus is subdivided into 'rings'
    segments along its circular centre line and into 'sides' segments
    along that line.
*/
void nPrimitiveServer::WireTorus( float /*innerRadius*/, float /*outerRadius*/, 
                                 int /*sides*/, int /*rings*/ )
{
}

//------------------------------------------------------------------------------
/**
    @brief Render a solid torus.
*/
void nPrimitiveServer::SolidTorus( float /*innerRadius*/, float /*outerRadius*/, 
                                  int /*sides*/, int /*rings*/ )
{
}

//------------------------------------------------------------------------------
/**
*/
void nPrimitiveServer::CheckerPlane(float planeExtent, float gridSize)
{
    float extent = float((int)(planeExtent / gridSize));
    
    int inc_y = 0;
    int inc_x = 0;
    for (float y = -extent; y < extent; y += gridSize)
    {
        for (float x = -extent; x < extent; x += gridSize)
        {
            this->Begin(N_PTYPE_TRIANGLE_STRIP);
            if ((inc_x + inc_y) % 2) 
                this->Rgba(1.0f,0.4f,0.4f,0.3f);
            else
                this->Rgba(0.4f,0.4f,1.0f,0.3f);
            this->Norm(0.0f, 1.0f, 0.0f);
            this->Coord(x+gridSize, 0.0f, y);
            this->Norm(0.0f, 1.0f, 0.0f);
            this->Coord(x, 0.0f, y);
            this->Norm(0.0f, 1.0f, 0.0f);
            this->Coord(x+gridSize, 0.0f, y+gridSize);
            this->Norm(0.0f, 1.0f, 0.0f);
            this->Coord(x, 0.0f, y+gridSize);
            this->End();
            inc_x++;
        }        
        inc_y++;
    }   
}


