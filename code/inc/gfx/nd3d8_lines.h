#ifndef N_D3D8LINES_H
#define N_D3D8LINES_H

#ifndef N_DYNVERTEXBUFFER_H
#include "gfx/ndynvertexbuffer.h"
#endif

#ifndef N_GFXSERVER_H
#include "gfx/ngfxserver.h"
#endif

#ifndef N_PIXELSHADERDESC_H
#include "gfx/npixelshader.h"
#endif

#include <d3d8.h>
#include <d3dx8.h>

class nD3D8Lines {

private:
	int myPrimitiveNumVerts, myPrimitiveMaxVerts;	// Keeing track of Primitive rendering
	D3DPRIMITIVETYPE    myPrimitiveType;            // The type we'll be rendering
	D3DCOLOR            myPrimitiveRGBA;		    // The most recent color
	nDynVertexBuffer    myPrimitiveDynVB;           // Our dynamic vertex buffer
	nVertexBuffer       *myPrimitiveVB;             // The VB in use, returned from DynVB->Begin()
	nIndexBuffer        *myPrimitiveIB;		        // Our indexbuffer for primitives
	nRef<nPixelShader>  myPrimitivePixelShader;		// PixelShader for the primitives

	nGfxServer		   *myGfxServer;
	nKernelServer	   *myKernelServer;
public:
	
	nD3D8Lines( nGfxServer *gs, nKernelServer *ks ) :
	  myGfxServer( gs ),
	  myKernelServer( ks ),
	  myPrimitiveDynVB( ks, gs),
      myPrimitiveVB(NULL),
	  myPrimitiveNumVerts(0),
	  myPrimitiveMaxVerts(0),
	  myPrimitiveRGBA(D3DCOLOR_COLORVALUE(1,1,1,0))
	{
		myPrimitivePixelShader = myGfxServer->NewPixelShader("prim_pizel_slader");

		nPixelShaderDesc *ps_desc = new nPixelShaderDesc();
		ps_desc->SetNumStages( 1 );
		ps_desc->SetColorOp( 0, nPSI::MUL, nPSI::TEX, nPSI::PREV, nPSI::NOARG, nPSI::ONE );
		ps_desc->SetAddressU( 0, N_TADDR_CLAMP );
		ps_desc->SetAddressV( 0, N_TADDR_CLAMP );
		ps_desc->SetMinFilter( 0, N_TFILTER_NEAREST_MIPMAP_LINEAR);
		ps_desc->SetMagFilter( 0, N_TFILTER_NEAREST_MIPMAP_LINEAR);
		ps_desc->SetTexCoordSrc( 0, N_TCOORDSRC_UV0 );
		ps_desc->SetDiffuse( vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
		ps_desc->SetEmissive( vector4( 0.0f, 0.0f, 0.0f, 0.0f ) );
		ps_desc->SetAmbient( vector4( 0.0f, 0.0f, 0.0f, 0.0f ) );
		ps_desc->SetLightEnable( false );
		ps_desc->SetAlphaEnable( false );
		ps_desc->SetZWriteEnable( false );
		ps_desc->SetFogEnable( true );
		ps_desc->SetZFunc( N_CMP_LESSEQUAL );
		myPrimitivePixelShader->SetShaderDesc( ps_desc );
	};
	
	//-------------------------------------------------------------------
	//  Begin()
	//		Defines the beginning of a new primitive to be drawn.
	//	18-Dec-00	WhiteGold	created
	//-------------------------------------------------------------------
	inline void Begin(nPrimType ptype)
	{
		D3DPRIMITIVETYPE mode;
    
		switch(ptype) {
			case N_PTYPE_TRIANGLE_LIST:  mode=D3DPT_TRIANGLELIST; break;
			case N_PTYPE_TRIANGLE_FAN:   mode=D3DPT_TRIANGLEFAN; break;
			case N_PTYPE_TRIANGLE_STRIP: mode=D3DPT_TRIANGLESTRIP; break;
			case N_PTYPE_LINE_LIST:      mode=D3DPT_LINELIST; break;
			case N_PTYPE_LINE_STRIP:     mode=D3DPT_LINESTRIP; break;
			case N_PTYPE_POINT_LIST:     mode=D3DPT_POINTLIST; break;
			default: n_error("nD3D8Server::Begin(): Unknown primitive type!"); break;
		}

		// We'll do it in N_MAX_P_VERTEX chunks... then swap
		if (!myPrimitiveDynVB.IsValid())
			 myPrimitiveDynVB.Initialize( N_VT_COORD | N_VT_NORM | N_VT_RGBA | N_VT_UV0, 0);

		nIndexBuffer *ibuf = myGfxServer->FindIndexBuffer("primitive_gfx");
		if (!ibuf) {
			ibuf = myGfxServer->NewIndexBuffer("primitive_gfx");
			myPrimitiveMaxVerts = myPrimitiveDynVB.GetNumVertices();
        
			// round down to multiple of 3
			myPrimitiveMaxVerts = (myPrimitiveMaxVerts / 3) * 3;
			ibuf->Begin(N_IBTYPE_WRITEONLY, ptype, myPrimitiveMaxVerts);
			int i;
			for (i=0; i< myPrimitiveMaxVerts; i++) {
				ibuf->Index(i, i);
			}
			ibuf->End();
		}

		myPrimitiveVB = myPrimitiveDynVB.Begin( ibuf, myPrimitivePixelShader.get(), NULL );

		n_assert(ibuf);
		myPrimitiveIB   = ibuf;
		myPrimitiveType = mode;
	};

	//-------------------------------------------------------------------
	//  End()
	//		All vertices are in place, render the new primitive
	//	18-Dec-00	WhiteGold	created
	//-------------------------------------------------------------------
	inline void End(void)
	{
		n_assert(myPrimitiveVB);
		myPrimitiveVB = NULL;

		myPrimitiveDynVB.End( myPrimitiveNumVerts, myPrimitiveNumVerts );
		myPrimitiveNumVerts = 0;
	};

	//-------------------------------------------------------------------
	//  Coord()
	//		Add a new vertex to the primitive
	//	18-Dec-00	WhiteGold	created
	//-------------------------------------------------------------------
	inline void Coord(float x, float y, float z)
	{
		n_assert(myPrimitiveVB);

		myPrimitiveVB->Coord( myPrimitiveNumVerts, vector3(x, y, z) );
		myPrimitiveVB->Norm ( myPrimitiveNumVerts, vector3(0, 0, -1) );
		myPrimitiveVB->Uv( myPrimitiveNumVerts, 0, vector2(1, 1) );
		myPrimitiveVB->Color( myPrimitiveNumVerts, myPrimitiveRGBA );

		n_assert( myPrimitiveNumVerts< myPrimitiveMaxVerts );

		if ( ++myPrimitiveNumVerts == ( myPrimitiveMaxVerts - 1 ))
		{
			myPrimitiveVB = myPrimitiveDynVB.Swap( myPrimitiveNumVerts, myPrimitiveNumVerts );
			myPrimitiveNumVerts = 0;
		}

	};

	//-------------------------------------------------------------------
	//  Norm()
	//		Add a normal to the current vertex
	//	18-Dec-00	WhiteGold	created
	//-------------------------------------------------------------------
	inline void Norm(float nx, float ny, float nz)
	{
		n_assert(myPrimitiveVB);

		myPrimitiveVB->Norm( myPrimitiveNumVerts, vector3(nx, ny, nz) );
	};

	//-------------------------------------------------------------------
	//  Rgba()
	//		Set the color for the subsequent vertices to be drawn in
	//		default is R 1.0, G 1.0, B 1.0, A 0.0
	//	18-Dec-00	WhiteGold	created
	//-------------------------------------------------------------------
	inline void Rgba(float r, float g, float b, float a)
	{
		myPrimitiveRGBA = D3DCOLOR_COLORVALUE( r, g, b, a);
	};

	//-------------------------------------------------------------------
	//  Uv() - only 1 per vertex
	//	18-Dec-00	WhiteGold	created
	//-------------------------------------------------------------------
	inline void Uv(ulong, float u, float v)
	{
		myPrimitiveVB->Uv( myPrimitiveNumVerts, 0, vector2( u, v ));
	};
	//-----------------------------------------------------------------------------
};

#endif