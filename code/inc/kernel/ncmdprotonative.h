#ifndef N_CMDPROTONATIVE_H
#define N_CMDPROTONATIVE_H
//------------------------------------------------------------------------------
/**
  @class nCmdProtoNative
  @ingroup ScriptServices
  @brief A factory for nCmd objects that correspond to natively implemented
         script commands.

  (c) 2003 Vadim Macagon
  
  nCmdProtoNative is licensed under the terms of the Nebula License
*/

#ifndef N_CMDPROTO_H
#include "kernel/ncmdproto.h"
#endif

#undef N_DEFINES
#define N_DEFINES nCmdProtoNative
#include "kernel/ndefdllclass.h"

//--------------------------------------------------------------------
class N_PUBLIC nCmdProtoNative : public nCmdProto 
{
  private:
    /// The command ID
    ulong id;
    
    /// pointer to C style command handler
    void (*cmd_proc)(void *, nCmd *);

  public:
    /// Class constructor
    nCmdProtoNative(const char *_proto_def, ulong _id, void (*)(void *, nCmd *));
    /// Class constructor
    nCmdProtoNative(nCmdProtoNative *cp);
    
    bool Dispatch(void *, nCmd *);
    
    /// Returns the ID of the command
    ulong GetID(void) { return id; }
};
//--------------------------------------------------------------------
#endif    

