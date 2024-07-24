#ifndef N_CMDPROTO_H
#define N_CMDPROTO_H
//------------------------------------------------------------------------------
/**
  @class nCmdProto
  @ingroup ScriptServices
  @brief A factory for nCmd objects

  An nCmdProto object holds the prototype description for an nCmd
  object and can construct nCmd objects based on the prototype
  description "blue print".

  The prototype description is given to the nCmdProto constructor
  as a string of the format

@verbatim
  outargs_name_inargs
@endverbatim

  @c "outargs" is a list of characters describing number and datatypes
  of output arguments, @c "inargs" describes the input args in the
  same way. @c "name" is the name of the command.

  The following datatypes are defined:

      - @c 'v'     - void
      - @c 'i'     - int
      - @c 'f'     - float
      - @c 's'     - string
      - @c 'b'     - bool

  Examples of prototype descriptions:

      - @c v_rotate_fff    - name is @c 'rotate', no output args, 3 float input args, 
      - @c v_set_si        - name is @c 'set', no output args, one string and one int input arg
      - @c fff_getrotate_v - 3 float output args, no input arg, name is @c 'getrotate'

  (C) 1999 A.Weissflog
*/

#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_CMD_H
#include "kernel/ncmd.h"
#endif

#ifndef N_HASHNODE_H
#include "util/nhashnode.h"
#endif

#undef N_DEFINES
#define N_DEFINES nCmdProto
#include "kernel/ndefdllclass.h"

//--------------------------------------------------------------------
class N_PUBLIC nCmdProto : public nHashNode {
public:
    /// Definition of constants
    enum {
        MAX_PROTOLEN = 96,
        MAX_INARGS   = 32,
        MAX_OUTARGS  = 32,
    };
    
protected:
    /// The complete prototype definition string
    char proto_def[MAX_PROTOLEN];
    /// The string of 'in' argument
    char in_args[MAX_INARGS];
    /// The string of 'out' arguments
    char out_args[MAX_OUTARGS];
    /// The count of 'in' arguments
    int num_in_args;
    /// The count of 'out' arguments
    int num_out_args;
    /// Pointer to the command
    nCmd *cmd_template;
    /// Flag for locking
    bool cmd_locked;

public:
    /// Class constructor
    nCmdProto(const char *_proto_def);
    /// Class constructor
    nCmdProto(nCmdProto *cp);
    /// Class destructor
    virtual ~nCmdProto();
    /// Execute a script command on the provided object
    virtual bool Dispatch(void *, nCmd *) = 0;

    /// Returns the proto command definition string
    const char *GetProtoDef(void) { 
        return proto_def; 
    };

    nCmd *NewCmd(void) {
        if (cmd_locked)
        {
            return n_new nCmd(this->cmd_template);
        }
        else
        {
            cmd_locked = true;
            cmd_template->Rewind();
            return cmd_template;
        }
    };
    void RelCmd(nCmd *cmd) {
        if (cmd != cmd_template)
        {
            // this was a created command, delete it
            n_delete cmd;
        }
        else
        {
            // this was the cached command template, just unlock it
            n_assert(cmd_locked);
            cmd_locked = false;
        }
    };

    /// get number of input args
    int GetNumInArgs()
    {
        return this->num_in_args;
    }

    /// get number of output args
    int GetNumOutArgs()
    {
        return this->num_out_args;
    }
};
//--------------------------------------------------------------------
#endif    

