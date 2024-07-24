#define N_IMPLEMENTS nD3D8Server
//-----------------------------------------------------------------------------
//  nd3d8_lines.cc
//  (C) 2001 A.Weissflog
//-----------------------------------------------------------------------------
#include "gfx/nd3d8server.h"
#include "gfx/nd3d8_lines.h"

// Primitive drawing -- I sure hope you call Begin first ;)
void nD3D8Server::Begin(nPrimType t)
{
	n_assert(this->inBeginScene);
	if (!myLines)
	{
		myLines = new nD3D8Lines( this, this->kernelServer );
	}
	
 	myLines->Begin( t );
};

// Primitive drawing
void nD3D8Server::Coord(float x, float y, float z)
{
	myLines->Coord(x, y, z);
}
// Primitive drawing
void nD3D8Server::Norm(float x, float y, float z)
{
	myLines->Norm(x, y, z);
}
// Primitive drawing
void nD3D8Server::Rgba(float r, float g, float b, float a)
{
	myLines->Rgba(r, g, b, a);
}

// Primitive drawing
void nD3D8Server::Uv(ulong layer, float u, float v)
{
	myLines->Uv(layer, u, v);
}
// Primitive drawing
void nD3D8Server::End(void) 
{
	n_assert(this->inBeginScene);
	myLines->End();
}

//-----------------------------------------------------------------------------
