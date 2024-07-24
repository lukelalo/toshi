#ifndef N_CMD_H
#define N_CMD_H
/*!
  \file
*/
//--------------------------------------------------------------------
/**
    @class nCmd
    @ingroup ScriptServices
    
    @brief implement a function call like message object

    Encapsulates a function call into a C++ object. A nCmd object
    has a name, a corresponding 32bit ID, a number of typed input
    args and a number of typed output args. It can be used to
    carry messages between Nebula objects if direct method
    calls cannot be used.

    nCmd objects are usually allocated by their parent nCmdProto:

@code
    nCmd * cmd = cmdProto->NewCmd();
@endcode

    and, if obtained in that manner, must be released by the
    nCmdProto as well:

@code
    cmdProto->RelCmd(cmd);
@endcode

    (C) 1999 A.Weissflog
*/
//--------------------------------------------------------------------
#include <stdlib.h>
#include <string.h>

#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_ARG_H
#include "kernel/narg.h"
#endif

//--------------------------------------------------------------------
class nCmdProto;
class nCmd {
    /// Number of maximum arguments allowed
    enum {
        N_MAXNUM_ARGS = 32,
    };
    /// The 'mother' of this object
    nCmdProto *proto;           
    /// Array of arguments
    nArg args[N_MAXNUM_ARGS];   
    /// Number of valid args in array
    int num_args;
    /// Index of the first 'in' arguments
    int first_in_arg;
    /// Index of current 'out' argument
    int out_arg_index;
    /// Index of current 'in' argument
    int in_arg_index;
        
public:
    /// Class constructor
    inline nCmd(nCmdProto *, int, int);
    /// Class constructor
    inline nCmd(nCmd *);
    /// Class destructor
    inline ~nCmd();
    /// Returns the command's prototype
    inline nCmdProto *GetProto(void);
    /// Returns the count of 'in' arguments
    inline int GetNumInArgs(void);
    /// Returns the count of 'out' arguments
    inline int GetNumOutArgs(void);
    /// Returns the current 'in' argument
    inline nArg *In(void); 
    /// Returns the current 'out' argument
    inline nArg *Out(void);
    /// Rewinds the pointers for the In and Out access functions to the start of the argumentarray
    inline void Rewind(void);
};

/**
  @param p Pointer to command prototype
  @param num_out_args The count of 'out' arguments
  @param num_in_args The count of 'in' arguments

*/
inline nCmd::nCmd(nCmdProto *p, int num_out_args, int num_in_args)
{
    this->proto = p;
    this->num_args = num_in_args + num_out_args;
    n_assert(this->num_args < N_MAXNUM_ARGS);
    this->first_in_arg  = num_out_args;    
    this->out_arg_index = 0;
    this->in_arg_index  = this->first_in_arg;
}
/**
  @param c Pointer to a command

  - 11-Aug-99   floh    args was simply copied with memcpy(), this was fatal
                        in the case of a string argument

  - 18-Aug-99   floh    another evil bug, num_args was evaluated in a loop
                        before being initailized

*/
inline nCmd::nCmd(nCmd *c)
{
    int i;
    this->proto         = c->proto;
    this->num_args      = c->num_args;
    this->first_in_arg  = c->first_in_arg;
    this->in_arg_index  = this->first_in_arg;
    this->out_arg_index = 0;
    for (i=0; i<this->num_args; i++) this->args[i].Set(c->args[i]);
}
//--------------------------------------------------------------------
inline nCmd::~nCmd() 
{ }
//--------------------------------------------------------------------
inline int nCmd::GetNumInArgs(void)
{
    return (this->num_args - this->first_in_arg);
}
//--------------------------------------------------------------------
inline int nCmd::GetNumOutArgs(void)
{
    return this->first_in_arg;
}
//--------------------------------------------------------------------
inline nCmdProto *nCmd::GetProto(void)
{
    return this->proto;
}

/**
  Current 'in' argument is set to the first 'in' argument and the current
  'out' argument is set to 0
*/
//--------------------------------------------------------------------
inline void nCmd::Rewind(void) 
{
    this->in_arg_index  = this->first_in_arg;
    this->out_arg_index = 0;
}
//--------------------------------------------------------------------
inline nArg *nCmd::In(void)
{
    n_assert(this->in_arg_index < this->num_args);
    return &(this->args[this->in_arg_index++]);
}
//--------------------------------------------------------------------
inline nArg *nCmd::Out(void)
{
    n_assert(this->out_arg_index < this->first_in_arg);
    return &(this->args[this->out_arg_index++]);
}
//--------------------------------------------------------------------
#endif    
