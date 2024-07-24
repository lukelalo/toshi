#define N_IMPLEMENTS nMeshEffect
//------------------------------------------------------------------------------
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "node/nmesheffect.h"
#include "kernel/npersistserver.h"

static void n_setreadonly(void*, nCmd*);
static void n_getreadonly(void*, nCmd*);
static void n_setmeshnode(void*, nCmd*);
static void n_getmeshnode(void*, nCmd*);
static void n_setamplitude(void*, nCmd*);
static void n_getamplitude(void*, nCmd*);
static void n_setfrequency(void*, nCmd*);
static void n_getfrequency(void*, nCmd*);
static void n_setfrequency(void*, nCmd*);
static void n_getfrequency(void*, nCmd*);
static void n_settwinktime(void*, nCmd*);
static void n_gettwinktime(void*, nCmd*);
static void n_settwink(void*, nCmd*);
static void n_gettwink(void*, nCmd*);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nmesheffect
    
    @superclass
    nanimnode

    @classinfo
    ...
*/

void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();

    clazz->AddCmd("v_setreadonly_b",       'SRDO', n_setreadonly);
    clazz->AddCmd("b_getreadonly_v",       'GRDO', n_getreadonly);
    clazz->AddCmd("v_setmeshnode_s",       'SMND', n_setmeshnode);
    clazz->AddCmd("s_getmeshnode_v",       'GMND', n_getmeshnode);
	clazz->AddCmd("v_setamplitude_f",	   'SAMP', n_setamplitude);
	clazz->AddCmd("f_getamplitude_v",	   'GAMP', n_getamplitude);	
	clazz->AddCmd("v_setfrequency_f",	   'SFRQ', n_setfrequency);
	clazz->AddCmd("f_getfrequency_v",	   'GFRQ', n_getfrequency);	
	clazz->AddCmd("v_settwinktime_f",	   'STWT', n_settwinktime);
	clazz->AddCmd("f_gettwinktime_v",	   'GTWT', n_gettwinktime);	
	clazz->AddCmd("v_settwink_b",		   'STWK', n_settwink);
	clazz->AddCmd("b_gettwink_v",		   'GTWK', n_gettwink);	

    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setreadonly

    @input
    b (ReadOnly)

    @output
    v

    @info
    Set read only state of mesh. If the mesh serves as input to another
    mesh animator, the read only flag must be set to true. If the mesh
    is going to be rendered the read only flag must be set to false (this
    is the default).
*/
static void
n_setreadonly(void* slf, nCmd* cmd)
{
    nMeshEffect* self = (nMeshEffect*) slf;
    self->SetReadOnly(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getreadonly

    @input
    v

    @output
    b (ReadOnly)

    @info
    Get the status of the readonly flag.
*/
static void
n_getreadonly(void* slf, nCmd* cmd)
{
    nMeshEffect* self = (nMeshEffect*) slf;
    cmd->Out()->SetB(self->GetReadOnly());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setmeshnode

    @input
    s (MeshNodePath)

    @output
    v

    @info
    Set the path to the source mesh. The source mesh should be a child
    of the nmeshsway object and must be readonly.
*/
static void
n_setmeshnode(void* slf, nCmd* cmd)
{
    nMeshEffect* self = (nMeshEffect*) slf;
    self->SetMeshNode(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getmeshnode

    @input
    v

    @output
    s (MeshNodePath)

    @info
    Get the path to the mesh node object.
*/
static void
n_getmeshnode(void* slf, nCmd* cmd)
{
    nMeshEffect* self = (nMeshEffect*) slf;
    cmd->Out()->SetS(self->GetMeshNode());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setamplitude

    @input
    f (Amplitude)

    @output
    v

    @info
    Set amplitude of the mesh effect
*/
static void
n_setamplitude(void* slf, nCmd* cmd)
{
    nMeshEffect* self = (nMeshEffect*) slf;
    self->SetAmplitude(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getamplitude

    @input
    v

    @output
    f (Amplitude)

    @info
    Get the status of the amplitude flag.
*/
static void
n_getamplitude(void* slf, nCmd* cmd)
{
    nMeshEffect* self = (nMeshEffect*) slf;
    cmd->Out()->SetF(self->GetAmplitude());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setfrequency

    @input
    f (Frequency)

    @output
    v

    @info
    Set frequency of the mesh effect
*/
static void
n_setfrequency(void* slf, nCmd* cmd)
{
    nMeshEffect* self = (nMeshEffect*) slf;
    self->SetFrequency(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getfrequency

    @input
    v

    @output
    f (Frequency)

    @info
    Get the status of the frequency flag.
*/
static void
n_getfrequency(void* slf, nCmd* cmd)
{
    nMeshEffect* self = (nMeshEffect*) slf;
    cmd->Out()->SetF(self->GetFrequency());
}

//------------------------------------------------------------------------------
/**
    @cmd
    settwinktime

    @input
    f (TwinkTime)

    @output
    v

    @info
    Set Twinktime of the mesh effect
*/
static void
n_settwinktime(void* slf, nCmd* cmd)
{
    nMeshEffect* self = (nMeshEffect*) slf;
    self->SetTwinkTime(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    gettwinktime

    @input
    v

    @output
    f (Twinktime)

    @info
    Get the status of the Twinktime flag.
*/
static void
n_gettwinktime(void* slf, nCmd* cmd)
{
    nMeshEffect* self = (nMeshEffect*) slf;
    cmd->Out()->SetF(self->GetTwinkTime());
}

//------------------------------------------------------------------------------
/**
    @cmd
    settwink

    @input
    b (Twink)

    @output
    v

    @info
    Allow Twink of the mesh effect
*/
static void
n_settwink(void* slf, nCmd* cmd)
{
    nMeshEffect* self = (nMeshEffect*) slf;
    self->SetTwink(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    gettwink

    @input
    v

    @output
    b (Twink)

    @info
    Get the status of the Twink flag.
*/
static void
n_gettwink(void* slf, nCmd* cmd)
{
    nMeshEffect* self = (nMeshEffect*) slf;
    cmd->Out()->SetB(self->GetTwink());
}

//------------------------------------------------------------------------------
/**
    @param  fileServer  writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool
nMeshEffect::SaveCmds(nPersistServer* fs)
{
    bool retval = false;

    if (nAnimNode::SaveCmds(fs))
    {
        nCmd* cmd;

        //--- setreadonly ---
        cmd = fs->GetCmd(this, 'SRDO');
        cmd->In()->SetB(this->GetReadOnly());
        fs->PutCmd(cmd);

        //--- setmeshnode ---
        cmd = fs->GetCmd(this, 'SMND');
        cmd->In()->SetS(this->GetMeshNode());
        fs->PutCmd(cmd);

		//--- setamplitude ---
        cmd = fs->GetCmd(this, 'SAMP');
        cmd->In()->SetF(this->GetAmplitude());
        fs->PutCmd(cmd);

		//--- setfrequency ---
        cmd = fs->GetCmd(this, 'SFRQ');
        cmd->In()->SetF(this->GetFrequency());
        fs->PutCmd(cmd);

		//--- settwinktime---
        cmd = fs->GetCmd(this, 'STWT');
        cmd->In()->SetF(this->GetTwinkTime());
        fs->PutCmd(cmd);

		//--- settwink---
        cmd = fs->GetCmd(this, 'STWK');
        cmd->In()->SetB(this->GetTwink());
        fs->PutCmd(cmd);

        retval = true;
    }
    return retval;
}
