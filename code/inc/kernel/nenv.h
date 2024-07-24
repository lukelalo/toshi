#ifndef N_ENV_H
#define N_ENV_H
/*!
  \file
*/
//--------------------------------------------------------------------
/**
    @class nEnv
    
    @brief simple env variable style class

    A typed value in a named object. Technically, a nArg object
    wrapped into a nRoot.

    (C) 1999 A.Weissflog
*/
//--------------------------------------------------------------------
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_ARG_H
#include "kernel/narg.h"
#endif

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#undef N_DEFINES
#define N_DEFINES nEnv
#include "kernel/ndefdllclass.h"

class nEnv : public nRoot {
public:
	/// Pointer to local class
    static nClass *local_cl;
	/// Pointer to the kernelserver
    static nKernelServer *ks;

private:
	/// The argument
    nArg arg;

public:
	/// Saves the nEnv variable
    virtual bool SaveCmds(nPersistServer *);

	/// Returns the type of this nEnv instance
    virtual nArg::ArgType GetType(void);
	/// Sets the value of this object to the passed integer
    virtual void  SetI(int i);
	/// Sets the value of this object to the passes boolean value
    virtual void  SetB(bool b);
	/// Sets the value of this object to the passed float value
    virtual void  SetF(float f);
	/// Sets the value of this object to the passed string
    virtual void  SetS(const char *s);
	/// Sets the value of this objet to the passed object
    virtual void  SetO(nRoot *o); 
	/// Returns the integer value
    virtual int   GetI(void);
	/// Returns the boolean value
    virtual bool  GetB(void);
	/// Returns the float value
    virtual float GetF(void);
	/// Returns the string
    virtual const char *GetS(void);
	/// Returns the object
    virtual nRoot *GetO(void);
};
//--------------------------------------------------------------------
#endif      
