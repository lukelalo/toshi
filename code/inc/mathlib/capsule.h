#ifndef N_CAPSULE_H
#define N_CAPSULE_H
//-------------------------------------------------------------------
/**
  @class capsule
  @brief a capsule in 3d space (cylinder with hemi-spheres on its ends)
    
  (c) 2003 Vadim Macagon
   
  capsule is licensed under the terms of the Nebula License.
*/
//-------------------------------------------------------------------
#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#ifndef N_LINE_H
#include "mathlib/line.h"
#endif

//-------------------------------------------------------------------
class capsule 
{
  public:
    line3 seg;   // central axis of cylinder
    float   r;   // radius
  
    capsule() {}
    capsule( vector3& start, vector3& end, float rad )
      : seg( start, end ), r( rad ) {} 

    const vector3& origin() const
    {
      return seg.start();
    }
    
    vector3 direction() const
    {
      return seg.m;
    }
};    
//-------------------------------------------------------------------
#endif
