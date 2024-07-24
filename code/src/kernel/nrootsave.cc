#define N_IMPLEMENTS nRoot
#define N_KERNEL
//--------------------------------------------------------------------
//  nrootsave.cc
//  (C) 1998-2000 Andre Weissflog
//--------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>

#include "kernel/nroot.h"
#include "kernel/nkernelserver.h"
#include "kernel/npersistserver.h"

//--------------------------------------------------------------------
/**
    - 04-Nov-98   floh    created
*/
//--------------------------------------------------------------------
bool nRoot::Save(void)
{
    return this->SaveAs(this->GetName());
}

//--------------------------------------------------------------------
/**
    - 04-Nov-98   floh    created
    - 11-Dec-98   floh    + beachtet jetzt das N_FLAG_SAVEUPSIDEDOWN,
                            und speichert in diesem Fall die
                            Child-Objekte zuerst ab
    - 02-May-00   floh    + support for Import(). if an import filename
                            is set, only the import statement will be
                            written instead of the sequence of commands.
                            Subobjects will be skipped as well.
    - 09-Aug-00   floh    + support for N_FLAG_SAVESHALLOW (don't save
                            child objects)
*/
//--------------------------------------------------------------------
bool nRoot::SaveAs(const char *name)
{
    n_assert(name);
    nPersistServer *fs = (nPersistServer *) ks->GetPersistServer();
    bool retval = FALSE;
    if (fs) {
        if (fs->BeginObject(this,name)) {
            if (this->import_file) {
                // write single import statement!
                nCmd *cmd;
                if ((cmd = fs->GetCmd(this,'IMPO'))) {
                    cmd->In()->SetS(this->import_file);
                    fs->PutCmd(cmd);
                }
                retval = true;
            } else {
                // ...the usual behaviour...
                nRoot *c;
                if (this->root_flags & N_FLAG_SAVEUPSIDEDOWN) {
                    // upsidedown: save children first, then own status
                    if (!(this->root_flags & N_FLAG_SAVESHALLOW)) {
                        for (c=this->GetHead(); c; c=c->GetSucc()) c->Save();
                    }
                    retval = this->SaveCmds(fs);
                } else {
                    // normal: save own status first, then children
                    retval = this->SaveCmds(fs);
                    if (!(this->root_flags & N_FLAG_SAVESHALLOW)) {
                        for (c=this->GetHead(); c; c=c->GetSucc()) c->Save();
                    }
                }
            }
            fs->EndObject();
        } else n_warn("nRoot::Save(): BeginObject() failed!");
    } else n_warn("nRoot::Save(): could not open fileserver!");
    return retval;
}

//--------------------------------------------------------------------
/**
    - 18-Dec-98   floh    created
    - 02-May-00   floh    + support for Import(). if an import filename
                            is set, only the import statement will be
                            written instead of the sequence of commands.
                            Subobjects will be skipped as well.
    - 09-Aug-00   floh    + support for shallow copy
*/
//--------------------------------------------------------------------
nRoot *nRoot::Clone(const char *name)
{
    n_assert(name);
    nRoot *clone = NULL;
    nPersistServer *fs = (nPersistServer *) ks->GetPersistServer();
    if (fs) {
        nPersistServer::nSaveMode old_sm = fs->GetSaveMode();
        fs->SetSaveMode(nPersistServer::SAVEMODE_CLONE);
        if (fs->BeginObject(this,name)) {
            if (this->import_file) {
                // write single import statement!
                nCmd *cmd;
                if ((cmd = fs->GetCmd(this,'IMPO'))) {
                    cmd->In()->SetS(this->import_file);
                    fs->PutCmd(cmd);
                }
            } else {
                // ...the usual behaviour...
                nRoot *c;
                if (this->root_flags & N_FLAG_SAVEUPSIDEDOWN) {
                    // upsidedown: save children first, then own status
                    if (!(this->root_flags & N_FLAG_SAVESHALLOW)) {
                        for (c=this->GetHead();c;c=c->GetSucc()) c->Clone(c->GetName());
                    }
                    this->SaveCmds(fs);
                } else {
                    // normal: save own status first, then children
                    this->SaveCmds(fs);
                    if (!(this->root_flags & N_FLAG_SAVESHALLOW)) {
                        for (c=this->GetHead();c;c=c->GetSucc()) c->Clone(c->GetName());
                    }
                }
            }
            fs->EndObject();
            clone = fs->GetClone();

        } else n_warn("nRoot::Clone(): BeginObject() failed!");
        fs->SetSaveMode(old_sm);
    } else n_warn("nRoot::Clone(): could not open fileserver!");
    return clone;
}

//--------------------------------------------------------------------
/**
    - 10-Jan-99   floh    created
*/
//--------------------------------------------------------------------
bool nRoot::Parse(const char *name)
{
    n_assert(name);
    return ks->GetPersistServer()->ParseFile(this,name);
}

//--------------------------------------------------------------------
/**
    Works like parse, but stores filename in
    object. When saving the object, only
    the import statement will be written,
    not the objects script commands. This is
    useful when working with libraries (for instance material
    libraries).

    - 02-May-00   floh    created
*/
//--------------------------------------------------------------------
bool nRoot::Import(const char *name)
{
    n_assert(name);
    if (this->import_file) n_free(this->import_file);
    this->import_file = n_strdup(name);
    return ks->GetPersistServer()->ParseFile(this,name);
}

//--------------------------------------------------------------------
/**
    - 04-Nov-98   floh    created
*/
//--------------------------------------------------------------------
bool nRoot::SaveCmds(nPersistServer *)
{
    return true;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------

