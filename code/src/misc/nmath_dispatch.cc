#define N_IMPLEMENTS nMathServer
//-------------------------------------------------------------------
//  nmath_dispatch.cc
//  (C) 1999 A.Weissflog
//-------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
    @scriptclass
    nmathserver

    @superclass
    nroot

    @classinfo
    The nMathServer is situated under /sys/servers/math and 
    implements a vector array operations specialized software cpu. 
    The idea behind the nMathServer is to have a central point for 
    heavy operations on matrix and vector arrays, which can be 
    massively optimized without regard to side effects or 
    universality. And it is the starting point for special 
    "hardware accelerated" classes which use assembler-, KNI- or 
    3DNow instructions, for example. 
    It showed that a similar agressive optimization with c++ 
    inline classes is either too complex or bloats the code 
    unnaturaly. Despite this are the common inline c++ classes 
    the better choice for normal operations which do not use 
    huge arrays or many repeating matrix operations (because 
    of the function call overhead and the bad optimization 
    possibilities for the compiler in such cases). You should 
    weigh exactly (and profile eventually) which method to use. 
*/

