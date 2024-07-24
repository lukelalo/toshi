#define N_IMPLEMENTS nCmd
#define N_KERNEL
//--------------------------------------------------------------------
//  IMPLEMENTATION
//      nCmd
//
//  (C) 1998 by Andre Weissflog
//--------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "kernel/ncmdproto.h"

//--------------------------------------------------------------------
//  21-Jun-99   floh    neuer Datentyp: Object
//--------------------------------------------------------------------
static bool is_valid_arg(char c)
{
    switch(c) {
        case 'i':
        case 'f':
        case 's':
        case 'v':
        case 'b':
        case 'o':
        case 'c':
        case 'l':
            return true;
        default:
            return false;
    }
}

//--------------------------------------------------------------------
//  nCmdProto(char *, ulong)
//  03-Nov-98   floh    created
//  08-Aug-99   floh    bastelt sofort eingebettetes Cmd-Template
//  02-Jan-00   floh    + cmd_proc
//--------------------------------------------------------------------
nCmdProto::nCmdProto(const char *_proto_def)
{
    char tmp[MAX_PROTOLEN];
    char *out_arg_str, *in_arg_str, *name_str;
    
    n_assert((strlen(_proto_def)+1) < MAX_PROTOLEN);
    strcpy(this->proto_def,_proto_def);
        
    // isoliere und validiere OutArgs, Name und InArgs...
    strcpy(tmp,this->proto_def);
    out_arg_str = strtok(tmp,"_");
    name_str    = strtok(NULL,"_");
    in_arg_str  = strtok(NULL,"_");
    if (out_arg_str && name_str && in_arg_str) {
        n_assert((strlen(out_arg_str)+1) < MAX_OUTARGS);
        n_assert((strlen(in_arg_str)+1) < MAX_INARGS);
        char c;

        // In Args ausfuellen
        this->num_in_args = 0;
        while ((c = *in_arg_str++)) {
            // Void-Argument ignorieren
            if (c != 'v') {
                if (!is_valid_arg(c)) n_error("nCmdProto::nCmdProto(): invalid input arg!");
                else this->in_args[this->num_in_args++] = c;
            }
        }
        this->in_args[this->num_in_args] = 0;

        // Out Args ausfuellen
        this->num_out_args = 0;
        while ((c = *out_arg_str++)) {
            // Void Arg ignorieren
            if (c != 'v') {
                if (!is_valid_arg(c)) n_error("nCmdProto::nCmdProto(): invalid output arg!");
                else this->out_args[this->num_out_args++] = c;
            }
        }
        this->out_args[this->num_out_args] = 0;

        // Name des nCmdProto in die Hashnode
        this->SetName(name_str);
    } 
    else 
      n_error("nCmdProto::nCmdProto(): invalid prototype definition!");

    // erzeuge Template-Cmd
    this->cmd_template = n_new nCmd(this,
                                    this->num_out_args,
                                    this->num_in_args);
    char *str;
    char c;
    // Input-Argument-Typen initialisieren
    str = this->in_args;
    while ((c = *str++)) {
        nCmd *cmd = this->cmd_template;
        switch(c) {
            case 'i':   cmd->In()->SetI(0);     break;
            case 'f':   cmd->In()->SetF(0.0f);  break;
            case 'b':   cmd->In()->SetB(FALSE); break;
            case 's':   cmd->In()->SetS(NULL);  break;
            case 'o':   cmd->In()->SetO(NULL);  break;
            case 'c':   cmd->In()->SetC(NULL);  break;
            case 'l':   cmd->In()->SetL(NULL,0); break;
            default:    break;
       } 
    }
    // Output-Argument-Typen initialisieren
    str = this->out_args;
    while ((c = *str++)) {
        nCmd *cmd = this->cmd_template;
        switch(c) {
            case 'i':   cmd->Out()->SetI(0);     break;
            case 'f':   cmd->Out()->SetF(0.0f);  break;
            case 'b':   cmd->Out()->SetB(FALSE); break;
            case 's':   cmd->Out()->SetS(NULL);  break;
            case 'o':   cmd->Out()->SetO(NULL);  break;
            case 'c':   cmd->Out()->SetC(NULL);  break;
            case 'l':   cmd->Out()->SetL(NULL,0); break;
            default:    break;
       } 
    }
    this->cmd_template->Rewind();
    this->cmd_locked = false;
}

//--------------------------------------------------------------------
//  nCmdProto(nCmdProto *)
//  03-Nov-98   floh    created
//  08-Aug-99   floh    + Cmd-Template wird initialisiert
//  02-Jan-00   floh    + cmd_proc
//--------------------------------------------------------------------
nCmdProto::nCmdProto(nCmdProto *cp)
{
    n_assert(cp);
    this->SetName(cp->GetName());
    strcpy(this->proto_def,cp->proto_def);
    this->num_in_args  = cp->num_in_args;
    this->num_out_args = cp->num_out_args;
    strcpy(this->in_args,cp->in_args);
    strcpy(this->out_args,cp->out_args);
    this->cmd_template = n_new nCmd(cp->cmd_template);
    this->cmd_locked = false;
}

//--------------------------------------------------------------------
//  ~nCmdProto()
//--------------------------------------------------------------------
nCmdProto::~nCmdProto() 
{
    if (this->cmd_template) 
        n_delete this->cmd_template;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
